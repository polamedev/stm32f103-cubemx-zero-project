#include "init.h"

#include <board.h>

#include <ctype.h>
#include <lame/event.h>
#include <tusb.h>

#include <stdbool.h>

static const uint32_t LOW_SPEED_BLINK_TIMEOUT  = 500;
static const uint32_t HIGH_SPEED_BLINK_TIMEOUT = LOW_SPEED_BLINK_TIMEOUT / 4;

LAME_event key_event;

// echo to either Serial0 or Serial1
// with Serial0 as all lower case, Serial1 as all upper case
static void echo_serial_port(uint8_t itf, uint8_t buf[], uint32_t count)
{
    const uint8_t case_diff = 'a' - 'A';

    for (uint32_t i = 0; i < count; i++) {
        if (itf == 0) {
            // echo back 1st port as lower case
            if (isupper(buf[i]))
                buf[i] += case_diff;
        }
        else {
            // echo back 2nd port as upper case
            if (islower(buf[i]))
                buf[i] -= case_diff;
        }

        tud_cdc_n_write_char(itf, buf[i]);
    }
    tud_cdc_n_write_flush(itf);
}

// Invoked when device is mounted
void tud_mount_cb(void)
{
    // blink_interval_ms = BLINK_MOUNTED;
    nop();
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    // blink_interval_ms = BLINK_NOT_MOUNTED;
    nop();
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
static void cdc_task(void)
{
    uint8_t itf;

    for (itf = 0; itf < CFG_TUD_CDC; itf++) {
        // connected() check for DTR bit
        // Most but not all terminal client set this when making connection
        // if ( tud_cdc_n_connected(itf) )
        {
            if (tud_cdc_n_available(itf)) {
                uint8_t buf[64];

                uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));

                // echo back to both serial ports
                echo_serial_port(itf, buf, count);
            }
        }
    }
}

// Invoked when cdc when line state changed e.g connected/disconnected
// Use to reset to DFU when disconnect with 1200 bps
void tud_cdc_line_state_cb(uint8_t instance, bool dtr, bool rts)
{
    (void)rts;

    // DTR = false is counted as disconnected
    if (!dtr) {
        // touch1200 only with first CDC instance (Serial)
        if (instance == 0) {
            cdc_line_coding_t coding;
            tud_cdc_get_line_coding(&coding);
            if (coding.bit_rate == 1200) {
                // if (board_reset_to_bootloader) {
                //   board_reset_to_bootloader();
                // }
            }
        }
    }
}

int main()
{
    LAME_event_init(&key_event);

    init();

    uint32_t timeout = LOW_SPEED_BLINK_TIMEOUT;

    while (1) {
        if (LAME_event_take(&key_event)) {
            timeout = (timeout == LOW_SPEED_BLINK_TIMEOUT) ? HIGH_SPEED_BLINK_TIMEOUT : LOW_SPEED_BLINK_TIMEOUT;
        }

        tud_task(); // tinyusb device task
        cdc_task();
    }
}
