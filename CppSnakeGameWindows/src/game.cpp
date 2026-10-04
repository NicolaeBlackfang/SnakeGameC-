#include "snake/game.hpp"
#include "snake/config.hpp"

#include <raylib.h>

#include <algorithm>
#include <fstream>
#include <string>

namespace snake {
namespace {

constexpr const char* kHighScoreFile = "highscore.dat";

bool validPendingDirection(const GameState& state, Direction requested) {
    if (isOpposite(state.direction, requested)) {
        return false;
    }

    if (state.pendingDirection.has_value() &&
        state.pendingDirection.value() != requested) {
        return false; // one accepted turn per simulation tick
    }

    return true;
}

} // namespace

void GameState::reset(std::mt19937& rng) {
    snake.clear();
    previousSnake.clear();

    occupied.assign(
        static_cast<std::size_t>(config::kBoardColumns * config::kBoardRows),
        0);

    const int centerX = config::kBoardColumns / 2;
    const int centerY = config::kBoardRows / 2;

    for (int i = 0; i < config::kInitialSnakeLength; ++i) {
        snake.push_back({centerX - i, centerY});
        setOccupied({centerX - i, centerY}, true);
    }

    previousSnake = snake;
    direction = Direction::Right;
    pendingDirection.reset();
    mode = GameMode::Playing;
    accumulator = 0.0;
    score = config::kStartingScore;

    spawnFood(rng);
}

void GameState::update(double deltaTime, std::mt19937& rng) {
    if (mode != GameMode::Playing) {
        return;
    }

    accumulator += std::min(deltaTime, config::kMaxFrameDelta);

    while (accumulator >= config::kMoveInterval && mode == GameMode::Playing) {
        accumulator -= config::kMoveInterval;
        simulationStep(rng);
    }
}

void GameState::applyDirectionInput(Direction requested) {
    if (!validPendingDirection(*this, requested)) {
        return;
    }

    if (requested == direction) {
        return;
    }

    pendingDirection = requested;
}

void GameState::togglePause() {
    if (mode == GameMode::Playing) {
        mode = GameMode::Paused;
    } else if (mode == GameMode::Paused) {
        mode = GameMode::Playing;
    }
}

void GameState::simulationStep(std::mt19937& rng) {
    previousSnake = snake;

    if (pendingDirection.has_value()) {
        direction = pendingDirection.value();
        pendingDirection.reset();
    }

    const GridPos delta = directionVector(direction);
    const GridPos head = snake.front();
    const GridPos newHead{head.x + delta.x, head.y + delta.y};

    if (hitsWall(newHead)) {
        mode = GameMode::GameOver;
        return;
    }

    const bool eating = (newHead == food);
    const GridPos tail = snake.back();
    const bool occupiedBySnake = isOccupied(newHead);

    // The tail leaves its cell during a normal move, so entering that cell is legal.
    if (occupiedBySnake && (eating || newHead != tail)) {
        mode = GameMode::GameOver;
        return;
    }

    snake.push_front(newHead);
    setOccupied(newHead, true);

    if (eating) {
        ++score;
        highScore = std::max(highScore, score);
        saveHighScore(highScore);
        spawnFood(rng);
    } else {
        snake.pop_back();
        setOccupied(tail, false);
    }
}

void GameState::spawnFood(std::mt19937& rng) {
    std::uniform_int_distribution<int> xDist(0, config::kBoardColumns - 1);
    std::uniform_int_distribution<int> yDist(0, config::kBoardRows - 1);

    // The board is small; random probing is fast in normal play.
    for (int attempt = 0; attempt < 200; ++attempt) {
        const GridPos candidate{xDist(rng), yDist(rng)};
        if (!isOccupied(candidate)) {
            food = candidate;
            return;
        }
    }

    // Deterministic fallback for near-full boards.
    for (int y = 0; y < config::kBoardRows; ++y) {
        for (int x = 0; x < config::kBoardColumns; ++x) {
            const GridPos candidate{x, y};
            if (!isOccupied(candidate)) {
                food = candidate;
                return;
            }
        }
    }

    // No free cells means the player has effectively filled the board.
    mode = GameMode::GameOver;
}

bool GameState::hitsWall(GridPos pos) const {
    return pos.x < 0 || pos.x >= config::kBoardColumns ||
           pos.y < 0 || pos.y >= config::kBoardRows;
}

bool GameState::isOccupied(GridPos pos) const {
    return occupied[gridIndex(pos, config::kBoardColumns)] != 0;
}

void GameState::setOccupied(GridPos pos, bool value) {
    occupied[gridIndex(pos, config::kBoardColumns)] = value ? 1 : 0;
}

int loadHighScore() {
    const std::string path = std::string(GetApplicationDirectory()) + "/" + kHighScoreFile;
    std::ifstream file(path);

    int score = 0;
    if (file >> score) {
        return std::max(score, 0);
    }

    return 0;
}

void saveHighScore(int score) {
    const std::string path = std::string(GetApplicationDirectory()) + "/" + kHighScoreFile;
    std::ofstream file(path, std::ios::trunc);
    if (file) {
        file << std::max(score, 0);
    }
}

} // namespace snake
