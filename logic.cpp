#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"
#include <sstream>

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    ifstream ifs(fileName);
    int door_counter = 0;
    int exit_counter = 0;

    
    if (!ifs.is_open()){
        return nullptr;
    }
    string line;
    
    //if maxRow or maxCol fail, (integer overflow)
    ifs >> maxRow;
    if (ifs.fail()){
        return nullptr;
    }
    if (maxRow <= 0){
        return nullptr;
    }
    ifs >> maxCol;
    if (ifs.fail()){
        return nullptr;
    }
    if (maxCol <= 0){
        return nullptr;
    }
    //check for integer overflow
    if (INT32_MAX/maxRow < maxCol){
        return nullptr;
    }

    ifs >> player.row;
    if (ifs.fail()){
        return nullptr;
    }
    if (player.row < 0 || player.row >= maxRow){
        return nullptr;
    }
    ifs >> player.col;
    if (ifs.fail()){
        return nullptr;
    }
    if (player.col < 0 || player.col >= maxRow){
        return nullptr;
    }
    
    char** grid = createMap(maxRow, maxCol);

    for (int i = 0; i < maxRow; i++){
        for (int j = 0; j < maxCol; j++){
            ifs >> grid[i][j];
            if (ifs.fail()){
                deleteMap(grid, maxRow);
                return nullptr;
            }
        }
    }

    //validate feasible dimensions
    if (maxRow < 1 || maxCol < 1){
        deleteMap(grid, maxRow);
        return nullptr;
    }

    //validates player location is on map
    if (player.row < 0 || player.row >= maxRow || player.col < 0 || player.col >= maxCol){
        deleteMap(grid, maxRow);
        return nullptr;
    }

    //validtes the presence of door or exit
    for (int i = 0; i < maxRow; i++){
        for (int j = 0; j < maxCol; j++){
            if (grid[i][j] == TILE_DOOR){
                door_counter++;
            }
            if (grid[i][j] == TILE_EXIT){
                exit_counter++;
            }
        }
    }
    if (door_counter < 1 && exit_counter < 1){
        deleteMap(grid, maxRow);
        return nullptr;
    }

    //validates that all characters are TILES
    for (int i = 0; i < maxRow; i++){
        for (int j = 0; j < maxCol; j++){
            if (grid[i][j] != TILE_DOOR && grid[i][j] != TILE_AMULET
            && grid[i][j] != TILE_OPEN && grid[i][j] != TILE_PLAYER 
            && grid[i][j] != TILE_TREASURE && grid[i][j] != TILE_MONSTER 
            && grid[i][j] != TILE_PILLAR && grid[i][j] != TILE_EXIT){
                deleteMap(grid, maxRow);
                return nullptr;
            }
        }
    }

    //validates that there are not too many data points or too few
    char extra_var;
    ifs >> extra_var;
    if (ifs.good()){
        deleteMap(grid, maxRow);
        return nullptr;
    }



    grid[player.row][player.col] = TILE_PLAYER;

    return grid;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    if (input == MOVE_UP){
        nextRow --;
    }
    if (input == MOVE_DOWN){
        nextRow ++;
    }
    if (input == MOVE_LEFT){
        nextCol --;
    }
    if (input == MOVE_RIGHT){
        nextCol ++;
    }
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    char** grid = new char*[maxRow];
    for (int i = 0; i < maxRow; i++){
        grid[i] = new char [maxCol];
    }

    //initializing array
    for (int i = 0; i < maxRow; i++){
        for (int j = 0; j < maxCol; j++){
            grid[i][j] = 'x';
        }
    }


    return grid;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    if (!(map==nullptr)){
        for (int i = 0; i < maxRow; i++){
            delete[] map[i];
        }
        delete[] map;
    }
    map = nullptr;
    maxRow = 0;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    int player_counter = 0;
    //new array allocation
    int bigRow = 2;
    int bigCol = 2;

    if (INT32_MAX / 2 < maxRow || INT32_MAX / 2 < maxCol){
        return nullptr;
    }
    if (INT32_MAX /bigRow < bigCol){
        return nullptr;
    }

    bigRow = bigRow * maxRow;
    bigCol = bigCol * maxCol;


    char** big_map = new char* [bigRow];
    for (int i = 0; i < bigRow; i++){
        big_map[i] = new char [bigCol];
    }

    for (int i = 0; i < bigRow; i++){
        for (int j = 0; j < bigCol; j++){
            big_map[i][j] = TILE_OPEN;
        }
    }


    //copying into new array
    //Top Left
    for (int i = 0; i < maxRow; i++){
        for (int j = 0; j < maxCol; j++){
            big_map[i][j] = map[i][j];
        }
    }
    //Top Right
    for (int i = 0; i < maxRow; i++){
        for (int j = 0; j < maxCol; j++){
            big_map[i][j + maxCol] = map[i][j];
        }
    }
    //Bottom Left
    for (int i = 0; i < maxRow; i++){
        for (int j = 0; j < maxCol; j++){
            big_map[i + maxRow][j] = map[i][j];
        }
    }
    //Bottom Right
    for (int i = 0; i < maxRow; i++){
        for (int j = 0; j < maxCol; j++){
            big_map[i + maxRow][j + maxCol] = map[i][j];
        }
    }

    //fixing the big map by removing duplicate players
    for (int i = 0; i < bigRow; i++){
        for (int j = 0; j < bigCol; j++){
            if (big_map[i][j] == TILE_PLAYER){
                player_counter++;
                if (player_counter > 1){
                    big_map[i][j] = TILE_OPEN;
                }
            }
        }
    }

    //deallocate old array
    for (int i = 0; i < maxRow; i++){
        delete[] map[i];
    }
    delete[] map;

    //set old array to new array
    map = big_map;



    maxRow = maxRow * 2;
    maxCol = maxCol * 2;

    return big_map;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    int status;
    //Handles out of array bounds
    if (nextRow >= maxRow || nextRow < 0){
        status = STATUS_STAY;
        nextRow = player.row;
        nextCol = player.col;
    }
    else if (nextCol >= maxCol || nextCol < 0){
        status = STATUS_STAY;
        nextRow = player.row;
        nextCol = player.col;
    }
    
    //Special tiles
    else if (map[nextRow][nextCol] == TILE_MONSTER || map[nextRow][nextCol] == TILE_PILLAR){
        status = STATUS_STAY;
        nextRow = player.row;
        nextCol = player.col;
    }
    else if (map[nextRow][nextCol] == TILE_TREASURE){
        status = STATUS_TREASURE;
        player.treasure++;
    }
    else if (map[nextRow][nextCol] == TILE_AMULET){
        status = STATUS_AMULET;
    }
    else if (map[nextRow][nextCol] == TILE_DOOR){
        status = STATUS_LEAVE;
    }
    else if (map[nextRow][nextCol] == TILE_EXIT){
        if (player.treasure > 0){
            status = STATUS_ESCAPE;
        }
        else {
            status = STATUS_STAY;
            nextRow = player.row;
            nextCol = player.col;
        }
    }

    //Normal move
    else {
        status = STATUS_MOVE;
        /*map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;
        player.row = nextRow;
        player.col = nextCol;*/
    }

    if (status != STATUS_STAY){
        map[player.row][player.col] = TILE_OPEN;
        map[nextRow][nextCol] = TILE_PLAYER;
        player.row = nextRow;
        player.col = nextCol;
    }
    
    return status;
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    //check up
    for (int i = player.row; i >= 0; i--){
        if (map[i][player.col] == TILE_PILLAR){
            break;
        }
        if (map[i][player.col] == TILE_MONSTER){
            map[i][player.col] = TILE_OPEN;
            map[i + 1][player.col] = TILE_MONSTER;
        }
    }
    //check down
    for (int i = player.row + 1; i < maxRow; i++){
        if (map[i][player.col] == TILE_PILLAR){
            break;
        }
        if (map[i][player.col] == TILE_MONSTER){
            map[i][player.col] = TILE_OPEN;
            map[i - 1][player.col] = TILE_MONSTER;
        }
    }
    //check Left
    for (int i = player.col; i >= 0; i--){
        if (map[player.row][i] == TILE_PILLAR){
            break;
        }
        if (map[player.row][i] == TILE_MONSTER){
            map[player.row][i] = TILE_OPEN;
            map[player.row][i + 1] = TILE_MONSTER;
        }
    }
    //check Right
    for (int i = player.col + 1; i < maxCol; i++){
        if (map[player.row][i] == TILE_PILLAR){
            break;
        }
        if (map[player.row][i] == TILE_MONSTER){
            map[player.row][i] = TILE_OPEN;
            map[player.row][i - 1] = TILE_MONSTER;
        }
    }
    //check if monster has killed player
    if (map[player.row][player.col] == TILE_MONSTER){
        return true;
    }

    return false;
}
