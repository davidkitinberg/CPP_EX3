// davidkitinberg@gmail.com


#include "../Headers/Game.hpp"
#include "../Headers/PlayerFactory.hpp"

namespace coup {

// Empty constructor
Game::Game() {}

// Global pointer to the player who made the last turn
Player* last_turn_player = nullptr;

// Destructor
Game::~Game() {
    for (Player* p : player_list) {
        delete p;
    }
    player_list.clear();
}

// Function to add player with random Role
void Game::addPlayerWithRandomRole(const std::string& name) {
    if (player_list.size() >= 6) {
        throw std::runtime_error("Maximum 6 players allowed");
    }
    static std::vector<std::string> roles = {"Baron", "Spy", "Governor", "Merchant", "Judge", "General"};

    // Use the Mersenne Twister engine for better randomization
    static std::random_device rd;
    static std::mt19937 gen(rd()); // seed once the random object
    std::uniform_int_distribution<> dis(0, roles.size() - 1);

    std::string role = roles[dis(gen)];


    Player* player = createPlayerByRole(role, *this, name); // Factory function
    player_list.push_back(player); // Add player to the game
}

// Function that adds player with desired role (only used for testing)
void Game::addPlayerWithRole(const std::string& name, const std::string& role) {
    if (player_list.size() >= 6) {
        throw std::runtime_error("Maximum 6 players allowed");
    }
    Player* player = createPlayerByRole(role, *this, name);
    player_list.push_back(player);
}


// Function that gives the Players of the game
std::vector<Player*> Game::getPlayers() const {
    std::vector<Player*> result;
    for (Player* p : player_list) {
        if (p->isActive()) {
            result.push_back(p);
        }
    }
    return result;
}

// Function that returns the current player that has the turn now
Player* Game::turn() {
    if (player_list.empty()) {
        throw std::runtime_error("No players in the game");
    }

    Player* p = player_list[current_turn];

    // Handle coup trial resolution: eliminate if not blocked
    if (p->onCoupTrial()) {
        p->eliminate();  // new function below
        nextTurn();      // skip to next valid player
        return turn();   // recursively return the next valid player
    }

    return p;
}

// Fucntion that return vector list of the current players names
std::vector<std::string> Game::players() const {
    std::vector<std::string> result;
    for (Player* p : player_list) {
        if (p->isActive()) {
            result.push_back(p->getName());
        }
    }
    return result;
}

// Function that declares winner
Player* Game::winner() const {
    int count = 0;
    Player* last_active = nullptr;

    for (Player* p : player_list) {
        if (p->isActive()) {
            last_active = p;
            count++;
        }
    }

    if (count == 1) {
        return last_active;
    }

    return nullptr; // No winner yet or more than one player still active
}

// Function to handle turn game with action that requires a target
void Game::handleTurnWithTarget(Player* player, const std::string& action, Player* target, std::vector<std::string>& log) {
    if (!player || !target) {
        log.push_back("Invalid turn: Player or target is null.");
        return;
    }

    if (!player->isActive()) {
        log.push_back(player->getName() + " is out of the game.");
        return;
    }

    if (!target->isActive()) {
        log.push_back("Invalid target: " + target->getName() + " is already eliminated.");
        return;
    }

    if(player->coins() >= 10)
    {
        try {
            player->coup(*target);
            log.push_back(player->getName() + " placed " + target->getName() + 
                    " on a coup trial. On " + target->getName() + "'s next turn, he/she will be eliminated");
            nextTurn();
        } 
        catch (const std::exception& e) {
            log.push_back("Action failed: " + std::string(e.what()));
        }
    }
    else // Handle action logic
    {
        
        if (action == "Arrest") {
            try {
                player->arrest(*target);
                log.push_back(player->getName() + " arrested " + target->getName());
                nextTurn();
            } 
            catch (const std::exception& e) {
                log.push_back("Action failed: " + std::string(e.what()));
            }
        }
        else if (action == "Sanction") {
            try {
                player->sanction(*target);
                log.push_back(player->getName() + " sanctioned " + target->getName());
                nextTurn();
            } 
            catch (const std::exception& e) {
                log.push_back("Action failed: " + std::string(e.what()));
            }
        }
        else if (action == "Coup") {
            try {
                player->coup(*target);
                log.push_back(player->getName() + " placed " + target->getName() + 
                    " on a coup trial. On " + target->getName() + "'s next turn, he/she will be eliminated");
                nextTurn();
            } 
            catch (const std::exception& e) {
                log.push_back("Action failed: " + std::string(e.what()));
            }
        }
        ////////////////////////////////Special actions /////////////////////////////
        else if (action == "BlockTax") {
            try {
                player->blockTax(*target);
                log.push_back(player->getName() + " tax blocked " + target->getName());
                nextTurn();
            } 
            catch (const std::exception& e) {
                log.push_back("Action failed: " + std::string(e.what()));
            }
        }
        else if (action == "BlockCoup") {
            try {
                General* general = dynamic_cast<General*>(player);
                general->preventCoup(*target);
                log.push_back(player->getName() + " blocked coup " + target->getName() + " is now saved from elimination");
                nextTurn();
            } 
            catch (const std::exception& e) {
                log.push_back("Action failed: " + std::string(e.what()));
            }
        }
        else if (action == "BlockBribe") {
            try {
                player->blockBribe(*target);
                log.push_back(player->getName() + " bribe blocked " + target->getName());
                nextTurn();
            } 
            catch (const std::exception& e) {
                log.push_back("Action failed: " + std::string(e.what()));
            }
        }
        else if (action == "BlockArrest") {
            try {
                player->blockArrest(*target);
                log.push_back(player->getName() + " arrest blocked " + target->getName());
                nextTurn();
            } 
            catch (const std::exception& e) {
                log.push_back("Action failed: " + std::string(e.what()));
            }
        }
        else {
            log.push_back("Unknown action: " + action);
            return;
        }
    }

}




// Function to handle turn game with action does not require a target
void Game::handleTurnWithNoTarget(Player* player, const std::string& action, std::vector<std::string>& log) {
    
    if (action == "Tax") {
            try {
                player->tax();
                log.push_back(player->getName() + " used tax ");
                nextTurn();
            } 
            catch (const std::exception& e) {
                log.push_back("Action failed: " + std::string(e.what()));
            }
    }
    else if (action == "Bribe") {
        if (!player->onBribe())
        {
            try {
                player->bribe();
                log.push_back(player->getName() + " used bribe ");
                
            } 
            catch (const std::exception& e) {
                log.push_back("Action failed: " + std::string(e.what()));
            }
        }
        else log.push_back(player->getName() + " already used bribe once this turn");
    }
    else if (action == "Gather") {
        try {
            player->gather();
            log.push_back(player->getName() + " used gather ");
            nextTurn();
        } 
        catch (const std::exception& e) {
            log.push_back("Action failed: " + std::string(e.what()));
        }
    }
    ////////////////////////////////Special actions /////////////////////////////
    else if (action == "Invest") {
        try {
            Baron* baron = dynamic_cast<Baron*>(player);
            baron->invest();
            log.push_back(player->getName() + " used invest ");
            nextTurn();
        } 
        catch (const std::exception& e) {
            log.push_back("Action failed: " + std::string(e.what()));
        }
    }
    else {
        log.push_back("Unknown action: " + action);
        return;
    }
}

// Special function that handles block consequences logic when its not the blocker's turn
bool Game::handleBlockConsequences(const std::string& action, Player* blocker, Player* initiator, std::vector<std::string>& log) {
    
    // The initiator should still lose his coins after Judge's bribe block
    if (action == "Bribe" && blocker->role() == "Judge") {
        // Bribe gets blocked — player loses 4 coins
        if (initiator->coins() >= 4) {
            initiator->deductCoins(4);
            log.push_back(initiator->getName() + " loses 4 coins due to blocked Bribe.");
            return true;
        } else {
            log.push_back(initiator->getName() + " tried Bribe but didn't have 4 coins to lose.");
            return false;
        }
    }

    // A General should pay 5 coins in order to block
    if (action == "Coup" && blocker->role() == "General") {
        if (blocker->coins() >= 5 && initiator->coins() >= 7) {
            blocker->deductCoins(5);
            initiator->deductCoins(7);
            log.push_back(blocker->getName() + " blocked Coup action and lost 5 coins.");
            log.push_back(initiator->getName() + "'s coup attempt got blocked and lost 7 coins.");
            return true;
        } else {
            log.push_back(blocker->getName() + " tried to block Coup but lacked 5 coins.");
            return false;
        }
    }

    return true; // No special cost or restriction — allow block
}

// Small simple function to handle Merchant passive ability
void Game::handleMerchantPassive(Player* player, std::vector<std::string>& log) {
    if (player->role() == "Merchant" && player->coins() >= 3) {
        player->addCoins(1);
        log.push_back(player->getName() + " (Merchant) gained 1 passive coin for having 3+ coins.");
    }
}

// Function that handles turn cycle, bribe handling and flag reset
void Game::nextTurn() {
    if (player_list.empty()) return;

    // Save current player before rotating
    last_turn_player = turn();

    

    // Advance only if bribe wasn't used
    if (!last_turn_player->onBribe()) {
        do {
            current_turn = (current_turn + 1) % player_list.size();
        } while (!player_list[current_turn]->isActive());
    }

    // Reset bribe state of the **previous** player
    if (last_turn_player->onBribe()) {
        last_turn_player->usedBribeTurn();
    }

    // Reset Arrest, bribe & sanction block flag for player's next turn
    last_turn_player->resetTurnFlags();
}

// Function that checks elimination before each player's turn and eliminates if there is a need to
void Game::checkElimination(std::vector<std::string>& log) {
    Player* p = player_list[current_turn];

    // Handle coup trial resolution: eliminate if not blocked
    if (p->onCoupTrial()) {
        p->eliminate();  // new function below
        log.push_back(p->getName() + " was eliminated due to an unresolved coup.");
        nextTurn();
    }
}

}
