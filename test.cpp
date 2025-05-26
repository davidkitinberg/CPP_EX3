#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../Headers/Game.hpp"
#include "../Headers/PlayerFactory.hpp"
#include <iostream>

using namespace coup;

// Adds new players to the game
void setUpGame(Game &game) {
    
    game.addPlayerWithRole("Dexter", "Spy");
    game.addPlayerWithRole("Debra", "Governor");
    game.addPlayerWithRole("Angel", "General");
    game.addPlayerWithRole("Joey", "Baron");
    game.addPlayerWithRole("James", "Merchant");
    game.addPlayerWithRole("Arthur", "Judge");
}

// Helper function to get a player with the wanted role
Player* getPlayerWithMatchingRole(Game& g, const std::string& s) {
    for (Player* p : g.getPlayers()) {
        if (p->role() == s) {
            return p;
        }
    }
    return nullptr; // No player with matching role found
}

// Prints logs
void printLog(std::vector<std::string>& log) {
    std::cout << "\nLogs: \n";
    for (const auto& entry : log)
        std::cout << " • " << entry << "\n";
    
    log.clear();
}

TEST_CASE("Game restrictions on players") {
    Game game;
    std::vector<std::string> log;
    
    setUpGame(game); // We added 6 players
    



    // Now lets try to add the seventh player
    std::cout << "\n=========== Test player number limit ===========\n";
    CHECK_THROWS(game.addPlayerWithRole("Leonardo","Spy"));
    printLog(log);
    CHECK_FALSE(game.getPlayers().size() == 7);

    // Check number of players in the game
    std::vector<Player*> players = game.getPlayers();
    CHECK(players.size() == 6);
    
    // Check turn start
    Player* current = game.turn();
    CHECK(current == players[0]);




    
    std::cout << "\n=========== Test turn changes logic ===========\n";
    // Check for turn changes - try to make a move with not enough coins
    std::cout << "\nThe player makes a move that he cannot afford:\n";
    game.handleTurnWithTarget(current,"Coup", game.getPlayers()[1],log);
    game.handleTurnWithNoTarget(current,"Bribe", log);
    game.handleTurnWithTarget(current,"Sanction", game.getPlayers()[1],log);
    printLog(log);

    
    std::cout << "\nTest player that is trying to make an action not on his turn:\n";
    // Action on turn control
    if(game.turn() != game.getPlayers()[4])
    {
        game.handleTurnWithNoTarget(game.getPlayers()[4],"Gather", log);
        printLog(log);
    }



    CHECK(current == game.turn()); // Player does not have the coins for that and turn should not change
    std::cout << "\nNow the player makes a valid move:\n";
    game.handleTurnWithNoTarget(current,"Gather", log); // valid move
    printLog(log);
    CHECK_FALSE(current == game.turn()); // Should be another player
    CHECK(game.getPlayers()[1] == game.turn()); // Turn rotation check



    std::cout << "\n=========== Test self targeting actions and their turn changes ===========\n";
    //Check for self targeting
    current = game.turn();
    game.handleTurnWithTarget(current,"Coup", current,log);
    game.handleTurnWithTarget(current,"Sanction", current,log);
    game.handleTurnWithTarget(current,"Arrest", current,log);
    printLog(log);
    CHECK(current == game.turn());
    


//     // Try gather
//     CHECK_NOTHROW(players[0]->gather());
//     game.nextTurn();

//     // Try tax
//     CHECK_NOTHROW(players[1]->tax());
//     game.nextTurn();

//     // Check coins
//     CHECK(players[0]->coins() == 1);
//     CHECK(players[1]->coins() == 2);
}

///////////////////////////// Test actions //////////////////////////////////
TEST_CASE("Test Gather") {
    Game game;
    setUpGame(game);
    std::vector<std::string> log;

    Player* current = game.turn();
    std::cout << "\n=========== Test Gather ===========\n";
    game.handleTurnWithNoTarget(current,"Gather", log);
    printLog(log);
    CHECK(current->coins() == 1);



}


