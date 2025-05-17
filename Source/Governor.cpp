// davidkitinberg@gmail.com

#include "../Headers/Governor.hpp"
#include "../Headers/Player.hpp"
#include <stdexcept>

namespace coup {

Governor::Governor(Game& g, const std::string& name) : Player(g, name) {}

void Governor::tax() {
    if (onSanctioned() || onTaxBlocked()) {
        throw std::runtime_error(name + " is currently sanctioned and therefore cannot perform tax.");
    }
    addCoins(3);
}

std::string Governor::role() const {
    return "Governor";
}

// Special governor action that blocks tax action
void Governor::blockTaxAction(Player& other) {
    // Can do action not on his turn
    if (!isActive()) {
        throw std::runtime_error("Inactive player cannot play");
    }
    if (&other == this) 
        throw std::runtime_error("Governor Cannot block tax action on himself");
    if (other.onSanctioned())
        throw std::runtime_error("Cannot block tax because the Player" + other.getName() + "is already sanctioned.");
    
    blockTax(other);
}

}
