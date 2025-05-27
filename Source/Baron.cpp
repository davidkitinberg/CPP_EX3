// davidkitinberg@gmail.com

#include "../Headers/Baron.hpp"
#include "../Headers/Player.hpp"
#include "../Headers/Game.hpp"
#include <stdexcept>

namespace coup {

    Baron::Baron(Game& g, const std::string& name) : Player(g, name) {}
    
    // Special function only for Baron role players. Takes away 3 coins and returns 6 coins (counts as a turn)
    void Baron::invest() {
        if (coins() < 3) throw std::runtime_error("Need 3 coins to invest");
        deductCoins(3);
        addCoins(6);
    }
    
    std::string Baron::role() const {
        return "Baron";
    }
    
    }