TEST_CASE("Test Tax") {
    Game game;
    setUpGame(game);
    std::vector<std::string> log;

    Player* current = game.turn();
    std::cout << "\n=========== Test Tax ===========\n";
    game.handleTurnWithNoTarget(current,"Tax", log); // Use tax
    printLog(log);
    if (current->role() == "Governor")
    {
        CHECK(current->coins() == 3);
    }
    else
        CHECK(current->coins() == 2);
    

}


TEST_CASE("Test Sanction") {
    // Initialization
    Game game;
    setUpGame(game);
    std::vector<std::string> log;


    std::cout << "\n=========== Test Sanction ===========\n";
    std::cout << "\nTest Sanction - normal case:\n";

    // Get pointers to current player and some target
    Player* current = game.turn();
    Player* anotherPlayer = getPlayerWithMatchingRole(game,"Governor");
    if (anotherPlayer == current) // make sure its not the same player
    {
        anotherPlayer = getPlayerWithMatchingRole(game,"General");
    }

    current->addCoins(3); // Add 3 coins for the sanction use

    
    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << anotherPlayer->getName() << "` has " << anotherPlayer->coins() << " coins\n";

    game.handleTurnWithTarget(current, "Sanction" ,anotherPlayer ,log); // Use sanction
    printLog(log);

    CHECK(current->coins() == 0); // Check coins after use of sanction

    

    if(current->role() == "Governor") // Governor sanctioned and cant use gather/tax
    {
        game.handleTurnWithNoTarget(current,"Gather", log); // Use tax
        game.handleTurnWithNoTarget(current,"Tax", log); // Use tax
        printLog(log);

        CHECK(current == game.turn()); // Turn should not be changed
    }

    ///////////////////////////////////////////////////
    std::cout << "\nTest Sanction - Baron & Judge cases:\n";

    // Get pointers to current player and some target
    current = game.turn();
    anotherPlayer = getPlayerWithMatchingRole(game,"Judge");
    if (anotherPlayer == current) // make sure its not the same player
    {
        anotherPlayer = getPlayerWithMatchingRole(game,"Baron");
    }


    current->addCoins(3); // Add 3 coins for the sanction use
    CHECK(anotherPlayer->coins() == 0); // Should have 0 coins on start


    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << anotherPlayer->getName() << "` has " << anotherPlayer->coins() << " coins\n";



    // Should fail, sanctioning Judge costs 4 coins and not 3
    game.handleTurnWithTarget(current, "Sanction" ,anotherPlayer ,log); // Use sanction
    printLog(log);

    anotherPlayer = getPlayerWithMatchingRole(game,"Baron"); 

    game.handleTurnWithTarget(current, "Sanction" ,anotherPlayer ,log); // Use sanction
    printLog(log);


    if (anotherPlayer->role() == "Baron") // Baron should get 1 coin compensation
    {
        CHECK(anotherPlayer->coins() == 1);
    }
    else
        CHECK(current->coins() == 2);
    
    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << anotherPlayer->getName() << "` has " << anotherPlayer->coins() << " coins\n";
    
}






