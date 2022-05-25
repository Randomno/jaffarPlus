#include "gameInstance.hpp"
#include "gameRule.hpp"

GameInstance::GameInstance(EmuInstance* emu, const nlohmann::json& config)
{
  // Setting emulator
  _emu = emu;

  frameCounter             = (uint8_t*)   &_emu->_baseMem[0x00BF];
  gameTime                 = (uint8_t*)   &_emu->_baseMem[0x0063];
  gameMode                 = (uint8_t*)   &_emu->_baseMem[0x002D];
  currentStage             = (uint8_t*)   &_emu->_baseMem[0x006D];
  currentSubStage          = (uint8_t*)   &_emu->_baseMem[0x006E];
  ninjalives               = (uint8_t*)   &_emu->_baseMem[0x0076];
  ninjaPower               = (uint8_t*)   &_emu->_baseMem[0x0064];
  ninjaHP                  = (uint8_t*)   &_emu->_baseMem[0x0065];
  bossHP                   = (uint8_t*)   &_emu->_baseMem[0x0066];
  ninjaStateFlags          = (uint8_t*)   &_emu->_baseMem[0x0084];
  ninjaIsDead              = (uint8_t*)   &_emu->_baseMem[0x008B];
  ninjaPosX                = (uint8_t*)   &_emu->_baseMem[0x0086];
  ninjaPosXFrac            = (uint8_t*)   &_emu->_baseMem[0x0085];
  ninjaSpeedX              = (uint8_t*)   &_emu->_baseMem[0x00AD];
  ninjaSpeedXFrac          = (uint8_t*)   &_emu->_baseMem[0x00AC];
  ninjaPosY                = (uint8_t*)   &_emu->_baseMem[0x008A];
  ninjaSpeedY              = (uint8_t*)   &_emu->_baseMem[0x0089];
  ninjaSpeedYFrac          = (uint8_t*)   &_emu->_baseMem[0x0087];
  ninjaCollisionFlags      = (uint8_t*)   &_emu->_baseMem[0x008C];
  ninjaFlinchDirection     = (uint8_t*)   &_emu->_baseMem[0x0094];
  ninjaInvincibilityTimer  = (uint8_t*)   &_emu->_baseMem[0x0095];
  ninjaWeapon              = (uint8_t*)   &_emu->_baseMem[0x00C9];
  ninjaAnimationType       = (uint8_t*)   &_emu->_baseMem[0x0080];
  ninjaAnimationOffset     = (uint8_t*)   &_emu->_baseMem[0x0081];
  ninjaAnimationTimer      = (uint8_t*)   &_emu->_baseMem[0x0082];
  ninjaAnimationFrame      = (uint8_t*)   &_emu->_baseMem[0x0083];
  enemyCount               = (uint8_t*)   &_emu->_baseMem[0x0072];
  timeoutSeconds1          = (uint8_t*)   &_emu->_baseMem[0x04C8];
  timeoutSeconds60         = (uint8_t*)   &_emu->_baseMem[0x04C7];
  screenScroll1            = (uint8_t*)   &_emu->_baseMem[0x0050];
  screenScroll2            = (uint8_t*)   &_emu->_baseMem[0x0051];
  screenScroll3            = (uint8_t*)   &_emu->_baseMem[0x0052];

  enemySlots              = (uint8_t*)   &_emu->_baseMem[0x0073];
  enemyPosX               = (uint8_t*)   &_emu->_baseMem[0x0460];
  enemyPosXFrac           = (uint8_t*)   &_emu->_baseMem[0x0458];
  enemyPosY               = (uint8_t*)   &_emu->_baseMem[0x0480];
  enemyPosYFrac           = (uint8_t*)   &_emu->_baseMem[0x0478];
  enemyType               = (uint8_t*)   &_emu->_baseMem[0x0400];
  enemyAITimer            = (uint8_t*)   &_emu->_baseMem[0x0408];
  enemyAIStage            = (uint8_t*)   &_emu->_baseMem[0x0410];
  enemyFlags              = (uint8_t*)   &_emu->_baseMem[0x0498];
  enemyHP                 = (uint8_t*)   &_emu->_baseMem[0x0490];
  enemyActions            = (uint8_t*)   &_emu->_baseMem[0x0074];
  enemyLastIdx            = (uint8_t*)   &_emu->_baseMem[0x0075];
  enemyCollision          = (uint8_t*)   &_emu->_baseMem[0x0488];

  enemyVelX               = (uint8_t*)   &_emu->_baseMem[0x0450];
  enemyVelXFrac           = (uint8_t*)   &_emu->_baseMem[0x0448];
  enemyVelY               = (uint8_t*)   &_emu->_baseMem[0x0470];
  enemyVelYFrac           = (uint8_t*)   &_emu->_baseMem[0x0468];

  ppuIndicator            = (uint8_t*)   &_emu->_baseMem[0x004C];

  // Timer tolerance
  if (isDefined(config, "Timer Tolerance") == true)
   timerTolerance = config["Timer Tolerance"].get<uint8_t>();
  else EXIT_WITH_ERROR("[Error] Game Configuration 'Timer Tolerance' was not defined\n");

  // Initialize derivative values
  updateDerivedValues();
}

