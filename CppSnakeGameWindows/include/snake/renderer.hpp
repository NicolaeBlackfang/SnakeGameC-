#pragma once

#include "snake/game.hpp"
#include <raylib.h>

#include <string>

namespace snake {

class Renderer {
public:
    bool initialize();
    void shutdown();
    void draw(const GameState& state) const;

private:
    void drawBackground() const;
    void drawBoard(const GameState& state) const;
    void drawFood(const GameState& state) const;
    void drawSnake(const GameState& state) const;
    void drawHud(const GameState& state) const;

    void drawCenteredText(const char* text, int y, int fontSize, Color tint) const;

    Texture2D background_{};
    bool backgroundLoaded_{false};
};

}
