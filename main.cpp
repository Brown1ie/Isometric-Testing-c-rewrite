#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <random>
#include <algorithm>
#include <map>
#include <SDL_ttf.h>
#include <string>
#include <cmath>

// Define constants for window size and map size
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int MAP_WIDTH = 40;
const int MAP_HEIGHT = 40;
const int TILE_SIZE = 32;
const int ISLAND_SIZE = 25;
const int NUM_ISLANDS = 15;  // Number of islands to generate
const int HALF_TILE = 16;
const int gridYOffset = 10;

bool gridVisible = false;  // Initially, the grid is not visible



std::string mouseCollisionDetection(int mouseX, int mouseY, const std::vector<SDL_Rect>& targets, const std::vector<std::string>& targetNames) {
    //std::cout << "Mouse Position: (" << mouseX << ", " << mouseY << ")" << std::endl; // Debug statement

    for (size_t i = 0; i < targets.size(); ++i) {
        const SDL_Rect& target = targets[i];
        //std::cout << "Checking target: " << targetNames[i] << " at (" << target.x << ", " << target.y << ", " << target.w << ", " << target.h << ")" << std::endl; // Debug statement

        if (mouseX > target.x && mouseX < target.x + target.w && mouseY > target.y && mouseY < target.y + target.h) {
            std::cout << "Mouse is over target: " << targetNames[i] << std::endl; // Debug statement
            return targetNames[i];
        }
    }
    return "";
}






SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load surface: " << path << " SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    // Set color key to remove black background
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Failed to create texture: " << path << " SDL_Error: " << SDL_GetError() << std::endl;
    }
    return texture;
}



std::pair<int, int> isoToGrid(int isoX, int isoY) {
    int x = (isoX / (TILE_SIZE / 2) + isoY / (TILE_SIZE / 4)) / 2;
    int y = (isoY / (TILE_SIZE / 4) - isoX / (TILE_SIZE / 2)) / 2;

    //std::cout << "isoToGrid - isoX: " << isoX << ", isoY: " << isoY << " -> gridX: " << x << ", gridY: " << y << std::endl;

    return { y , x };
}


std::pair<int, int> screenToIsometricGrid(int screenX, int screenY, int camX, int camY) {
    // Adjust screen coordinates by camera position
    int adjustedX = screenX + camX;
    int adjustedY = screenY + camY;

    // Convert to isometric grid coordinates
    int x = (adjustedY / (TILE_SIZE / 4) - adjustedX / (TILE_SIZE / 2)) / 2;
    int y = (adjustedY / (TILE_SIZE / 4) + adjustedX / (TILE_SIZE / 2)) / 2;

    //std::cout << "screenToIsometricGrid - screenX: " << screenX << ", screenY: " << screenY << ", camX: " << camX << ", camY: " << camY << " -> gridX: " << x << ", gridY: " << y << std::endl;

    return { y , x };
}


// Player class
//class Player {
//public:
//    Player(int x, int y, SDL_Texture* texture) : posX(x), posY(y), texture(texture) {
//        rect.w = TILE_SIZE;
//        rect.h = TILE_SIZE;
//    }
//
//    void handleEvent(SDL_Event& e, const std::vector<std::vector<int>>& map) {
//        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
//            int newX = posX;
//            int newY = posY;
//
//
//            switch (e.key.keysym.sym) {
//            case SDLK_w:
//                newY -= (0.5 * HALF_TILE);
//                newX += (0.5 * TILE_SIZE);
//                break;
//            case SDLK_s:
//                newY += (0.5 * HALF_TILE);
//                newX -= (0.5 * TILE_SIZE);
//                break;
//            case SDLK_a:
//                //posX -= (TILE_SIZE);
//                newY -= (0.5 * HALF_TILE);
//                //posX += (0.5 * TILE_SIZE);
//                //posY -= (0.5 * HALF_TILE);
//                newX -= (HALF_TILE);
//                break;
//            case SDLK_d:
//                //posX += (TILE_SIZE);
//                newY += (0.5 * HALF_TILE);
//                //posX -= (0.5 * TILE_SIZE);
//                //posY += (0.5 * HALF_TILE);
//                newX += (HALF_TILE);
//                break;
//            }
//        
//
//            // Convert new player coordinates to grid coordinates 
//            std::pair<int, int> gridPos = isoToGrid(newX, newY - (TILE_SIZE / 4));
//
//            // Convert current player coordinates to grid coordinates 
//            std::pair<int, int> currentGridPos = isoToGrid(posX, posY - (TILE_SIZE / 4)); 
//            
//            // Convert new player coordinates to grid coordinates 
//            std::pair<int, int> newGridPos = isoToGrid(newX, newY - (TILE_SIZE / 4)); 
//
//
//            //std::cout << "Current Grid Position: (" << currentGridPos.first << ", " << currentGridPos.second << ")" << std::endl; 
//            //std::cout << "New Grid Position: (" << newGridPos.first << ", " << newGridPos.second << ")" << std::endl;
//            
//            // Check if new position is within map bounds and over a block 
//            if (newGridPos.first + 2 >= 0 && newGridPos.first + 2 < MAP_WIDTH && newGridPos.second + 2 >= 0 && newGridPos.second + 2 < MAP_HEIGHT) { 
//                if (map[newGridPos.second + 2][newGridPos.first + 2 ] != 0) { 
//                    // Update actual player coordinates 
//                    posX = newX; 
//                    posY = newY; 
//                } 
//                else { 
//                    std::cout << "Cannot move to: (" << newGridPos.first + 2 << ", " << newGridPos.second + 3 << ") - No block present" << std::endl; } 
//            } else { 
//                std::cout << "Out of bounds move to: (" << newGridPos.first + 2 << ", " << newGridPos.second + 3<< ")" << std::endl; 
//            }
//        }
//    
//    }
//
//    void render(SDL_Renderer* renderer, int camX, int camY) {
//        rect.x = posX - camX;
//        rect.y = posY - camY;
//        SDL_RenderCopy(renderer, texture, NULL, &rect);
//    }
//
//    int getPosX()  const { return posX; }
//    int getPosY() const { return posY; }
//
//private:
//    int posX, posY;
//    SDL_Rect rect;
//    SDL_Texture* texture;
//};

