// davidkitinberg@gmail.com

#include "../Headers/General.hpp"
#include "../Headers/Player.hpp"
#include "../Headers/Game.hpp"

namespace coup {

    General::General(Game& g, const std::string& name) : Player(g, name) {}

    std::string General::role() const {
        return "General";
    }

    // A function to prevent coup attempt
    void General::preventCoup(Player& target) {
        if (!isActive()) {
            throw std::runtime_error("Inactive player cannot play");
        }
        if (coin_count < 5) throw std::runtime_error("Not enough coins to prevent coup (require 5 coins)");

        if (target.onCoupTrial() == false) throw std::runtime_error("The player is not on coup trial. Please choose another player");

        deductCoins(5);
        blockCoup(target);
    }
    
    }