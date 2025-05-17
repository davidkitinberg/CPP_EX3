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
    Game& game;
    std::string name;
    int coin_count = 0;
    PlayerState state;
    void validateAction(const std::string& actionName = "") const;

public:
    Player(Game& game, const std::string& name);
    virtual ~Player() = default;

    void blockTax(Player& target);  // prevents only tax
    void blockArrest(Player& target); // prevents arrest
    void blockCoup(Player& target); // Prevents coup
    void blockBribe(Player& target); // Prevents bribe
    bool onTaxBlocked() const;
    bool onArrestedBlocked() const;
    //void resetArrestBlock();
    bool onCoupTrial() const;
    void eliminate();
    bool onBribe() const;
    //void resetBribeBlock();
    void usedBribeTurn();
    std::string getName() const;
    int coins() const;
    bool isActive() const;
    virtual std::string role() const = 0;
    virtual void gather();
    virtual void tax();
    virtual void bribe();
    virtual void arrest(Player& target);
    virtual void sanction(Player& target);
    //void resetSanction();
    virtual bool onSanctioned() const;
    //virtual bool onArrested() const;
    virtual void coup(Player& target);

    void resetTurnFlags();
    

    void addCoins(int amount);
    void deductCoins(int amount);
    void deactivate();
};

} // namespace coup