class Player {
public:
    enum Direction {
        UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT, NONE
    };

    Player(int x, int y, SDL_Texture* initialTexture) : posX(x), posY(y), texture(initialTexture), direction(UP), lastMoveTime(0), moveInterval(250) {
        rect.w = TILE_SIZE;
        rect.h = TILE_SIZE;
        std::fill_n(keyStates, 256, false);
    }

    void setTexture(SDL_Texture* newTexture) {
        texture = newTexture;
    }

    /*void handleEvent(SDL_Event& e, const std::vector<std::vector<int>>& map) {*/
    void handleEvent(SDL_Event& e) {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            keyStates[e.key.keysym.sym] = true;
            updateDirection();
            //move(map);
        }
        else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
            keyStates[e.key.keysym.sym] = false;
            updateDirection();
        }
    }

    void updateDirection() {
        //      UP
        //  LEft    Right
        //      Down

        if (keyStates[SDLK_w] && keyStates[SDLK_d]) {
            direction = UP;
        }
        else if (keyStates[SDLK_w] && keyStates[SDLK_a]) {
            direction = LEFT;
        }
        else if (keyStates[SDLK_s] && keyStates[SDLK_d]) {
            direction = RIGHT;
        }
        else if (keyStates[SDLK_s] && keyStates[SDLK_a]) {
            direction = DOWN;
        }
        else if (keyStates[SDLK_w]) {
            direction = UP_RIGHT;
        }
        else if (keyStates[SDLK_d]) {
            direction = DOWN_RIGHT;
        }
        else if (keyStates[SDLK_s]) {
            direction = DOWN_LEFT;
        }
        else if (keyStates[SDLK_a]) {
            direction = UP_LEFT;
        }
        else {
            direction = NONE; //no movement keys are being pressed
        }
    }

    void move(const std::vector<std::vector<int>>& map) {

        Uint32 currentTime = SDL_GetTicks(); 
        if (currentTime - lastMoveTime < moveInterval) {
            return; // Not enough time has passed since the last move 
        }

        int newX = posX;
        int newY = posY;

        switch (direction) {
        case UP_RIGHT:                    //w key
            newY -= (0.5 * HALF_TILE);
            newX += (0.5 * TILE_SIZE);
            break;
        case UP:                             //W + D keys
            //w component
            newY -= (0.5 * HALF_TILE);
            newX += (0.5 * TILE_SIZE);

            //d component
            //posX += (TILE_SIZE);
            newY += (0.5 * HALF_TILE);
            //posX -= (0.5 * TILE_SIZE);
            //posY += (0.5 * HALF_TILE);
            newX += (HALF_TILE);

            break;
        case DOWN_RIGHT:     //D key
            //posX += (TILE_SIZE);
            newY += (0.5 * HALF_TILE);
            //posX -= (0.5 * TILE_SIZE);
            //posY += (0.5 * HALF_TILE);
            newX += (HALF_TILE);
            break;
        case RIGHT:      //S + D keys
            //s component
            newY += (0.5 * HALF_TILE);
            newX -= (0.5 * TILE_SIZE);

            //d component
            //posX += (TILE_SIZE);
            newY += (0.5 * HALF_TILE);
            //posX -= (0.5 * TILE_SIZE);
            //posY += (0.5 * HALF_TILE);
            newX += (HALF_TILE);


            break;
        case DOWN_LEFT:   // S keys
            newY += (0.5 * HALF_TILE);
            newX -= (0.5 * TILE_SIZE);
            break;
        case DOWN:  //S + A keys
            //s component
            newY += (0.5 * HALF_TILE);
            newX -= (0.5 * TILE_SIZE);

            //a component
            //posX -= (TILE_SIZE);
            newY -= (0.5 * HALF_TILE);
            //posX += (0.5 * TILE_SIZE);
            //posY -= (0.5 * HALF_TILE);
            newX -= (HALF_TILE);

            break;
        case UP_LEFT: // A key
            //posX -= (TILE_SIZE);
            newY -= (0.5 * HALF_TILE);
            //posX += (0.5 * TILE_SIZE);
            //posY -= (0.5 * HALF_TILE);
            newX -= (HALF_TILE);
            break;
        case LEFT: // W+A key
            //w component
            newY -= (0.5 * HALF_TILE);
            newX += (0.5 * TILE_SIZE);

            //a component
            //posX -= (TILE_SIZE);
            newY -= (0.5 * HALF_TILE);
            //posX += (0.5 * TILE_SIZE);
            //posY -= (0.5 * HALF_TILE);
            newX -= (HALF_TILE);

            break;
        }

        std::pair<int, int> newGridPos = isoToGrid(newX, newY - (TILE_SIZE / 4));

        if (newGridPos.first + 2 >= 0 && newGridPos.first + 2 < MAP_WIDTH && newGridPos.second + 2 >= 0 && newGridPos.second + 2 < MAP_HEIGHT) {
            if (map[newGridPos.second + 2][newGridPos.first + 2] != 0) {
                posX = newX;
                posY = newY;
                lastMoveTime = currentTime; 
            }
            else {
                std::cout << "Cannot move to: (" << newGridPos.first + 2 << ", " << newGridPos.second + 3 << ") - No block present" << std::endl;
            }
        }
        else {
            std::cout << "Out of bounds move to: (" << newGridPos.first + 2 << ", " << newGridPos.second + 3 << ")" << std::endl;
        }
    }

    void updateTexture(std::map<Direction, SDL_Texture*>& textures) {
        setTexture(textures[direction]);
    }

    void render(SDL_Renderer* renderer, int camX, int camY) {
        rect.x = posX - camX;
        rect.y = posY - camY;
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }

    void update(const std::vector<std::vector<int>>& map, std::map<Direction, SDL_Texture*>& textures) {
        //updateDirection();
        if (keyStates[SDLK_w % 256] || keyStates[SDLK_d % 256] || keyStates[SDLK_s % 256] || keyStates[SDLK_a % 256]) {
            move(map);
        }
        if (direction != NONE) {
            updateTexture(textures);
        }
        
    }

    int getPosX() const { return posX; }
    int getPosY() const { return posY; }

