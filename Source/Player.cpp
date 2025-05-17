// davidkitinberg@gmail.com

#include "../Headers/Player.hpp"
#include "../Headers/Game.hpp"

#include <stdexcept>
#include <iostream>

namespace coup {

Player::Player(Game& g, const std::string& name) : game(g), name(name) {
    
}

// Returns name of player
std::string Player::getName() const {
    return name;
}

// Returns number of coins the player has
int Player::coins() const {
    return coin_count;
}

// Returns the state of player
bool Player::isActive() const {
    return state.active;
}

// Adds coins to the player
void Player::addCoins(int amount) {
    coin_count += amount;
}

// Deducts coins from the player
void Player::deductCoins(int amount) {
    if (coin_count < amount) {
        throw std::runtime_error("Not enough coins");
    }
    coin_count -= amount;
}

// The player was tagged for a coup, only the general can undo this. On his next turn he will be terminated
void Player::deactivate() {
    state.onCoupTrial = true;
}

void Player::eliminate() {
    state.active = false;
    state.onCoupTrial = false;
}

// Gives one coin to the player
void Player::gather() {
    validateAction("Gather");
 
    addCoins(1);
    // advance turn should happen in game
}

// Gives 2 coins to the player
void Player::tax() {
    validateAction("Tax");
    if(onSanctioned()) {
        throw std::runtime_error(name + " is currently sanctioned and therefore cannot perform tax.");
    }
    if(onTaxBlocked()) {
        throw std::runtime_error(name + " is currently taxed blocked and therefore cannot perform tax.");
    }
    else {
        addCoins(2);
    }
    
}

void Player::bribe() {
    validateAction("Bribe");

    // Check if the player is bribe blocked
    if (state.isBribeBlocked) {
        throw std::runtime_error(name + " is bribe-blocked and cannot use bribe this turn.");
    }

    if (coin_count < 4)
        throw std::runtime_error(name + " cannot perform bribe (not enough coins - require 4 coins)");
    else {
        deductCoins(4);

        state.bribedThisTurn = true;
    }
}

void Player::arrest(Player& target) {
    validateAction("Arrest");

    if (&target == this)
        throw std::runtime_error("Cannot perform arrest on yourself");

    if (target.state.isRecentlyArrested)
        throw std::runtime_error(target.getName() + " was recently arrested and cannot be arrested again yet.");


    // Do the arrest logic
    target.deductCoins(1);
    addCoins(1);

    if (target.role() == "General") {
        target.addCoins(1);
    }
    if (target.role() == "Merchant") {
        target.deductCoins(1);
        deductCoins(1);
    }

    // Clear "recently arrested" state from all other players
    for (Player* p : game.getPlayers()) {
        if (p != &target) {
            p->state.isRecentlyArrested = false;
        }
    }

    // Then mark the target as newly arrested
    target.state.isRecentlyArrested = true;
}

void Player::sanction(Player& target) {
    validateAction("Sanction");
    if (&target == this)
        throw std::runtime_error("Cannot perform sanctions on yourself");
    if (coin_count < 3)
        throw std::runtime_error("Not enough coins for sanctions (require 3 coins)");
    if ((target.role() == "Judge") && coin_count < 4)
        throw std::runtime_error("Not enough coins for sanctions on Judge (require 4 coins)");
    else
    {
        deductCoins(3);
        target.state.isSanctioned = true;
        if(target.role() == "Baron") target.addCoins(1); // Special Baron compensation
        if(target.role() == "Judge") deductCoins(1);; // Special Baron compensation
    }
}

// Helper function to see sunction state
bool Player::onSanctioned() const {
    return state.isSanctioned;
}

// Helper function to see arrested state
// bool Player::onArrested() const {
//     return state.isArrestedBlocked;
// }


void Player::coup(Player& target) {
    validateAction("Coup");
    if (&target == this) throw std::runtime_error("Cannot perform coup on yourself");
    if (coin_count < 7) throw std::runtime_error("Not enough coins to coup (require 7 coins)");
    deductCoins(7);
    target.deactivate();
}


// Helper method to validate action and save duplicated code
void Player::validateAction(const std::string& actionName) const {
    if (!isActive()) {
        throw std::runtime_error("Inactive player cannot play");
    }

    if (game.turn() != this) {
        throw std::runtime_error("Not your turn");
    }

    if (onSanctioned() && (actionName == "Tax" || actionName == "Gather")) {
        throw std::runtime_error(name + " is currently sanctioned and therefore cannot perform " + actionName + ".");
    }

    if (state.isArrestedBlocked && actionName == "Arrest") {
        throw std::runtime_error(name + " is currently blocked from using arrest (Spy ability).");
    }
}

// Governor only method that blocks the tax action on another player
void Player::blockTax(Player& target) {
    if(this->role() != "Governor") throw std::runtime_error("Tax block action only permitted to Governor");
    target.state.isTaxBlocked = true;
}

// Spy only method that blocks the arrest action on another player
void Player::blockArrest(Player& target) {
    if(this->role() != "Spy") throw std::runtime_error("Arrest block action only permitted to Spy");
    target.state.isArrestedBlocked = true;
}

void Player::blockCoup(Player& target) {
    if(this->role() != "General") throw std::runtime_error("Coup block action only permitted to General");
    target.state.onCoupTrial = false;

}

void Player::blockBribe(Player& target) {
    if(this->role() != "Judge") throw std::runtime_error("Coup block action only permitted to Judge");
    
    if (game.turn() == this) {
        // Judge is using block during their own turn: apply future bribe block
        target.state.isBribeBlocked = true;
    } else {
        // Passive block via reaction: cancel bribe attempt this turn
        target.state.bribedThisTurn = false;
    }

}

// Helper function to general that checks if player is on coop trial
bool Player::onCoupTrial() const {
    return state.onCoupTrial;
}

// Helper function to spy that checks if player is blocked on arrest
bool Player::onArrestedBlocked() const {
    return state.isArrestedBlocked; 
}

// Helper function to governor that checks if player is blocked on tax
bool Player::onTaxBlocked() const {
    return state.isTaxBlocked; 
}

// Helper function to judge that checks if player has taken a bribe (for 2 actions on a turn)
bool Player::onBribe() const {
    return state.bribedThisTurn; 
}

void Player::usedBribeTurn() {
    if(!onBribe())
        throw std::runtime_error(this->getName() + " Did not use bribe this turn");
    
    state.bribedThisTurn = false;
}

// void Player::resetArrestBlock() {
//     state.isArrestedBlocked = false;
// }

// void Player::resetBribeBlock() {
//     state.isBribeBlocked = false;
// }

// void Player::resetSanction() {
//     state.isSanctioned = false;
// }

void Player::resetTurnFlags() {
    state.bribedThisTurn = false;
    state.isSanctioned = false;
    state.isTaxBlocked = false;
    state.isArrestedBlocked = false;
    state.isBribeBlocked = false;
}


}
