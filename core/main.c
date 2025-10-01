#include "init.h"

#include <bsp/board.h>

#include <ctype.h>
#include <lame/Event.h>
#include <lame/SoftTimer.h>
#include <tusb.h>

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

static void setPwmControlChangeTimer(int period_ms);
static void setPwmControlPwmPeriod(uint32_t period);
static void setPwmControlLargeDutyCycle(uint32_t largeDutyCycle);
static void setPwmControlSmallDutyCycle(uint32_t smallDutyCycle);

#define MAX_COMMAND_SIZE    100
#define MAX_ARGUMENT_SIZE   25
#define MAX_ARGUMENTS_COUNT 4

typedef char Argument[MAX_ARGUMENT_SIZE];

typedef struct {
    Argument arguments[MAX_ARGUMENTS_COUNT];
    int      argumentCount;
} ArgumentParser;

bool receivedCommand = false;

static const int usb_descr = 0;

static const uint32_t LOW_SPEED_BLINK_TIMEOUT  = 500;
static const uint32_t HIGH_SPEED_BLINK_TIMEOUT = LOW_SPEED_BLINK_TIMEOUT / 4;

LAME_Event key_event;

ArgumentParser argumentParser;

LAME_SoftTimer txPeriodTimer;

struct {
    bool     countOutput;
    uint32_t pwmPeriod;
    uint32_t largeDutyCycle;
    uint32_t smallDutyCycle;
    int      pwmChangePeriod;
} appSettings;

typedef enum {
    PwmControlState_Large,
    PwmControlState_Small,
} PwmControlState;

typedef struct {
    LAME_SoftTimer  pwmChangeTimer;
    PwmControlState state;
    uint32_t        pwmPeriod;
    uint32_t        largeDutyCycle;
    uint32_t        smallDutyCycle;
    int             pwmChangePeriod;
} PwmControl;

PwmControl pwmControl;

static void debugOut(const char *str)
{
    tud_cdc_n_write_str(usb_descr, str);
    tud_cdc_n_write_char(usb_descr, '\n');

    tud_cdc_n_write_flush(usb_descr);
}

