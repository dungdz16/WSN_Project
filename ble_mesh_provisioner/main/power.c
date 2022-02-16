#include "power.h"

#define TAG "POWER_CLIENT"
void example_ble_mesh_send_gen_power_level_get(void)
{
    esp_ble_mesh_generic_client_get_state_t get = {0};
    esp_ble_mesh_client_common_param_t common = {0};
    //esp_ble_mesh_set_msg_common(&common, node, onoff_client.model, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET);
    common.opcode = ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET;
    common.model = power_level_client.model;
    common.ctx.net_idx = 0;
    common.ctx.app_idx = 0;
    common.ctx.addr = 0x0005;
    common.ctx.send_ttl = 3;
    common.ctx.send_rel = false;
    common.msg_timeout = 0;
    common.msg_role = ROLE_FAST_PROV;
    esp_err_t err = esp_ble_mesh_generic_client_get_state(&common, &get);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Fail to send GET POWER LEVEL message");
    }
}

void example_ble_mesh_generic_client_cb(esp_ble_mesh_generic_client_cb_event_t event,
                                        esp_ble_mesh_generic_client_cb_param_t *param)
{
    ESP_LOGI(TAG, "Generic client, event %u, error code %d, opcode is 0x%04x",
        event, param->error_code, param->params->opcode);

    switch (event) {
    case ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT");
        if (param->params->opcode == ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET) {
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET, POWER LEVEL: %d", param->status_cb.power_level_status.present_power);
        }
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT");
        if (param->params->opcode == ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET) {
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET, POWER LEVEL %d", param->status_cb.power_level_status.present_power);
        }
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT");
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT");
        break;
    default:
        break;
    }
}