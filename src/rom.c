#include <string.h>
#include <stdint.h>

#include <logging.h>

#include "rom.h"

static const char nintendo_logo[] = {
        0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
};

uint8_t ram_bank_count(cart_header* cart) {
    switch (cart->ram_size) {
        case 2:
            return 1;
        case 3:
            return 4;
        case 4:
            return 16;
        case 5:
            return 8;
        default:
            return 0;
    }
}

hardware_flags get_cart_hardware(cart_header* cart) {
    hardware_flags output = {0};
    switch (cart->cart_hardware_flags) {
        case ROM_ONLY:
            break;
        case MBC1_ONLY:
            output.MBC1 = true;
            break;
        case MBC1_RAM:
            output.MBC1 = true;
            output.has_ram = true;
            break;
        case MBC1_RAM_BATTERY:
            output.MBC1 = true;
            output.has_ram = true;
            output.has_battery = true;
            break;
        case MBC2_ONLY:
            output.MBC2 = true;
        case MBC2_BATTERY:
            output.MBC2 = true;
            output.has_battery = true;
        case ROM_RAM:
            output.has_ram = true;
        case ROM_RAM_BATTERY:
            output.has_ram = true;
            output.has_battery = true;
        case MMM01_ONLY:
            output.MMM01 = true;
        case MMM01_RAM:
            output.MMM01 = true;
            output.has_ram = true;
        case MMM01_RAM_BATTERY:
            output.MMM01 = true;
            output.has_ram = true;
            output.has_battery = true;
        case MBC3_TIMER_BATTERY:
            output.MBC3 = true;
            output.has_timer = true;
            output.has_battery = true;
        case MBC3_TIMER_RAM_BATTERY:
            output.MBC3 = true;
            output.has_ram = true;
            output.has_timer = true;
            output.has_battery = true;
        case MBC3_ONLY:
            output.MBC3 = true;
        case MBC3_RAM:
            output.MBC3 = true;
            output.has_ram = true;
        case MBC3_RAM_BATTERY:
            output.MBC3 = true;
            output.has_ram = true;
            output.has_battery = true;
        case MBC5_ONLY:
            output.MBC5 = true;
        case MBC5_RAM:
            output.MBC5 = true;
            output.has_ram = true;
        case MBC5_RAM_BATTERY:
            output.MBC5 = true;
            output.has_battery = true;
            output.has_ram = true;
        case MBC5_RUMBLE:
            output.MBC5 = true;
            output.has_rumble = true;
        case MBC5_RUMBLE_RAM:
            output.MBC5 = true;
            output.has_rumble = true;
            output.has_ram = true;
        case MBC5_RUMBLE_RAM_BATTERY:
            output.MBC5 = true;
            output.has_rumble = true;
            output.has_ram = true;
            output.has_battery = true;
        case MBC6_ONLY:
            output.MBC6 = true;
        case MBC7_SENSOR_RUMBLE_RAM_BATTERY:
            output.MBC7 = true;
            output.has_sensor = true;
            output.has_rumble = true;
            output.has_ram = true;
            output.has_battery = true;
        case POCKET_CAMERA:
            output.has_camera = true;
        case BANDAI_TAMA5:
            output.BANDAI_TAMA5 = true;
        case HuC3_ONLY:
            output.HuC3 = true;
        case HuC1_RAM_BATTERY:
            output.HuC3 = true;
            output.has_ram = true;
            output.has_battery = true;
        default:
            break;
    }
    return output;
}

void print_rom_info(cart_header* rom_header) {
    if (memcmp(&nintendo_logo, rom_header->nintendo_logo, sizeof(nintendo_logo)) != 0) {
        log_error(WARNING, "Failed Nintendo logo check, proceeding anyway\n");
    }
    // In older games, 16 bytes were used for the ROM name. In newer games, only 11 bytes are available (adding 1 to account for null terminator)
    if (strlen(rom_header->name_old_format) + 1 > 11) {
        log_error(INFO, "ROM Name: %s\n", rom_header->name_old_format);
    }
    else {
        log_error(INFO, "ROM Name: %s\n", rom_header->name_new_format);
        log_error(INFO, "Publisher Code: %c%c%c%c\n", rom_header->manufacturer_code[0], rom_header->manufacturer_code[1], rom_header->manufacturer_code[2], rom_header->manufacturer_code[3]);
        // TODO: Detect PGB Mode (https://gbdev.io/pandocs/The_Cartridge_Header.html#0143--cgb-flag)
        if (rom_header->color_support == 0x80 || rom_header->color_support == 0xC0) {
            log_error(INFO, "This game supports color enhancements, but is compatible with an original Game Boy.\n");
        }
        else {
            log_error(WARNING, "Unimplemented: Console should be in \"PGB\" mode (0x%x)\n", rom_header->color_support);
        }
    }
    if (rom_header->rom_size < 9) {
        uint32_t rom_size = 2 * (1 << rom_header->rom_size);
        log_error(INFO, "ROM Size: %d banks (%dKiB)\n", rom_size, rom_size * 16);
    }
    if (rom_header->ram_size != 0 && rom_header->ram_size != 1) {
        uint8_t ram_banks = ram_bank_count(rom_header);
        log_error(INFO, "Cart has %d 8KiB RAM bank(s) (%dKiB total)\n", ram_banks, ram_banks * 8);
    }
    else {
        log_error(INFO, "Cart has no extra RAM.\n");
    }

    log_error(INFO, "Card Hardware code: 0x%x\n", rom_header->cart_hardware_flags);

    if (rom_header->region == 0) {
        log_error(INFO, "ROM Region: World\n");
    }
    else {
        log_error(INFO, "ROM Region: Overseas only\n");
    }
    log_error(INFO, "Game Version: %d\n", rom_header->game_version);
}

