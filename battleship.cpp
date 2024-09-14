/* 
Final Project
Daniil Kruklinskii, last updated 22/05/2022

This is the .cpp file which contains all the method definitions which are used to play the battleship game.
*/

#include<iostream>
#include<vector>
#include<string>
#include<utility>
#include<algorithm>
#include<memory>

#include "battleship.h"


//----------------------------------------Global Functions, Custom Data Types, Operator Overloadings----------------------------------------//


// This function is used to check whether an input string is an integer. Used to check location input.
bool is_number(const std::string& string)
{
    // Lambda function is used to check each digit
    return !string.empty() && std::find_if(string.begin(), string.end(), [](unsigned char character) { return !std::isdigit(character); }) == string.end();
}

// This function is used to hide the opponent's board from the next player so that they can play on one computer.
void next_player_turn() 
{
    std::string skip;
    std::cout << "Type ANYTHING to pass the turn" << std::endl;
    std::cin >> skip;
    for (int i = 0; i < 80; i++) { // 80 lines
        std::cout << std::endl;
    }

    std::cout << "Type ANYTHING to continue" << std::endl;
    std::cin >> skip;
    for (int i = 0; i < 80; i++) {
        std::cout << std::endl;
    }
}

// This overloads << operator to output character assigned to a flag.
std::ostream& operator<<(std::ostream& output, const flags flag)
{
    return output << static_cast<char>(flag);
}

/* 
This overloads >> operator so that the user correctly inputs the orientation.
H or h for horizontal and V and v for vertical. 
*/
std::istream& operator>>(std::istream& input, orientation& orientation) 
{
    bool error_flag = true;

    while (error_flag == true) {
        std::cout << "Input orientation (h for horizontal and v for vertical):" << std::endl;
        error_flag = false;
        std::string temporary_str {};
        input >> temporary_str;

        try {
            if (temporary_str.size() == 1) {
                if (temporary_str == "H" || temporary_str == "h") {
                    orientation = Horizontal;
                } else if (temporary_str == "V" || temporary_str == "v") {
                    orientation = Vertical;
                } else {
                    throw 1;
                }
            } else {
                throw 1;
            }
        }

        catch (int error) {
            error_flag == true;
            std::cout << "WRONG INPUT!" << std::endl;
        }
    }
    return input;
}

/*
This overloads >> operator so that the user correctly inputs the location.
For row integers from 1 to 10 are allowed and for column letters from A to J or a to j are allowed.
The order does not matter.
*/
std::istream& operator>>(std::istream& input, std::pair<int, int>& location)
{
    bool error_flag = true;

    while (error_flag == true) {
        std::cout << "Input location (INTEGER from 1 to 10 for ROW and CHARACTER from A to J for COLUMN e.g. 5 A):" << std::endl;

        try { 
            error_flag = false;
            std::string coordinate_1 {};
            std::string coordinate_2 {};
            input >> coordinate_1 >> coordinate_2;
            
            // First row then column.
            if (is_number(coordinate_1) == true && is_number(coordinate_2) == false && coordinate_2.size() == 1) {
                char column {coordinate_2[0]};
                if (column >= 'A' && column <= 'J' && std::stoi(coordinate_1) >= 0 && std::stoi(coordinate_1) <= 10) {
                    location.first = std::stoi(coordinate_1);
                    location.second = static_cast<int>(column) - 64; // ASCII code for A is 65
                } else if (column >= 'a' && column <= 'j' && std::stoi(coordinate_1) >= 0 && std::stoi(coordinate_1) <= 10) {
                    location.first = std::stoi(coordinate_1);
                    location.second = static_cast<int>(column) - 96; // ASCII code for a is 97      
                } else {
                    throw 2;
                }
            // First column then row.
            } else if (is_number(coordinate_2) == true && is_number(coordinate_1) == false && coordinate_1.size() == 1) {
                char column {coordinate_1[0]};
                if (column >= 'A' && column <= 'J' && std::stoi(coordinate_2) >= 0 && std::stoi(coordinate_2) <= 10) {
                    location.first = std::stoi(coordinate_2);
                    location.second = static_cast<int>(column) - 64;      
                } else if (column >= 'a' && column <= 'j' && std::stoi(coordinate_2) >= 0 && std::stoi(coordinate_2) <= 10) {
                    location.first = std::stoi(coordinate_2);
                    location.second = static_cast<int>(column) - 96;              
                } else {
                    throw 2;
                }
            } else {
                throw 1;
            }
        }

        catch (int error) {
            error_flag = true;
            switch (error) {
                case 1: 
                    std::cout << "WRONG LOCATION INPUT TYPE!" << std::endl;
                    break;
                case 2: 
                    std::cout << "WRONG LOCATION INPUT RANGE!" << std::endl;
                    break;
            }   
        }
    }
    return input;
}

