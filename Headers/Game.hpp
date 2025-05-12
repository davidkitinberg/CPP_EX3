#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include "../Headers/Player.hpp"
//#include <QObject>
//#include <QStringList>


namespace coup {

class Game {
private:
    
    std::vector<Player*> player_list;
    size_t current_turn = 0;
    bool didPayBribe = false;

    

public:
    Game();

    std::vector<Player*> getPlayers() const;
    void addPlayer(Player* player);
    Player* turn() const;
    std::vector<std::string> players() const;
    std::string winner() const;
    void addPlayerWithRandomRole(const std::string& name);
    //void paidBribe();

    void nextTurn(); // Called manually after a player's action
};

}
