#include <chrono>
#include <jaffarCommon/include/json.hpp>
#include <jaffarCommon/extern/argparse/argparse.hpp>
#include <jaffarCommon/include/logger.hpp>
#include <jaffarCommon/include/string.hpp>
#include "../emulators/emulatorList.hpp"
#include "../games/gameList.hpp"
#include "game.hpp"
#include "emulator.hpp"
#include "runner.hpp"
#include "playback.hpp"

int main(int argc, char *argv[])
{
 // Parsing command line arguments
  argparse::ArgumentParser program("jaffar-tester", "1.0");

  program.add_argument("configFile")
    .help("path to the Jaffar configuration script (.jaffar) file to run.")
    .required();

  program.add_argument("solutionFile")
    .help("path to the solution sequence file (.sol) to reproduce.")
    .required();

  program.add_argument("--reproduce")
    .help("Plays the entire sequence without interruptions and exit at the end.")
    .default_value(false)
    .implicit_value(true);

  program.add_argument("--disableRender")
    .help("Do not render game window.")
    .default_value(false)
    .implicit_value(true);
    
  // Try to parse arguments
  try { program.parse_args(argc, argv);  }
  catch (const std::runtime_error &err) { EXIT_WITH_ERROR("%s\n%s", err.what(), program.help().str().c_str()); }

  // Parsing config file
  const std::string configFile = program.get<std::string>("configFile");
  std::string configFileString;
  if (jaffarCommon::loadStringFromFile(configFileString, configFile) == false) 
    EXIT_WITH_ERROR("[ERROR] Could not find or read from Jaffar config file: %s\n%s \n", configFile.c_str(), program.help().str().c_str());

  // If sequence file defined, load it and play it
  const std::string solutionFile = program.get<std::string>("solutionFile");
  std::string solutionFileString;
  if (jaffarCommon::loadStringFromFile(solutionFileString, solutionFile) == false) 
    EXIT_WITH_ERROR("[ERROR] Could not find or read from solution sequence file: %s\n%s \n", solutionFile.c_str(), program.help().str().c_str());

  // Getting reproduce flag
  bool isReproduce = program.get<bool>("--reproduce");
  
  // Getting reproduce flag
  bool disableRender = program.get<bool>("--disableRender");

  // Parsing configuration file
  nlohmann::json config;
  try { config = nlohmann::json::parse(configFileString); }
  catch (const std::exception &err) { EXIT_WITH_ERROR("[ERROR] Parsing configuration file %s. Details:\n%s\n", configFile.c_str(), err.what()); }

  // Getting initial state file from the configuration
  const auto initialStateFilePath = JSON_GET_STRING(config, "Initial State File Path");

  // Getting emulator name from the configuration
  const auto emulatorName = JSON_GET_STRING(config, "Emulator");

// Getting game name from the configuration
  const auto gameName = JSON_GET_STRING(config, "Game");

  // Getting emulator from its name and configuring it
  auto e = jaffarPlus::Emulator::getEmulator(emulatorName, JSON_GET_OBJECT(config, "Emulator Configuration"));

  // Initializing emulator
  e->initialize();

  // Enabling rendering
  e->enableRendering();
  
  // If initial state file defined, load it
  if (initialStateFilePath.empty() == false) e->loadStateFile(initialStateFilePath);

  // Getting game from its name and configuring it
  auto g = jaffarPlus::Game::getGame(gameName, e, JSON_GET_OBJECT(config, "Game Configuration"));

  // Initializing game
  g->initialize();

  // Parsing script rules
  g->parseRules(JSON_GET_ARRAY(config, "Rules"));

  // Creating runner from game instance
  jaffarPlus::Runner r(g, JSON_GET_OBJECT(config, "Runner Configuration"));

  // Parsing Possible game inputs
  r.parseGameInputs(JSON_GET_ARRAY(config, "Game Inputs"));

  // Getting game state size
  const auto stateSize = r.getStateSize();

  // Getting input sequence
  const auto solutionSequence = jaffarCommon::split(solutionFileString, ' ');

  // Variable for current step in view
  ssize_t currentStep = 0;

  // Getting sequence length
  const ssize_t sequenceLength = solutionSequence.size();

  // Initializing terminal
  jaffarCommon::initializeTerminal();

  // Printing information
  LOG("[J+] Emulator Name:                   '%s'\n", emulatorName.c_str());
  LOG("[J+] Config File:                     '%s'\n", configFile.c_str());
  LOG("[J+] Solution File:                   '%s'\n", solutionFile.c_str());
  LOG("[J+] State Size:                       %lu\n", stateSize);
  LOG("[J+] Sequence Length:                  %lu\n", sequenceLength);
  LOG("[J+] ********** Creating Playback Sequence **********\n");
  jaffarCommon::refreshTerminal();

  // Instantiating playback object
  jaffarPlus::Playback p(r, solutionSequence);

// Flag to display frame information
  bool showFrameInfo = true;

  // Flag to continue running playback
  bool continueRunning = true;

  // Interactive section
  while (continueRunning)
  {
    // Updating display
    if (disableRender == false) p.renderFrame(currentStep);

    // Getting input
    const auto &inputString = p.getStateInputString(currentStep);

    // Getting input
    const auto &inputIndex = p.getStateInputIndex(currentStep);

    // Getting state hash
    const auto hash = p.getStateHash(currentStep);

    // Getting state data
    const auto stateData = p.getStateData(currentStep);

    // Printing data and commands
    if (showFrameInfo)
    {
      jaffarCommon::clearTerminal();

      LOG("[] ----------------------------------------------------------------\n");
      LOG("[] Current Step #: %lu / %lu\n", currentStep + 1, sequenceLength);
      LOG("[] Input:          %s (0x%X)\n", inputString.c_str(), inputIndex);
      LOG("[] State Hash:     0x%lX%lX\n", hash.first, hash.second);

      // Only print commands if not in reproduce mode
      if (isReproduce == false) LOG("[] Commands: n: -1 m: +1 | h: -10 | j: +10 | y: -100 | u: +100 | k: -1000 | i: +1000 | s: quicksave | p: play | q: quit\n");

      jaffarCommon::refreshTerminal();
    }

    // Resetting show frame info flag
    showFrameInfo = true;

    // Get command
    auto command = jaffarCommon::getKeyPress();

    // Advance/Rewind commands
    if (command == 'n') currentStep = currentStep - 1;
    if (command == 'm') currentStep = currentStep + 1;
    if (command == 'h') currentStep = currentStep - 10;
    if (command == 'j') currentStep = currentStep + 10;
    if (command == 'y') currentStep = currentStep - 100;
    if (command == 'u') currentStep = currentStep + 100;
    if (command == 'k') currentStep = currentStep - 1000;
    if (command == 'i') currentStep = currentStep + 1000;

    // Correct current step if requested more than possible
    if (currentStep < 0) currentStep = 0;
    if (currentStep > sequenceLength) currentStep = sequenceLength;

    // Quicksave creation command
    if (command == 's')
    {
      // Storing state file
      std::string saveFileName = "quicksave.state";

      std::string saveData;
      saveData.resize(stateSize);
      memcpy(saveData.data(), stateData, stateSize);
      if (jaffarCommon::saveStringToFile(saveData, saveFileName.c_str()) == false) EXIT_WITH_ERROR("[ERROR] Could not save state file: %s\n", saveFileName.c_str());
      LOG("[] Saved state to %s\n", saveFileName.c_str());

      // Do no show frame info again after this action
      showFrameInfo = false;
    }

    // Start playback from current point
    if (command == 'p') isReproduce = true;

    // Start playback from current point
    if (command == 'q') continueRunning = false;
  }

  // Ending ncurses window
  jaffarCommon::finalizeTerminal();
}
