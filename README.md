# SnakeGame — C++ Windows Edition

A polished, high-performance classic Snake game built with **C++20 + raylib 6.0**.

## Features

- High-precision `std::chrono::steady_clock` frame timing.
- Fixed-step grid simulation for deterministic movement.
- `std::deque` body storage with O(1) head insertion and tail removal.
- O(1) self-collision checks using a compact occupancy grid.
- One-turn-per-tick input buffering to prevent rapid double-turn bugs.
- Smooth render interpolation between logical grid ticks.
- Procedural-style PNG background included in `assets/`.
- Persistent high score stored beside the executable.
- Windows GitHub Actions build on every push.
- Tagged releases can be turned into downloadable Windows ZIP releases.

## Local build on Linux

The normal local build is for Linux. It requires internet access the first time because CMake FetchContent downloads raylib 6.0.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/bin/SnakeGame
```

## Windows build through GitHub

Push the repository to GitHub. The workflow at `.github/workflows/windows.yml` runs on a Windows runner and produces:

```text
SnakeGame-Windows/
├── SnakeGame.exe
└── assets/
    └── background.png
```

The ZIP is uploaded as a GitHub Actions artifact.

## Controls

- `WASD` or Arrow Keys — Move
- `P` / `Space` — Pause / Resume
- `R` — Restart after Game Over
- `Esc` — Quit
