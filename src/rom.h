#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t entry_point[4];
    uint8_t nintendo_logo[48];
    union {
        char name_old_format[16];
        struct
        {
            char name_new_format[11];
            uint8_t manufacturer_code[4]; // This can't be a uint32_t or it will add padding and mess up the size
            uint8_t color_support;
        };
    };
    uint16_t new_licensee_code;
    uint8_t super_game_boy_support;
    uint8_t cart_hardware_flags;
    uint8_t rom_size;
    uint8_t ram_size;
    uint8_t region;
    uint8_t old_licensee_code; // 0x33 means the new code should be used instead
    uint8_t game_version;
    uint8_t header_checksum;
    uint16_t global_checksum;
}cart_header;

typedef enum
{
    ROM_ONLY = 0x0,
    MBC1_ONLY = 0x1,
    MBC1_RAM = 0x2,
    MBC1_RAM_BATTERY = 0x3,
    MBC2_ONLY = 0x5,
    MBC2_BATTERY = 0x6,
    ROM_RAM = 0x8,
    ROM_RAM_BATTERY = 0x9,
    MMM01_ONLY = 0xB,
    MMM01_RAM = 0xC,
    MMM01_RAM_BATTERY = 0xD,
    MBC3_TIMER_BATTERY = 0xF,
    MBC3_TIMER_RAM_BATTERY = 0x10,
    MBC3_ONLY = 0x11,
    MBC3_RAM = 0x12,
    MBC3_RAM_BATTERY = 0x13,
    MBC5_ONLY = 0x19,
    MBC5_RAM = 0x1A,
    MBC5_RAM_BATTERY = 0x1B,
    MBC5_RUMBLE = 0x1C,
    MBC5_RUMBLE_RAM = 0x1D,
    MBC5_RUMBLE_RAM_BATTERY = 0x1E,
    MBC6_ONLY = 0x20,
    MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
    POCKET_CAMERA = 0xFC,
    BANDAI_TAMA5 = 0xFD,
    HuC3_ONLY = 0xFE,
    HuC1_RAM_BATTERY = 0xFF
}cart_hardware;

typedef struct
{
    bool has_ram: 1;
    bool has_battery: 1;
    bool has_timer: 1;
    bool has_rumble: 1;
    bool has_camera: 1;
    bool has_sensor: 1;
    bool MBC1: 1;
    bool MBC2: 1;
    bool MBC3: 1;
    bool MBC5: 1;
    bool MBC6: 1;
    bool MBC7: 1;
    bool MMM01: 1;
    bool BANDAI_TAMA5: 1;
    bool HuC3: 1;
}hardware_flags;

uint8_t ram_bank_count(cart_header* cart);
hardware_flags get_cart_hardware(cart_header* cart);
void print_rom_info(cart_header* rom_header);