private:
    int posX, posY;
    SDL_Rect rect;
    SDL_Texture* texture;
    Direction direction;
    bool keyStates[256]; // Track the state of each key
    Uint32 lastMoveTime; // Track the last move time 
    Uint32 moveInterval; // Interval between moves in milliseconds
};


// Function to generate islands
void generateIsland(std::vector<std::vector<int>>& map, int startX, int startY, int islandNumber) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dirDis(0, 3);

    const int maxIslandSize = ISLAND_SIZE;
    int currentSize = 0;
    std::vector<std::pair<int, int>> stack = { {startX, startY} };
    map[startY][startX] = islandNumber;  // Set starting tile with the island number

    while (currentSize < maxIslandSize && !stack.empty()) {
        int index = dirDis(gen) % stack.size();
        int x = stack[index].first;
        int y = stack[index].second;

        std::vector<std::pair<int, int>> directions = {
            {0, -1}, {0, 1}, {-1, 0}, {1, 0}
        };

        std::shuffle(directions.begin(), directions.end(), gen);

        bool placedTile = false;
        for (const auto& dir : directions) {
            int newX = x + dir.first;
            int newY = y + dir.second;
            if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT && map[newY][newX] == 0) {
                map[newY][newX] = islandNumber;  // Update with the island number
                stack.push_back({ newX, newY });
                placedTile = true;
                currentSize++;
                break;
            }
        }

        if (!placedTile) {
            stack.erase(stack.begin() + index);
        }
    }
}



// Function to generate the map with multiple islands
std::vector<std::vector<int>> generateMap() {
    std::vector<std::vector<int>> map(MAP_HEIGHT, std::vector<int>(MAP_WIDTH, 0));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, MAP_HEIGHT - 1);
    std::uniform_int_distribution<> islandNumDis(1, 4);

    // First island at (6, 6) with island number 1
    generateIsland(map, 6, 6, 1);

    // Generate remaining islands
    for (int i = 1; i < NUM_ISLANDS; ++i) {
        int x = dis(gen);
        int y = dis(gen);
        int islandNumber = islandNumDis(gen);
        generateIsland(map, x, y, islandNumber);
    }

    return map;
}




std::vector<std::vector<int>> testIsland() {
    return {
        {0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
}








// Function to render the map
void renderMap(SDL_Renderer* renderer, const std::map<int, SDL_Texture*>& textures, const std::vector<std::vector<int>>& map, int camX, int camY) {
    SDL_Rect dstRect;
    dstRect.w = TILE_SIZE;
    dstRect.h = TILE_SIZE;

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            int tileType = map[y][x];
            if (map[y][x] != 0) {
                // Isometric transformation
                //int isoX = (x - y) * (TILE_SIZE / 2) - camX; //use this for isometric directed to left
                int isoX = -(x - y) * (TILE_SIZE / 2) - camX;   //use this one for isometric directed to right(like origial python ver)
                int isoY = (x + y) * (TILE_SIZE / 4) - camY;
                dstRect.x = isoX;
                dstRect.y = isoY;

                // Render the appropriate texture 
                auto it = textures.find(tileType); 
                if (it != textures.end()) { 
                   SDL_RenderCopy(renderer, it->second, NULL, &dstRect); 
                }
            }
        }
    }
}

void printMap(const std::vector<std::vector<int>>& map) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            std::cout << map[y][x] << " ";
        }
        std::cout << std::endl;
    }
}



SDL_Texture* renderText(const std::string& message, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
    if (!surface) {
        std::cerr << "Failed to create surface. TTF_Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "Failed to create texture. SDL_Error: " << SDL_GetError() << std::endl;
    }
    return texture;
}



