#pragma once

#include <lame/Led.h>

#include <stddef.h>
#include <stdint.h>

extern LAME_Led led;

void   board_init();

void led_toggle();

void nop();

size_t board_get_unique_id(uint8_t id[], size_t max_len);
size_t board_usb_get_serial(uint16_t desc_str1[], size_t max_chars);
