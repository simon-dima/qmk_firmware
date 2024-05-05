// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "print.h"
#include "rgb.h"

// keymaps is defined in bandoneo.c

void keyboard_post_init_user(void) {
  uprintf("hi from keyboard_post_init_user\n");
  // Customise these values to desired behaviour
  debug_enable=true;
  // debug_matrix=true;
  debug_keyboard=true;
  debug_mouse=true;

  register_rgb_rpc();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case KC_ENTER:
      if (record->event.pressed) {
        uprintf("pressed\n");
	slave_setrgb(RGB_GREEN);
	master_setrgb(RGB_BLUE);
      } else {
	uprintf("released\n");
	slave_setrgb(RGB_BLACK);
	master_setrgb(RGB_BLACK);
      }
      return true; // Let QMK send the enter press/release events
    default:
      return true; // Process all other keycodes normally
  }
}

void housekeeping_task_user(void) {
}
