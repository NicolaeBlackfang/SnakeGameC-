#include "snake/config.hpp"
#include "snake/game.hpp"
#include "snake/input.hpp"
#include "snake/renderer.hpp"

#include <raylib.h>

#include <chrono>
#include <random>

int main() {
    InitWindow(snake::config::kWindowWidth, snake::config::kWindowHeight, "SnakeGame - C++ Professional Edition");
    SetTargetFPS(240);
    SetExitKey(KEY_ESCAPE);

    std::random_device rd;
    std::mt19937 rng(rd());

    snake::GameState state;
    state.highScore = snake::loadHighScore();
    state.reset(rng);
    state.highScore = snake::loadHighScore();

    snake::Renderer renderer;
    renderer.initialize();

    using Clock = std::chrono::steady_clock;
    auto previous = Clock::now();

    while (!WindowShouldClose()) {
        const auto now = Clock::now();
        const double deltaTime = std::chrono::duration<double>(now - previous).count();
        previous = now;

        if (IsKeyPressed(KEY_R) && state.mode == snake::GameMode::GameOver) {
            state.reset(rng);
            state.highScore = snake::loadHighScore();
        }

        snake::InputSystem::update(state);
        state.update(deltaTime, rng);
        renderer.draw(state);
    }

    renderer.shutdown();
    CloseWindow();
    return 0;
}
