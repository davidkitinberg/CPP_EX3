// davidkitinberg@gmail.com

#include "../Headers/Spy.hpp"
#include "../Headers/Player.hpp"

namespace coup {

    Spy::Spy(Game& g, const std::string& name) : Player(g, name) {}
    
    std::string Spy::role() const {
        return "Spy";
    }

    // Special spy action that blocks arrest action
    void Spy::blockArrestAction(Player& other) {
        // Can do action not on his turn
        if (!isActive()) {
            throw std::runtime_error("Inactive player cannot play");
        }
        if (&other == this) 
            throw std::runtime_error("Spy Cannot block arrest action on himself");
        
        
        blockArrest(other);
    }
    }
