// davidkitinberg@gmail.com

#include "../Headers/Merchant.hpp"
#include "../Headers/Player.hpp"

namespace coup {

    Merchant::Merchant(Game& g, const std::string& name) : Player(g, name) {}
    
    std::string Merchant::role() const {
        return "Merchant";
    }
    
    }