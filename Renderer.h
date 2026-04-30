#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "GameState.h"

// SFML renderer. Holds no game state of its own
// only visual state (font, the window reference, transient flash timers).
// Pure read of GameState; no mutation.
class Renderer {
public:
    // windowWidth / windowHeight are passed in so layout math is consistent
    // with whatever main.cpp sized the window to.
    Renderer(unsigned windowWidth, unsigned windowHeight);

    // Loads the font. Returns false if no usable font was found anywhere.
    bool loadFont();

    // Draws everything for one frame.
    //   tickProgress: 0.0 to 1.0, fraction of the way to the next tick.
    //                 Used to interpolate falling-arrow positions so motion
    //                 is smooth even though game logic ticks at 10 Hz.
    void draw(sf::RenderWindow& window, const GameState& game, float tickProgress);

    // External hooks: main.cpp calls these when it observes that a combo
    // just triggered, so the renderer can flash the appropriate column.
    void notifyComboFlash(int playerId);

    // Call once per real-time frame (any frame rate) so flash timers decay.
    // dtSeconds is the elapsed wall-clock time since the previous draw.
    void updateAnimations(float dtSeconds);

private:
    // --- Layout constants (computed in constructor) ---
    unsigned winW_;
    unsigned winH_;
    float    columnWidth_;
    float    columnTopY_;     // y where age=0 strings spawn
    float    columnBottomY_;  // y where age=maxAge strings hit "the floor"
    float    p1ColumnX_;      // x center of P1's column
    float    p2ColumnX_;      // x center of P2's column

    // --- Visual state ---
    sf::Font font_;
    bool     fontLoaded_ = false;

    // Flash timers (seconds remaining). One per player; decays to 0.
    float p1FlashTime_ = 0.0f;
    float p2FlashTime_ = 0.0f;
    static constexpr float FLASH_DURATION = 0.4f;

    // --- Internal drawing helpers ---

    // Draws one player's column: HUD + queue of falling strings.
    void drawPlayerColumn(sf::RenderWindow& window,
                          const PlayerState& player,
                          float columnCenterX,
                          int   maxStringAge,
                          float tickProgress,
                          float flashStrength) const;

    // Draws a single ArrowString at the given y-center.
    //   isActive: true if this is the front (bottommost) string;
    //             draws the cursor highlight at the pointer position.
    void drawArrowString(sf::RenderWindow& window,
                         const ArrowString& str,
                         float centerX,
                         float centerY,
                         bool  isActive) const;

    // Draws the divider, global tick counter, speed multiplier.
    void drawHud(sf::RenderWindow& window, const GameState& game) const;

    // Draws the game-over overlay (winner banner + restart hint).
    void drawGameOver(sf::RenderWindow& window, const GameState& game) const;

    // Helpers to make text creation less verbose.
    sf::Text makeText(const std::string& s,
                      unsigned size,
                      sf::Color color,
                      float x, float y,
                      bool centerOrigin = false) const;
};