//-------------------------------------------------Board-------------------------------------------------//


// Constructor that initialises all squared to empty.
board::board()
{
    std::vector<flags> empty_vector_1{};
    for (int i = 0; i < size_x; i++) {
        my_grid.push_back(empty_vector_1);
        for (int j = 0; j < size_y; j++) {
            my_grid[i].push_back(empty);
        }
    }

    std::vector<flags> empty_vector_2{};
    for (int i = 0; i < size_x; i++) {
        opponent_grid.push_back(empty_vector_2);
        for (int j = 0; j < size_y; j++) {
            opponent_grid[i].push_back(empty);
        }
    }
}

// Destructor that clears grid vectors
board::~board()
{
    my_grid.clear();
    opponent_grid.clear();
}

//This function changes the square on the grid.
void board::place_flag(const flags flag, const std::pair<int, int> location, const bool is_my_grid) 
{
    if (is_my_grid){
        my_grid[location.first - 1][location.second - 1] = flag; // -1 because locations start at 1
    } else {
        opponent_grid[location.first - 1][location.second - 1] = flag;
    }
}

// This friend function prints out both grids for the player
void print_board(const board& board, const bool is_my_grid)
{   
    if (is_my_grid) {
        std::cout << "                 Your Board" << std::endl;
    } else {
        std::cout << "              Opponent's Board" << std::endl;
    }

    std::cout << "    A   B   C   D   E   F   G   H   I   J";
    for (int i = 0; i < board.size_x; i++) {
        std::cout << std::endl;
        std::cout << " ";

        for (int j = 0; j < board.size_y; j++) {
            std::cout << "   _";
        }

        std::cout << std::endl;
        std::cout << i+1;

        if (i + 1 < 10) {
            std::cout << " ";
        }

        std::cout << "| ";

        for (int j = 0; j < board.size_y; j++) {
            if (is_my_grid) {
                std::cout << board.my_grid[i][j];
            } else {
                std::cout << board.opponent_grid[i][j];
            }
            std::cout << " | ";
        }

        std::cout << std::endl;
    }

    std::cout << " ";
    for (int j = 0; j < board.size_y; j++) {
        std::cout << "   _";
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

//-------------------------------------------------Ships-------------------------------------------------//


// Destructor that clears the shape vector.
ship::~ship() 
{
    shape.clear();
}

// This returns a shape.
std::vector<std::pair<flags, std::pair<int, int>>>  ship::return_shape() const
{
    return shape;
}

// This function changes ship flag when it is hit. It also checks whether the ship is destroyed.
void ship::change_flag (const std::pair<int, int> location, const flags flag) 
{
    int destroy_counter {0};
    if (!is_destroyed) {
        for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) {
            if (shape_it->second == location) {
                shape_it->first = flag;
            }
            if (shape_it->first == hit) {
                destroy_counter++;
            }
            if (destroy_counter == shape.size()) {
                is_destroyed = true;
                std::cout << "Ship destroyed!" << std::endl;
            }
        }
    }
}

// This function returns the destroyed status of the ship.
bool ship::is_ship_destroyed () const 
{
    return is_destroyed;
}

// Constructor that initialises the shape of the small ship based location and orientation.
small_ship::small_ship(std::pair<int, int> location, const orientation orientation)
{
    shape.push_back(std::make_pair(h_ship, std::make_pair(-1, -1))); // Two squares
    shape.push_back(std::make_pair(h_ship, std::make_pair(-1, -1)));

    if (orientation == Horizontal) {
        for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) { // Interator used
            shape_it->first = h_ship;
            shape_it->second = location;
            location.second += 1;
        }
    } else {
        for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) {
            shape_it->first = v_ship;
            shape_it->second = location;
            location.first += 1;
        }
    }
}

