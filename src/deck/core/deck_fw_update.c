/**
 * ,---------,       ____  _ __
 * |  ,-^-,  |      / __ )(_) /_______________ _____  ___
 * | (  O  ) |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * | / ,--´  |    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *    +------`   /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2021 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * deck_fw_update.c - implements functionality for updating firmware
 *                    and binary data in decks.
 *
 */

#include <string.h>
#include "deck_fw_update.h"
#include "deck_core.h"
#include "mem.h"
#include "test_support.h"


static const uint32_t DECK_FW_MAX_SIZE = 0x10000000;

static uint32_t handleMemGetSize(void) { return DECK_FW_MAX_SIZE * (DECK_MAX_COUNT + 1); }
TESTABLE_STATIC bool handleMemRead(const uint32_t memAddr, const uint8_t readLen, uint8_t* buffer);
TESTABLE_STATIC bool handleMemWrite(const uint32_t memAddr, const uint8_t writeLen, const uint8_t* buffer);
static const MemoryHandlerDef_t memoryDef = {
  .type = MEM_TYPE_DECK_MEM,
  .getSize = handleMemGetSize,
  .read = handleMemRead,
  .write = handleMemWrite,
};

static const uint8_t VERSION = 1;

static const int DECK_MEMORY_INFO_SIZE = 0x20;

static const int OFFS_BITFIELD = 0x00;
static const int OFFS_REQ_HASH = 0x01;
static const int OFFS_REQ_LEN = 0x05;
static const int OFFS_BASE_ADDR = 0x09;
static const int OFFS_NAME = 0x0D;

static const int NAME_LEN_EX_ZERO_TREM = 18;

static const uint8_t MASK_IS_VALID          = 1;
static const uint8_t MASK_IS_STARTED        = 2;
static const uint8_t MASK_SUPPORTS_READ     = 4;
static const uint8_t MASK_SUPPORTS_WRITE    = 8;
static const uint8_t MASK_SUPPORTS_UPGRADE  = 16;
static const uint8_t MASK_UPGRADE_REQUIRED  = 32;
static const uint8_t MASK_BOOTLOADER_ACTIVE = 64;

static uint8_t populateBitfield(const DeckMemDef_t* memDef) {
    uint8_t result = MASK_IS_VALID;

    const uint8_t properties = memDef->properties();
    if (properties & DECK_MEMORY_MASK_STARTED) {
        result |= MASK_IS_STARTED;
    }

    if (properties & DECK_MEMORY_MASK_UPGRADE_REQUIRED) {
        result |= MASK_UPGRADE_REQUIRED;
    }

    if (properties & DECK_MEMORY_MASK_BOOT_LOADER_ACTIVE) {
        result |= MASK_BOOTLOADER_ACTIVE;
    }

    if (memDef->read) {
        result |= MASK_SUPPORTS_READ;
    }

    if (memDef->write) {
        result |= MASK_SUPPORTS_WRITE;
    }

    if (memDef->supportsUpgrade) {
        result |= MASK_SUPPORTS_UPGRADE;
    }

    return result;
}

static void populateDeckFwInfo(uint8_t buffer[], const uint8_t deckNr) {
    DeckInfo* info = deckInfo(deckNr);
    memset(buffer, 0, DECK_MEMORY_INFO_SIZE);

    const DeckMemDef_t* deckMemDef = info->driver->memoryDef;
    if (deckMemDef) {
        uint32_t baseAddress = (deckNr + 1) * DECK_FW_MAX_SIZE;

        buffer[OFFS_BITFIELD] = populateBitfield(deckMemDef);
        memcpy(&buffer[OFFS_REQ_HASH], &deckMemDef->requiredHash, 4);
        memcpy(&buffer[OFFS_REQ_LEN], &deckMemDef->requiredSize, 4);
        memcpy(&buffer[OFFS_BASE_ADDR], &baseAddress, 4);
        strncpy((char*)&buffer[OFFS_NAME], info->driver->name, NAME_LEN_EX_ZERO_TREM);
    }
}

