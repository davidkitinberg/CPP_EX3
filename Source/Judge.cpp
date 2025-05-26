// davidkitinberg@gmail.com

#include "../Headers/Judge.hpp"
#include "../Headers/Player.hpp"
#include "../Headers/Game.hpp"

namespace coup {

    Judge::Judge(Game& g, const std::string& name) : Player(g, name) {}
    

    std::string Judge::role() const {
        return "Judge";
    }

    
    }