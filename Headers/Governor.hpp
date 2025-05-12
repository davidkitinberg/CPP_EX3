#pragma once
#include "Player.hpp"

namespace coup {

class Governor : public Player {
public:
    Governor(Game& game, const std::string& name);
    void tax() override; // Gets 3 coins instead of 2
    std::string role() const override; // Return player's role
    
    void blockTaxAction(Player& other); // Can block another player's tax action
};

}
