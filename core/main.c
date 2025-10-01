#include "init.h"

#include <bsp/board.h>

#include <ctype.h>
#include <lame/Event.h>
#include <lame/SoftTimer.h>
#include <tusb.h>

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

static void setPwmControlActive(bool active);
static void setPwmControlChangeTimer(int period_ms);
static void setPwmControlPwmPeriod(uint32_t period);
static void setPwmControlLargeDutyCycle(uint32_t largeDutyCycle);
static void setPwmControlSmallDutyCycle(uint32_t smallDutyCycle);

enum LedState {
    LedState_PWM_Enable  = 1,
    LedState_PWM_Disable = 2,
};

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

typedef struct {
    uint32_t pwmPeriod;
    uint32_t largeDutyCycle;
    uint32_t smallDutyCycle;
    int      pwmChangePeriod;
} ApSettings;

ApSettings appSettings;

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
    int             cycleNumber;
    bool            isActive;
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
    static volatile ApSettings appSettingsTemp;
    Board_FlashRead((uint32_t *)&appSettingsTemp, sizeof(appSettingsTemp));

    if (appSettingsTemp.pwmPeriod == 0xFFFFFFFF) {
        appSettings.pwmPeriod       = 20000; // 20 мс
        appSettings.largeDutyCycle  = 2000;  // 2 мс
        appSettings.smallDutyCycle  = 1000;  // 1 мс
        appSettings.pwmChangePeriod = 10000; // 10 сек
    }
    else {
        appSettings = appSettingsTemp;
    }
}

static bool saveAppSettings()
{
    return Board_FlashWrite((uint32_t *)&appSettings, sizeof(appSettings));
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
            "PWM state    - %i\n"
            "PWM Period   - %lu mcs\n"
            "Large PWM DC - %lu mcs\n"
            "Small PWM DC - %lu mcs\n"
            "Change Delay - %i ms\n",
            pwmControl.isActive,
            appSettings.pwmPeriod,
            appSettings.largeDutyCycle,
            appSettings.smallDutyCycle,
            appSettings.pwmChangePeriod);
}

static const char *getHelp()
{
    const char *str =
        "help    - Show help\n"
        "show    - Show settings\n"
        "period  - Set PWM period, mcs\n"
        "large   - Set large PWM Duty Cycle, mcs\n"
        "small   - Set small PWM Duty Cycle, mcs\n"
        "delay   - Set delay of change pwm cycle, ms\n"
        "stop    - Stop output PWM\n"
        "start   - Start output PWM\n"
        "save    - Save PWM settings to Flash\n";
    return str;
}

static void processCommandTask()
{
    if (!receivedCommand) {
        return;
    }
    receivedCommand = false;

    if (strcmp(argumentParser.arguments[0], "stop") == 0) {
        debugOut("PWM Stop");
        setPwmControlActive(false);
    }
    else if (strcmp(argumentParser.arguments[0], "start") == 0) {
        debugOut("PWM Start");
        setPwmControlActive(true);
    }
    else if (strcmp(argumentParser.arguments[0], "period") == 0) {
        uint32_t pwmPeriod = strtoul(argumentParser.arguments[1], NULL, 10);
        setPwmPeriod(pwmPeriod);
        setPwmControlPwmPeriod(pwmPeriod);
        debugOut("PWM Period is set");
    }
    else if (strcmp(argumentParser.arguments[0], "large") == 0) {
        uint32_t largeDutyCycle = strtoul(argumentParser.arguments[1], NULL, 10);
        setLargeDutyCycle(largeDutyCycle);
        setPwmControlLargeDutyCycle(largeDutyCycle);
        debugOut("Large PWM duty cycle is set");
    }
    else if (strcmp(argumentParser.arguments[0], "small") == 0) {
        uint32_t smallDutyCycle = strtoul(argumentParser.arguments[1], NULL, 10);
        setSmallDutyCycle(smallDutyCycle);
        setPwmControlSmallDutyCycle(smallDutyCycle);
        debugOut("Small PWM duty cycle is set");
    }
    else if (strcmp(argumentParser.arguments[0], "delay") == 0) {
        uint32_t period = strtoul(argumentParser.arguments[1], NULL, 10);
        setChangePwmPeriod(period);
        debugOut("Delay is set");
    }
    else if (strcmp(argumentParser.arguments[0], "show") == 0) {
        char str[100];
        getStrSettings(str);
        debugOut(str);
    }
    else if (strcmp(argumentParser.arguments[0], "help") == 0) {
        debugOut(getHelp());
    }
    else if (strcmp(argumentParser.arguments[0], "save") == 0) {

        if (saveAppSettings()) {
            debugOut("Settings is saved");
        }
        else {
            debugOut("Save error!");
        }
    }
    else {
        const char *str = "Input error";
        debugOut(str);
    }
}