// Constructor that initialises the shape of the medium ship based location and orientation.
medium_ship::medium_ship(std::pair<int, int> location, const orientation orientation) 
{
    shape.push_back(std::make_pair(h_ship, std::make_pair(-1, -1))); // Three squares
    shape.push_back(std::make_pair(h_ship, std::make_pair(-1, -1)));
    shape.push_back(std::make_pair(h_ship, std::make_pair(-1, -1)));

    if (orientation == Horizontal) {
        for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) {
            shape_it->first = h_ship;
            shape_it->second = location;
            location.second += 1;
        }
    } else {
        for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) {
            shape_it->first = v_ship;
            shape_it->second = location;
            location.first += 1;
        }
    }
}

// Constructor that initialises the shape of the large ship based location and orientation.
large_ship::large_ship(std::pair<int, int> location, const orientation orientation) 
{
    shape.push_back(std::make_pair(h_ship, std::make_pair(-1, -1))); // Five squares
    shape.push_back(std::make_pair(h_ship, std::make_pair(-1, -1)));
    shape.push_back(std::make_pair(h_ship, std::make_pair(-1, -1)));
    shape.push_back(std::make_pair(h_ship, std::make_pair(-1, -1)));
    shape.push_back(std::make_pair(h_ship, std::make_pair(-1, -1)));

    if (orientation == Horizontal) {
        for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) {
            shape_it->first = h_ship;
            shape_it->second = location;
            location.second += 1;
        }
    } else {
        for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) {
            shape_it->first = v_ship;
            shape_it->second = location;
            location.first += 1;
        }
    }
}

//-------------------------------------------------Abilities-------------------------------------------------//


int ability::return_hit_count () const
{
    return hit_count;
}

int ability::return_miss_count () const
{
    return miss_count;
}

// Function to use a canon.
void canon::use_ability(std::vector<std::shared_ptr<ship>>& opponent_ships, board& my_board, board& opponent_board, int& my_energy, int& opponent_energy)
{
    if (my_energy < canon_energy_requirement) { // Check whether enough energy to use a canon.
        std::cout << "No canon charges!" << std::endl;
        return;
    }

    bool good_location {false};
    std::pair<int, int> location;
    std::cin >> location;
    
    for (auto ships_it = opponent_ships.begin(); ships_it < opponent_ships.end(); ++ships_it) {
        std::vector<std::pair<flags, std::pair<int, int>>> shape {(*ships_it)->return_shape()}; 

        for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) {
            // If the ship is hit.
            if (location == shape_it->second && shape_it->first != hit) {
                std::cout << "Enemy ship hit!" << std::endl;

                (*ships_it)->change_flag(location, hit);
                hit_count += 1;
                my_board.place_flag(hit, location, false);
                opponent_board.place_flag(hit, location, true);
                opponent_energy += 2; // Energy is granted to the opponent if his ship is hit
                return;
            // This avoids replacing already hit location with miss.
            } else if (location == shape_it->second && shape_it->first == hit) {
                miss_count += 1;
                std::cout << "Miss!" << std::endl;
                my_energy -= 1; // take energy if missed.
                return;
            }
        }
    };

    std::cout << "Miss!" << std::endl;
    miss_count += 1;
    my_board.place_flag(missed, location, false);
    opponent_board.place_flag(missed, location, true);
    my_energy -= 1; // take energy if missed.
}


