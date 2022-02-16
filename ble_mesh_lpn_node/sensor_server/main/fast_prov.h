#ifndef _FAST_PROV_H_
#define _FAST_PROV_H_

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_err.h"

#include "ble_mesh_example_init.h"
#include "sensor_server.h"
#include "power.h"
esp_err_t ble_mesh_init(void);             

#endif