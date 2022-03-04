#include "power.h"

#define TAG "POWER_SERVER"
static void handle_gen_power_level_msg(esp_ble_mesh_model_t *model, 
                                esp_ble_mesh_msg_ctx_t *ctx,
                                esp_ble_mesh_gen_power_level_state_t *state)
{
  uint8_t status[2];
  switch(ctx->recv_op)
  {
    case ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK:
    // status[0] = state->power_actual >> 8;
    // status[1] = state->power_actual && 0xFF;
    status[0] = 10;
    status[1] = 11;
    esp_ble_mesh_server_model_send_msg(model, ctx, 
                                      ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS,
                                      sizeof(status), status);
    break;
  }
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
      if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS)
      {

      }
      break;
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT:
      ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT");
      if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET)
      {
        srv = param->model->user_data;
        ESP_LOGI(TAG, "Power Level: %04x", srv->state->power_actual);
        handle_gen_power_level_msg(param->model, &param->ctx, srv->state);
      }
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT:
      ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT");
      if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK)
      {
        srv = param->model->user_data;
        ESP_LOGI(TAG, "Power Level: %04x", srv->state->power_actual);
        handle_gen_power_level_msg(param->model, &param->ctx, srv->state);
      }
      break;
    case ESP_BLE_MESH_GENERIC_SERVER_EVT_MAX:
      break;
  }
}