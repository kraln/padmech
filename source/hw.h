/*
 * This file is part of the BTLE+SNES Firmware
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#ifndef _hw_h
#define _hw_h

#ifdef __arm__
#define WFE() __WFE()
#define WFI() __WFI()
#define SEV() __SEV()
#else
#define WFE()
#define WFI()
#define SEV()
#endif

/* LFCLK frequency in Hertz, constant. */
#define LFCLK_FREQUENCY       (32768UL)

/* Required RTC working clock RTC_FREQUENCY Hertz. Changable. */
#define RTC_FREQUENCY         (1000UL)  /* 1k gives approx resolution of 1mS */

/* f = LFCLK/(prescaler + 1) */
#define COUNTER_PRESCALER     ((LFCLK_FREQUENCY/RTC_FREQUENCY) - 1)

/* GPIO defines */
// as per Taiyo-Yuden Doc, Retrieved https://statics3.seeedstudio.com/images/opl/datasheet/317030133.pdf
#define BTN_DU     0 
#define BTN_DL     1
#define BTN_DR     6
#define BTN_DD     2
#define BTN_SELECT 5
#define BTN_START  4
#define BTN_B      21 
#define BTN_A      22
#define BTN_Y      20
#define BTN_X      24
#define BTN_R      25
#define BTN_L      28

#define SNES_DATA  3
#define SNES_CLOCK 17
#define SNES_LATCH 18

typedef void (*gpioIntFnPtr)(void);
gpioIntFnPtr gpioFn;

void wait_for_val_ne(volatile uint32_t *value);
void hw_init(void);
void hw_latch_interrupt(bool enable);
void hw_rtc_wakeup(uint32_t ms);
uint32_t hw_rtc_value(void);
void hw_rtc_start(void);
void hw_rtc_clear(void);
void hw_rtc_stop(void);
void hw_sleep_power_off(void);
void hw_sleep_power_on(void);
void hw_clear_port_event();
void hw_switch_to_lfclock(void);
void hw_switch_to_hfclock(void);
void hw_stop_LF_clk(void);
void hw_start_LF_clk(void);
void hw_read_reset_reason(uint32_t *resetreas);
uint32_t hw_ficr_deviceid(size_t index);

#endif