// This function computes the hash for the current state
uint64_t GameInstance::computeHash() const
{
  // Storage for hash calculation
  MetroHash64 hash;

  hash.Update(*gameMode);
  hash.Update(*currentStage);
  hash.Update(*currentSubStage);
  hash.Update(*ninjalives);
  hash.Update(*ninjaPower);
  hash.Update(*ninjaHP);
  hash.Update(*bossHP);
  hash.Update(*ninjaStateFlags);
  hash.Update(*ninjaIsDead);
  hash.Update(*ninjaPosX);
  hash.Update(*ninjaPosXFrac);
  hash.Update(*ninjaSpeedX);
  hash.Update(*ninjaSpeedXFrac);
  hash.Update(*ninjaPosY);
  hash.Update(*ninjaSpeedY);
  hash.Update(*ninjaSpeedYFrac);
  hash.Update(*ninjaCollisionFlags);
  hash.Update(*ninjaFlinchDirection);
  hash.Update(*ninjaInvincibilityTimer);
  hash.Update(*ninjaWeapon);
  hash.Update(*ninjaAnimationType);
  hash.Update(*ninjaAnimationOffset);
  hash.Update(*ninjaAnimationTimer);
  hash.Update(*ninjaAnimationFrame);

  hash.Update(*timeoutSeconds1);
  hash.Update(*timeoutSeconds60);
  hash.Update(*screenScroll1);
  hash.Update(*screenScroll2);
  hash.Update(*screenScroll3);

  // Using stage timer to allow pauses
//  hash.Update(*frameCounter % timerTolerance);

  // Enemies
//  hash.Update(*enemyCount);
//  hash.Update(*enemySlots);
//  hash.Update(*enemyActions);
//  hash.Update(*enemyLastIdx);
//
//  for (uint8_t i = 0; i < ENEMY_COUNT; i++)
//   if (((i == 7) && (*enemySlots & 0b10000000)) ||
//       ((i == 6) && (*enemySlots & 0b01000000)) ||
//       ((i == 5) && (*enemySlots & 0b00100000)) ||
//       ((i == 4) && (*enemySlots & 0b00010000)) ||
//       ((i == 3) && (*enemySlots & 0b00001000)) ||
//       ((i == 2) && (*enemySlots & 0b00000100)) ||
//       ((i == 1) && (*enemySlots & 0b00000010)) ||
//       ((i == 0) && (*enemySlots & 0b00000001)))
//   {
//    hash.Update(*(enemyType+i));
//    hash.Update(*(enemyHP+i));
//    hash.Update(*(enemyFlags+i));
//    hash.Update(*(enemyPosX+i));
//    hash.Update(*(enemyPosXFrac+i));
//    hash.Update(*(enemyVelX+i));
//    hash.Update(*(enemyVelXFrac+i));
//    hash.Update(*(enemyPosY+i));
//    hash.Update(*(enemyPosYFrac+i));
//    hash.Update(*(enemyVelY+i));
//    hash.Update(*(enemyVelYFrac+i));
//    hash.Update(*(enemyCollision+i));
//    hash.Update(*(enemyAITimer+i));
//    hash.Update(*(enemyAIStage+i));
//   }

  hash.Update(*ppuIndicator);

  uint64_t result;
  hash.Finalize(reinterpret_cast<uint8_t *>(&result));
  return result;
}


void GameInstance::updateDerivedValues()
{
 double _screenScroll3 = *screenScroll3 == 255 ? 0.0 : (double)*screenScroll3;
 double _screenScroll2 = *screenScroll3 == 255 ? 0.0 : (double)*screenScroll2;
 absolutePosX = _screenScroll3 * 256.0 + (_screenScroll2 + (double)*ninjaPosX) + ((double)*screenScroll1 + (double)*ninjaPosXFrac)/ 256.0;

 ppuIndicatorBit6 = *ppuIndicator & 0b01000000;
}

// Function to determine the current possible moves
std::vector<std::string> GameInstance::getPossibleMoves() const
{
 std::vector<std::string> moveList = {"."};

 // First pass stage 00-00
 moveList.insert(moveList.end(), { ".......A", "......B.", "...U..B.", "..D.....", "..D...B.", ".L......", ".L.....A", ".L....B.", ".LDU....", "R.......", "R......A", "R.....B.", ".RDU....", "RL......"});

 return moveList;
}

