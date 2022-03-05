#ifndef _SENSOR_CLIENT_H_
#define _SENSOR_CLIENT_H_

#include <stdio.h>
#include <esp_system.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_sensor_model_api.h"

#include "ble_mesh_example_init.h"

#define SENSOR_PROPERTY_ID_0 0x0056 /* Present Ambient Temperature */
#define SENSOR_PROPERTY_ID_1 0x005B /* Present Ambient Humidity */

static uint32_t send_opcode[] = {
    [0] = ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET,
    [1] = ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET,
    [2] = ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET,
    [3] = ESP_BLE_MESH_MODEL_OP_SENSOR_GET,
    [4] = ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET,
};

void example_ble_mesh_sensor_client_cb(esp_ble_mesh_sensor_client_cb_event_t event,
                                       esp_ble_mesh_sensor_client_cb_param_t *param);
// void example_ble_mesh_send_sensor_message(esp_ble_mesh_node_t *node, 
//                                           esp_ble_mesh_client_t sensor_client, uint32_t opcode);

#endif