TEST_CASE("Test Bribe") {
    // Initialization
    Game game;
    setUpGame(game);
    std::vector<std::string> log;


    std::cout << "\n=========== Test Bribe ===========\n";
    std::cout << "\nTest Bribe - normal case:\n";

    // Get pointers to current player and some target
    Player* current = game.turn();


    current->addCoins(3); // Add 3 coins for the bribe use

    
    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    
    game.handleTurnWithNoTarget(current, "Bribe" ,log); // Try using Bribe
    printLog(log);

    current->addCoins(1); // Add another coin for the bribe use

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    
    game.handleTurnWithNoTarget(current, "Bribe" ,log); // Use Bribe
    printLog(log);

    CHECK(current->coins() == 0); // Check coins after use of bribe


    game.handleTurnWithNoTarget(current, "Gather" ,log);
    CHECK(game.turn() == current);
    game.handleTurnWithNoTarget(current, "Gather" ,log);
    CHECK_FALSE(game.turn() == current);

    printLog(log);

    /////////////////////////////////////////////////////////////
    std::cout << "\nTest Bribe - Judge block:\n";

    // Get pointers to current player and some target
    current = game.getPlayers()[2];

    current->addCoins(4); // Add 4 coins for the bribe use

    
    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";

    // Block the bribe action
    Player* Judge = getPlayerWithMatchingRole(game,"Judge"); 
    game.handleTurnWithTarget(Judge,"BlockBribe",current ,log); // Block current's player Bribe action
    
    if(game.handleBlockConsequences("Bribe", Judge, current, log))
    {
        game.handleTurnWithNoTarget(current, "Bribe" ,log); // Try using Bribe
    }
    
    printLog(log);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";

    // See that the turn did not change after the block
    CHECK(current == game.getPlayers()[2]);

    game.handleTurnWithNoTarget(current, "Gather", log);
    printLog(log);

    current = game.turn();
    std::cout << "\nCurrent player is `" << current->getName() << "`\n";

}

TEST_CASE("Test Arrest") {
    // Initialization
    Game game;
    setUpGame(game);
    std::vector<std::string> log;


    std::cout << "\n=========== Test Arrest ===========\n";
    std::cout << "\nTest Arrest - normal case:\n";

    // Get pointers to current player and some target
    Player* current = game.turn();
    Player* target = game.getPlayers()[1];

    std::cout << "\nSpy is trying to arrest Governor: \n";
    // We will artificially add coins to the Governor
    target->addCoins(1);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    game.handleTurnWithTarget(current, "Arrest", target, log); // make arrest
    printLog(log);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    //////////////////////////////////////////
    std::cout << "\nTest Arrest - arrest Merchant/General:\n";

    current = game.turn();
    target = game.getPlayers()[2]; // General

    std::cout << "\nGovernor is trying to arrest General: \n";

    // We will artificially add coins to the General
    target->addCoins(1);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    game.handleTurnWithTarget(current, "Arrest", target, log); // make arrest
    printLog(log);


    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    // Now merchant:

    current = game.turn();
    target = game.getPlayers()[4]; // Merchant

    std::cout << "\nGeneral is trying to arrest Merchant: \n";

    // We will artificially add coins to the Merchant
    target->addCoins(2);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    game.handleTurnWithTarget(current, "Arrest", target, log); // make arrest
    printLog(log);


    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    /////////////////////////////////////////
    std::cout << "\nTest Arrest - arrest a player twice in a row:\n";

    current = game.turn(); // Update current player pointer
    target = game.getPlayers()[4]; // Same merchant

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";
    std::cout << "\nLets try to arrest `" << target->getName() << "` again:\n";

    game.handleTurnWithTarget(current, "Arrest", target, log); // make arrest
    printLog(log);

    CHECK(current == game.turn()); // Turn should not change due to invalid action
    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    //////////////////////////////////////////
    std::cout << "\nTest Arrest - arrest block:\n";

    target = game.getPlayers()[4]; // Target is now James (Marchant)
    Player* spy = game.getPlayers()[0]; // Dexter is the spy
    game.handleTurnWithTarget(spy, "BlockArrest", target, log);
    printLog(log);
    
    // Now we will make an attempt to use arrest with joey
    target = game.getPlayers()[5]; // Target is now arthur (Judge)
    target->addCoins(1);
    current = game.turn();

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    game.handleTurnWithTarget(current, "Arrest", target, log); // make arrest
    printLog(log);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";
    CHECK(current == game.turn()); // Turn should not change due to invalid action

}






