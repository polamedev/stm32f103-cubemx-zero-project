#include "init.h"

#include <bsp/board.h>

#include <ctype.h>
#include <lame/Event.h>
#include <lame/SoftTimer.h>
#include <tusb.h>

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MAX_COMMAND_SIZE    64
#define MAX_ARGUMENT_SIZE   12
#define MAX_ARGUMENTS_COUNT 6

typedef char Argument[MAX_ARGUMENT_SIZE];

typedef struct {
    Argument arguments[MAX_ARGUMENTS_COUNT];
    int      argumentCount;
} ArgumentParser;

static const int usb_descr = 0;

static const uint32_t LOW_SPEED_BLINK_TIMEOUT  = 500;
static const uint32_t HIGH_SPEED_BLINK_TIMEOUT = LOW_SPEED_BLINK_TIMEOUT / 4;

LAME_Event key_event;

ArgumentParser argumentParser;

LAME_SoftTimer txPeriodTimer;

static void parseArgumentString(ArgumentParser *parser, const uint8_t *str, int size)
{
    memset(parser->arguments, 0, sizeof(Argument) * MAX_ARGUMENTS_COUNT);
    parser->argumentCount = 0;

    if (size >= MAX_COMMAND_SIZE) {
        return;
    }

    char buf[MAX_COMMAND_SIZE];
    memcpy(buf, str, size);
    buf[size] = 0;

    int   argCount = 0;
    char *pch      = strtok(buf, " ");

    while (pch != NULL) {
        if (argCount >= MAX_ARGUMENTS_COUNT) {
            break;
        }
        strncpy(parser->arguments[argCount], pch, sizeof(Argument));
        argCount++;
        pch = strtok(NULL, " ");
    }

    parser->argumentCount = argCount;
}

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
    // connected() check for DTR bit
    // Most but not all terminal client set this when making connection
    // if ( tud_cdc_n_connected(usb_descr) )
    {
        if (tud_cdc_n_available(usb_descr)) {
            uint8_t buf[64];

            uint32_t count = tud_cdc_n_read(usb_descr, buf, sizeof(buf));

            // echo back to both serial ports

            parseArgumentString(&argumentParser, buf, count);

            char str[30];
            snprintf(str, sizeof(str), "s %lu: ", count);
            tud_cdc_n_write_str(usb_descr, str);

            for (int i = 0; i < argumentParser.argumentCount; ++i) {
                snprintf(str, sizeof(str), "%i-%s ", i, argumentParser.arguments[i]);
                tud_cdc_n_write_str(usb_descr, str);
            }

            // echo_serial_port(usb_descr, buf, count);

            tud_cdc_n_write_flush(usb_descr);
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
    LAME_Event_Init(&key_event);

    init();

    uint32_t timeout = LOW_SPEED_BLINK_TIMEOUT;

    while (1) {
        if (LAME_Event_Take(&key_event)) {
            timeout = (timeout == LOW_SPEED_BLINK_TIMEOUT) ? HIGH_SPEED_BLINK_TIMEOUT : LOW_SPEED_BLINK_TIMEOUT;
        }

        tud_task(); // tinyusb device task
        cdc_task();

        // volatile clock_t cl = clock();

        LAME_Led_Task();
    }
}