void bomb::use_ability(std::vector<std::shared_ptr<ship>>&  opponent_ships, board& my_board, board& opponent_board, int& my_energy, int& opponent_energy)
{
    if (my_energy < energy_requirement) { // Check whether enough energy.
        std::cout << "Not enough energy!" << std::endl;
        return;
    }

    bool good_location {false};
    std::pair<int, int> location;
    while (good_location == false) {
        std::cin >> location;
        good_location = true;
        try {
            // This makes sure that the entire blast area in inside the map
            if (location.first < 1 || location.first > 9 || location.second < 1 || location.second > 9) {
                throw 1;
            }
        }
        catch (int error) {
            good_location = false;
            std::cout << "INVALID LOCATION! BOMB USED OUTSIDE THE MAP!" << std::endl;
        }
    }

    my_energy -= energy_requirement; // 5 energy is used up.
    // Blast area locations
    std::vector<std::pair<int, int>> locations_vector {location, std::make_pair(location.first, location.second + 1), std::make_pair(location.first + 1, location.second), std::make_pair(location.first + 1, location.second + 1)};

    for (auto locations_it = locations_vector.begin(); locations_it < locations_vector.end(); ++locations_it) {
        bool hit_flag {false};

        for (auto ships_it = opponent_ships.begin(); ships_it < opponent_ships.end(); ++ships_it) {
        std::vector<std::pair<flags, std::pair<int, int>>> shape {(*ships_it)->return_shape()};   

            for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) {

                if (*locations_it == shape_it->second && (shape_it->first == v_ship || shape_it->first == h_ship)) {
                    std::cout << "Enemy ship hit!" << std::endl;
                    hit_flag = true;

                    (*ships_it)->change_flag(*locations_it, hit);
                    hit_count += 1;
                    my_board.place_flag(hit, *locations_it, false);
                    opponent_board.place_flag(hit, *locations_it, true);
                    opponent_energy += 2; // Energy is granted to the opponent if his ship is hit
                    break;
                } else if (*locations_it == shape_it->second && shape_it->first == hit) {
                    hit_flag = true;
                    std::cout << "Miss" << std::endl;

                    miss_count += 1;
                }
            }
        
            if (hit_flag) {
                break;
            } else if (ships_it == opponent_ships.end() - 1) {
                std::cout << "Miss" << std::endl;

                miss_count += 1;
                my_board.place_flag(missed, *locations_it, false);  
                opponent_board.place_flag(missed, *locations_it, true);
            }
        }
    };
}


