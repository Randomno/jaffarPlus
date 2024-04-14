#pragma once

#include <emulator.hpp>
#include "quickerNES/quickerNES.hpp"
#include "quickerSDLPoP/quickerSDLPoP.hpp"
#include "quickerSnes9x/quickerSnes9x.hpp"
#include "quickerGPGX/quickerGPGX.hpp"

namespace jaffarPlus
{
#define DETECT_EMULATOR(EMULATOR)                                                                                                                                                  \
  if (emulatorName == EMULATOR::getName())                                                                                                                                         \
  {                                                                                                                                                                                \
    e            = std::make_unique<EMULATOR>(emulatorConfig);                                                                                                                     \
    isRecognized = true;                                                                                                                                                           \
  }

std::unique_ptr<Emulator> Emulator::getEmulator(const nlohmann::json &emulatorConfig)
{
  // Base pointer for the emulator
  std::unique_ptr<Emulator> e;

  // Flag to indicate whether the name was recognized
  bool isRecognized = false;

  // Getting emulator name
  const auto &emulatorName = jaffarCommon::json::getString(emulatorConfig, "Emulator Name");

  // Detecting emulator
  DETECT_EMULATOR(emulator::QuickerNES);
  DETECT_EMULATOR(emulator::QuickerSDLPoP);
  DETECT_EMULATOR(emulator::QuickerSnes9x);
  DETECT_EMULATOR(emulator::QuickerGPGX);

  // Check if recognized
  if (isRecognized == false) JAFFAR_THROW_LOGIC("Emulator '%s' not recognized\n", emulatorName.c_str());

  // Returning emulator pointer
  return e;
}

} // namespace jaffarPlus
