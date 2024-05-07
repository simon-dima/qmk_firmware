// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "print.h"
#include "comms.h"

// keymaps is defined in bandoneo.c

bool seen_init = 0;

void keyboard_post_init_user(void) {
  uprintf("hi from keyboard_post_init_user\n");
  // Customise these values to desired behaviour
  debug_enable=true;
  // debug_matrix=true;
  // debug_keyboard=true;
  debug_mouse=true;

  register_user_rpcs();
  if (!is_keyboard_master()) {
          slave_print("Init\n"); // does not appear, why?
          seen_init = 1;
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case KC_ENTER:
      if (record->event.pressed) {
        uprintf("pressed\n");
	setrgb_slave(RGB_GREEN);
	setrgb_master(RGB_BLUE);
      } else {
	uprintf("released\n");
	setrgb_slave(RGB_BLACK);
	setrgb_master(RGB_BLACK);
      }
      return true; // Let QMK send the enter press/release events
    default:
      return true; // Process all other keycodes normally
  }
}

void housekeeping_task_user(void) {
    if (is_keyboard_master()) {
        recv_comms_master();
    } else {
      char s[50];
      snprintf(s, 50,  "Seen init: %d\n", seen_init);
      slave_print(s);
      slave_print("Housekeeping\n");
  }

}
