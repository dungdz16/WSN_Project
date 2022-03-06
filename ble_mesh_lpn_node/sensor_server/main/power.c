#include "power.h"
#include "prov.h"
extern esp_ble_mesh_key prov_key;
static esp_ble_mesh_msg_ctx_t contex;
static esp_ble_mesh_model_t *server_model = NULL;
void handle_gen_power_level_msg(uint32_t voltage)
{
  float value = (float)voltage * 2 * 3.3 * 16 / 4.2;
  uint8_t status[2];
  status[1] = (uint32_t)(value) >> 8;
  status[0] = (uint32_t)(value) & 0xFF;
  contex.addr = 0x0001;
  contex.app_idx = 0;
  contex.net_idx = 0;
  contex.send_ttl = 3;
  contex.send_rel = false;
  //ESP_LOG_BUFFER_HEX(TAG, status, 2);
  // if (contex != NULL)
  // {
    esp_err_t err = esp_ble_mesh_server_model_send_msg(server_model, &contex, 
                                      ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS,
                                      sizeof(status), status);
    if (err != ESP_OK)
    {
      ESP_LOGI(TAG,"Cannot Send Msg");
    }
  //}
  //   break;
  // }
}


void ble_mesh_generic_server_cb(esp_ble_mesh_generic_server_cb_event_t event,
                                esp_ble_mesh_generic_server_cb_param_t *param)
{
  esp_ble_mesh_gen_power_level_srv_t *srv;
  ESP_LOGI(TAG, "event 0x%02x, opcode 0x%04x, src 0x%04x, dst 0x%04x",
      event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);
  switch(event)
  {
    case ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT:
      break;
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT:
      ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT");
      if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET)
      {
        srv = param->model->user_data;
        ESP_LOGI(TAG, "Power Level: %04x", srv->state->power_actual);
        server_model = param->model;
        contex = (param->ctx);
        handle_gen_power_level_msg(0);
      }
      break;
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT:
      ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT");
      if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK)
      {
        srv = param->model->user_data;
        
        
        ESP_LOGI(TAG, "Power Level: %04x", srv->state->power_actual);
        //handle_gen_power_level_msg();
      }
      break;
    case ESP_BLE_MESH_GENERIC_SERVER_EVT_MAX:
      break;
  }
}