#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <map>
#include <optional>
#include <string>

#include "img.hpp"
#include "render_model.hpp"
#include "model/position.hpp"

class Renderer
{
public:
    explicit Renderer(
        const std::string& assetsDir
    );

    void render(
        const RenderModel& renderModel,
        bool gameOver,
        int whiteScore,
        int blackScore,
        const std::optional<Position>& selectedPosition
    );

private:
    std::string boardPath;

    std::map<
        std::string,
        std::string
    > piecePaths;

    int cellSize;
};

#endif