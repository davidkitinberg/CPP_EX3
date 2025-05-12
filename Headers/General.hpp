#pragma once
#include "Player.hpp"

namespace coup {

class General : public Player {
public:
    General(Game& game, const std::string& name);
    std::string role() const override; // Return player's role
    void preventCoup(Player& target); // Special action for General. Can undo coups (even on himself)
};

}