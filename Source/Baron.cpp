// davidkitinberg@gmail.com

#include "../Headers/Baron.hpp"
#include "../Headers/Player.hpp"
#include "../Headers/Game.hpp"
#include <stdexcept>

namespace coup {

    Baron::Baron(Game& g, const std::string& name) : Player(g, name) {}
    
    void Baron::invest() {
        if (coins() < 3) throw std::runtime_error("Need 3 coins to invest");
        deductCoins(3);
        addCoins(6);
        // When I will add another logic of the real time turns, I will add that on the next turn it will be like that
    }
    
    std::string Baron::role() const {
        return "Baron";
    }
    
    }