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
    std::vector<Player*> getPlayers() const;
    //void addPlayer(Player* player);
    // Player* turn() const;
    Player* turn();
    void handleTurnWithTarget(Player* player, const std::string& action, Player* target, std::vector<std::string>& log);
    void handleTurnWithNoTarget(Player* player, const std::string& action, std::vector<std::string>& log);
    bool handleBlockConsequences(const std::string& action, Player* blocker, Player* initiator, std::vector<std::string>& log);
    void handleMerchantPassive(Player* player, std::vector<std::string>& log);
    std::vector<std::string> players() const;
    Player* winner() const;
    void addPlayerWithRandomRole(const std::string& name);
    void checkElimination(std::vector<std::string>& log);
    //void paidBribe();

    void nextTurn(); // Called manually after a player's action
};

}
