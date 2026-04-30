#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "GameState.h"
#include "Renderer.h"

// ---------------------------------------------------------------------------
// PvP-DDR — SFML entry point
// ---------------------------------------------------------------------------
// Architecture:
//   - GameState ticks at a FIXED 10 Hz (one logical tick every 100 ms).
//     This matches the constants in the original CLI design (e.g.
//     MEGA_DEBUFF_TICKS = 30 ≈ 3 seconds).
//   - The window renders at 60 FPS. Between game ticks, the renderer
//     interpolates falling-arrow positions using tickProgress (0..1).
//   - Input is event-driven: each KeyPressed immediately feeds GameState.
//     We don't wait for the next tick, so input feels instant.
//
// Controls:
//   P1: W / A / S / D
//   P2: I / J / K / L
//   R:  restart after game over
//   Esc: quit
// ---------------------------------------------------------------------------

static constexpr unsigned WINDOW_W = 1200;
static constexpr unsigned WINDOW_H = 800;
static constexpr float    TICK_INTERVAL = 0.1f; // 100 ms = 10 ticks/sec
static constexpr unsigned FRAMERATE_CAP = 60;

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // --- Window setup ---
    sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H), "PvP-DDR");
    window.setFramerateLimit(FRAMERATE_CAP);
    window.setKeyRepeatEnabled(false); // crucial: held keys must NOT auto-repeat

    // --- Renderer setup ---
    Renderer renderer(WINDOW_W, WINDOW_H);
    if (!renderer.loadFont()) {
        // We continue anyway — text just won't draw. The shapes will still
        // render so it's still visually intelligible. Stderr message printed
        // by Renderer::loadFont().
    }

    // --- Game state ---
    GameState game;

    // --- Timing ---
    sf::Clock loopClock;        // wall-clock, used for both tick accumulator and animation dt
    float tickAccumulator = 0.0f;
    float lastFrameDt = 0.0f;

    while (window.isOpen()) {
        // Wall-clock elapsed since last loop iteration
        float dt = loopClock.restart().asSeconds();
        // Defensive cap: if the OS deschedules us for a long time (debugger,
        // sleep), don't replay 5 seconds of ticks at once.
        if (dt > 0.25f) dt = 0.25f;
        lastFrameDt = dt;

        // ----------------------------------------------------------------
        // 1. Process input events
        // ----------------------------------------------------------------
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                sf::Keyboard::Key key = event.key.code;

                // Always-available keys
                if (key == sf::Keyboard::Escape) {
                    window.close();
                }
                else if (key == sf::Keyboard::R && game.isOver()) {
                    // Restart: reconstruct the GameState in place
                    game = GameState();
                }

                // Game inputs (only when game is live)
                if (!game.isOver()) {
                    Direction d;
                    for (int playerId = 0; playerId < 2; ++playerId) {
                        if (keyToDir(key, d, playerId)) {
                            int comboBefore = game.player(playerId).combo();
                            game.feedInput(playerId, d);
                            int comboAfter = game.player(playerId).combo();

                            // Detect combo trigger crossings.
                            // GameState::handleComboTrigger fires on EXACT
                            // equality with COMBO_STANDARD (5) or COMBO_MEGA
                            // (10), so we mirror that condition here.
                            if (comboAfter > comboBefore) {
                                if (comboAfter == PlayerState::COMBO_STANDARD ||
                                    comboAfter == PlayerState::COMBO_MEGA)
                                {
                                    renderer.notifyComboFlash(playerId);
                                }
                            }
                            break; // a key belongs to only one player
                        }
                    }
                }
            }
        }

        // ----------------------------------------------------------------
        // 2. Advance game logic at a fixed rate (10 Hz)
        // ----------------------------------------------------------------
        tickAccumulator += dt;
        while (tickAccumulator >= TICK_INTERVAL) {
            tickAccumulator -= TICK_INTERVAL;
            if (!game.isOver()) {
                game.tick();
            }
        }

        // ----------------------------------------------------------------
        // 3. Update animation timers (combo flashes etc.)
        // ----------------------------------------------------------------
        renderer.updateAnimations(lastFrameDt);

        // ----------------------------------------------------------------
        // 4. Render
        // ----------------------------------------------------------------
        // tickProgress: how far along we are toward the *next* tick (0..1).
        // Used by the renderer to smoothly interpolate falling-arrow y.
        float tickProgress = tickAccumulator / TICK_INTERVAL;

        renderer.draw(window, game, tickProgress);
        window.display();
    }

    return 0;
}