static bool parseArgumentString(ArgumentParser *parser, const uint8_t *str, int size)
{
    memset(parser->arguments, 0, sizeof(Argument) * MAX_ARGUMENTS_COUNT);
    parser->argumentCount = 0;

    if (size >= MAX_COMMAND_SIZE) {
        return false;
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

    return argCount > 0 ? true : false;
}

// echo to either Serial0 or Serial1
// with Serial0 as all lower case, Serial1 as all upper case
// static void echo_serial_port(uint8_t itf, uint8_t buf[], uint32_t count)
// {
//     const uint8_t case_diff = 'a' - 'A';

//     for (uint32_t i = 0; i < count; i++) {
//         if (itf == 0) {
//             // echo back 1st port as lower case
//             if (isupper(buf[i]))
//                 buf[i] += case_diff;
//         }
//         else {
//             // echo back 2nd port as upper case
//             if (islower(buf[i]))
//                 buf[i] -= case_diff;
//         }

//         tud_cdc_n_write_char(itf, buf[i]);
//     }
// }

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

            if (!parseArgumentString(&argumentParser, buf, count)) {
                return;
            }

            receivedCommand = true;

            // { // Отладка ввода
            //     char str[30];
            //     snprintf(str, sizeof(str), "s %lu: ", count);
            //     tud_cdc_n_write_str(usb_descr, str);

            //     for (int i = 0; i < argumentParser.argumentCount; ++i) {
            //         snprintf(str, sizeof(str), "%i-%s ", i, argumentParser.arguments[i]);
            //         tud_cdc_n_write_str(usb_descr, str);
            //     }

            //     tud_cdc_n_write_char(usb_descr, '\n');
            // }

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

static void initAppSetting()
{
    appSettings.countOutput     = true;
    appSettings.pwmPeriod       = 20000; // 20 мс
    appSettings.largeDutyCycle  = 2000;  // 2 мс
    appSettings.smallDutyCycle  = 1000;  // 1 мс
    appSettings.pwmChangePeriod = 1000;  // 1 сек
}

static bool setPwmPeriod(uint32_t period)
{
    appSettings.pwmPeriod = period;
    return true;
}

static bool setLargeDutyCycle(uint32_t largeDutyCycle)
{
    appSettings.largeDutyCycle = largeDutyCycle;
    return true;
}

static bool setSmallDutyCycle(uint32_t smallDutyCycle)
{
    appSettings.smallDutyCycle = smallDutyCycle;
    return true;
}

static bool setChangePwmPeriod(uint32_t pwmChangePeriod)
{
    appSettings.pwmChangePeriod = pwmChangePeriod;
    setPwmControlChangeTimer(appSettings.pwmChangePeriod);
    return true;
}

static void getStrSettings(char *str)
{
    sprintf(str,
            "PWM Period - %lu\n"
            "largeDC - %lu\n"
            "smallDC - %lu\n"
            "pwmChangePeriod - %i\n",
            appSettings.pwmPeriod,
            appSettings.largeDutyCycle,
            appSettings.smallDutyCycle,
            appSettings.pwmChangePeriod);
}

static const char *getHelp()
{
    const char *str =
        "help\n"
        "showSettings - show settings\n"
        "pwmPeriod - Set PWM period\n"
        "largeDC - Set large duty Cycle\n"
        "smallDC - Set small duty Cycle\n"
        "period - Set period of change pwm cycle\n"
        "stop - Stop output Cycle\n"
        "start - Start output Cycle\n";
    return str;
}

static void processCommandTask()
{
    if (!receivedCommand) {
        return;
    }
    receivedCommand = false;

    if (strcmp(argumentParser.arguments[0], "stop") == 0) {
        appSettings.countOutput = false;
        debugOut("Output Stop");
    }
    else if (strcmp(argumentParser.arguments[0], "start") == 0) {
        appSettings.countOutput = true;
        debugOut("Output Start");
    }
    else if (strcmp(argumentParser.arguments[0], "pwmPeriod") == 0) {
        uint32_t pwmPeriod = strtoul(argumentParser.arguments[1], NULL, 10);
        setPwmPeriod(pwmPeriod);
        setPwmControlPwmPeriod(pwmPeriod);
    }
    else if (strcmp(argumentParser.arguments[0], "largeDC") == 0) {
        uint32_t largeDutyCycle = strtoul(argumentParser.arguments[1], NULL, 10);
        setLargeDutyCycle(largeDutyCycle);
        setPwmControlLargeDutyCycle(largeDutyCycle);
    }
    else if (strcmp(argumentParser.arguments[0], "smallDC") == 0) {
        uint32_t smallDutyCycle = strtoul(argumentParser.arguments[1], NULL, 10);
        setSmallDutyCycle(smallDutyCycle);
        setPwmControlSmallDutyCycle(smallDutyCycle);
    }
    else if (strcmp(argumentParser.arguments[0], "period") == 0) {
        uint32_t period = strtoul(argumentParser.arguments[1], NULL, 10);
        setChangePwmPeriod(period);
    }
    else if (strcmp(argumentParser.arguments[0], "showSettings") == 0) {
        char str[100];
        getStrSettings(str);
        debugOut(str);
    }
    else if (strcmp(argumentParser.arguments[0], "help") == 0) {
        debugOut(getHelp());
    }
    else {
        const char *str = "Input error";
        debugOut(str);
    }
}

static void countOutputTask()
{
    if (!appSettings.countOutput) {
        LAME_SoftTimer_Stop(&txPeriodTimer);
        return;
    }
    else {
        LAME_SoftTimer_Start(&txPeriodTimer);
    }

    if (LAME_SoftTimer_Occur(&txPeriodTimer)) {
        const char *tx = "55\n";
        tud_cdc_n_write_str(usb_descr, tx);
        tud_cdc_n_write_flush(usb_descr);
    }
}

static void setPwmControlPwmPeriod(uint32_t period)
{
    pwmControl.pwmPeriod = period;
    Board_SetPwmPeriod(period);
}

static void setPwmControlLargeDutyCycle(uint32_t largeDutyCycle)
{
    pwmControl.largeDutyCycle = largeDutyCycle;
    if (pwmControl.state == PwmControlState_Large) {
        Board_SetPwmFront(pwmControl.largeDutyCycle);
    }
}

static void setPwmControlSmallDutyCycle(uint32_t smallDutyCycle)
{
    pwmControl.smallDutyCycle = smallDutyCycle;
    if (pwmControl.state == PwmControlState_Small) {
        Board_SetPwmFront(pwmControl.smallDutyCycle);
    }
}

static void setPwmControlChangeTimer(int period_ms)
{
    pwmControl.pwmChangePeriod = period_ms;
    LAME_SoftTimer_Stop(&pwmControl.pwmChangeTimer);
    LAME_SoftTimer_SetPeriod(&pwmControl.pwmChangeTimer, period_ms);
    LAME_SoftTimer_Start(&pwmControl.pwmChangeTimer);
}

static void pwmControlInit()
{
    pwmControl.state = PwmControlState_Large;

    LAME_SoftTimer_Init(&pwmControl.pwmChangeTimer, LAME_SoftTimer_ModePeriodic, appSettings.pwmChangePeriod);

    setPwmControlPwmPeriod(appSettings.pwmPeriod);
    setPwmControlLargeDutyCycle(appSettings.largeDutyCycle);
    setPwmControlSmallDutyCycle(appSettings.smallDutyCycle);
    setPwmControlChangeTimer(appSettings.pwmChangePeriod);
}

static void pwmControlTask()
{
    if (!LAME_SoftTimer_Occur(&pwmControl.pwmChangeTimer)) {
        return;
    }

    if (pwmControl.state == PwmControlState_Large) {
        pwmControl.state = PwmControlState_Small;
        Board_SetPwmFront(pwmControl.smallDutyCycle);

        char str[30];
        sprintf(str, "PWM Small - %lu", pwmControl.smallDutyCycle);
        debugOut(str);
    }
    else {
        pwmControl.state = PwmControlState_Large;
        Board_SetPwmFront(pwmControl.largeDutyCycle);

        char str[30];
        sprintf(str, "PWM Large - %lu", pwmControl.largeDutyCycle);
        debugOut(str);
    }
}

int main()
{
    LAME_Event_Init(&key_event);

    init();
    initAppSetting();

    uint32_t timeout = LOW_SPEED_BLINK_TIMEOUT;

    LAME_SoftTimer_Init(&txPeriodTimer, LAME_SoftTimer_ModePeriodic, 3000);
    LAME_SoftTimer_Start(&txPeriodTimer);

    Board_SetPwmActive(true);

    pwmControlInit();

    while (1) {
        if (LAME_Event_Take(&key_event)) {
            timeout = (timeout == LOW_SPEED_BLINK_TIMEOUT) ? HIGH_SPEED_BLINK_TIMEOUT : LOW_SPEED_BLINK_TIMEOUT;
        }

        tud_task(); // tinyusb device task
        cdc_task();

        // volatile clock_t cl = clock();
        processCommandTask();
        countOutputTask();
        pwmControlTask();
        LAME_Led_Task();
    }
}