void renderIsometricGrid(SDL_Renderer* renderer, int camX, int camY, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White color for grid lines

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            // Isometric transformation
            //int gridYOffset = 10;
            int isoX1 = -(x - y) * (TILE_SIZE / 2) - camX;
            int isoY1 = (x + y) * (TILE_SIZE / 4) - camY - gridYOffset;
            int isoX2 = -((x + 1) - y) * (TILE_SIZE / 2) - camX;
            int isoY2 = ((x + 1) + y) * (TILE_SIZE / 4) - camY - gridYOffset;
            int isoX3 = -(x - (y + 1)) * (TILE_SIZE / 2) - camX;
            int isoY3 = (x + (y + 1)) * (TILE_SIZE / 4) - camY - gridYOffset;

            // Draw horizontal line
            SDL_RenderDrawLine(renderer, isoX1, isoY1, isoX2, isoY2);

            // Draw vertical line
            SDL_RenderDrawLine(renderer, isoX1, isoY1, isoX3, isoY3);
        }
    }

    // Render X-axis label close to the grid's origin 
    std::string xLabel = "X";
    SDL_Texture* xTexture = renderText(xLabel, font, { 255, 255, 255, 255 }, renderer);
    int xWidth, xHeight; SDL_QueryTexture(xTexture, NULL, NULL, &xWidth, &xHeight);
    SDL_Rect xRect = { -(0 - 1) * (TILE_SIZE / 2) - camX - 90, (0 + 1) * (TILE_SIZE / 4) - camY - 30 , xWidth, xHeight };
    SDL_RenderCopy(renderer, xTexture, NULL, &xRect);
    SDL_DestroyTexture(xTexture);

    // Render Y-axis label close to the grid's origin 
    std::string yLabel = "Y";
    SDL_Texture* yTexture = renderText(yLabel, font, { 255, 255, 255, 255 }, renderer);
    int yWidth, yHeight; SDL_QueryTexture(yTexture, NULL, NULL, &yWidth, &yHeight);
    SDL_Rect yRect = { -(0 - 1) * (TILE_SIZE / 2) - camX + 40, (0 + 1) * (TILE_SIZE / 4) - camY - 30, yWidth, yHeight };
    SDL_RenderCopy(renderer, yTexture, NULL, &yRect);
    SDL_DestroyTexture(yTexture);





}









//#include <cmath>

// Helper function to draw a filled circle
void drawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
            }
        }
    }
}

// Function to create a rounded rectangle texture
SDL_Texture* createRoundedRectangle(SDL_Renderer* renderer, int width, int height, SDL_Color color, int radius) {
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // Drawing the rounded rectangle
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Draw the corners
    drawFilledCircle(renderer, radius, radius, radius, color); // Top-left corner
    drawFilledCircle(renderer, width - radius - 1, radius, radius, color); // Top-right corner
    drawFilledCircle(renderer, radius, height - radius - 1, radius, color); // Bottom-left corner
    drawFilledCircle(renderer, width - radius - 1, height - radius - 1, radius, color); // Bottom-right corner

    // Draw filled rectangles for the sides and center
    SDL_Rect rect = { radius, 0, width - 2 * radius, height };
    SDL_RenderFillRect(renderer, &rect);
    rect = { 0, radius, width, height - 2 * radius };
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderTarget(renderer, NULL);
    return texture;
}



// use to label inventory stuff

///////////////////////
// Function to create a rounded rectangle texture
SDL_Texture* createRoundedRectangle(SDL_Renderer* renderer, int width, int height, SDL_Color color, int radius);

class TextBox {
public:
    TextBox(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color textColor, SDL_Color bgColor, int textSize, int bgWidth, int bgHeight, int x, int y)
        : renderer(renderer), font(font), text(text), textColor(textColor), bgColor(bgColor), textSize(textSize), bgWidth(bgWidth), bgHeight(bgHeight), posX(x), posY(y) {
        createBackground();
        createTextTexture();
    }

    virtual ~TextBox() {
        if (textTexture) {
            SDL_DestroyTexture(textTexture);
        }
        if (bgTexture) {
            SDL_DestroyTexture(bgTexture);
        }
    }

    void createBackground() {
        bgTexture = createRoundedRectangle(renderer, bgWidth, bgHeight, bgColor, 10);
    }

    void createTextTexture() {
        if (textTexture) {
            SDL_DestroyTexture(textTexture);
        }
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
        if (!textSurface) {
            std::cerr << "TTF_RenderText_Blended: " << TTF_GetError() << std::endl;
            return;
        }
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (!textTexture) {
            std::cerr << "SDL_CreateTextureFromSurface: " << SDL_GetError() << std::endl;
        }
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;
        SDL_FreeSurface(textSurface);
    }

    void setPosition(int x, int y) {
        posX = x;
        posY = y;
    }

    virtual void render() {
        // Render background
        SDL_Rect bgRect = { posX, posY, bgWidth, bgHeight };
        SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);

        // Render text
        textRect.x = posX + (bgWidth - textRect.w) / 2;
        textRect.y = posY + (bgHeight - textRect.h) / 2;
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    }

    void setText(const std::string& newText) {
        text = newText;
        createTextTexture();
    }

protected:
    SDL_Renderer* renderer;
    TTF_Font* font;
    std::string text;
    SDL_Color textColor;
    SDL_Color bgColor;
    int textSize;
    int bgWidth;
    int bgHeight;
    int posX;
    int posY;
    SDL_Texture* textTexture = nullptr;
    SDL_Texture* bgTexture = nullptr;
    SDL_Rect textRect;
};

////////////////////////////////////////////////



/////objective box stuff


// Function to wrap text to fit within a specified width
std::vector<std::string> wrapText(TTF_Font* font, const std::string& text, int maxWidth) {
    std::vector<std::string> lines;
    std::istringstream words(text);
    std::string word;
    std::string line;

    while (words >> word) {
        std::string testLine = line + (line.empty() ? "" : " ") + word;
        int width;
        TTF_SizeText(font, testLine.c_str(), &width, nullptr);
        if (width > maxWidth) {
            if (!line.empty()) {
                lines.push_back(line);
                line.clear();
            }
            line = word;
        }
        else {
            line = testLine;
        }
    }
    if (!line.empty()) {
        lines.push_back(line);
    }

    return lines;
}

class ObjectiveBox : public TextBox {
public:
    ObjectiveBox(SDL_Renderer* renderer, TTF_Font* font, const std::string& title, const std::vector<std::string>& objectives, int textSize, int bgWidth, int bgHeight, int x, int y)
        : TextBox(renderer, font, title, { 255, 255, 255, 255 }, { 0, 0, 0, 128 }, textSize, bgWidth, bgHeight, x, y),
        objectives(objectives), activeObjective(0) {
        createObjectiveTextures();
    }

