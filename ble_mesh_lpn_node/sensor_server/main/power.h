#ifndef _POWER_H_
#define _POWER_H_

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#define POWER_LEVEL_MODEL_RANGE_MAX 0xFFFF
#define POWER_LEVEL_MODEL_RANGE_MIN 0xE189   //3.7-4.2
#define POWER_LEVEL_MODEL_POWER_DEFAULT 0xFFFF

static esp_ble_mesh_gen_power_level_state_t power_state[1] =
{
  [0] = {
    .power_range_max = POWER_LEVEL_MODEL_RANGE_MAX,
    .power_range_min = POWER_LEVEL_MODEL_RANGE_MIN,
    .power_default   = POWER_LEVEL_MODEL_POWER_DEFAULT,
  }
  
};

ESP_BLE_MESH_MODEL_PUB_DEFINE(power_pub, 20, ROLE_NODE);
static esp_ble_mesh_gen_power_level_srv_t power_server = {
  .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
  .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
  .state = power_state,
};

ESP_BLE_MESH_MODEL_PUB_DEFINE(power_setup_pub, 20, ROLE_NODE);
static esp_ble_mesh_gen_power_level_setup_srv_t power_setup_server = {
  .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
  .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
  .state = power_state,
};

void ble_mesh_generic_server_cb(esp_ble_mesh_generic_server_cb_event_t event,
                                esp_ble_mesh_generic_server_cb_param_t *param);

#endif