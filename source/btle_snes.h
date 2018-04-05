/*
 * This file is part of the BTLE+SNES Firmware
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#include "stdint.h"
#include <stdbool.h>
#include "hw.h"

#ifndef BTLE_SNES_H
#define BTLE_SNES_H

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

/* Button State */
typedef struct
{
  bool btn_b;
  bool btn_y;
  bool btn_select;
  bool btn_start;
  bool btn_dpad_up;
  bool btn_dpad_down;
  bool btn_dpad_left;
  bool btn_dpad_right;
  bool btn_a;
  bool btn_x;
  bool btn_l;
  bool btn_r;
} btn_state_t;
 
typedef struct
{
  btn_state_t button_state;
  uint32_t latch_count;
} pad_state_t;

/* Pad state stored here (global) */
pad_state_t global_state;

void pad_step(pad_state_t *);
void pad_gpio_interrupt(void);
void pad_handle_shifting(pad_state_t *);
void pad_wait_clock_edge(void);
void pad_update_buttons(pad_state_t *);

#endif
