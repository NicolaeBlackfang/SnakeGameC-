#pragma once

#include <cstddef>

namespace snake {

struct GridPos {
    int x{};
    int y{};

    constexpr bool operator==(const GridPos&) const = default;
};

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

enum class GameMode {
    Playing,
    Paused,
    GameOver
};

constexpr GridPos directionVector(Direction direction) {
    switch (direction) {
        case Direction::Up:    return {0, -1};
        case Direction::Down:  return {0, 1};
        case Direction::Left:  return {-1, 0};
        case Direction::Right: return {1, 0};
    }
    return {0, 0};
}

constexpr bool isOpposite(Direction a, Direction b) {
    return (a == Direction::Up && b == Direction::Down) ||
           (a == Direction::Down && b == Direction::Up) ||
           (a == Direction::Left && b == Direction::Right) ||
           (a == Direction::Right && b == Direction::Left);
}

constexpr std::size_t gridIndex(GridPos p, int width) {
    return static_cast<std::size_t>(p.y * width + p.x);
}

}
