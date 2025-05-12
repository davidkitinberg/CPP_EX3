// Headers/PlayerFactory.hpp
#pragma once
#include <string>
#include "Player.hpp"
#include "Baron.hpp"
#include "Spy.hpp"
#include "Governor.hpp"
#include "Merchant.hpp"
#include "Judge.hpp"
#include "Game.hpp"
#include "General.hpp"

namespace coup {
    Player* createPlayerByRole(const std::string& role, Game& game, const std::string& name);
}
