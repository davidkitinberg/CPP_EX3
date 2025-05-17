// davidkitinberg@gmail.com

#pragma once
#include "Player.hpp"

namespace coup {

class Baron : public Player {
public:
    Baron(Game& game, const std::string& name);
    void invest(); // custom ability
    std::string role() const override; // Return player's role
};

}