#include "snake/input.hpp"

#include <raylib.h>

namespace snake {

void InputSystem::update(GameState& state) {
    int key = 0;
    bool acceptedTurnThisFrame = state.pendingDirection.has_value();

    while ((key = GetKeyPressed()) != 0) {
        if (key == KEY_P || key == KEY_SPACE) {
            state.togglePause();
            continue;
        }

        if (state.mode != GameMode::Playing) {
            continue;
        }

        if (acceptedTurnThisFrame) {
            // Ignore additional turns until the simulation consumes the pending turn.
            continue;
        }

        Direction requested;
        bool isDirection = true;

        switch (key) {
            case KEY_UP:
            case KEY_W:
                requested = Direction::Up;
                break;
            case KEY_DOWN:
            case KEY_S:
                requested = Direction::Down;
                break;
            case KEY_LEFT:
            case KEY_A:
                requested = Direction::Left;
                break;
            case KEY_RIGHT:
            case KEY_D:
                requested = Direction::Right;
                break;
            default:
                isDirection = false;
                break;
        }

        if (isDirection) {
            const auto before = state.pendingDirection;
            state.applyDirectionInput(requested);
            acceptedTurnThisFrame = state.pendingDirection.has_value() || before.has_value();
        }
    }
}

} // namespace snake
