#ifndef _PROV_H_
#define _PROV_H_

#include "esp_system.h"
#include "esp_log.h"
#include "sensor_client.h"

typedef struct {
    uint8_t  uuid[16];
    uint16_t unicast;
    uint8_t  elem_num;
    uint8_t  onoff;
    uint8_t sensor;
    uint8_t power;
    uint16_t net_idx;
    uint16_t app_idx;
} esp_ble_mesh_node_info_t;

typedef struct{
    uint16_t net_idx;
    uint16_t app_idx;
    uint8_t  app_key[16];
}esp_ble_mesh_key;

typedef struct {
    uint8_t attribute;
    uint32_t data;
    uint8_t address;
}LPNData;
esp_err_t ble_mesh_init(void);
#endif