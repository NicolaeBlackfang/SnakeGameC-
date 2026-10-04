#pragma once

#include "snake/types.hpp"

#include <deque>
#include <optional>
#include <random>
#include <vector>

namespace snake {

class GameState {
public:
    std::deque<GridPos> snake;
    std::deque<GridPos> previousSnake;
    std::vector<unsigned char> occupied;

    GridPos food{};
    Direction direction{Direction::Right};
    std::optional<Direction> pendingDirection{};
    GameMode mode{GameMode::Playing};

    double accumulator{0.0};
    int score{0};
    int highScore{0};

    void reset(std::mt19937& rng);
    void update(double deltaTime, std::mt19937& rng);
    void applyDirectionInput(Direction requested);
    void togglePause();

private:
    void simulationStep(std::mt19937& rng);
    void spawnFood(std::mt19937& rng);
    bool hitsWall(GridPos pos) const;
    bool isOccupied(GridPos pos) const;
    void setOccupied(GridPos pos, bool value);
};

int loadHighScore();
void saveHighScore(int score);

}
