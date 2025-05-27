// davidkitinberg@gmail.com

#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include "../Headers/Player.hpp"
#include <random>

namespace coup {

class Game {
private:
    
    std::vector<Player*> player_list;
    size_t current_turn = 0;
    bool didPayBribe = false;

    

public:
    Game();
    ~Game();
    std::vector<Player*> getPlayers() const; // Returns vector of the current players in the game (in Player objects)
    Player* turn(); // Returns the player whose turn it is

    // Two general functions that know how to handle a turn with each action
    void handleTurnWithTarget(Player* player, const std::string& action, Player* target, std::vector<std::string>& log); 
    void handleTurnWithNoTarget(Player* player, const std::string& action, std::vector<std::string>& log);

    // Special function that handles block consequences logic when its not the blocker's turn
    bool handleBlockConsequences(const std::string& action, Player* blocker, Player* initiator, std::vector<std::string>& log);

    // Small simple function to handle Merchant passive ability
    void handleMerchantPassive(Player* player, std::vector<std::string>& log);

    std::vector<std::string> players() const; // Returns vector of the current players in the game (in string)
    Player* winner() const; // Function that declares winner
    void addPlayerWithRandomRole(const std::string& name); // Function to add player with random Role
    void checkElimination(std::vector<std::string>& log); // Function that checks elimination before each player's turn and eliminates if there is a need to

    void nextTurn(); // Called manually after a player's action - changes turns

    
    void addPlayerWithRole(const std::string& name, const std::string& role); // Function that adds player with desired role (only used for testing)
    
};

}
