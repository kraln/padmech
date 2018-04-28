/*
 * This file is part of the BTLE+SNES Firmware
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#include "string.h"
#include "btle_snes.h"
#include "hw.h"
#include "debug.h"
#include "nrf_delay.h"
#include "nrf_nvmc.h"
#include "nrf_gpio.h"

// reference: https://tresi.github.io/nes/
// reference: http://electrogeek.cc/Around%20a%20SNES.html
volatile bool clock_edge = false;
volatile bool latch_edge = false;
void pad_gpio_interrupt(void)
{
  if (nrf_gpio_pin_read(SNES_LATCH))
  {
    latch_edge = true;

    /* Increment latch counter */
    global_state.latch_count++;
  } 
}

/* The SNES samples on the falling edge */
void pad_handle_shifting(pad_state_t * state)
{
  /* Already assert the B state (A for NES) */
  if (state->button_state.btn_b) nrf_gpio_pin_set(SNES_DATA); else nrf_gpio_pin_clear(SNES_DATA);

  /* wait for latch to de-assert */
  while(nrf_gpio_pin_read(SNES_LATCH));
  pad_wait_clock_edge();

  /* Assert the Y state (B for NES) */
  if (state->button_state.btn_y) nrf_gpio_pin_set(SNES_DATA); else nrf_gpio_pin_clear(SNES_DATA);
  pad_wait_clock_edge();

  /* Assert the Select state */
  if (state->button_state.btn_select) nrf_gpio_pin_set(SNES_DATA); else nrf_gpio_pin_clear(SNES_DATA);
  pad_wait_clock_edge();
  
  /* Assert the Start state */
  if (state->button_state.btn_start) nrf_gpio_pin_set(SNES_DATA); else nrf_gpio_pin_clear(SNES_DATA);
  pad_wait_clock_edge();
  
  /* Assert the DPAD Up state */
  if (state->button_state.btn_dpad_up) nrf_gpio_pin_set(SNES_DATA); else nrf_gpio_pin_clear(SNES_DATA);
  pad_wait_clock_edge();
  
  /* Assert the DPAD Down state */
  if (state->button_state.btn_dpad_down) nrf_gpio_pin_set(SNES_DATA); else nrf_gpio_pin_clear(SNES_DATA);
  pad_wait_clock_edge();
  
  /* Assert the DPAD Left state */
  if (state->button_state.btn_dpad_left) nrf_gpio_pin_set(SNES_DATA); else nrf_gpio_pin_clear(SNES_DATA);
  pad_wait_clock_edge();
  
  /* Assert the DPAD Right state */
  if (state->button_state.btn_dpad_right) nrf_gpio_pin_set(SNES_DATA); else nrf_gpio_pin_clear(SNES_DATA);
  pad_wait_clock_edge();
  
  /* Assert the A state */
  if (state->button_state.btn_a) nrf_gpio_pin_set(SNES_DATA); else nrf_gpio_pin_clear(SNES_DATA);
  pad_wait_clock_edge();
  
  /* Assert the X state */
  if (state->button_state.btn_x) nrf_gpio_pin_set(SNES_DATA); else nrf_gpio_pin_clear(SNES_DATA);
  pad_wait_clock_edge();
  
  /* Assert the L state */
  if (state->button_state.btn_l) nrf_gpio_pin_set(SNES_DATA); else nrf_gpio_pin_clear(SNES_DATA);
  pad_wait_clock_edge();
  
  /* Assert the R state */
  if (state->button_state.btn_r) nrf_gpio_pin_set(SNES_DATA); else nrf_gpio_pin_clear(SNES_DATA);
  pad_wait_clock_edge();

#if 0
  /* Output 0 until next latch */
  nrf_gpio_pin_clear(SNES_DATA);
#else
  /* Output 1 until next latch */
  nrf_gpio_pin_set(SNES_DATA);
#endif
}

/* wait for rising edge */
void pad_wait_clock_edge()
{
  while(nrf_gpio_pin_read(SNES_CLOCK));
  nrf_delay_us(1);
  while(!nrf_gpio_pin_read(SNES_CLOCK));
  nrf_delay_us(1);
}

void pad_update_buttons(pad_state_t * state)
{
  state->button_state.btn_dpad_up    = nrf_gpio_pin_read(BTN_DU);
  state->button_state.btn_dpad_left  = nrf_gpio_pin_read(BTN_DL);
  state->button_state.btn_dpad_right = nrf_gpio_pin_read(BTN_DR);
  state->button_state.btn_dpad_down  = nrf_gpio_pin_read(BTN_DD);
  state->button_state.btn_select     = nrf_gpio_pin_read(BTN_SELECT);
  state->button_state.btn_start      = nrf_gpio_pin_read(BTN_START);
  state->button_state.btn_b          = nrf_gpio_pin_read(BTN_B);
  state->button_state.btn_a          = nrf_gpio_pin_read(BTN_A);
  state->button_state.btn_y          = nrf_gpio_pin_read(BTN_Y);
  state->button_state.btn_x          = nrf_gpio_pin_read(BTN_X);
  state->button_state.btn_r          = nrf_gpio_pin_read(BTN_R);
  state->button_state.btn_l          = nrf_gpio_pin_read(BTN_L);

#if 2
  /* Debugging info every 5 secs */
  if (state->latch_count % 300 == 0)
  {
    _debug_printf("Buttons: %d%d%d%d%d%d%d%d%d%d%d%d, Latch Count %d!", 
    state->button_state.btn_b,
    state->button_state.btn_y,
    state->button_state.btn_select,
    state->button_state.btn_start,
    state->button_state.btn_dpad_up,
    state->button_state.btn_dpad_down,
    state->button_state.btn_dpad_left,
    state->button_state.btn_dpad_right,
    state->button_state.btn_a,
    state->button_state.btn_x,
    state->button_state.btn_l,
    state->button_state.btn_r,
    state->latch_count);
  }
  #endif
}