static void setPwmControlActive(bool active)
{
    if (pwmControl.isActive == active) {
        return;
    }

    pwmControl.isActive = active;

    if (active) {
        // Установка начального состояния
        pwmControl.state = PwmControlState_Small;

        setPwmControlSmallDutyCycle(pwmControl.smallDutyCycle);
        Board_SetPwmActive(true);
        LAME_SoftTimer_Start(&pwmControl.pwmChangeTimer);

        LAME_Led_SetBlinkCount(led, LedState_PWM_Enable);

        char str[50];
        sprintf(str, "%i: PWM Small T=%lumcs, DC=%lumcs", pwmControl.cycleNumber, pwmControl.pwmPeriod, pwmControl.smallDutyCycle);
        debugOut(str);
    }
    else {
        Board_SetPwmActive(false);
        LAME_SoftTimer_Stop(&pwmControl.pwmChangeTimer);

        LAME_Led_SetBlinkCount(led, LedState_PWM_Disable);
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
    pwmControl.cycleNumber = 1;
    LAME_SoftTimer_Init(&pwmControl.pwmChangeTimer, LAME_SoftTimer_ModePeriodic, appSettings.pwmChangePeriod);

    setPwmControlPwmPeriod(appSettings.pwmPeriod);
    setPwmControlLargeDutyCycle(appSettings.largeDutyCycle);
    setPwmControlSmallDutyCycle(appSettings.smallDutyCycle);
    setPwmControlChangeTimer(appSettings.pwmChangePeriod);

    setPwmControlActive(true);
}

static void pwmControlTask()
{

    if (!LAME_SoftTimer_Occur(&pwmControl.pwmChangeTimer)) {
        return;
    }

    char str[50];

    if (pwmControl.state == PwmControlState_Large) {
        pwmControl.state = PwmControlState_Small;
        Board_SetPwmFront(pwmControl.smallDutyCycle);

        sprintf(str, "%i: PWM Small T=%lumcs, DC=%lumcs", pwmControl.cycleNumber, pwmControl.pwmPeriod, pwmControl.smallDutyCycle);
    }
    else {
        pwmControl.state = PwmControlState_Large;
        Board_SetPwmFront(pwmControl.largeDutyCycle);

        sprintf(str, "%i: PWM Large T=%lumcs, DC=%lumcs", pwmControl.cycleNumber, pwmControl.pwmPeriod, pwmControl.largeDutyCycle);

        pwmControl.cycleNumber++;
    }
    debugOut(str);
}

int main()
{
    LAME_Event_Init(&key_event);

    init();
    initAppSetting();

    uint32_t timeout = LOW_SPEED_BLINK_TIMEOUT;

    pwmControlInit();

    while (1) {
        if (LAME_Event_Take(&key_event)) {
            timeout = (timeout == LOW_SPEED_BLINK_TIMEOUT) ? HIGH_SPEED_BLINK_TIMEOUT : LOW_SPEED_BLINK_TIMEOUT;
        }

        tud_task(); // tinyusb device task
        cdc_task();

        processCommandTask();
        pwmControlTask();
        LAME_Led_Task();

        static volatile bool needWrite = false;
        if (needWrite) {
            needWrite = false;
            saveAppSettings();
        }

        static volatile ApSettings appSettingsTemp;
        Board_FlashRead((uint32_t *)&appSettingsTemp, sizeof(appSettingsTemp));
    }
}