void torpedo::use_ability(std::vector<std::shared_ptr<ship>>& opponent_ships, board& my_board, board& opponent_board, int& my_energy, int& opponent_energy) 
{
    if (my_energy < energy_requirement) {
        std::cout << "Not enough energy!" << std::endl;
        return;
    }
    my_energy -= energy_requirement; // 9 energy points are used.

    bool good_location {false};
    std::pair<int, int> location;
    orientation orientation {};
    std::cin >> location;
    std::cin >> orientation;
    std::vector<std::pair<int, int>> locations_vector {};
    
    //This creates correct blas are vector.
    if (orientation == Horizontal) {
        for  (int i = 1; i < 11; i++) {
            locations_vector.push_back(std::make_pair(location.first, i));
        }
    } else {
        for  (int i = 1; i < 11; i++) {
            locations_vector.push_back(std::make_pair(i , location.second));
        }    
    }

    for (auto locations_it = locations_vector.begin(); locations_it < locations_vector.end(); ++locations_it) {
        bool hit_flag {false};

        for (auto ships_it = opponent_ships.begin(); ships_it < opponent_ships.end(); ++ships_it) {
        std::vector<std::pair<flags, std::pair<int, int>>> shape {(*ships_it)->return_shape()};   

            for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) {

                if (*locations_it == shape_it->second && (shape_it->first == v_ship || shape_it->first == h_ship)) {
                    std::cout << "Enemy ship hit!" << std::endl;
                    hit_flag = true;

                    hit_count += 1;
                    (*ships_it)->change_flag(*locations_it, hit);
                    my_board.place_flag(hit, *locations_it, false);
                    opponent_board.place_flag(hit, *locations_it, true);
                    opponent_energy += 2; // Energy is granted to the opponent if his ship is hit
                    break;
                } else if (*locations_it == shape_it->second && shape_it->first == hit) {
                    hit_flag = true;
                    std::cout << "Miss" << std::endl;

                    miss_count += 1;
                }
            }
        
            if (hit_flag) {
                break;
            } else if (ships_it == opponent_ships.end() - 1) {
                std::cout << "Miss" << std::endl;

                miss_count += 1;
                my_board.place_flag(missed, *locations_it, false);  
                opponent_board.place_flag(missed, *locations_it, true);
            }
        }
    };
}

//-------------------------------------------------Player-------------------------------------------------//


// This function checks whether a newly placed ship "collides" with already placed ships. Returns true if location is valid.
bool player::is_valid_ship_location (const std::pair<int, int> location, const orientation orientation, const char ship_type) const
{
    try {
        ship temporary_ship;

        // Shallow copying.
        if (ship_type == 's') {
            small_ship temporary_ship_s(location, orientation);
            temporary_ship = temporary_ship_s;
        } else if (ship_type == 'm') {
            medium_ship temporary_ship_m(location, orientation);
            temporary_ship = temporary_ship_m;
        } else if (ship_type == 'l') {
            large_ship temporary_ship_l(location, orientation);
            temporary_ship = temporary_ship_l;
        }

        std::vector<std::pair<flags, std::pair<int, int>>> temporary_shape {temporary_ship.return_shape()};
        std::vector<std::pair<int, int>> ship_locations {};
        
        // Puts all ship location in one vector.
        for (auto ships_it = ships.begin(); ships_it < ships.end(); ++ships_it) {
            std::vector<std::pair<flags, std::pair<int, int>>> shape {(*ships_it)->return_shape()};   
            for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) {
                ship_locations.push_back(shape_it->second);
            }  
        }

        for (auto shape_it = temporary_shape.begin(); shape_it < temporary_shape.end(); ++shape_it) {
            // Checks whether ship is outside the map.
            if (shape_it->second.first < 1 || shape_it->second.first > 10 || shape_it->second.second < 1 || shape_it->second.second > 10) {
                throw 1;
            }

            int counter {0};
            // This checks whether there are identical locations, which would indicate a ship clash.
            counter = std::count(ship_locations.begin(), ship_locations.end(), shape_it->second);
            if (counter != 0) {
                throw 2;
            }
        }

    }
    catch (int error) {
        switch (error) {
            case 1: 
                std::cout << "INVALID LOCATION! SHIP OUTSIDE THE MAP!" << std::endl;
                break;
            case 2:
                std::cout << "INVALID LOCATION! SHIP CLASH!" << std::endl;
                break;
        }
        return false;
    }
    return true;
};