    void completeObjective(size_t index) {
        if (index < objectives.size()) {
            objectiveColors[index] = { 255, 0, 0, 255 }; // Red for completed
            if (index + 1 < objectives.size()) {
                objectiveColors[index + 1] = { 255, 255, 255, 255 }; // White for the next active
                activeObjective = index + 1; // Update the active objective
            }
        }
    }

    void render() override {
        // Render background
        SDL_Rect bgRect = { posX, posY, bgWidth, bgHeight };
        SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);

        // Render title at the top of the box
        SDL_Surface* titleSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
        SDL_Rect titleRect = { posX + 10, posY + 10, titleSurface->w, titleSurface->h };
        SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
        SDL_DestroyTexture(titleTexture);
        SDL_FreeSurface(titleSurface);

        // Render the objectives below the title
        int offsetY = posY + titleRect.h + 20; // Start rendering below the title
        for (size_t i = 0; i < objectives.size(); ++i) {
            std::vector<std::string> wrappedText = wrapText(font, objectives[i], bgWidth - 20);
            for (const std::string& line : wrappedText) {
                SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), objectiveColors[i]);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect rect = { posX + 10, offsetY, surface->w, surface->h };
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
                offsetY += surface->h + 5; // Adjust offset for the next line
            }
        }
    }

private:
    std::vector<std::string> objectives;
    std::vector<SDL_Color> objectiveColors;
    size_t activeObjective;

    void createObjectiveTextures() {
        objectiveColors.resize(objectives.size(), { 0, 0, 0, 255 }); // Black for inactive
        if (!objectives.empty()) {
            objectiveColors[0] = { 255, 255, 255, 255 }; // White for active
        }
    }
};




///////////////////




void renderInventory(SDL_Renderer* renderer, SDL_Texture* selectedTexture, const std::vector<SDL_Texture*>& inventoryTextures, SDL_Texture* greyBackground, SDL_Texture* greenSelectionBackground, TTF_Font* font, SDL_Color textColor) {
    int x = 225; // Starting position for inventory slots
    int y = 50; // Position for the inventory bar

    // Render grey background for the inventory bar
    SDL_Rect backgroundRect = { x - 10, y - 10, 60 * inventoryTextures.size() + 20, 60 };
    SDL_RenderCopy(renderer, greyBackground, NULL, &backgroundRect);

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    for (size_t i = 0; i < inventoryTextures.size(); ++i) {
        SDL_Rect dstRect = { x + i * 60, y - 5, 50, 50 };

        // Render green selection background if this slot is selected
        if (inventoryTextures[i] == selectedTexture) {
            SDL_RenderCopy(renderer, greenSelectionBackground, NULL, &dstRect);
        }

        // Render the inventory texture
        SDL_RenderCopy(renderer, inventoryTextures[i], NULL, &dstRect);

        
    }

    
    
}




// unlock criteria for solar block
bool grassPlaced = false, stonePlaced = false, sandPlaced = false, snowPlaced = false;

bool SolarPlaced = false, TeleportPlaced = false, powerExceeded25 = false;

bool standingOnTeleport = false;

std::vector<std::pair<int, int>> teleportBlockLocations;


// Function to print the locations of all teleport blocks on the map 
void printTeleportBlockLocations(const std::vector<std::vector<int>>& map) { 
    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            if (map[y][x] == 6) {
                std::cout << "Teleport block at: (" << x << ", " << y << ")" << std::endl;
            }
        }
    }
}

void findTeleportBlockLocations(const std::vector<std::vector<int>>& map, std::vector<std::pair<int, int>>& teleportBlockLocations) {
    teleportBlockLocations.clear();  // Clear the list before updating
    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            if (map[y][x] == 6) {  // Assuming 6 is the teleport block
                teleportBlockLocations.emplace_back(x, y);
                std::cout << "Teleport block at: (" << x << ", " << y << ")" << std::endl;
            }
        }
    }
}


bool checkUnlockCriteria() {
    return grassPlaced && stonePlaced && sandPlaced && snowPlaced;
}

int power = 0;
Uint32 lastUpdateTime = 0;
const int updateInterval = 3000; // 3 seconds in milliseconds
int solarBlockCount = 0;



