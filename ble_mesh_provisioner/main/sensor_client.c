/* main.c - Application main entry point */

/*
 * Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sensor_client.h"
#include "prov.h"
#define TAG "SENSOR_CLIENT"

#define CID_ESP             0x02E5

#define MSG_SEND_TTL        3
#define MSG_SEND_REL        false
#define MSG_TIMEOUT         0
#define MSG_ROLE            ROLE_PROVISIONER

#define COMP_DATA_PAGE_0    0x00

#define COMP_DATA_1_OCTET(msg, offset)      (msg[offset])
#define COMP_DATA_2_OCTET(msg, offset)      (msg[offset + 1] << 8 | msg[offset])

static uint16_t server_address = ESP_BLE_MESH_ADDR_UNASSIGNED;
static uint16_t sensor_prop_id;

static void example_ble_mesh_set_msg_common(esp_ble_mesh_client_common_param_t *common,
                                            esp_ble_mesh_node_t *node,
                                            esp_ble_mesh_model_t *model, uint32_t opcode, uint16_t app_idx)
{
    common->opcode = opcode;
    common->model = model;
    common->ctx.net_idx = node->net_idx;
    common->ctx.app_idx = app_idx;
    common->ctx.addr = node->unicast_addr;
    common->ctx.send_ttl = MSG_SEND_TTL;
    common->ctx.send_rel = MSG_SEND_REL;
    common->msg_timeout = MSG_TIMEOUT;
    common->msg_role = MSG_ROLE;
}


static void example_ble_mesh_parse_node_comp_data(const uint8_t *data, uint16_t length)
{
    uint16_t cid, pid, vid, crpl, feat;
    uint16_t loc, model_id, company_id;
    uint8_t nums, numv;
    uint16_t offset;
    int i;

    cid = COMP_DATA_2_OCTET(data, 0);
    pid = COMP_DATA_2_OCTET(data, 2);
    vid = COMP_DATA_2_OCTET(data, 4);
    crpl = COMP_DATA_2_OCTET(data, 6);
    feat = COMP_DATA_2_OCTET(data, 8);
    offset = 10;

    ESP_LOGI(TAG, "********************** Composition Data Start **********************");
    ESP_LOGI(TAG, "* CID 0x%04x, PID 0x%04x, VID 0x%04x, CRPL 0x%04x, Features 0x%04x *", cid, pid, vid, crpl, feat);
    for (; offset < length; ) {
        loc = COMP_DATA_2_OCTET(data, offset);
        nums = COMP_DATA_1_OCTET(data, offset + 2);
        numv = COMP_DATA_1_OCTET(data, offset + 3);
        offset += 4;
        ESP_LOGI(TAG, "* Loc 0x%04x, NumS 0x%02x, NumV 0x%02x *", loc, nums, numv);
        for (i = 0; i < nums; i++) {
            model_id = COMP_DATA_2_OCTET(data, offset);
            ESP_LOGI(TAG, "* SIG Model ID 0x%04x *", model_id);
            offset += 2;
        }
        for (i = 0; i < numv; i++) {
            company_id = COMP_DATA_2_OCTET(data, offset);
            model_id = COMP_DATA_2_OCTET(data, offset + 2);
            ESP_LOGI(TAG, "* Vendor Model ID 0x%04x, Company ID 0x%04x *", model_id, company_id);
            offset += 4;
        }
    }
    ESP_LOGI(TAG, "*********************** Composition Data End ***********************");
}


void example_ble_mesh_send_sensor_message(esp_ble_mesh_node_t *node, 
                                          esp_ble_mesh_client_t sensor_client, uint32_t opcode, uint16_t app_idx)
{
    esp_ble_mesh_sensor_client_get_state_t get = {0};
    //esp_ble_mesh_sensor_client_set_state_t set = {0};
    esp_ble_mesh_client_common_param_t common = {0};
    esp_err_t err = ESP_OK;

    //node = esp_ble_mesh_provisioner_get_node_with_addr(server_address);
    if (node == NULL) {
        ESP_LOGE(TAG, "Node is not exists");
        return;
    }
    example_ble_mesh_set_msg_common(&common, node, sensor_client.model, opcode, app_idx);
    switch (opcode) {
    case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET:
        get.cadence_get.property_id = sensor_prop_id;
        break;
    case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET:
        get.settings_get.sensor_property_id = sensor_prop_id;
        break;
    case ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET:
        get.series_get.property_id = sensor_prop_id;
        break;
    //case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK:
        
    default:
        break;
    }

    err = esp_ble_mesh_sensor_client_get_state(&common, &get);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send sensor message 0x%04x", opcode);
    }
}

static void example_ble_mesh_sensor_timeout(esp_ble_mesh_node_t *node, 
                                          esp_ble_mesh_client_t sensor_client, uint32_t opcode, uint16_t app_idx)
{
    switch (opcode) {
    case ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET:
        ESP_LOGW(TAG, "Sensor Descriptor Get timeout, opcode 0x%04x", opcode);
        break;
    case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET:
        ESP_LOGW(TAG, "Sensor Cadence Get timeout, opcode 0x%04x", opcode);
        break;
    case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET:
        ESP_LOGW(TAG, "Sensor Cadence Set timeout, opcode 0x%04x", opcode);
        break;
    case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET:
        ESP_LOGW(TAG, "Sensor Settings Get timeout, opcode 0x%04x", opcode);
        break;
    case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_GET:
        ESP_LOGW(TAG, "Sensor Setting Get timeout, opcode 0x%04x", opcode);
        break;
    case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET:
        ESP_LOGW(TAG, "Sensor Setting Set timeout, opcode 0x%04x", opcode);
        break;
    case ESP_BLE_MESH_MODEL_OP_SENSOR_GET:
        ESP_LOGW(TAG, "Sensor Get timeout 0x%04x", opcode);
        break;
    case ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET:
        ESP_LOGW(TAG, "Sensor Column Get timeout, opcode 0x%04x", opcode);
        break;
    case ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET:
        ESP_LOGW(TAG, "Sensor Series Get timeout, opcode 0x%04x", opcode);
        break;
    default:
        ESP_LOGE(TAG, "Unknown Sensor Get/Set opcode 0x%04x", opcode);
        return;
    }

    example_ble_mesh_send_sensor_message(node, sensor_client, opcode, app_idx);
}

void example_ble_mesh_sensor_client_cb(esp_ble_mesh_sensor_client_cb_event_t event,
                                       esp_ble_mesh_sensor_client_cb_param_t *param)
{
    esp_ble_mesh_node_t *node = NULL;

    ESP_LOGI(TAG, "Sensor client, event %u, addr 0x%04x", event, param->params->ctx.addr);

    if (param->error_code) {
        ESP_LOGE(TAG, "Send sensor client message failed (err %d)", param->error_code);
        return;
    }

    node = esp_ble_mesh_provisioner_get_node_with_addr(param->params->ctx.addr);
    if (!node) {
        ESP_LOGE(TAG, "Node 0x%04x not exists", param->params->ctx.addr);
        return;
    }

    switch (event) {
    case ESP_BLE_MESH_SENSOR_CLIENT_GET_STATE_EVT:
        switch (param->params->opcode) {
        case ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET:
            ESP_LOGI(TAG, "Sensor Descriptor Status, opcode 0x%04x", param->params->ctx.recv_op);
            if (param->status_cb.descriptor_status.descriptor->len != ESP_BLE_MESH_SENSOR_SETTING_PROPERTY_ID_LEN &&
                param->status_cb.descriptor_status.descriptor->len % ESP_BLE_MESH_SENSOR_DESCRIPTOR_LEN) {
                ESP_LOGE(TAG, "Invalid Sensor Descriptor Status length %d", param->status_cb.descriptor_status.descriptor->len);
                return;
            }
            if (param->status_cb.descriptor_status.descriptor->len) {
                ESP_LOG_BUFFER_HEX("Sensor Descriptor", param->status_cb.descriptor_status.descriptor->data,
                    param->status_cb.descriptor_status.descriptor->len);
                /* If running with sensor server example, sensor client can get two Sensor Property IDs.
                 * Currently we use the first Sensor Property ID for the following demonstration.
                 */
                sensor_prop_id = param->status_cb.descriptor_status.descriptor->data[1] << 8 |
                                 param->status_cb.descriptor_status.descriptor->data[0];
            }
            break;
        case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET:
            ESP_LOGI(TAG, "Sensor Cadence Status, opcode 0x%04x, Sensor Property ID 0x%04x",
                param->params->ctx.recv_op, param->status_cb.cadence_status.property_id);
            ESP_LOG_BUFFER_HEX("Sensor Cadence", param->status_cb.cadence_status.sensor_cadence_value->data,
                param->status_cb.cadence_status.sensor_cadence_value->len);
            break;
        case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET:
            ESP_LOGI(TAG, "Sensor Settings Status, opcode 0x%04x, Sensor Property ID 0x%04x",
                param->params->ctx.recv_op, param->status_cb.settings_status.sensor_property_id);
            ESP_LOG_BUFFER_HEX("Sensor Settings", param->status_cb.settings_status.sensor_setting_property_ids->data,
                param->status_cb.settings_status.sensor_setting_property_ids->len);
            break;
        case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_GET:
            ESP_LOGI(TAG, "Sensor Setting Status, opcode 0x%04x, Sensor Property ID 0x%04x, Sensor Setting Property ID 0x%04x",
                param->params->ctx.recv_op, param->status_cb.setting_status.sensor_property_id,
                param->status_cb.setting_status.sensor_setting_property_id);
            if (param->status_cb.setting_status.op_en) {
                ESP_LOGI(TAG, "Sensor Setting Access 0x%02x", param->status_cb.setting_status.sensor_setting_access);
                ESP_LOG_BUFFER_HEX("Sensor Setting Raw", param->status_cb.setting_status.sensor_setting_raw->data,
                    param->status_cb.setting_status.sensor_setting_raw->len);
            }
            break;
        case ESP_BLE_MESH_MODEL_OP_SENSOR_GET:
            ESP_LOGI(TAG, "Sensor Status, opcode 0x%04x", param->params->ctx.recv_op);
            if (param->status_cb.sensor_status.marshalled_sensor_data->len) {
                ESP_LOG_BUFFER_HEX("Sensor Data", param->status_cb.sensor_status.marshalled_sensor_data->data,
                    param->status_cb.sensor_status.marshalled_sensor_data->len);
                uint8_t *data = param->status_cb.sensor_status.marshalled_sensor_data->data;
                uint16_t length = 0;
                for (; length < param->status_cb.sensor_status.marshalled_sensor_data->len; ) {
                    uint8_t fmt = ESP_BLE_MESH_GET_SENSOR_DATA_FORMAT(data);
                    uint8_t data_len = ESP_BLE_MESH_GET_SENSOR_DATA_LENGTH(data, fmt);
                    uint16_t prop_id = ESP_BLE_MESH_GET_SENSOR_DATA_PROPERTY_ID(data, fmt);
                    uint8_t mpid_len = (fmt == ESP_BLE_MESH_SENSOR_DATA_FORMAT_A ?
                                        ESP_BLE_MESH_SENSOR_DATA_FORMAT_A_MPID_LEN : ESP_BLE_MESH_SENSOR_DATA_FORMAT_B_MPID_LEN);
                    ESP_LOGI(TAG, "Format %s, length 0x%02x, Sensor Property ID 0x%04x",
                        fmt == ESP_BLE_MESH_SENSOR_DATA_FORMAT_A ? "A" : "B", data_len, prop_id);
                    if (data_len != ESP_BLE_MESH_SENSOR_DATA_ZERO_LEN) {
                        ESP_LOG_BUFFER_HEX("Sensor Data", data + mpid_len, data_len + 1);
                        length += mpid_len + data_len + 1;
                        data += mpid_len + data_len + 1;
                    } else {
                        length += mpid_len;
                        data += mpid_len;
                    }
                }
            }
            break;
        case ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET:
            ESP_LOGI(TAG, "Sensor Column Status, opcode 0x%04x, Sensor Property ID 0x%04x",
                param->params->ctx.recv_op, param->status_cb.column_status.property_id);
            ESP_LOG_BUFFER_HEX("Sensor Column", param->status_cb.column_status.sensor_column_value->data,
                param->status_cb.column_status.sensor_column_value->len);
            break;
        case ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET:
            ESP_LOGI(TAG, "Sensor Series Status, opcode 0x%04x, Sensor Property ID 0x%04x",
                param->params->ctx.recv_op, param->status_cb.series_status.property_id);
            ESP_LOG_BUFFER_HEX("Sensor Series", param->status_cb.series_status.sensor_series_value->data,
                param->status_cb.series_status.sensor_series_value->len);
            break;
        default:
            ESP_LOGE(TAG, "Unknown Sensor Get opcode 0x%04x", param->params->ctx.recv_op);
            break;
        }
        break;
    case ESP_BLE_MESH_SENSOR_CLIENT_SET_STATE_EVT:
        switch (param->params->opcode) {
        case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET:
            ESP_LOGI(TAG, "Sensor Cadence Status, opcode 0x%04x, Sensor Property ID 0x%04x",
                param->params->ctx.recv_op, param->status_cb.cadence_status.property_id);
            ESP_LOG_BUFFER_HEX("Sensor Cadence", param->status_cb.cadence_status.sensor_cadence_value->data,
                param->status_cb.cadence_status.sensor_cadence_value->len);
            break;
        case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET:
            ESP_LOGI(TAG, "Sensor Setting Status, opcode 0x%04x, Sensor Property ID 0x%04x, Sensor Setting Property ID 0x%04x",
                param->params->ctx.recv_op, param->status_cb.setting_status.sensor_property_id,
                param->status_cb.setting_status.sensor_setting_property_id);
            if (param->status_cb.setting_status.op_en) {
                ESP_LOGI(TAG, "Sensor Setting Access 0x%02x", param->status_cb.setting_status.sensor_setting_access);
                ESP_LOG_BUFFER_HEX("Sensor Setting Raw", param->status_cb.setting_status.sensor_setting_raw->data,
                    param->status_cb.setting_status.sensor_setting_raw->len);
            }
            break;
        default:
            ESP_LOGE(TAG, "Unknown Sensor Set opcode 0x%04x", param->params->ctx.recv_op);
            break;
        }
        break;
    case ESP_BLE_MESH_SENSOR_CLIENT_PUBLISH_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_SENSOR_CLIENT_PUBLISH_EVT");
        ESP_LOGI(TAG, "Sensor Status, opcode 0x%04x", param->params->ctx.recv_op);
            if (param->status_cb.sensor_status.marshalled_sensor_data->len) {
                ESP_LOG_BUFFER_HEX("Sensor Data", param->status_cb.sensor_status.marshalled_sensor_data->data,
                    param->status_cb.sensor_status.marshalled_sensor_data->len);
                uint8_t *data = param->status_cb.sensor_status.marshalled_sensor_data->data;
                uint16_t length = 0;
                for (; length < param->status_cb.sensor_status.marshalled_sensor_data->len; ) {
                    uint8_t fmt = ESP_BLE_MESH_GET_SENSOR_DATA_FORMAT(data);
                    uint8_t data_len = ESP_BLE_MESH_GET_SENSOR_DATA_LENGTH(data, fmt);
                    uint16_t prop_id = ESP_BLE_MESH_GET_SENSOR_DATA_PROPERTY_ID(data, fmt);
                    uint8_t mpid_len = (fmt == ESP_BLE_MESH_SENSOR_DATA_FORMAT_A ?
                                        ESP_BLE_MESH_SENSOR_DATA_FORMAT_A_MPID_LEN : ESP_BLE_MESH_SENSOR_DATA_FORMAT_B_MPID_LEN);
                    ESP_LOGI(TAG, "Format %s, length 0x%02x, Sensor Property ID 0x%04x",
                        fmt == ESP_BLE_MESH_SENSOR_DATA_FORMAT_A ? "A" : "B", data_len, prop_id);
                    if (data_len != ESP_BLE_MESH_SENSOR_DATA_ZERO_LEN) {
                        ESP_LOG_BUFFER_HEX("Sensor Data", data + mpid_len, data_len + 1);
                        length += mpid_len + data_len + 1;
                        data += mpid_len + data_len + 1;
                    } else {
                        length += mpid_len;
                        data += mpid_len;
                    }
                }
            }
        break;
    case ESP_BLE_MESH_SENSOR_CLIENT_TIMEOUT_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_SENSOR_CLIENT_TIMEOUT_EVT");
        //example_ble_mesh_sensor_timeout(param->params->opcode);
    default:
        break;
    }
}