static bool handleInfoSectionRead(const uint32_t memAddr, const uint8_t readLen, uint8_t* buffer, const int nrOfDecks) {
    uint32_t index = 0;
    uint8_t bytesLeft = readLen;

    // First byte is the version
    if (0 == (memAddr + index) && bytesLeft > 0) {
        buffer[index] = VERSION;
        index++;
        bytesLeft--;
    }

    // Deck memory infos
    while (bytesLeft > 0) {
        int deckNr = (memAddr + index - 1) / DECK_MEMORY_INFO_SIZE;

        if (deckNr >= nrOfDecks) {
            break;
        }

        uint8_t fwInfo[DECK_MEMORY_INFO_SIZE];
        populateDeckFwInfo(fwInfo, deckNr);

        int startAddrOfThisInfo = deckNr * DECK_MEMORY_INFO_SIZE + 1;
        int firstByteToUse = memAddr + index - startAddrOfThisInfo;

        int bytesToUse = DECK_MEMORY_INFO_SIZE - firstByteToUse;
        if (bytesLeft < bytesToUse) {
            bytesToUse = bytesLeft;
        }

        memcpy(buffer + index, (uint8_t*)(&fwInfo) + firstByteToUse, bytesToUse);
        bytesLeft -= bytesToUse;
        index += bytesToUse;
    }

    return true;
}

static bool handleDeckSectionRead(const uint32_t memAddr, const uint8_t readLen, uint8_t* buffer, const int deckNr) {
    bool result = false;

    const DeckInfo* info = deckInfo(deckNr);
    const DeckMemDef_t* deckMemDef = info->driver->memoryDef;
    if (deckMemDef) {
        if (deckMemDef->read) {
            uint32_t baseAddress = (deckNr + 1) * DECK_FW_MAX_SIZE;
            uint32_t deckAddress = memAddr - baseAddress;
            result = deckMemDef->read(deckAddress, readLen, buffer);
        }
    }

    return result;
}

static bool handleDeckSectionWrite(const uint32_t memAddr, const uint8_t writeLen, const uint8_t* buffer, const int deckNr) {
    bool result = false;

    const DeckInfo* info = deckInfo(deckNr);
    const DeckMemDef_t* deckMemDef = info->driver->memoryDef;
    if (deckMemDef) {
        if (deckMemDef->write) {
            uint32_t baseAddress = (deckNr + 1) * DECK_FW_MAX_SIZE;
            uint32_t deckAddress = memAddr - baseAddress;
            result = deckMemDef->write(deckAddress, writeLen, buffer);
        }
    }

    return result;
}

TESTABLE_STATIC bool handleMemRead(const uint32_t memAddr, const uint8_t readLen, uint8_t* buffer) {
    bool result = false;
    int nrOfDecks = deckCount();

    // Assume the buffer is fully within one section. It may not be true, but unlikely
    // and not an important use case to support
    const uint32_t section = memAddr / DECK_FW_MAX_SIZE;
    if (section == 0) {
        result = handleInfoSectionRead(memAddr, readLen, buffer, nrOfDecks);
    } else {
        int deckNr = section - 1;
        if (deckNr < nrOfDecks) {
            result = handleDeckSectionRead(memAddr, readLen, buffer, deckNr);
        }
    }

    return result;
}

TESTABLE_STATIC bool handleMemWrite(const uint32_t memAddr, const uint8_t writeLen, const uint8_t* buffer) {
    bool result = false;

    // Assume the buffer is fully within one section. It may not be true, but unlikely
    // and not an important use case to support
    const uint32_t section = memAddr / DECK_FW_MAX_SIZE;
    if (section > 0) {
        int nrOfDecks = deckCount();
        int deckNr = section - 1;
        if (deckNr < nrOfDecks) {
            result = handleDeckSectionWrite(memAddr, writeLen, buffer, deckNr);
        }
    }

    return result;
}

void deckFwUpdateInit() {
    memoryRegisterHandler(&memoryDef);
}