// Function to get magnet information
magnetSet_t GameInstance::getMagnetValues(const bool* rulesStatus) const
{
 // Storage for magnet information
 magnetSet_t magnets;

 for (size_t ruleId = 0; ruleId < _rules.size(); ruleId++)
  if (rulesStatus[ruleId] == true)
    magnets = _rules[ruleId]->_magnets;

 return magnets;
}

// Obtains the score of a given frame
float GameInstance::getStateReward(const bool* rulesStatus) const
{
  // Getting rewards from rules
  float reward = 0.0;
  for (size_t ruleId = 0; ruleId < _rules.size(); ruleId++)
   if (rulesStatus[ruleId] == true)
    reward += _rules[ruleId]->_reward;

  // Getting magnet values for the kid
  auto magnets = getMagnetValues(rulesStatus);

  // Container for bounded value and difference with center
  float boundedValue = 0.0;
  float diff = 0.0;

  // Evaluating ninja magnet's reward on position X
  boundedValue = absolutePosX;
  boundedValue = std::min(boundedValue, magnets.ninjaHorizontalMagnet.max);
  boundedValue = std::max(boundedValue, magnets.ninjaHorizontalMagnet.min);
  diff = std::abs(magnets.ninjaHorizontalMagnet.center - boundedValue);
  reward += magnets.ninjaHorizontalMagnet.intensity * -diff;

  // Evaluating ninja magnet's reward on position Y
  boundedValue = (float)*ninjaPosY;
  boundedValue = std::min(boundedValue, magnets.ninjaVerticalMagnet.max);
  boundedValue = std::max(boundedValue, magnets.ninjaVerticalMagnet.min);
  diff = std::abs(magnets.ninjaVerticalMagnet.center - boundedValue);
  reward += magnets.ninjaVerticalMagnet.intensity * -diff;

  // Evaluating ninja power magnet
  boundedValue = (float)*ninjaPower;
  boundedValue = std::min(boundedValue, magnets.ninjaPowerMagnet.max);
  boundedValue = std::max(boundedValue, magnets.ninjaPowerMagnet.min);
  diff = std::abs(magnets.ninjaPowerMagnet.center - boundedValue);
  reward += magnets.ninjaPowerMagnet.intensity * -diff;

  // Evaluating ninja's weapon magnet
  if (magnets.ninjaWeaponMagnet.weapon == *ninjaWeapon) reward += magnets.ninjaWeaponMagnet.reward;

  // Evaluating boss health magnet
  reward += magnets.bossHealthMagnet * *bossHP;

  // Returning reward
  return reward;
}

void GameInstance::setRNGState(const uint64_t RNGState)
{
}