/*
This constructor places all ships on the board (2 small, 2 medium and 1 large).
It also arms the player with canon, bomb and torpedo.
*/ 
player::player(board& board) 
{  
    std::pair<int, int>  location {};
    orientation orientation {};
    print_board(board, true);
    std::cout << "Input location of first small ship." << std::endl;
    std::cin >> location;
    std::cin >> orientation;
    while (is_valid_ship_location(location, orientation, 's') == false) {
        std::cin >> location;
        std::cin >> orientation;
    }
    ships.push_back(std::make_shared<small_ship>(location, orientation));
    place_ships(board);
    print_board(board, true);

    std::cout << "Input location of second small ship." << std::endl;
    std::cin >> location;
    std::cin >> orientation;
    while (is_valid_ship_location(location, orientation, 's') == false) {
        std::cin >> location;
        std::cin >> orientation;
    }
    ships.push_back(std::make_shared<small_ship>(location, orientation));
    place_ships(board);
    print_board(board, true);

    std::cout << "Input location of first medium ship:" << std::endl;
    std::cin >> location;
    std::cin >> orientation;
    while (is_valid_ship_location(location, orientation, 'm') == false) {
        std::cin >> location;
        std::cin >> orientation;
    }
    ships.push_back(std::make_shared<medium_ship>(location, orientation));
    place_ships(board);
    print_board(board, true);

    std::cout << "Input location of second medium ship:" << std::endl;
    std::cin >> location;
    std::cin >> orientation;
    while (is_valid_ship_location(location, orientation, 'm') == false) {
        std::cin >> location;
        std::cin >> orientation;
    }
    ships.push_back(std::make_shared<medium_ship>(location, orientation));
    place_ships(board);
    print_board(board, true);

    std::cout << "Input location of large ship:" << std::endl;
    std::cin >> location;
    std::cin >> orientation;
    while (is_valid_ship_location(location, orientation, 'l') == false) {
        std::cin >> location;
        std::cin >> orientation;
    }
    ships.push_back(std::make_shared<large_ship>(location, orientation));
    place_ships(board);
    print_board(board, true);

    abilities.push_back(std::make_unique<canon>());
    abilities.push_back(std::make_unique<bomb>());
    abilities.push_back(std::make_unique<torpedo>());

}

// Destructor that clears all vectors.
player::~player() {
    ships.clear();
    abilities.clear();
}

// This function is used to make a player move, namely to use the abilities.
void player::turn (player& opponent, board& my_board, board& opponent_board) 
{
    int ability_number {0}; // 1: canon   2: bomb   3: torpedo
    canon_energy = 1; // One canon charge is given every turn.
    print_ship_info(opponent, false);
    print_weapon_info(*this);

    while (!opponent.is_defeated()) {
        std::cout << "Choose ability to use (from 1 to 3 or anything else to pass the turn)." << std::endl;
        std::string temp_string;
        std::cin >> temp_string;

        // Turn is skipped if the input is not an integer from 1 to 3.
        if (!is_number(temp_string)) {
            return;
        }
        ability_number = std::stoi(temp_string);
        if (ability_number < 1 || ability_number > 3) {
            return;
        }

        if (ability_number == 1) {
            abilities[ability_number - 1]->use_ability(opponent.return_ships(), my_board, opponent_board, canon_energy, opponent.return_energy());
        } else {
            abilities[ability_number - 1]->use_ability(opponent.return_ships(), my_board, opponent_board, energy, opponent.return_energy());
        }
        print_board(my_board, false);
        print_ship_info(opponent, false);
        print_weapon_info(*this);
    }

}

// This function is used to place ships on the board in the initialiser
void player::place_ships(board& board) const 
{
    for (auto ships_it = ships.begin(); ships_it < ships.end(); ++ships_it) {
        std::vector<std::pair<flags, std::pair<int, int>>> shape {(*ships_it)->return_shape()};

        for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) {
            board.place_flag(shape_it->first, shape_it->second, true);
        }  
    }
}

// This function returns vector of ships. Shared pointer allows that to be done.
std::vector<std::shared_ptr<ship>>& player::return_ships() 
{
    return ships;
}

// This function checks whether player is defeated by checking the status of all the ships.
bool player::is_defeated () const
{
    for (auto ships_it = ships.begin(); ships_it < ships.end(); ++ships_it) {
        if (!(*ships_it)->is_ship_destroyed()) {
            return false;
        }
    }
    return true;
}

