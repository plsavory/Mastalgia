/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

/*
Master System memory map:
0x0000-0x3FFF : Rom Slot 1
0x4000-0x7FFF : Rom Slot 2
0x8000-0xBFFF : Rom Slot 3 / RAM Slot
0xC000-0xDFFF : RAM
0xE000-0xFFFF : Mirrored RAM
 */
#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "Utils.h"

Memory::Memory(Cartridge *cart)
{
  smsCartridge = cart;

  // Clear memory
  for (int i = 0; i<0x4000;i++) {
    ram[i] = 0x0;
  }

  // Clear RAM banks
  for (int bank = 0; bank < 2; bank++) {
    for (int i = 0; i<0x4000;i++) {
      ramBank[bank][i] = 0x0;
    }
  }

  for (int i = 0; i < 3; i++) {
    memoryPages[i] = 0x0;
  }

  ramBanked = false;
  currentRamBank = 0;
}

Memory::~Memory()
{

}

/**
 * [read Read from system memory]
 * @param  location [Memory location to read from]
 * @return          [Value within memory at location]
 */
unsigned char Memory::read(unsigned short location)
{
  // Handle mirroring
  if (location >= 0xFFFC) {
    location-=0x2000;
  } else if (location < 0x4000) {
    // Page 1
    unsigned short address = location + (0x4000 * memoryPages[0]);
    return smsCartridge->read(address);
  } else if (location < 0x8000) {
    // Page 2
    unsigned short address = location + (0x4000 * memoryPages[1]);
    address-=0x4000; // Remove offset
    return smsCartridge->read(address);
  } else if (location < 0xC000) {
    // Page 3 (or extra RAM)
    if (ramBanked) {
      return ramBank[currentRamBank][location-0x8000];
    } else {
      unsigned short address = location + (0x4000 * memoryPages[2]);
      address-=0x8000;
      return smsCartridge->read(address);
    }
  }

  return ram[location];
}

/**
 * [write Writes to system memory]
 * @param location [Memory location to write to]
 * @param value    [The byte to write]
 */
void Memory::write(unsigned short location, unsigned char value)
{
  if (smsCartridge->isCodemasters()) {
    if (location == 0x0 || location == 0x4000 || location == 0x8000) {
      memoryPage(true,location,value);
    }
  }

  if (location < 0x8000) {
    // Attempting to write to ROM, disallow this...
    return;
  }
  else if (location < 0xC000) {
    // Allow a write if RAM is banked into this address range
    if (ramBanked) {
      ramBank[currentRamBank][location-0x8000] = value;
    } else {
      return;
    }
  }

  // If we've reached this point, it's all good to write to RAM
  ram[location] = value;

  // Handle mirrored addresses
  if (location >= 0xC000 && location < 0xDFFC) {
    ram[location-0x2000] = value;
  }

  if (location >= 0xE000) {
    ram[location-0x2000] = value;
  }

}

/**
 * [Memory::memoryPage Perform a memory page operation]
 * @param Codemasters [Whether the game is codemasters or not]
 */
void Memory::memoryPage(bool Codemasters, unsigned short location, unsigned char value)
{
  if (Codemasters) {
    unsigned short page = smsCartridge->isMegCartridge()?value & 0x3F: value &0x1F;

    switch(location)  {
      case 0x0:
      memoryPages[0] = page;
      break;
      case 0x4000:
      memoryPages[1] = page;
      break;
      case 0x8000:
      memoryPages[2] = page;
      break;
      default:
      break;
    }

  } else {
    unsigned short page = smsCartridge->isMegCartridge()?value & 0x3F: value & 0x1F;

    switch(location) {
      case 0xFFFC:
      if (Utils::testBit(3, value)) {
        if (Utils::testBit(2,value)) {
          currentRamBank = 1;
        } else {
          currentRamBank = 0;
        }
      } else {
        ramBanked = false;
      }
      break;
      case 0xFFFD:
      memoryPages[0] = page;
      break;
      case 0xFFFE:
      memoryPages[1] = page;
      break;
      case 0xFFFF:
      // ROM banking in slot 3 - Only allow if there is no RAM there
      if (!Utils::testBit(3,ram[0xFFFC])) {
        memoryPages[2] = page;
      }
      break;
      default:
      break;
    }
  }
}