void GameInstance::printStateInfo(const bool* rulesStatus) const
{
 LOG("[Jaffar]  + Current Stage:                     %02u-%02u\n", *currentStage, *currentSubStage);
 LOG("[Jaffar]  + Frame Counter:                     %02u\n", *frameCounter);
 LOG("[Jaffar]  + Game Timer:                        %02u\n", *gameTime);
 LOG("[Jaffar]  + Game Mode:                         %02u\n", *gameMode);
 LOG("[Jaffar]  + PPU Indicator:                     %02u B6:(%02u)\n", *ppuIndicator, ppuIndicatorBit6);
 LOG("[Jaffar]  + Reward:                            %f\n", getStateReward(rulesStatus));
 LOG("[Jaffar]  + Hash:                              0x%lX\n", computeHash());
 LOG("[Jaffar]  + Ninja Lives:                       %02u\n", *ninjalives);
 LOG("[Jaffar]  + Ninja Power:                       %02u\n", *ninjaPower);
 LOG("[Jaffar]  + Ninja HP:                          %02u\n", *ninjaHP);
 LOG("[Jaffar]  + Ninja Animation:                   %02u %02u %02u %02u\n", *ninjaAnimationType, *ninjaAnimationOffset, *ninjaAnimationTimer, *ninjaAnimationFrame);
 LOG("[Jaffar]  + Ninja Weapon:                      %02u\n", *ninjaWeapon);
 LOG("[Jaffar]  + Ninja State Flags:                 %02u, D:(%02u)\n", *ninjaStateFlags, *ninjaIsDead);
 LOG("[Jaffar]  + Ninja Position X:                  %f: %02u + %02u\n", absolutePosX, *ninjaPosX, *ninjaPosXFrac);
 LOG("[Jaffar]  + Ninja Speed X:                     %02u + %02u\n", *ninjaSpeedX, *ninjaSpeedXFrac);
 LOG("[Jaffar]  + Ninja Position Y:                  %02u\n", *ninjaPosY);
 LOG("[Jaffar]  + Ninja Speed Y:                     %02u + %02u\n", *ninjaSpeedY, *ninjaSpeedYFrac);
 LOG("[Jaffar]  + Ninja Collision Flags:             %02u\n", *ninjaCollisionFlags);
 LOG("[Jaffar]  + Ninja Flinch:                      %02u\n", *ninjaFlinchDirection);
 LOG("[Jaffar]  + Ninja Invincibility Timer:         %02u\n", *ninjaInvincibilityTimer);
 LOG("[Jaffar]  + General Timer:                     %02u + %02u\n", *timeoutSeconds1, *timeoutSeconds60);
 LOG("[Jaffar]  + Screen Scroll:                     %02u + %02u + %02u\n", *screenScroll3, *screenScroll2, *screenScroll1);
 LOG("[Jaffar]  + Boss HP:                           %02u\n", *bossHP);

 LOG("[Jaffar]  + Enemy Count:                       %02u (%02u)\n", *enemyCount, *enemySlots);
 LOG("[Jaffar]  + Enemy Actions:                     %02u\n", *enemyActions);
 LOG("[Jaffar]  + Enemy Last Idx:                    %02u\n", *enemyLastIdx);

 for (uint8_t i = 0; i < ENEMY_COUNT; i++)
  if (((i == 7) && (*enemySlots & 0b10000000)) ||
      ((i == 6) && (*enemySlots & 0b01000000)) ||
      ((i == 5) && (*enemySlots & 0b00100000)) ||
      ((i == 4) && (*enemySlots & 0b00010000)) ||
      ((i == 3) && (*enemySlots & 0b00001000)) ||
      ((i == 2) && (*enemySlots & 0b00000100)) ||
      ((i == 1) && (*enemySlots & 0b00000010)) ||
      ((i == 0) && (*enemySlots & 0b00000001)))
      LOG("[Jaffar]    + Enemy %02u - T: (%03u), HP:(%03u), F:(%03u), X:(%03u, %03u), vX(%03u, %03u), Y:(%03u, %03u), vY(%03u, %03u), C(%03u), AI: (%03u, %03u)\n", i, *(enemyType+i), *(enemyHP+i), *(enemyFlags+i), *(enemyPosX+i), *(enemyPosXFrac+i), *(enemyVelX+i), *(enemyVelXFrac+i), *(enemyPosY+i), *(enemyPosYFrac+i), *(enemyVelY+i), *(enemyVelYFrac+i), *(enemyCollision+i), *(enemyAITimer+i), *(enemyAIStage+i));

 LOG("[Jaffar]  + Rule Status: ");
 for (size_t i = 0; i < _rules.size(); i++) LOG("%d", rulesStatus[i] ? 1 : 0);
 LOG("\n");

 auto magnets = getMagnetValues(rulesStatus);

 if (std::abs(magnets.ninjaHorizontalMagnet.intensity) > 0.0f)     LOG("[Jaffar]  + Ninja Horizontal Magnet        - Intensity: %.5f, Center: %3.3f, Min: %3.3f, Max: %3.3f\n", magnets.ninjaHorizontalMagnet.intensity, magnets.ninjaHorizontalMagnet.center, magnets.ninjaHorizontalMagnet.min, magnets.ninjaHorizontalMagnet.max);
 if (std::abs(magnets.ninjaVerticalMagnet.intensity) > 0.0f)       LOG("[Jaffar]  + Ninja Vertical Magnet          - Intensity: %.5f, Center: %3.3f, Min: %3.3f, Max: %3.3f\n", magnets.ninjaVerticalMagnet.intensity, magnets.ninjaVerticalMagnet.center, magnets.ninjaVerticalMagnet.min, magnets.ninjaVerticalMagnet.max);
 if (std::abs(magnets.ninjaPowerMagnet.intensity) > 0.0f)          LOG("[Jaffar]  + Ninja Power Magnet             - Intensity: %.5f, Center: %3.3f, Min: %3.3f, Max: %3.3f\n", magnets.ninjaPowerMagnet.intensity, magnets.ninjaPowerMagnet.center, magnets.ninjaPowerMagnet.min, magnets.ninjaPowerMagnet.max);
 if (std::abs(magnets.ninjaWeaponMagnet.reward) > 0.0f)            LOG("[Jaffar]  + Ninja Weapon Magnet            - Reward:    %.1f, Weapon: %u\n", magnets.ninjaWeaponMagnet.reward, magnets.ninjaWeaponMagnet.weapon);
 if (std::abs(magnets.bossHealthMagnet) > 0.0f)                    LOG("[Jaffar]  + Boss Health Magnet             - Intensity: %.5f\n", magnets.bossHealthMagnet);
}

