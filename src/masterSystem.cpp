/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "MasterSystem.h"

MasterSystem::MasterSystem()
{
  // Initialize the Emulator

  #ifdef VERBOSE_MODE
  std::cout<<"Initializing Emulator..."<<std::endl;
  #endif

  smsCartridge = new Cartridge();
  smsMemory = new Memory();
  smsCPU = new CPUZ80();
  smsVdp = new VDP();
}

MasterSystem::~MasterSystem()
{

}

bool MasterSystem::start(std::string romFilename)
{
  // TODO: Load a ROM into memory

  // Enter the main loop
  run();

  return true;
}

void MasterSystem::tick()
{

int z80ClockCycles = smsCPU->execute();

int machineClicks = z80ClockCycles*3;

float vdpCycles = machineClicks/2;

int soundCycles = z80ClockCycles;

smsVdp->execute(vdpCycles);

// TODO: Update sound when it exists

  return;
}

void MasterSystem::run()
{
  // TODO: Implement this
}
