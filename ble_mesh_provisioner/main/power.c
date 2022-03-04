#include "power.h"
#include "prov.h"
#define TAG "POWER_CLIENT"
extern esp_ble_mesh_key prov_key;
extern xQueueHandle queue_lpn_data;;
LPNData powerData;
void example_ble_mesh_send_gen_power_level_get(esp_ble_mesh_node_info_t node, esp_ble_mesh_client_t power_level_client)
{
    esp_ble_mesh_generic_client_get_state_t get = {0};
    esp_ble_mesh_client_common_param_t common = {0};
    common.opcode = ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET;
    common.model = power_level_client.model;
    common.ctx.net_idx = prov_key.net_idx;
    common.ctx.app_idx = prov_key.app_idx;
    common.ctx.addr = node.unicast;
    common.ctx.send_ttl = 3;
    common.ctx.send_rel = false;
    common.msg_timeout = 0;
    common.msg_role = ROLE_PROVISIONER;
    esp_err_t err = esp_ble_mesh_generic_client_get_state(&common, &get);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Fail to send GET POWER LEVEL message");
    }
}

void example_ble_mesh_send_gen_power_level_set(esp_ble_mesh_node_info_t node, esp_ble_mesh_client_t power_level_client)
{
    esp_ble_mesh_generic_client_set_state_t set = {0};
    esp_ble_mesh_client_common_param_t common = {0};
    common.opcode = ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK;
    common.model = power_level_client.model;
    common.ctx.net_idx = prov_key.net_idx;
    common.ctx.app_idx = prov_key.app_idx;
    common.ctx.addr = node.unicast;
    common.ctx.send_ttl = 3;
    common.ctx.send_rel = false;
    common.msg_timeout = 0;
    common.msg_role = ROLE_PROVISIONER;
    set.power_level_set.op_en = false;
    set.power_level_set.power = 16;
    esp_err_t err = esp_ble_mesh_generic_client_set_state(&common, &set);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Fail to send GET POWER LEVEL message");
    }
}

// void example_ble_mesh_send_gen_power_level_set(esp_ble_mesh_node_t *node, esp_ble_mesh_client_t power_level_client, uint16_t app_idx)
// {
//     esp_ble_mesh_generic_client_set_state_t set = {0};
//     esp_ble_mesh_client_common_param_t common = {0};
//     common.opcode = ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET;
//     common.model = power_level_client.model;
//     common.ctx.net_idx = node->net_idx;
//     common.ctx.app_idx = app_idx;
//     common.ctx.addr = node->addr;
//     common.ctx.send_ttl = 3;
//     common.ctx.send_rel = false;
//     common.msg_timeout = 0;
//     common.msg_role = ROLE_PROVISIONER;
//     set.power_level_set.op_en = false;
//     set.power_level_set.power = 16;
//     esp_err_t err = esp_ble_mesh_generic_client_set_state(&common, &set);
//     if (err != ESP_OK)
//     {
//         ESP_LOGE(TAG, "Fail to send SET POWER LEVEL message");
//     }
// }

void example_ble_mesh_generic_client_cb(esp_ble_mesh_generic_client_cb_event_t event,
                                        esp_ble_mesh_generic_client_cb_param_t *param)
{
    ESP_LOGI(TAG, "Generic client, event %u, error code %d, opcode is 0x%04x",
        event, param->error_code, param->params->opcode);

    //example_node_info_t *node = NULL;
    uint32_t opcode;
    uint16_t address;

    ESP_LOGI(TAG, "%s, error_code = 0x%02x, event = 0x%02x, addr: 0x%04x",
             __func__, param->error_code, event, param->params->ctx.addr);

    opcode  = param->params->opcode;
    address = param->params->ctx.addr;

    switch (event) {
    case ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT");
        if (param->params->opcode == ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET) {
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET, POWER LEVEL: %d", param->status_cb.power_level_status.present_power);
        }
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT");
        switch (param->params->opcode ) {
            case (ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET):
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET, POWER LEVEL %d", param->status_cb.power_level_status.present_power);
            break;
        // case (ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET):
        //     node->onoff = param->status_cb.onoff_status.present_onoff;
        //     ESP_LOGI(TAG, "node->onoff: 0x%02x", node->onoff);
        //     break;
        }
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT");
        switch (param->params->opcode ) {
            case (ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET):
                ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET, POWER LEVEL %d", param->status_cb.power_level_status.present_power);
                powerData.attribute = 0;
                powerData.data = param->status_cb.power_level_status.present_power * 100/65536;
                powerData.address = address;
                xQueueSend(queue_lpn_data, (void*)&powerData, 0);
                break;
            case (ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS):
                ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS, POWER LEVEL %d", param->status_cb.power_level_status.present_power);
                powerData.attribute = 0;
                powerData.data = param->status_cb.power_level_status.present_power;
                powerData.address = address;
                xQueueSend(queue_lpn_data, (void*)&powerData, 0);
                break;
        }
    case ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT:
        // if (param->params->opcode == ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET)
        // {
        //     ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_CLIENT_TIME_OUT_EVT");
        //     //example_ble_mesh_send_gen_power_level_get(param->params->model.);
        // }
        ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT");

        break;
    default:
        break;
    }
}