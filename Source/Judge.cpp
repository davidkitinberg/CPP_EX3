#include "../Headers/Judge.hpp"
#include "../Headers/Player.hpp"
#include "../Headers/Game.hpp"

namespace coup {

    Judge::Judge(Game& g, const std::string& name) : Player(g, name) {}
    

    std::string Judge::role() const {
        return "Judge";
    }

    void Judge::undoBribe(Player& target) {
        if (target.onBribe() == false) throw std::runtime_error("Cannot block bribe because there is no one active");
        if (game.turn() != &target) {
            throw std::runtime_error("Bribe block happenes during the targets player's turn");
        }
        if (this == &target) throw std::runtime_error("Why would you even try to bribe block yourself");
        blockBribe(target);
    }
    
    }