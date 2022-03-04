#ifndef _POWER_H_
#define _POWER_H_
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "prov.h"
void example_ble_mesh_generic_client_cb(esp_ble_mesh_generic_client_cb_event_t event,
                                        esp_ble_mesh_generic_client_cb_param_t *param);
#endif