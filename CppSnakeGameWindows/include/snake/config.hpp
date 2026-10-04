#pragma once

namespace snake::config {
constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 760;

constexpr int kBoardColumns = 48;
constexpr int kBoardRows = 30;
constexpr int kCellSize = 20;
constexpr int kBoardWidth = kBoardColumns * kCellSize;
constexpr int kBoardHeight = kBoardRows * kCellSize;
constexpr int kBoardOffsetX = 40;
constexpr int kBoardOffsetY = 80;

constexpr double kMoveInterval = 0.095;
constexpr double kMaxFrameDelta = 0.25;
constexpr int kInitialSnakeLength = 5;
constexpr int kStartingScore = 0;
}