TEST_CASE("Test coup") {
    // Initialization
    Game game;
    setUpGame(game);
    std::vector<std::string> log;


    std::cout << "\n=========== Test Coup ===========\n";
    std::cout << "\nTest Coup - normal case:\n";

    // Get pointers to current player and some target
    Player* current = game.turn();
    Player* target = game.getPlayers()[1];

    std::cout << "\nSpy is trying to coup Governor: \n";
    // We will artificially add coins to the Spy (Dexter)
    current->addCoins(7);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    game.handleTurnWithTarget(current, "Coup", target, log); // make arrest
    game.checkElimination(log);
    printLog(log);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";
    // The game should automatically remove the target becuase the next turn is his
    CHECK(game.getPlayers().size() == 5); 
    
    /////////////////////////////////////////////////////////////
    std::cout << "\nTest Coup - a player with 10+ coins MUST COUP:\n";


    // Get pointers to current player and some target
    current = game.turn(); // Should be now General (Angel)
    target = game.getPlayers()[2]; // Should be now Baron (Joey)


    std::cout << "\nGeneral is trying to make an action that is not 'coup' while he has 10+ coins: \n";
    // We will artificially add coins to the General (Angel)
    current->addCoins(11);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    game.handleTurnWithNoTarget(current, "Gather", log); // Try using Gather
    printLog(log);

    CHECK(current == game.turn()); // Turn should not change untill the player make coup



    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    game.handleTurnWithTarget(current, "Coup", target, log); // make coup
    game.checkElimination(log);
    printLog(log);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";
    CHECK_FALSE(current == game.turn());

    /////////////////////////////////////////////////////////////
    std::cout << "\nTest Coup - block coup:\n";

    current = game.turn(); // Update current player's pointer
    target = game.getPlayers()[0]; // Should be now Spy (Dexter)
    Player* general = game.getPlayers()[1]; // Angel (General)
    general->addCoins(5); // We will artificially add coins to the General (Angel) - needs 5 to coup block someone
    current->addCoins(7); // We will artificially add coins to use coup on someone
    
    
    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    // Place Dexter (Spy) on coup trial
    game.handleTurnWithTarget(current, "Coup", target, log); // make coup
    game.checkElimination(log);
    printLog(log);
    CHECK_FALSE(current == game.turn());

    // block coup (aka. save player from elimination) to target player:
    game.handleTurnWithTarget(general, "BlockCoup", target, log);
    printLog(log);

    current = game.turn(); // Update current player's pointer
    CHECK(current->getName() == "Dexter");

}

TEST_CASE("Player blocking and special interactions") {
    Game game;
    Player* spy = createPlayerByRole("Spy", game, "Spy");
    Player* governor = createPlayerByRole("Governor", game, "Gov");
    game.getPlayers().push_back(spy);
    game.getPlayers().push_back(governor);

    CHECK(game.getPlayers().size() == 2);
    CHECK(game.turn() == spy);

    CHECK_NOTHROW(spy->gather());
    game.nextTurn();

    // Governor blocks tax
    CHECK_NOTHROW(governor->blockTax(*spy));
    CHECK(spy->onTaxBlocked());

    // Should fail tax now
    game.nextTurn(); // back to spy
    CHECK_THROWS(spy->tax());
}

TEST_CASE("Sanction and arrest logic") {
    Game game;
    Player* baron = createPlayerByRole("Baron", game, "Baron");
    Player* judge = createPlayerByRole("Judge", game, "Judge");
    game.getPlayers().push_back(baron);
    game.getPlayers().push_back(judge);

    baron->addCoins(10);
    judge->addCoins(10);

    game.turn(); // set up turn order
    CHECK_NOTHROW(baron->sanction(*judge));
    CHECK(judge->onSanctioned());

    game.nextTurn();
    CHECK_THROWS(judge->tax());  // should fail due to sanction
}

TEST_CASE("Coup and elimination") {
    Game game;
    Player* general = createPlayerByRole("General", game, "General");
    Player* spy = createPlayerByRole("Spy", game, "Spy");
    game.getPlayers().push_back(general);
    game.getPlayers().push_back(spy);

    general->addCoins(10);
    game.turn(); // initialize

    CHECK_NOTHROW(general->coup(*spy));
    CHECK(spy->onCoupTrial());

    game.nextTurn(); // Spy's turn should cause elimination
    //game.checkElimination();
    CHECK_FALSE(spy->isActive());
}
