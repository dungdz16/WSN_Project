#include "prov.h"
#include "freeRTOS/timers.h"
#include "freeRTOS/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
//#include "sensor_server.h"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   5          //Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
#if CONFIG_IDF_TARGET_ESP32
static const adc_channel_t channel = ADC_CHANNEL_0;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
#elif CONFIG_IDF_TARGET_ESP32S2
static const adc_channel_t channel = ADC_CHANNEL_6;     // GPIO7 if ADC1, GPIO17 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_13;
#endif
//static const dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_2;

extern uint8_t dev_uuid[16];
extern void handle_gen_power_level_msg(uint32_t voltage);
extern void example_ble_mesh_send_sensor_status(uint8_t humidity, uint8_t temperature);
xTimerHandle sendDataTimerHandle;

void handleData(void* parameter);
void setupADC();
static void check_efuse(void);
static void print_char_val_type(esp_adc_cal_value_t val_type);

void sendDataCallback(xTimerHandle xTimer)
{   
    xTaskCreatePinnedToCore(handleData,
                            "handle and send data",
                            1024 * 3,
                            NULL,
                            1,
                            NULL,
                            1);
}

void app_main(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing...");

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }

    ble_mesh_get_dev_uuid(dev_uuid);

    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
        return;
    }
    sendDataTimerHandle = xTimerCreate("Send Data Timer",
                                        pdMS_TO_TICKS(30000),
                                        true,
                                        0,
                                        sendDataCallback);
    xTimerStart(sendDataTimerHandle, 0);
    setupADC();
}

void handleData(void* parameter)
{
    while(1)
    {
        uint32_t adc_reading = 0;
        uint8_t temperature = 23 + esp_random()/(UINT32_MAX);
        uint8_t humidity = 87 + esp_random()/(UINT32_MAX);
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, width, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;
        //Convert adc_reading to voltage in mV
        float voltage = adc_reading * 3.3 / 4096;
        printf("Raw: %d\tVoltage: %fmV\n", adc_reading, voltage);
        // if (dht_read_data(sensor_type, dht_gpio, &humidity, &temperature) == ESP_OK)
        // {
            ESP_LOGI(TAG, "Humidity: %d%% Temp: %dC\n", humidity, temperature);
            ESP_LOGI(TAG, "Server Model Send Msg");
            example_ble_mesh_send_sensor_status(humidity, temperature);
        //}
        // else
        // {
        //     ESP_LOGE(TAG, "Could not read data from sensor");
        // }
        // if (adc_reading != 0)
        // {
            handle_gen_power_level_msg(adc_reading);
        //}
        vTaskDelete(NULL);
    }
}
void setupADC()
{
    //Check if Two Point or Vref are burned into eFuse

    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(channel, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    //Characterize ADC
    // adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    // esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    // print_char_val_type(val_type);
}

static void check_efuse(void)
{
#if CONFIG_IDF_TARGET_ESP32
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("Cannot retrieve eFuse Two Point calibration values. Default calibration values will be used.\n");
    }
#else
#error "This example is configured for ESP32/ESP32S2."
#endif
}


static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}






