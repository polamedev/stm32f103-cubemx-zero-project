#include <cubemx.h>
#include <st_hal.h>

#define BOARD_GET_UNIQUE_ID 1

size_t board_get_unique_id(uint8_t id[], size_t max_len);

// Get USB Serial number string from unique ID if available. Return number of character.
// Input is string descriptor from index 1 (index 0 is type + len)
size_t board_usb_get_serial(uint16_t desc_str1[], size_t max_chars)
{
    uint8_t uid[16];
    size_t  uid_len;

    // TODO work with make, but not working with esp32s3 cmake
    if (BOARD_GET_UNIQUE_ID) {
        uid_len = board_get_unique_id(uid, sizeof(uid));
    }
    else {
        // fixed serial string is 01234567889ABCDEF
        uint32_t *uid32 = (uint32_t *)(uintptr_t)uid;
        uid32[0]        = 0x67452301;
        uid32[1]        = 0xEFCDAB89;
        uid_len         = 8;
    }

    if (uid_len > max_chars / 2)
        uid_len = max_chars / 2;

    for (size_t i = 0; i < uid_len; i++) {
        for (size_t j = 0; j < 2; j++) {
            const char nibble_to_hex[16] = {
                '0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
            const uint8_t nibble       = (uid[i] >> (j * 4)) & 0xf;
            desc_str1[i * 2 + (1 - j)] = nibble_to_hex[nibble]; // UTF-16-LE
        }
    }

    return 2 * uid_len;
}

size_t board_get_unique_id(uint8_t id[], size_t max_len)
{
    (void)max_len;
    volatile uint32_t *stm32_uuid = (volatile uint32_t *)UID_BASE;
    uint32_t          *id32       = (uint32_t *)(uintptr_t)id;
    const uint8_t      len        = 12;

    id32[0] = stm32_uuid[0];
    id32[1] = stm32_uuid[1];
    id32[2] = stm32_uuid[2];

    return len;
}
