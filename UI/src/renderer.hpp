#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>
#include <map>
#include "img.hpp" // או איפה שהמחלקה Img מוגדרת
#include "game_snapshot.hpp"
#include "engine/GameEngine.hpp" 
#include "input/Controller.hpp"

class Renderer {
public:
    Renderer(const std::string& assetsDir);
    void render(const GameSnapshot& snapshot, const GameEngine& gameEngine, const Controller& controller);

private:
    std::string boardPath;
    std::map<std::string, std::string> piecePaths;
    int cellSize;
};

#endif