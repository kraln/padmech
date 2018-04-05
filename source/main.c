/*
 * This file is part of the BTLE+SNES Firmware
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#include "btle_snes.h"
#include "hw.h"
#include "spi_master.h"
#include "debug.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

/*****************************************************************************/
/** Main **/
/*****************************************************************************/
extern volatile bool latch_edge;
int main(void)
{
  /* Set up the core */
  hw_init();

  _debug_printf("Starting up!");

  gpioFn = &pad_gpio_interrupt;

  for (;;)
  {  
    /* wait for transition from low to high on LATCH */
    while(!nrf_gpio_pin_read(SNES_LATCH)) nrf_delay_us(1);
  
    /* set the data line high on latch */
    nrf_gpio_pin_set(SNES_DATA);

    /* wait for latch to be over */
    while(nrf_gpio_pin_read(SNES_LATCH)) nrf_delay_us(1);

    global_state.latch_count++;

    /* shift the buttons out */
    pad_handle_shifting(&global_state);

    /* update the buttons */
    pad_update_buttons(&global_state);
  }
}
