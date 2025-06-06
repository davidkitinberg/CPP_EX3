#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../Headers/Game.hpp"
#include "../Headers/PlayerFactory.hpp"
#include <iostream>


/* Disclaimer: Due to the nature of this project, testing actions and game flow without the GUI is not intuitive.
** The program was designed for real-time user interaction. Therefore, the tests simulate in-game scenarios to reflect
** realistic gameplay. Some aspects of the game logic may be abstracted or simplified as a result, but the tests remain
** faithful to the intended game behavior.
*/

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
    std::cout << "\nTest Tax - normal case:\n";
    game.handleTurnWithNoTarget(current,"Tax", log); // Use tax
    printLog(log);
    if (current->role() == "Governor")
    {
        CHECK(current->coins() == 3);
    }
    else
        CHECK(current->coins() == 2);
    
    ///////////////////////////////////////////////
    std::cout << "\nTest Tax - Tax block case:\n";

    current = game.turn(); // Current's player is Governor now
    Player* target = game.getPlayers()[2]; // Target to block tax - General (Angel)

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    game.handleTurnWithTarget(current,"BlockTax", target, log); // Block tax action
    printLog(log);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    std::cout << "Target player `" << target->getName() << "` has " << target->coins() << " coins\n";

    current = game.turn(); // Current's player is General now
    game.handleTurnWithNoTarget(current,"Tax", log); // Try to use tax
    printLog(log);

    CHECK(current->coins() == 0); // Should not gain any coins
    CHECK(current == game.turn()); // Failed action does not result in turn change

    game.handleTurnWithNoTarget(current,"Gather", log); // Use gather
    printLog(log);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";

    CHECK(current->coins() == 1); // Only gain 1 coin after Gather
    CHECK_FALSE(current == game.turn()); // Now turn should change



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

////////////////////////// Test Special abilities (passive and active) that don't involve generic actions //////////////////////////


TEST_CASE("Test Invest") {
    // Initialization
    Game game;
    setUpGame(game);
    std::vector<std::string> log;


    std::cout << "\n=========== Test Invest ===========\n";
    

    // Get pointers to current player
    Player* current = game.turn();

    while(game.turn()->role() != "Baron")
    {
        game.handleTurnWithNoTarget(current,"Gather", log);
        current = game.turn();
    }

    // We add artificially coins to Baron to use invest
    CHECK(current->role() == "Baron");
    current->addCoins(3);
    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";

    game.handleTurnWithNoTarget(current,"Invest", log);
    printLog(log);

    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    CHECK(current != game.turn()); // After Invest the turn should change

    
}

TEST_CASE("Merchant passive coin gain ability") {
    // Initialization
    Game game;
    setUpGame(game);
    std::vector<std::string> log;


    std::cout << "\n=========== Test Merchant passive coin gain ability ===========\n";

    // Get pointers to current player
    Player* current = game.turn();

    // Run up to his turn
    while(game.turn()->role() != "Merchant")
    {
        game.handleTurnWithNoTarget(current,"Gather", log);
        current = game.turn();
    }
    current->addCoins(3); // We add artificially coins to Merchant to see the passive works
    game.handleMerchantPassive(current,log); // Use the function for it
    printLog(log);
    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";

    CHECK(current->role() == "Merchant");
    CHECK(current->coins() == 4);

    std::cout << "\nShow that the passive does not work on less than 3 coins:\n";

    // Make him waste 4 coins and earn 2 so he would have only 2 coins now
    game.handleTurnWithNoTarget(current,"Bribe", log);
    game.handleTurnWithNoTarget(current,"Gather", log);
    game.handleTurnWithNoTarget(current,"Gather", log);
    printLog(log);
    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    CHECK(current->coins() == 2);


    // Run up to his turn again
    while(game.turn()->role() != "Merchant")
    {
        game.handleTurnWithNoTarget(current,"Gather", log);
        current = game.turn();
    }
    game.handleMerchantPassive(current,log); // Use the function for it
    printLog(log);
    std::cout << "\nCurrent player `" << current->getName() << "` has " << current->coins() << " coins\n";
    CHECK(current->coins() == 2);

}

TEST_CASE("Test Winner") {
    // Initialization
    Game game;
    std::vector<std::string> log;

    game.addPlayerWithRole("Dexter", "Spy");
    game.addPlayerWithRole("Debra", "Governor");

    Player* current = game.turn(); // Dexter
    Player* target = game.getPlayers()[1]; // Debra

    current->addCoins(7); // Add coins artificially to make coup

    CHECK(game.winner() == nullptr); // Should be still 2 players in the game - no winner

    game.handleTurnWithTarget(current, "Coup", target, log); // Coup Debra
    game.checkElimination(log); // Check eliminiation status
    printLog(log);

    CHECK(game.winner() == current); // Dexter is the only one left in the game
    
}

