#include "../Headers/Game.hpp"
#include "../Headers/Exception.hpp"
#include "../Headers/PlayerFactory.hpp"

namespace coup {

// Empty constructor
Game::Game() {}

// Function that adds player to the game
void Game::addPlayer(Player* player) {
    if (player_list.size() >= 6) {
        throw std::runtime_error("Maximum 6 players allowed");
    }
    player_list.push_back(player);

}

// Function to add player with random Role
void Game::addPlayerWithRandomRole(const std::string& name) {
    static std::vector<std::string> roles = {"Baron", "Spy", "Governor", "Merchant", "Judge", "General"};
    std::string role = roles[rand() % roles.size()];
    Player* player = createPlayerByRole(role, *this, name); // Your factory function
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
Player* Game::turn() const {
    if (player_list.empty()) throw std::runtime_error("No players in the game");
    return player_list[current_turn];
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
std::string Game::winner() const {
    std::string last_name;
    int count = 0;
    for (Player* p : player_list) 
    {
        if (p->isActive()) 
        {
            last_name = p->getName();
            count++;
        }
    }
    if (count == 1) return last_name;
    throw std::runtime_error("Game is still ongoing");
}

// Function that chnages turns to the next player
void Game::nextTurn() {
    if (player_list.empty()) return;

    do {
        current_turn = (current_turn + 1) % player_list.size();
    } while (!player_list[current_turn]->isActive());
}

}
