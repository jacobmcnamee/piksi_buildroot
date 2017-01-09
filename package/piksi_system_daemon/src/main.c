/*
 * Copyright (C) 2016 Swift Navigation Inc.
 * Contact: Gareth McMullin <gareth@swiftnav.com>
 *
 * This source is subject to the license found in the file 'LICENSE' which must
 * be be distributed together with this source. All other rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <stdio.h>
#include <sbp_zmq.h>
#include <sbp_settings.h>

    #include <libsbp/settings.h>

static sbp_zmq_state_t sbp_zmq_state;

static int settings_msg_send(u16 msg_type, u8 len, u8 *payload)
{
  return sbp_zmq_message_send(&sbp_zmq_state, msg_type, len, payload);
}

static int settings_msg_cb_register(u16 msg_type,
                                    sbp_msg_callback_t cb, void *context,
                                    sbp_msg_callbacks_node_t **node)
{
  return sbp_zmq_callback_register(&sbp_zmq_state, msg_type, cb, context, node);
}

static int settings_msg_cb_remove(sbp_msg_callbacks_node_t *node)
{
  return sbp_zmq_callback_remove(&sbp_zmq_state, node);
}

static int settings_msg_loop_timeout(u32 timeout_ms)
{
  return sbp_zmq_loop_timeout(&sbp_zmq_state, timeout_ms);
}

static int settings_msg_loop_interrupt(void)
{
  return sbp_zmq_loop_interrupt(&sbp_zmq_state);
}


bool settings_register_ok = false;
void settings_write_callback(u16 sender_id, u8 len, u8 msg[], void *context)
{
  printf("settings write callback: len = %d\n", len);

  static int i = 0;
  if (++i == 3) {
    sbp_zmq_state_t *sbp_zmq_state = (sbp_zmq_state_t *)context;
    printf("interrupting\n");
    settings_register_ok = true;
    sbp_zmq_loop_interrupt(sbp_zmq_state);
  }

}

int main(void)
{
  printf("hello world\n");

  /* Set up SBP ZMQ */
  sbp_zmq_config_t sbp_zmq_config = {
    .sbp_sender_id = 0x1233,
    .pub_endpoint = ">tcp://localhost:43011",
    .sub_endpoint = ">tcp://localhost:43010"
  };
  if (sbp_zmq_init(&sbp_zmq_state, &sbp_zmq_config) != 0) {
    exit(EXIT_FAILURE);
  }

  /* Set up settings */
  settings_interface_t settings_interface = {
    .msg_send = settings_msg_send,
    .msg_cb_register = settings_msg_cb_register,
    .msg_cb_remove = settings_msg_cb_remove,
    .msg_loop_timeout = settings_msg_loop_timeout,
    .msg_loop_interrupt = settings_msg_loop_interrupt
  };
  settings_setup(&settings_interface);

  static char test_str2[] = "abczyx123";
  READ_ONLY_PARAMETER("test_section", "test_str2", test_str2, TYPE_STRING);
    READ_ONLY_PARAMETER("test_section", "test_str3", test_str2, TYPE_STRING);
      READ_ONLY_PARAMETER("test_section", "test_str4", test_str2, TYPE_STRING);
        READ_ONLY_PARAMETER("test_section", "test_str5", test_str2, TYPE_STRING);
          READ_ONLY_PARAMETER("test_section", "test_str6", test_str2, TYPE_STRING);

  sbp_zmq_callback_register(&sbp_zmq_state, SBP_MSG_SETTINGS_WRITE,
                            settings_write_callback, &sbp_zmq_state, NULL);


/*
  int ret;
  int i;

  for (i=0; i<8; i++) {
    char msg[] = "test_section\0test_setting\0a12346";
    sbp_zmq_message_send(&sbp_zmq_state, SBP_MSG_SETTINGS_REGISTER,
                         sizeof(msg), msg);
    ret = sbp_zmq_loop_timeout(&sbp_zmq_state, 1000);
    printf("sbp_zmq_loop_timeout %d\n", ret);

    if (settings_register_ok) {
      printf("settings register ok\n");
      break;
    }
  }

  ret = sbp_zmq_loop_timeout(&sbp_zmq_state, 10000);
  printf("sbp_zmq_loop_timeout %d\n", ret);

  ret = sbp_zmq_loop(&sbp_zmq_state);
  printf("sbp_zmq_loop %d\n", ret);
*/

  printf("goodbye\n");
  sbp_zmq_deinit(&sbp_zmq_state);
  exit(EXIT_SUCCESS);
}
