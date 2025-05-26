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


}