// This function returns canon energy.
int& player::return_canon_energy() 
{
    return canon_energy;
}
// This function returns energy.
int& player::return_energy() 
{
    return energy;
}


void print_ship_info (const player& player, const bool is_my_ships)
{
    if (is_my_ships) {
        std::cout << "Your ships:   ";
    } else {
        std::cout << "Opponent's ships:   ";
    }
    for (auto ships_it = player.ships.begin(); ships_it < player.ships.end(); ++ships_it) {
        std::vector<std::pair<flags, std::pair<int, int>>> shape {(*ships_it)->return_shape()};

        for (auto shape_it = shape.begin(); shape_it < shape.end(); ++shape_it) {

            if (shape_it->first == v_ship) {
                std::cout << h_ship; //This avoids giving away opponent's ship orientation
            } else {
                std::cout << shape_it->first;
            }
        }
        std::cout << "   ";
    }
    std::cout << std::endl;
}
 
void print_weapon_info (const player& player)
{
    std::cout << "1: Canon   2: Bomb (Energy Requirement = 5)   3: Torpedo (Energy Requirement = 9)   Canon energy = " << player.canon_energy << "   Energy = " << player.energy << std::endl; 
}

// Prints scoreboard after the game ends.
void scoreboard (const player& player_1, const player& player_2)
{
    int player_1_ships_destroyed {0}; // How many did player one destroy.
    int player_2_ships_destroyed {0};
    int player_1_missed {0};
    int player_2_missed {0};
    int player_1_hit {0};
    int player_2_hit {0};
    int accuracy_1 {0};
    int accuracy_2 {0};

    // How many ships did player 1 destroyed
    for (auto ships_it = player_2.ships.begin(); ships_it < player_2.ships.end(); ++ships_it) {
        if ((*ships_it)->is_ship_destroyed()) {
            player_1_ships_destroyed += 1;
        }     
    }

    // How many ships did player 2 destroyed
    for (auto ships_it = player_1.ships.begin(); ships_it < player_1.ships.end(); ++ships_it) {
        if ((*ships_it)->is_ship_destroyed()) {
            player_2_ships_destroyed += 1;
        }     
    }

    // Hit and miss counts of player 2
    for (auto ability_it = player_2.abilities.begin(); ability_it < player_2.abilities.end(); ++ability_it) {
        player_2_hit += (*ability_it)->return_hit_count(); 
        player_2_missed += (*ability_it)->return_miss_count(); 
    }

    // Hit and miss counts of player 1
    for (auto ability_it = player_1.abilities.begin(); ability_it < player_1.abilities.end(); ++ability_it) {
        player_1_hit += (*ability_it)->return_hit_count(); 
        player_1_missed += (*ability_it)->return_miss_count(); 
    }

    if ((player_1_hit + player_1_missed) == 0) {
        accuracy_1 = 0;
    } else {
        accuracy_1 = static_cast<int>(100*player_1_hit/(player_1_hit + player_1_missed));
    }

    if ((player_2_hit + player_2_missed) == 0) {
        accuracy_2 = 0;
    } else {
        accuracy_2 = static_cast<int>(100*player_2_hit/(player_2_hit + player_2_missed));
    }

    // Output
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "                              SCOREBOARD" << std::endl;
    std::cout << "                   Player 1               Player 2" << std::endl;

    if (player_1.is_defeated()) {
        std::cout << "                   Defeated               Victorious" << std::endl;
    } else if (player_2.is_defeated()) {
        std::cout << "                   Victorious             Defeated" << std::endl;
    }

    std::cout << "Ships destroyed:   " << player_1_ships_destroyed << "/5                    " << player_2_ships_destroyed << "/5           " << std::endl;
    std::cout << "Accuracy:          " << accuracy_1 << "%                     " << accuracy_2 << "%" << std::endl;
}