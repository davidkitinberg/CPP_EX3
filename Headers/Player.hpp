// davidkitinberg@gmail.com

#pragma once
#include <string>
#include <vector>
#include <stdexcept>

namespace coup {

class Game;

struct PlayerState {
    bool isSanctioned = false; // The player can't preform tax & gather actions for his next turn
    bool isTaxBlocked = false; // The player can't preform tax action for his next turn
    bool isArrestedBlocked = false; // The player can't preform arrest action for his next turn
    bool isRecentlyArrested = false; // for arrest cooldown logic
    bool isBribeBlocked = false; // The player can't preform bribe action for his next turn
    bool bribedThisTurn = false; // The player can preform 2 actions on his turn
    bool onCoupTrial = false; // The player was tagged for a coup, only the general can undo this. On his next turn he will be terminated
    bool active = true; // The player is in/out of the game
};

class Player {

    
protected:
    Game& game; // Game instance
    std::string name; // Player's name
    int coin_count = 0; // Player's coins
    PlayerState state; // Struct for player's state (sanction,tax blocked, etc..)
    void validateAction(const std::string& actionName = "") const; // Helper method to validate action and save duplicated code

public:
    Player(Game& game, const std::string& name); // Constructor
    virtual ~Player() = default; // Default destructor

    // Special actions that are role based
    void blockTax(Player& target);  // Governor only method that blocks the tax action on another player
    void blockArrest(Player& target); // Spy only method that blocks the arrest action on another player
    void blockCoup(Player& target); // General only method that blocks the coup action on another player, or prevents elimination on another player
    void blockBribe(Player& target); // Judge only method that blocks the bribe action on another player

    // Helper functions for managing actions logic
    bool onTaxBlocked() const; // Helper function to governor that checks if player is blocked on tax
    bool onArrestedBlocked() const; // Helper function to spy that checks if player is blocked on arrest
    bool onCoupTrial() const; // Helper function to general that checks if player is on coop trial
    bool onBribe() const; // Helper function to judge that checks if player has taken a bribe (for 2 actions on a turn)
    void usedBribeTurn(); // Function that resets "bribedThisTurn" flag in player state - called after the player used his bribe turn
    virtual bool onSanctioned() const; // Helper function to see sanction state

    void eliminate(); // Function that eliminates a player that reached his turn in a on coup trial state
    
    // Getters
    std::string getName() const; // Getter for player's name
    int coins() const; // Getter for number of coins the player has
    bool isActive() const; // Returns the state of player
    virtual std::string role() const = 0; // Pure virtual function that returns from each player it's role

    // Actions
    virtual void gather(); // Function for the action "Gather" that gives the player 1 coin
    virtual void tax(); // Function for the action "Tax" that gives the player 2 coins 
    virtual void bribe(); // Function for the action "Bribe" that gives the player another turn
    virtual void arrest(Player& target); // Function for the action "Arrest" that steals one coin from a target player and gives the caller 1 coin
    virtual void sanction(Player& target); // Function for the action "Sanction" that blocks the targeted player from using Tax & Gather (costs 3 coins to the caller)
    virtual void coup(Player& target); // Function for the action "Coup" that places the targeted player on "coup trial" (costs 7 coins to the caller)

    void resetTurnFlags(); // Resets state flags of actions that are blocking action for one turn
    

    void addCoins(int amount); // Adds coins to the player
    void deductCoins(int amount); // Deducts coins from the player
    void deactivate(); // Place a player on coup trial (on his next turn he will be terminated), Only a General can undo it
};

}