int main(int argc, char* argv[]) {
    std::cout << "Initializing SDL..." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL. SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Isometric Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window. SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer. SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::cout << "Initializing SDL_ttf..." << std::endl;
    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize SDL_ttf. TTF_Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("assets/font.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font. TTF_Error: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TTF_Init(); 
    TTF_Font* UIfont = TTF_OpenFont("assets/UIFont.ttf", 16); // Load the UIFont 
    if (!font) { 
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl; 
        return -1; 
    }
    SDL_Color WHITE = { 255, 255, 255, 255 }; // White text color
    SDL_Color SEMIBLACK = { 0, 0, 0, 125 }; // nlack semi transparent color




    //labels
    TextBox GrassLabel(renderer, UIfont, "Grass", WHITE, SEMIBLACK, 8, 50, 25, 225, 20);  //text col, bg col, size of text, size of backgroundx, size of bg y, positionx, position y
    TextBox SandLabel(renderer, UIfont, "Sand", WHITE, SEMIBLACK, 8, 50, 25, 285, 20);  //text col, bg col, size of text, size of backgroundx, size of bg y, positionx, position y
    TextBox StoneLabel(renderer, UIfont, "Stone", WHITE, SEMIBLACK, 8, 50, 25, 345, 20);  //text col, bg col, size of text, size of backgroundx, size of bg y, positionx, position y
    TextBox SnowLabel(renderer, UIfont, "Snow", WHITE, SEMIBLACK, 8, 50, 25, 405, 20);  //text col, bg col, size of text, size of backgroundx, size of bg y, positionx, position y
    TextBox SolarLabel(renderer, UIfont, "Solar Block", WHITE, SEMIBLACK, 8, 125, 25, 425, 20);  //text col, bg col, size of text, size of backgroundx, size of bg y, positionx, position y
    TextBox TeleportLabel(renderer, UIfont, "Teleport Block", WHITE, SEMIBLACK, 8, 125, 25, 490, 20);  //text col, bg col, size of text, size of backgroundx, size of bg y, positionx, position y

    

    /////////////////////////////////////
    // Define the target objects (blocks in the inventory) 
    std::vector<SDL_Rect> inventoryTargets = { 
        {225, 50, 50, 50}, // Grass Block 
        {285, 50, 50, 50}, // Sand Block 
        {345, 50, 50, 50}, // Stone Block 
        {405, 50, 50, 50}, // Snow Block 
        {465, 50, 50, 50}, // Solar Block 
        {525, 50, 50, 50} // Teleport Block 
    };

    std::vector<std::string> targetNames = { 
        "Grass Block", 
        "Sand Block", 
        "Stone Block", 
        "Snow Block", 
        "Solar Block", 
        "Teleport Block" 
    };

    /////////////////////////////////////////

    IMG_Init(IMG_INIT_PNG);

    // Load textures
    SDL_Texture* grassTexture = LoadTexture("assets/grass.png", renderer);
    SDL_Texture* stoneTexture = LoadTexture("assets/stone.png", renderer);
    SDL_Texture* sandTexture = LoadTexture("assets/sand.png", renderer);
    SDL_Texture* snowTexture = LoadTexture("assets/snow.png", renderer);
    SDL_Texture* GrassSelectionTexture = LoadTexture("assets/grassSelection.png", renderer);
    SDL_Texture* SolarGrassTexture = LoadTexture("assets/Solargrass.png", renderer);
    SDL_Texture* TeleportTexture = LoadTexture("assets/teleport.png", renderer);
    SDL_Texture* playerTexture = LoadTexture("assets/bluetriangle.png", renderer);

    if (!grassTexture || !stoneTexture || !sandTexture || !playerTexture || !snowTexture || !GrassSelectionTexture || !SolarGrassTexture || !TeleportTexture){
        std::cerr << "Failed to load all textures." << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }


    // Create a map of textures 
    std::map<int, SDL_Texture*> textures = {
        {1, grassTexture},
        {2, sandTexture},
        {3, stoneTexture},
        {4, snowTexture},
        {5, SolarGrassTexture},
        {6, TeleportTexture}
    };


    std::vector<SDL_Texture*> inventoryTextures = { grassTexture, sandTexture, stoneTexture, snowTexture, SolarGrassTexture, TeleportTexture };
    SDL_Texture* selectedTexture = grassTexture; // Default selected texture



    //player textures
    SDL_Texture* characterUpTexture = IMG_LoadTexture(renderer, "assets/player/character_up.png");
    SDL_Texture* characterUpRightTexture = IMG_LoadTexture(renderer, "assets/player/character_up_right.png");
    SDL_Texture* characterRightTexture = IMG_LoadTexture(renderer, "assets/player/character_right.png");
    SDL_Texture* characterDownRightTexture = IMG_LoadTexture(renderer, "assets/player/character_down_right.png");
    SDL_Texture* characterDownTexture = IMG_LoadTexture(renderer, "assets/player/character_down.png");
    SDL_Texture* characterDownLeftTexture = IMG_LoadTexture(renderer, "assets/player/character_down_left.png");
    SDL_Texture* characterLeftTexture = IMG_LoadTexture(renderer, "assets/player/character_left.png");
    SDL_Texture* characterUpLeftTexture = IMG_LoadTexture(renderer, "assets/player/character_up_left.png");

    std::map<Player::Direction, SDL_Texture*> characterTextures = { 
        {Player::UP, characterUpTexture}, 
        {Player::UP_RIGHT, characterUpRightTexture}, 
        {Player::RIGHT, characterRightTexture}, 
        {Player::DOWN_RIGHT, characterDownRightTexture}, 
        {Player::DOWN, characterDownTexture}, 
        {Player::DOWN_LEFT, characterDownLeftTexture}, 
        {Player::LEFT, characterLeftTexture}, 
        {Player::UP_LEFT, characterUpLeftTexture} 
    };



    // Create grey background texture 
    SDL_Color greyColor = {192, 192, 192, 255}; // Grey color 
    SDL_Texture* greyBackground = createRoundedRectangle(renderer, 60 * inventoryTextures.size() + 20, 60, greyColor, 10); 
    
    // Create green selection background texture 
    SDL_Color greenColor = {0, 255, 0, 255}; // Green color 
    SDL_Texture* greenSelectionBackground = createRoundedRectangle(renderer, 50, 50, greenColor, 10);


    // Choose between random map and test island 
    bool useTestIsland = false; // Set this to true to use the test island, false for random map

    

    // Generate map 
    std::vector<std::vector<int>> map;
    if (useTestIsland) { 
        map = testIsland(); 
    }
    else {
        map = generateMap();
    }



    // Generate map
    // std::vector<std::vector<int>> map = generateMap();

    printMap(map);

    int mapX = 6;
    int mapY = 6;
    // Calculate isometric coordinates for the player 
    int spawnX = -(mapX - mapY) * (TILE_SIZE / 2) ; 
    int spawnY = (mapX + mapY) * (TILE_SIZE / 4) + 1;


    // Initialize player
    //Player player(spawnX * TILE_SIZE, (spawnY * TILE_SIZE) + (0.5 * TILE_SIZE), playerTexture);
    Player player(spawnX, spawnY, characterDownTexture);


    // Camera position
    int camX = 0, camY = 0;




    // Flag to track inventory visibility 
    bool inventoryVisible = false; 
    //int mouseX = 0, mouseY = 0; 
    




    // Flag to track grid visibility 
    bool gridVisible = false;

    int mouseX = 0, mouseY = 0;

    std::string hoveredItem;

    // Create the ObjectiveBox 

    // Create the ObjectiveBox with specific locations 
    std::vector<std::string> objectives = { "1. Place at least one: Grass, sand, stone and snow block", "2. Place a Solar Block and generate 25 Power", "3. Place a teleport block", "4. Teleport" }; 
    ObjectiveBox objectiveBox(renderer, UIfont, "Objectives", objectives, 8, 175, 300, 600, 50);

    // Main game loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB) {
                //toggles grid visibility
                gridVisible = !gridVisible;
            }else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_e) {
                //toggle inventory visibility with the E key
                inventoryVisible = !inventoryVisible;

            } else if (event.type == SDL_MOUSEMOTION){
                //update mouse position
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                hoveredItem = mouseCollisionDetection(mouseX, mouseY, inventoryTargets, targetNames);

                if (!hoveredItem.empty()) {
                    //std::cout << "Mouse is hovering over " << hoveredItem << "!!" << std::endl;
                }
                
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;

                    


                    // Check for clicking in the inventory bar 
                    if (inventoryVisible && mouseY > 50 && mouseY < 100) {
                        size_t clickedIndex = (mouseX - 225) / 60;
                        if (clickedIndex < inventoryTextures.size()) {
                            //check if a locked block is chosen
                            if (inventoryTextures[clickedIndex] == SolarGrassTexture && !checkUnlockCriteria()) {
                                std::cout << "Block is locked" << std::endl;
                            }
                            else if (inventoryTextures[clickedIndex] == TeleportTexture && power < 25) {
                                std::cout << "Block is locked" << std::endl;
                            }
                            else {
                                selectedTexture = inventoryTextures[clickedIndex];
                            }

                            
                            

                        }
                    }

                    // Convert mouse position to isometric grid coordinates 
                    std::pair<int, int> gridPos = screenToIsometricGrid(mouseX, mouseY, camX, camY);

                    std::cout << "Clicked grid position: (" << gridPos.second<< ", " << gridPos.first + 1  << ")" << std::endl;

                    // Calculate the distance from the player 
                    std::pair<int, int> playerGridPos = isoToGrid(player.getPosX(), player.getPosY() - (TILE_SIZE / 4)); 
                    playerGridPos.first += 3;
                    playerGridPos.second += 2;
                    //int distanceX = std::abs(gridPos.first  - playerGridPos.first); 
                    //int distanceY = std::abs(gridPos.second - playerGridPos.second); 
                    int distanceX = std::abs(gridPos.second + 1 - (playerGridPos.first ));
                    int distanceY = std::abs(gridPos.first  - (playerGridPos.second ));

                    //std::cout << "Distance X: " << distanceX << " Distance Y: " << distanceY << std::endl;

                    //std::cout << "gridpos(second,first): (" << gridPos.second << ", " << gridPos.first  << ")" << std::endl;
                    //std::cout << "playergridpos(second,first): (" << playerGridPos.second << ", " << playerGridPos.first << ")" << std::endl;

                    //std::cout << "Distance from player: (" << gridPos.second - playerGridPos.first << ", " << gridPos.first - playerGridPos.second << ")" << std::endl;

                    // Check if the clicked position is within a 3-block radius 
                    if (distanceX <= 3 && distanceY <= 3) { 
                        if (gridPos.second >= 0 && gridPos.second < MAP_HEIGHT && gridPos.first >= 0 && gridPos.first < MAP_WIDTH) { 
                            // Update the map to build a block corrosponding to what has been clicked 
                            if (selectedTexture == inventoryTextures[0]) {
                                map[gridPos.first][gridPos.second] = 1;
                                grassPlaced = true;
                            }
                            else if (selectedTexture == inventoryTextures[1]) {
                                map[gridPos.first][gridPos.second] = 2;
                                sandPlaced = true;
                            }
                            else if (selectedTexture == inventoryTextures[2]) {
                                map[gridPos.first][gridPos.second] = 3;
                                stonePlaced = true;
                            }
                            else if (selectedTexture == inventoryTextures[3]) {
                                map[gridPos.first][gridPos.second] = 4;
                                snowPlaced = true;
                            }
                            else if (selectedTexture == inventoryTextures[4]) {
                                map[gridPos.first][gridPos.second] = 5;
                                solarBlockCount++;
                                SolarPlaced = true;
                            }
                            else if (selectedTexture == inventoryTextures[5]) {
                                if (power >= 25) {
                                    map[gridPos.first][gridPos.second] = 6;
                                    power -= 25;
                                    TeleportPlaced = true;
                                    if (power < 25) {
                                        selectedTexture = grassTexture;
                                    }
                                }
                                
                                
                            }
                        } 
                    } else { 
                        std::cout << "Out of range" << std::endl; 
                    } 
                }

                

            }
            else {
                player.handleEvent(event);
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastUpdateTime >= updateInterval) {
            if (power <= 100) {

                power += solarBlockCount;
                lastUpdateTime = currentTime;
            }
        }
        
        if (grassPlaced && snowPlaced && stonePlaced && sandPlaced) {
            objectiveBox.completeObjective(0);
        }


        if (power > 25) {
            powerExceeded25 = true;
        }
        if (SolarPlaced && powerExceeded25) {
            objectiveBox.completeObjective(1);
        }

        if (TeleportPlaced) {
            objectiveBox.completeObjective(2);
        }

        if (standingOnTeleport) {
            objectiveBox.completeObjective(3);
        }

        //printTeleportBlockLocations(map);
        // Find and store teleport block locations 
        findTeleportBlockLocations(map, teleportBlockLocations);

        // Check if player is standing on a teleport block 
        std::pair<int, int> playerPos = isoToGrid(player.getPosX() , player.getPosY() - (TILE_SIZE / 4) ); 
        playerPos.first += 2;
        playerPos.second += 2;
        //std::cout << "Player grid position: (" << playerPos.first << ", " << playerPos.second << ")" << std::endl; 
        
        standingOnTeleport = false; 
        for (const auto& teleportPos : teleportBlockLocations) { 
            if (teleportPos == playerPos) { 
                if (!standingOnTeleport) { 
                    //std::cout << "Player is standing on a teleport block!" << std::endl; 
                } 
                standingOnTeleport = true; 
                break; 
            } 
        } 
        if (!standingOnTeleport) { 
            //std::cout << "Player is not standing on a teleport block." << std::endl; 
        }

        ///

        // Update camera position
        camX = player.getPosX() - WINDOW_WIDTH / 2;
        camY = player.getPosY() - WINDOW_HEIGHT / 2;

        // Set background color
        SDL_SetRenderDrawColor(renderer, 44, 87, 93, 255);  // LIGHTBLUE color
        SDL_RenderClear(renderer);

        // Render map and player
        player.update(map, characterTextures);
        renderMap(renderer, textures, map, camX, camY);
        player.render(renderer, camX, camY);


        //render grid if visible
        if (gridVisible) {

            // Render the block placement preview 
            std::pair<int, int> gridPos = screenToIsometricGrid(mouseX, mouseY, camX, camY);
            int isoX = (gridPos.first - gridPos.second) * (TILE_SIZE / 2) - camX;
            int isoY = (gridPos.first + gridPos.second) * (TILE_SIZE / 4) - camY;
            SDL_Rect dstRect = { isoX, isoY, TILE_SIZE, TILE_SIZE };
            SDL_RenderCopy(renderer, GrassSelectionTexture, NULL, &dstRect);

            renderIsometricGrid(renderer, camX, camY, font);
            
            

        }


        //render inventory if visible
        if (inventoryVisible) {
            renderInventory(renderer, selectedTexture, inventoryTextures, greyBackground, greenSelectionBackground, UIfont, WHITE);


            if (hoveredItem == "Grass Block") {
                GrassLabel.render();
            }
            else if (hoveredItem == "Sand Block") {
                SandLabel.render();
            }

            else if (hoveredItem == "Stone Block") {
                StoneLabel.render();
            }

            else if (hoveredItem == "Snow Block") {
                SnowLabel.render();
            }

            else if (hoveredItem == "Solar Block") {
                SolarLabel.render();
            }

            else if (hoveredItem == "Teleport Block") {
                TeleportLabel.render();
            }


            objectiveBox.render();


            //power ui
            SDL_Color powerTextColor = { 255, 255, 255, 255 }; // White color
            std::string powerText = "Power: " + std::to_string(power);
            SDL_Texture* powerTextTexture = renderText(powerText, font, powerTextColor, renderer);
            if (powerTextTexture) {
                int powerTextWidth, powerTextHeight;
                SDL_QueryTexture(powerTextTexture, NULL, NULL, &powerTextWidth, &powerTextHeight);
                SDL_Rect powerTextRect = { 10, 10, powerTextWidth, powerTextHeight }; // Adjust position as needed
                SDL_RenderCopy(renderer, powerTextTexture, NULL, &powerTextRect);
                SDL_DestroyTexture(powerTextTexture);
            }


            

        }



        // Convert player screen coordinates back to map grid coordinates 
        std::pair<int, int> gridPos = isoToGrid(player.getPosX(), player.getPosY());

        // Render player coordinates 
        SDL_Color textColor = { 255, 255, 255, 255 }; // White color 
        std::string coordText = "X: " + std::to_string(gridPos.first + 1) + " Y: " + std::to_string(gridPos.second + 2); 
        SDL_Texture* textTexture = renderText(coordText, font, textColor, renderer); 
        if (textTexture) { 
            int textWidth, textHeight; 
            SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight); 
            SDL_Rect textRect = { WINDOW_WIDTH - textWidth - 10, 10, textWidth, textHeight }; 
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect); 
            SDL_DestroyTexture(textTexture); 
        }


        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyTexture(grassTexture);
    SDL_DestroyTexture(sandTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(snowTexture);
    SDL_DestroyTexture(stoneTexture);
    SDL_DestroyTexture(GrassSelectionTexture);
    SDL_DestroyTexture(SolarGrassTexture);
    SDL_DestroyTexture(TeleportTexture);
    SDL_DestroyTexture(greyBackground); 
    SDL_DestroyTexture(greenSelectionBackground);
    SDL_DestroyTexture(characterUpTexture); 
    SDL_DestroyTexture(characterUpRightTexture); 
    SDL_DestroyTexture(characterRightTexture); 
    SDL_DestroyTexture(characterDownRightTexture); 
    SDL_DestroyTexture(characterDownTexture); 
    SDL_DestroyTexture(characterDownLeftTexture); 
    SDL_DestroyTexture(characterLeftTexture);
    SDL_DestroyTexture(characterUpLeftTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
