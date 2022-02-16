#ifndef _SENSOR_SERVER_H_
#define _SENSOR_SERVER_H_
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_sensor_model_api.h"
#include "ble_mesh_example_init.h"

#define TAG "SENSOR_SERVER"

#define CID_ESP 0x02E5

#define SENSOR_PROPERTY_ID_0 0x0056 /* Present Ambient Temperature */
//#define SENSOR_PROPERTY_ID_1 0x005B /* Present Ambient Temperature */

static int8_t indoor_temp = 40;

#define SENSOR_POSITIVE_TOLERANCE   ESP_BLE_MESH_SENSOR_UNSPECIFIED_POS_TOLERANCE
#define SENSOR_NEGATIVE_TOLERANCE   ESP_BLE_MESH_SENSOR_UNSPECIFIED_NEG_TOLERANCE
#define SENSOR_SAMPLE_FUNCTION      ESP_BLE_MESH_SAMPLE_FUNC_UNSPECIFIED
#define SENSOR_MEASURE_PERIOD       ESP_BLE_MESH_SENSOR_NOT_APPL_MEASURE_PERIOD
#define SENSOR_UPDATE_INTERVAL      ESP_BLE_MESH_SENSOR_NOT_APPL_UPDATE_INTERVAL

NET_BUF_SIMPLE_DEFINE_STATIC(sensor_data, 1);
//NET_BUF_SIMPLE_DEFINE_STATIC(sensor_data_1, 1);

static esp_ble_mesh_sensor_state_t sensor_states[1] = {
    [0] = {
        .sensor_property_id = SENSOR_PROPERTY_ID_0,
        .descriptor.positive_tolerance = SENSOR_POSITIVE_TOLERANCE,
        .descriptor.negative_tolerance = SENSOR_NEGATIVE_TOLERANCE,
        .descriptor.sampling_function = SENSOR_SAMPLE_FUNCTION,
        .descriptor.measure_period = SENSOR_MEASURE_PERIOD,
        .descriptor.update_interval = SENSOR_UPDATE_INTERVAL,
        .sensor_data.format = ESP_BLE_MESH_SENSOR_DATA_FORMAT_A,
        .sensor_data.length = 0, /* 0 represents the length is 1 */
        .sensor_data.raw_value = &sensor_data,
    },
};

ESP_BLE_MESH_MODEL_PUB_DEFINE(sensor_pub, 20, ROLE_FAST_PROV);
static esp_ble_mesh_sensor_srv_t sensor_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    .state_count = ARRAY_SIZE(sensor_states),
    .states = sensor_states,
};

ESP_BLE_MESH_MODEL_PUB_DEFINE(sensor_setup_pub, 20, ROLE_FAST_PROV);
static esp_ble_mesh_sensor_setup_srv_t sensor_setup_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    .state_count = ARRAY_SIZE(sensor_states),
    .states = sensor_states,
};

void ble_mesh_sensor_server_cb(esp_ble_mesh_sensor_server_cb_event_t event,
                               esp_ble_mesh_sensor_server_cb_param_t *param);

#endif