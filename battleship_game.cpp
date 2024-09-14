/* 
Final Project
Daniil Kruklinskii, last updated 22/05/2022

This is the .cpp file which contains the main() function, which is used to run and play the battleship game.
battleship.cpp and battleship.h files are required to use this program.

This battleship game is the non-vanilla version played by two players on one computer.
The player communicates with the game using the command line terminal.
The following game version contains additional weapons and other alternative game mechanics 
(more on that in battleship.cpp and battleship.h files and program report).
*/

#include "battleship.h"

int main()
{
    // Create boards for both players.
    board sea_1 {};
    board sea_2 {};

    // players and ships are initialised.
    player player_1(sea_1);
    player_1.place_ships(sea_1);

    next_player_turn();

    player player_2(sea_2);
    player_2.place_ships(sea_2);

    // The game begins
    while (1) {
        next_player_turn();

        print_board(sea_1, true);
        std::cout << std::endl;
        print_ship_info(player_1, true);
        std::cout << std::endl;
        print_board(sea_1, false);
        player_1.turn(player_2, sea_1, sea_2);


        if (player_1.is_defeated() || player_2.is_defeated()) {
            break;
        }

        next_player_turn();

        print_board(sea_2, true);
        std::cout << std::endl;
        print_ship_info(player_2, true);
        std::cout << std::endl;
        print_board(sea_2, false);
        player_2.turn(player_1, sea_2, sea_1);

        if (player_1.is_defeated() || player_2.is_defeated()) {
            break;
        }
        // After every turn cycle each player gets 2 energy points
        player_1.return_energy() +=2;
        player_2.return_energy() +=2;
    }

    scoreboard(player_1, player_2);
    
    return 0;
}