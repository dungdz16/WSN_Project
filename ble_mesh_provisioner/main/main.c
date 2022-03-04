#include "prov.h"
#include "freeRTOS/timers.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#define TAG "BLE_MESH_PROVISIONER"
#define WEB_SERVER "api.thingspeak.com"
#define WEB_PORT "80"
#define WIFI_CONNECTED_BIT BIT0

char REQUEST[512];
char SUBREQUEST[100];
char recv_buf[512];
uint32_t voltage;

extern uint8_t dev_uuid[16];;
extern void example_ble_mesh_send_gen_power_level_get(esp_ble_mesh_node_t *node, esp_ble_mesh_client_t power_level_client, uint16_t app_idx);
extern void example_ble_mesh_send_gen_power_level_set(esp_ble_mesh_node_t *node, esp_ble_mesh_client_t power_level_client, uint16_t app_idx);
extern void example_ble_mesh_send_sensor_message(esp_ble_mesh_node_t *node, 
                                                esp_ble_mesh_client_t sensor_client, uint32_t opcode, uint16_t app_idx);
extern esp_ble_mesh_client_t power_level_client;
extern esp_ble_mesh_client_t sensor_client;
SemaphoreHandle_t fastProvNodeSemaphore;
xTimerHandle getPwrLvlTimer;
xSemaphoreHandle sem_test;
void getPwrLvlCallback(TimerHandle_t xTimer);
void test(void* parameter);
static void http_get_task(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[64];

    while(1) {
// connect
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Code to print the resolved IP.

           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... allocated socket");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "... connected");
        freeaddrinfo(res);
// send HTTP REQUEST
        // create REQUEST string
        int tem = 25;
        int hum = 25;
         sprintf(SUBREQUEST, "api_key=3HX6JGXXI589JEZI&field1=%d&field2=%d", tem, hum);    
         sprintf(REQUEST, "POST /update.json HTTP/1.1\nHost: api.thingspeak.com\nConnection: close\nContent-Type: application/x-www-form-urlencoded\nContent-Length:%d\n\n%s\n",strlen(SUBREQUEST),SUBREQUEST);
        
       //sprintf(REQUEST, "https://api.thingspeak.com/update?api_key=A7B92CTA3AXZZ12E&field1=5 ");
       // sprintf(REQUEST, "https://api.thingspeak.com/update?api_key=A7B92CTA3AXZZ12E&field2=hum\n\n");
        if (write(s, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(TAG, "... socket send failed");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... socket send success");

// set timeout for receiver response
        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... set socket receiving timeout success");
// Read response
        /* Read HTTP response */
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                putchar(recv_buf[i]);
            }
        } while(r > 0);

// delay 10s
        ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
        close(s);
        for(int countdown = 10; countdown >= 0; countdown--) {
            ESP_LOGI(TAG, "%d... ", countdown);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Starting again!");
    }
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
        ESP_LOGE(TAG, "Failed to initialize BLE Mesh (err %d)", err);
    }
    getPwrLvlTimer = xTimerCreate("Send Power Level Get",
                                    pdMS_TO_TICKS(20000),
                                    true,
                                    0,
                                    getPwrLvlCallback);
    // xTaskCreate(test,
    //             "test",
    //             1024 * 2,
    //             NULL,
    //             1,
    //             NULL);
    if (getPwrLvlTimer == NULL)
    {
        ESP_LOGE(TAG, "Fail to creat send power level timer");
    }
    else 
    {
        xTimerStart(getPwrLvlTimer, 0);
    }
    // fastProvNodeSemaphore = xSemaphoreCreateBinary();
    // while(1)
    // {
    //     if (xSemaphoreTake(fastProvNodeSemaphore, portMAX_DELAY) == pdTRUE)
    //     {
    //         ESP_LOGI(TAG, "Info Data: %04x", fastProvInfo.len);
    //         ESP_LOG_BUFFER_HEX(TAG, fastProvInfo.data, fastProvInfo.len);
    //     }
    // }
    // ESP_ERROR_CHECK( nvs_flash_init() );
    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    //ESP_ERROR_CHECK(example_connect());
    //wifi_console_init();
    //xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);
    
}

void test(void* parameter)
{
    while(1)
    {
        // fastProvNodeInfo* info = NULL;
        // fastProvNodeInfo* info_buffer = NULL;
        // if (fastProvNodeInfoHandle == NULL)
        // {
        //     ESP_LOGE(TAG, "%s:fastProvNode Queue is NULL",__func__);
        //     //return ESP_FAIL;
        //     vTaskDelete(NULL);
        // }
        // else
        // {
        //     if (xQueueReceive(fastProvNodeInfoHandle, &info_buffer, 1000) == pdTRUE)
        //     {
        //         if (info_buffer == NULL)
        //         {
        //             //return ESP_FAIL;
        //             vTaskDelete(NULL);
        //         }
        //         ESP_LOGI(TAG, "OK");
        //     }
        //     else
        //     {
        //         vTaskDelete(NULL);
        //     }   
        // }
        // info->len = info_buffer->len;
        // info->data = info_buffer->data;
        // ESP_LOGI(TAG, "Info Data: %04x", info->len);
        // ESP_LOG_BUFFER_HEX(TAG, info->data, info->len);
        // uint16_t unicast_addr = info->data[0];
        // uint8_t uuid[16];
        // for (uint8_t index = 2; index < info->len; index++)
        // {
        //     uuid[index - 2] = info->data[index];
        // }
        // uuid[15] = 0;
        // uuid[16] = 0;
        // ESP_LOGI(TAG, "New Prov Node Addr: %d", unicast_addr);
        // ESP_LOG_BUFFER_HEX(TAG, uuid, 16);
        //esp_err_t err = example_store_node_info(uuid, unicast_addr, 1, prov_info.net_idx, prov_info.app_idx, LED_OFF);
        // if (err != ESP_OK) {
        //     ESP_LOGE(TAG, "%s: Failed to set node info", __func__);
        //     return;
        // }
        // free(info_buffer);
        //esp_ble_mesh_node_t *node = esp_ble_mesh_provisioner_get_node_with_addr(0x0006);
        // if (node != NULL)
        // {
        //     example_ble_mesh_send_gen_power_level_get(prov_info, power_level_client, 0x0006);
        //     ESP_LOGI(TAG, "sen GET POWER");
        // // example_ble_mesh_send_gen_power_level_set(node, power_level_client, prov_info.app_idx);
        // // ESP_LOGI(TAG, "sen SET POWER");
        //     example_ble_mesh_send_sensor_message(node, sensor_client, send_opcode[3], prov_info.app_idx);
        //     ESP_LOGI(TAG, "sen GET SENSOR");
        // }
        //vTaskDelete(NULL);
    }
}
void getPwrLvlCallback(TimerHandle_t xTimer)
{
    // esp_ble_mesh_node_t *node = esp_ble_mesh_provisioner_get_node_with_addr(0x0006);
    // if (node != NULL)
    // {
        // example_node_info_t *node = example_get_node_info(0x0006);
        // if (!node) {
        // ESP_LOGE(TAG, "%s: Failed to get node info", __func__);
        // }
        // example_ble_mesh_send_gen_power_level_get(prov_info, power_level_client, 0x0006);
        // ESP_LOGI(TAG, "sen GET POWER");
    // example_ble_mesh_send_gen_power_level_set(node, power_level_client, prov_info.app_idx);
    // ESP_LOGI(TAG, "sen SET POWER");
        // example_ble_mesh_send_sensor_message(node, sensor_client, send_opcode[3], prov_info.app_idx);
        // ESP_LOGI(TAG, "sen GET SENSOR");
    //}
}