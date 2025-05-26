// davidkitinberg@gmail.com

#include "../Headers/Spy.hpp"
#include "../Headers/Player.hpp"

namespace coup {

    Spy::Spy(Game& g, const std::string& name) : Player(g, name) {}
    
    std::string Spy::role() const {
        return "Spy";
    }

    }
