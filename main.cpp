#include "Headers/Game.hpp"
#include "Headers/Player.hpp"
#include "Headers/PlayerFactory.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>
#include <ctime>

using namespace coup;

void printLog(const std::vector<std::string>& log) {
    std::cout << "\nLogs: \n";
    for (const auto& entry : log)
        std::cout << " • " << entry << "\n";
}

Player* getRandomTarget(Player* attacker, const std::vector<Player*>& players) {
    std::vector<Player*> candidates;
    
    // Filter out the attacker
    for (Player* p : players) {
        if (p != attacker && p->isActive()) {
            candidates.push_back(p);
        }
    }

    // If no valid targets, return nullptr
    if (candidates.empty()) return nullptr;

    // Pick one randomly
    int index = rand() % candidates.size();
    return candidates[index];
}

void filterMoves(Player* current, std::vector<std::string>& actions) {

    // If a player has at least 10 coins - delete all options except coup
    if(current->coins() >= 10 )
    {
        for (size_t i = 0; i < actions.size(); ) {
            if (actions[i] == "Coup") { 
                actions.erase(actions.begin() + i);
            } else {
                ++i;  // Only increment if we didn't erase
            }
        }
    }

    // Delete options due to lack of money - bribe = 4 , sanction = 3 , coup = 7
    if(current->coins() < 10 )
    {
        for (size_t i = 0; i < actions.size(); ) {
            const std::string& action = actions[i];

            if ((action == "Bribe" && current->coins() < 4) ||
                (action == "Sanction" && current->coins() < 3) ||
                (action == "Coup" && current->coins() < 7)) {
                actions.erase(actions.begin() + i);
            } else {
                ++i;
            }
        }
    }
}

// Handles a randomized action selection and execution
void randomTurn(Game& game, Player* current, std::vector<std::string>& log, const std::vector<Player*>& players) {
    std::string role = current->role();
    std::string name = current->getName();

    // Generic actions for all roles
    std::vector<std::string> generic = {"Gather", "Tax", "Bribe","Arrest","Coup","Sanction"};

    // Special actions mapped by role
    std::map<std::string, std::vector<std::string>> roleActions = {
        {"Spy", {"BlockArrest"}},
        {"Governor", {"BlockTax"}},
        {"Judge", {"BlockBribe"}},
        {"General", {"BlockCoup"}},
        {"Baron", {"Invest"}},
        {"Merchant", {}}
    };

    // Merge generic and special for this role
    std::vector<std::string> actions = generic;
    if (roleActions.count(role)) {
        actions.insert(actions.end(), roleActions[role].begin(), roleActions[role].end());
    }

    filterMoves(current,actions); // Helper function that makes random choices "smarter"

    // Shuffle & pick one
    std::string action = actions[rand() % actions.size()];

    try {
        if (action == "Gather" || action == "Tax" || action == "Bribe" || action == "Invest") {
            game.handleTurnWithNoTarget(current, action, log);
        } else {
            Player* target = getRandomTarget(current, players);
            if (!target) {
                log.push_back(name + " wanted to use " + action + " but no target was available.");
                return;
            }
            game.handleTurnWithTarget(current, action, target, log);
        }
    } catch (const std::exception& e) {
        log.push_back("Error: " + name + "'s action " + action + " failed: " + e.what());
    }
}


Player* isSomeoneOnCoupTrial(Game game) {
    for (Player* p : game.getPlayers())
        {
            if ( p->onCoupTrial())
                return p;
        }
    return nullptr;
}





int main() {
    try {
        Game game;
        std::vector<std::string> log;

        game.addPlayerWithRandomRole("Alice");
        game.addPlayerWithRandomRole("Bob");
        game.addPlayerWithRandomRole("Charlie");
        game.addPlayerWithRandomRole("Dana");
        game.addPlayerWithRandomRole("Eli");
        game.addPlayerWithRandomRole("Frank");

        std::vector<Player*> Players_list = game.getPlayers();


        std::cout << "Starting Game with 6 Players:\n";
        
        for (Player* p : Players_list)
        {
            std::cout << " - " << p->getName() << " (" << p->role() << ")\n";
        }
        



        int round = 0;
        while (round < 60 && game.getPlayers().size() > 1) {
            Player* current = game.turn();
            std::string name = current->getName();
            std::string role = current->role();
            std::cout << "\n--- Round " << round + 1 << ": " << name << " (" << role << ") — Coins: " << current->coins() << " ---\n";


            while (current == game.turn()) {
                randomTurn(game, current, log, game.getPlayers());
            }


            // Print the logs for this round
            printLog(log);
            // Clear for next round
            log.clear();
            
            game.checkElimination(log);
            
            round++;
        }

        // Print final results
        std::cout << "\n==================================================================================\n";
        std::cout << "\n Final Player States:\n";
        for (Player* p : game.getPlayers()) {
            std::cout << " - " << p->getName() << " (" << p->role() << ") - Coins: " << p->coins() << "\n";
        }

        
        std::cout << "\n Game ended.\n";

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
    }

    return 0;
}
