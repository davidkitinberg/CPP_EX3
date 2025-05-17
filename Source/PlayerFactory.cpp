// davidkitinberg@gmail.com

#include "../Headers/Baron.hpp"
#include "../Headers/Spy.hpp"
#include "../Headers/Governor.hpp"
#include "../Headers/Merchant.hpp"
#include "../Headers/Judge.hpp"
#include "../Headers/General.hpp"
#include "../Headers/PlayerFactory.hpp"


namespace coup {

Player* createPlayerByRole(const std::string& role, Game& game, const std::string& name) {
    if (role == "Baron") return new Baron(game, name);
    if (role == "Spy") return new Spy(game, name);
    if (role == "Governor") return new Governor(game, name);
    if (role == "Merchant") return new Merchant(game, name);
    if (role == "Judge") return new Judge(game, name);
    if (role == "General") return new General(game, name);
    throw std::invalid_argument("Invalid role: " + role);
}

}
