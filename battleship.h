/* 
Final Project
Daniil Kruklinskii, last updated 22/05/2022

This is the .h file which contains all the methods, data types, class declarations which are used to play the battleship game.
*/

#pragma once

#include<iostream>
#include<vector>
#include<string>
#include<utility>
#include<memory>

//----------------------------------------Global Functions, Custom Data Types, Operator Overloadings----------------------------------------//


// This enum is used to create a custom data type to indicate all possible flags that can be put on the board.
enum flags {hit = 'x', empty = ' ', missed = 'o', h_ship = '-', v_ship = '|'};

/*
This enum is used to indicate two possible ship orientations when they are placed on the board by the user.
It is also used for torpedo orientation.
*/
enum orientation {Horizontal, Vertical};

bool is_number(const std::string& string);
void next_player_turn();

std::ostream& operator<<(std::ostream& output, const flags flag);
std::istream& operator>>(std::istream& input, orientation& orientation);
std::istream& operator>>(std::istream& input, std::pair<int, int>& location);

//-------------------------------------------------Board-------------------------------------------------//


/*
This is the board class. A 10x10 grid is used for the game.
It contains two grids: one for the player and one for his opponent.
Also, it contains methods to output it and alter the squares.
*/
class board
{
    private:
        std::vector<std::vector<flags>> my_grid;
        std::vector<std::vector<flags>> opponent_grid;
        const int size_x = 10;
        const int size_y = 10;

    public:
        board();
        ~board();

        void place_flag(const flags flag, const std::pair<int, int> location, const bool is_my_grid);
        friend void print_board(const board& board, const bool is_my_grid);
};

//-------------------------------------------------Ships-------------------------------------------------//


/*
This is the abstract base class for other ships.
It contains an empty shape (flag, location), a boolean indicating whether it is destroyed and different methods.
The location that the user inputs is the top left square of the ship. 
*/
class ship
{
    protected:
        bool is_destroyed{false};
        std::vector<std::pair<flags, std::pair<int, int>>> shape {}; // This is vector of pair of flag and its location

    public:
        virtual ~ship();

        virtual std::vector<std::pair<flags, std::pair<int, int>>>  return_shape() const;
        virtual void change_flag (const std::pair<int, int> location, const flags flag);
        virtual bool is_ship_destroyed () const;
};

// Derived class for two-square ship
class small_ship: public ship 
{
    public:
        small_ship(std::pair<int, int> location, const orientation orientation);
};

// Derived class for three-square ship
class medium_ship: public ship 
{
    public:
        medium_ship(std::pair<int, int> location, const orientation orientation);
};

// Derived class for five-square ship
class large_ship: public ship 
{
    public:
        large_ship(std::pair<int, int> location, const orientation orientation);
};

//-------------------------------------------------Abilities-------------------------------------------------//


// This is the abstract base class for player abilities/weapons. Contains hit and miss counts and method to use it. 
class ability 
{
    protected:
        int hit_count {0};
        int miss_count {0};

    public:
        // Pure virtual function for ability use.
        virtual void use_ability(std::vector<std::shared_ptr<ship>>& opponent_ships, board& my_board, board& opponent_board, int& my_energy, int& opponent_energy)=0;

        virtual int return_hit_count () const;
        virtual int return_miss_count () const;
};

/*
Derived class for canon.
This weapon's blast radius is one square, and it recharges every time the ship is hit. Otherwise, it runs out of charges.
*/
class canon: public ability 
{
    private:
        int canon_energy_requirement {1};

    public:
        canon() = default;
        
        void use_ability(std::vector<std::shared_ptr<ship>>& opponent_ships, board& my_board, board& opponent_board, int& my_energy, int& opponent_energy);
};

/*
Derived class for a bomb. Cross on the right inicates the used input location.   x 0
This weapon's blast area is four squares, and it uses energy (not canon energy). 0 0
*/
class bomb: public ability 
{
    private:
        int energy_requirement {5};

    public:
        bomb() = default;

        void use_ability(std::vector<std::shared_ptr<ship>>&  opponent_ships, board& my_board, board& opponent_board, int& my_energy, int& opponent_energy);
};


/*
Derived class for a torpedo.
This weapon's blast area is ten squares (whole row or column), and it uses energy (not canon energy).
To use it, user chooses location and then orientation (H for row and V for column).
*/
class torpedo: public ability 
{
    private :
        int energy_requirement {9};

    public:
        torpedo() = default;

        void use_ability(std::vector<std::shared_ptr<ship>>& opponent_ships, board& my_board, board& opponent_board, int& my_energy, int& opponent_energy);
};

//-------------------------------------------------Player-------------------------------------------------//


/*
This is the player class. It contains the bulk of the functionality and communicates with all the other classes. 
It encapsulates canon energy, energy (for all other weapons), vector of shared pointers to ships,
and vector of unique pointers to all the abilities. 
*/
class player
{
    private:
        int canon_energy {1};
        int energy {0};
        std::vector<std::shared_ptr<ship>> ships; // Shared pointers allow more than one pointer to the ship's location
        std::vector<std::unique_ptr<ability>> abilities;

        bool is_valid_ship_location (const std::pair<int, int> location, const orientation orientation, const char ship_type) const;

    public:
        player(board& board);
        ~player();

        void turn (player& opponent, board& my_board, board& opponent_board);
        void place_ships(board& board) const;
        std::vector<std::shared_ptr<ship>>& return_ships();
        bool is_defeated () const;
        int& return_canon_energy();
        int& return_energy();

        friend void print_ship_info (const player& player, const bool is_my_ships);
        friend void print_weapon_info (const player& player);
        friend void scoreboard (const player& player_1, const player& player_2);
};

void print_ship_info (const player& player, const bool is_my_ships);
 
void print_weapon_info (const player& player);

// Prints scoreboard after the game ends.
void scoreboard (const player& player_1, const player& player_2);
