#ifndef _PROV_H_
#define _PROV_H_
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_err.h"

#include "ble_mesh_example_init.h"
#include "sensor_server.h"
#include "power.h"
typedef struct{
    uint16_t net_idx;
    uint16_t app_idx;
    uint8_t  app_key[16];
}esp_ble_mesh_key;

esp_err_t ble_mesh_init(void);    

#endif