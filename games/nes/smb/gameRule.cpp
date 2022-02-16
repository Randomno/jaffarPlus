#include "gameRule.hpp"

GameRule::GameRule() : Rule()
{
 _magnets.marioScreenOffsetMagnet = magnet_t { .intensity = 0.0f, .min = 0.0f, .max = 0.0f };
 _magnets.marioHorizontalMagnet = magnet_t { .intensity = 0.0f, .min = 0.0f, .max = 0.0f };
 _magnets.marioVerticalMagnet = magnet_t { .intensity = 0.0f, .min = 0.0f, .max = 0.0f };
}

bool GameRule::parseGameAction(nlohmann::json actionJs, size_t actionId)
{
  bool recognizedActionType = false;
  std::string actionType = actionJs["Type"].get<std::string>();

  if (actionType == "Set Mario Screen Offset Magnet")
  {
    if (isDefined(actionJs, "Intensity") == false) EXIT_WITH_ERROR("[ERROR] Magnet in Rule %lu Action %lu missing 'Intensity' key.\n", _label, actionId);
    if (isDefined(actionJs, "Min") == false) EXIT_WITH_ERROR("[ERROR] Magnet in Rule %lu Action %lu missing 'Min' key.\n", _label, actionId);
    if (isDefined(actionJs, "Max") == false) EXIT_WITH_ERROR("[ERROR] Magnet in Rule %lu Action %lu missing 'Max' key.\n", _label, actionId);
    _magnets.marioScreenOffsetMagnet = magnet_t { .intensity = actionJs["Intensity"].get<float>(), .min = actionJs["Min"].get<float>(), .max = actionJs["Max"].get<float>() };
    recognizedActionType = true;
  }

  if (actionType == "Set Mario Horizontal Magnet")
  {
   if (isDefined(actionJs, "Intensity") == false) EXIT_WITH_ERROR("[ERROR] Magnet in Rule %lu Action %lu missing 'Intensity' key.\n", _label, actionId);
   if (isDefined(actionJs, "Min") == false) EXIT_WITH_ERROR("[ERROR] Magnet in Rule %lu Action %lu missing 'Min' key.\n", _label, actionId);
   if (isDefined(actionJs, "Max") == false) EXIT_WITH_ERROR("[ERROR] Magnet in Rule %lu Action %lu missing 'Max' key.\n", _label, actionId);
   _magnets.marioHorizontalMagnet = magnet_t { .intensity = actionJs["Intensity"].get<float>(), .min = actionJs["Min"].get<float>(), .max = actionJs["Max"].get<float>() };
    recognizedActionType = true;
  }

  if (actionType == "Set Mario Vertical Magnet")
  {
   if (isDefined(actionJs, "Intensity") == false) EXIT_WITH_ERROR("[ERROR] Magnet in Rule %lu Action %lu missing 'Intensity' key.\n", _label, actionId);
   if (isDefined(actionJs, "Min") == false) EXIT_WITH_ERROR("[ERROR] Magnet in Rule %lu Action %lu missing 'Min' key.\n", _label, actionId);
   if (isDefined(actionJs, "Max") == false) EXIT_WITH_ERROR("[ERROR] Magnet in Rule %lu Action %lu missing 'Max' key.\n", _label, actionId);
   _magnets.marioVerticalMagnet = magnet_t { .intensity = actionJs["Intensity"].get<float>(), .min = actionJs["Min"].get<float>(), .max = actionJs["Max"].get<float>() };
    recognizedActionType = true;
  }

  return recognizedActionType;
}

datatype_t GameRule::getPropertyType(const std::string &property)
{
  if (property == "Mario State") return dt_uint8;
  if (property == "Mario Animation") return dt_uint8;
  if (property == "Mario Walking Frame") return dt_uint8;
  if (property == "Mario Walking Mode") return dt_uint8;
  if (property == "Mario Floating Mode") return dt_uint8;

  if (property == "Screen Position X") return dt_uint16;

  if (property == "Mario Position Y") return dt_uint8;
  if (property == "Mario Velocity X") return dt_int8;

  if (property == "Mario Base Position X") return dt_uint8;
  if (property == "Mario Relative Position X") return dt_uint8;
  if (property == "Mario Position X") return dt_uint16;

  if (property == "Current World") return dt_uint8;
  if (property == "Current Stage") return dt_uint8;
  if (property == "Current Screen") return dt_uint8;
  if (property == "Level Entry Flag") return dt_uint8;
  if (property == "Game Mode") return dt_uint8;
  if (property == "Warp Area Offset") return dt_uint16;
  if (property == "Enemy 1 Type") return dt_uint8;
  if (property == "Enemy 2 Type") return dt_uint8;
  if (property == "Enemy 3 Type") return dt_uint8;
  if (property == "Enemy 4 Type") return dt_uint8;
  if (property == "Enemy 5 Type") return dt_uint8;
  if (property == "Mario Screen Offset") return dt_int16;

  EXIT_WITH_ERROR("[Error] Rule %lu, unrecognized property: %s\n", _label, property.c_str());

  return dt_uint8;
}

void* GameRule::getPropertyPointer(const std::string &property, GameInstance* gameInstance)
{
  if (property == "Mario State") return gameInstance->marioState;
  if (property == "Mario Animation") return gameInstance->marioAnimation;
  if (property == "Mario Walking Frame") return gameInstance->marioWalkingFrame;
  if (property == "Mario Walking Mode") return gameInstance->marioWalkingMode;
  if (property == "Mario Floating Mode") return gameInstance->marioFloatingMode;

  if (property == "Screen Position X") return &gameInstance->screenPosX; // Derivative value

  if (property == "Mario Base Position X") return gameInstance->marioBasePosX;
  if (property == "Mario Relative Position X") return gameInstance->marioRelPosX;
  if (property == "Mario Position X") return &gameInstance->marioPosX; // Derivative value

  if (property == "Mario Position Y") return gameInstance->marioPosY;
  if (property == "Mario Velocity X") return gameInstance->marioVelX;
  if (property == "Current World") return &gameInstance->currentWorld; // Derivative value
  if (property == "Current Stage") return &gameInstance->currentStage; // Derivative value
  if (property == "Current Screen") return gameInstance->screenBasePosX;
  if (property == "Level Entry Flag") return gameInstance->levelEntryFlag;
  if (property == "Game Mode") return gameInstance->gameMode;
  if (property == "Warp Area Offset") return gameInstance->warpAreaOffset;
  if (property == "Enemy 1 Type") return gameInstance->enemy1Type;
  if (property == "Enemy 2 Type") return gameInstance->enemy2Type;
  if (property == "Enemy 3 Type") return gameInstance->enemy3Type;
  if (property == "Enemy 4 Type") return gameInstance->enemy4Type;
  if (property == "Enemy 5 Type") return gameInstance->enemy5Type;
  if (property == "Mario Screen Offset") return &gameInstance->marioScreenOffset; // Derivative value

  EXIT_WITH_ERROR("[Error] Rule %lu, unrecognized property: %s\n", _label, property.c_str());

  return NULL;
}
