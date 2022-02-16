#include "fast_prov_client.h"
#include "freeRTOS/timers.h"
#define TAG "BLE_MESH_PROVISIONER"
extern uint8_t dev_uuid[ESP_BLE_MESH_OCTET16_LEN];
extern void example_ble_mesh_send_gen_power_level_get(void);
xTimerHandle getPwrLvlTimer;
void getPwrLvlCallback(TimerHandle_t xTimer);
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
        ESP_LOGE(TAG, "Failed to initialize BLE Mesh (err %d)", err);
    }
    getPwrLvlTimer = xTimerCreate("Send Power Level Get",
                                    pdMS_TO_TICKS(10000),
                                    true,
                                    0,
                                    getPwrLvlCallback);
    if (getPwrLvlTimer == NULL)
    {
        ESP_LOGE(TAG, "Fail to creat send power level timer");
    }
    else 
    {
        xTimerStart(getPwrLvlTimer, 0);
    }
}

void getPwrLvlCallback(TimerHandle_t xTimer)
{
    example_ble_mesh_send_gen_power_level_get();
}