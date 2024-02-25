#include <jaffarCommon/include/json.hpp>
#include <jaffarCommon/extern/argparse/argparse.hpp>
#include <jaffarCommon/include/logger.hpp>
#include <jaffarCommon/include/string.hpp>
#include "driver.hpp"

int main(int argc, char *argv[])
{
 // Parsing command line arguments
  argparse::ArgumentParser program("jaffar", "1.0");

  program.add_argument("configFile")
    .help("path to the Jaffar configuration script (.jaffar) file to run.")
    .required();

  // Try to parse arguments
  try { program.parse_args(argc, argv);  }
  catch (const std::runtime_error &err) { EXIT_WITH_ERROR("%s\n%s", err.what(), program.help().str().c_str()); }

  // Getting config file name
  const std::string configFile = program.get<std::string>("configFile");

  // Reporting script file
  LOG("[J++] Loading script file: '%s'\n", configFile.c_str());

  // Loading script file contents
  std::string configFileString;
  if (jaffarCommon::loadStringFromFile(configFileString, configFile) == false) 
    EXIT_WITH_ERROR("[ERROR] Could not find or read from Jaffar config file: %s\n%s \n", configFile.c_str(), program.help().str().c_str());

  // Parsing JSON from script file
  nlohmann::json config;
  try { config = nlohmann::json::parse(configFileString); }
  catch (const std::exception &err) { EXIT_WITH_ERROR("[ERROR] Parsing configuration file %s. Details:\n%s\n", configFile.c_str(), err.what()); }

  // Creating driver to run the Jaffar engine
  auto d = jaffarPlus::Driver::getDriver(config);

  // Running driver
  d->run();
}

