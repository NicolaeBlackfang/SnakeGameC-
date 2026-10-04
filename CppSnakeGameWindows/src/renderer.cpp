#include "snake/renderer.hpp"
#include "snake/config.hpp"

#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <string>

namespace snake {
namespace {

Color alpha(Color color, unsigned char value) {
    color.a = value;
    return color;
}

Vector2 cellCenter(GridPos pos) {
    return {
        static_cast<float>(config::kBoardOffsetX + pos.x * config::kCellSize + config::kCellSize / 2),
        static_cast<float>(config::kBoardOffsetY + pos.y * config::kCellSize + config::kCellSize / 2)
    };
}

Vector2 lerpGrid(const GridPos& from, const GridPos& to, float t) {
    const Vector2 a = cellCenter(from);
    const Vector2 b = cellCenter(to);
    return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
}

} // namespace

bool Renderer::initialize() {
    const std::string path = std::string(GetApplicationDirectory()) + "/assets/background.png";
    background_ = LoadTexture(path.c_str());
    backgroundLoaded_ = background_.id != 0;
    return true;
}

void Renderer::shutdown() {
    if (backgroundLoaded_) {
        UnloadTexture(background_);
        backgroundLoaded_ = false;
    }
}

void Renderer::draw(const GameState& state) const {
    BeginDrawing();

    drawBackground();
    drawBoard(state);
    drawFood(state);
    drawSnake(state);
    drawHud(state);

    EndDrawing();
}

void Renderer::drawBackground() const {
    if (backgroundLoaded_) {
        DrawTexturePro(
            background_,
            {0, 0, static_cast<float>(background_.width), static_cast<float>(background_.height)},
            {0, 0, static_cast<float>(config::kWindowWidth), static_cast<float>(config::kWindowHeight)},
            {0, 0}, 0.0f, WHITE);
    } else {
        ClearBackground({8, 10, 18, 255});
    }

    DrawRectangle(0, 0, config::kWindowWidth, config::kWindowHeight, alpha(BLACK, 75));
}

void Renderer::drawBoard(const GameState&) const {
    const Rectangle board{
        static_cast<float>(config::kBoardOffsetX),
        static_cast<float>(config::kBoardOffsetY),
        static_cast<float>(config::kBoardWidth),
        static_cast<float>(config::kBoardHeight)
    };

    DrawRectangleRounded(board, 0.025f, 8, alpha({10, 16, 26, 255}, 235));
    DrawRectangleLinesEx(board, 2.0f, alpha({80, 220, 190, 255}, 110));

    for (int x = 1; x < config::kBoardColumns; ++x) {
        const int px = config::kBoardOffsetX + x * config::kCellSize;
        DrawLine(px, config::kBoardOffsetY, px,
                 config::kBoardOffsetY + config::kBoardHeight,
                 alpha({80, 100, 120, 255}, 22));
    }

    for (int y = 1; y < config::kBoardRows; ++y) {
        const int py = config::kBoardOffsetY + y * config::kCellSize;
        DrawLine(config::kBoardOffsetX, py,
                 config::kBoardOffsetX + config::kBoardWidth, py,
                 alpha({80, 100, 120, 255}, 22));
    }
}

void Renderer::drawFood(const GameState& state) const {
    const Vector2 center = cellCenter(state.food);
    const float pulse = 1.0f + 0.08f * std::sin(static_cast<float>(GetTime() * 6.0));

    DrawCircleV(center, 12.0f * pulse, alpha({255, 70, 105, 255}, 35));
    DrawCircleV(center, 7.0f * pulse, {255, 85, 110, 255});
    DrawCircleV({center.x - 2.0f, center.y - 2.0f}, 2.0f, {255, 225, 230, 255});
}

void Renderer::drawSnake(const GameState& state) const {
    const float t = static_cast<float>(state.accumulator / config::kMoveInterval);
    const float bodyPad = 2.5f;

    for (std::size_t i = 0; i < state.snake.size(); ++i) {
        const GridPos& current = state.snake[i];
        GridPos previous = current;

        if (i < state.previousSnake.size()) {
            previous = state.previousSnake[i];
        }

        // When the snake grows, the new head smoothly starts at the old head.
        if (i == 0 && !state.previousSnake.empty()) {
            previous = state.previousSnake.front();
        }

        const Vector2 center = lerpGrid(previous, current, t);
        const float size = static_cast<float>(config::kCellSize) - bodyPad * 2.0f;

        Rectangle segment{
            center.x - size / 2.0f,
            center.y - size / 2.0f,
            size,
            size
        };

        const Color glow = (i == 0)
            ? alpha({80, 255, 195, 255}, 42)
            : alpha({40, 225, 170, 255}, 25);

        DrawRectangleRounded(
            {segment.x - 3, segment.y - 3, segment.width + 6, segment.height + 6},
            0.28f, 8, glow);

        DrawRectangleRounded(
            segment,
            0.24f, 8,
            i == 0 ? Color{100, 255, 205, 255} : Color{48, 205, 160, 255});

        if (i == 0) {
            // Minimal eyes to make the head readable without using a sprite sheet.
            const float eyeOffset = 4.0f;
            Vector2 e1{center.x, center.y};
            Vector2 e2{center.x, center.y};

            switch (state.direction) {
                case Direction::Up:
                    e1 = {center.x - eyeOffset, center.y - 3};
                    e2 = {center.x + eyeOffset, center.y - 3};
                    break;
                case Direction::Down:
                    e1 = {center.x - eyeOffset, center.y + 3};
                    e2 = {center.x + eyeOffset, center.y + 3};
                    break;
                case Direction::Left:
                    e1 = {center.x - 3, center.y - eyeOffset};
                    e2 = {center.x - 3, center.y + eyeOffset};
                    break;
                case Direction::Right:
                    e1 = {center.x + 3, center.y - eyeOffset};
                    e2 = {center.x + 3, center.y + eyeOffset};
                    break;
            }

            DrawCircleV(e1, 1.6f, {10, 18, 24, 255});
            DrawCircleV(e2, 1.6f, {10, 18, 24, 255});
        }
    }
}

void Renderer::drawHud(const GameState& state) const {
    DrawText("SNAKE", 40, 25, 34, {110, 255, 205, 255});
    DrawText("PRO", 170, 27, 26, {230, 240, 250, 255});

    DrawText(TextFormat("SCORE  %04d", state.score), 380, 27, 24, WHITE);
    DrawText(TextFormat("BEST  %04d", state.highScore), 575, 27, 24, {210, 220, 235, 255});

    const char* controls = "WASD / ARROWS  MOVE     P / SPACE  PAUSE     R  RESTART";
    const int textWidth = MeasureText(controls, 18);
    DrawText(controls, config::kWindowWidth - textWidth - 40, config::kWindowHeight - 34, 18,
             alpha({210, 225, 235, 255}, 185));

    if (state.mode == GameMode::Paused) {
        DrawRectangle(0, 0, config::kWindowWidth, config::kWindowHeight, alpha(BLACK, 135));
        drawCenteredText("PAUSED", config::kWindowHeight / 2 - 35, 54, {110, 255, 205, 255});
        drawCenteredText("Press P or SPACE to resume", config::kWindowHeight / 2 + 32, 22, WHITE);
    }

    if (state.mode == GameMode::GameOver) {
        DrawRectangle(0, 0, config::kWindowWidth, config::kWindowHeight, alpha(BLACK, 145));
        drawCenteredText("GAME OVER", config::kWindowHeight / 2 - 55, 58, {255, 105, 130, 255});
        drawCenteredText(TextFormat("Score: %d", state.score), config::kWindowHeight / 2 + 15, 24, WHITE);
        drawCenteredText("Press R to play again  •  Esc to quit", config::kWindowHeight / 2 + 55, 21,
                         {215, 225, 235, 255});
    }
}

void Renderer::drawCenteredText(const char* text, int y, int fontSize, Color tint) const {
    const int width = MeasureText(text, fontSize);
    DrawText(text, (config::kWindowWidth - width) / 2, y, fontSize, tint);
}

} // namespace snake
