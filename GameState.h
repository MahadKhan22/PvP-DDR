#pragma once
#include "PlayerState.h"

// Top-level game coordinator. Owns both players, global clock, and
// cross-player sabotage logic. Completely independent of rendering.
class GameState {
public:
    GameState();

    // Called once per "tick" (in CLI: after each input round;
    // in SFML: every frame or on a fixed timestep).
    // Returns true if the game is now over.
    bool tick();

    // Feed input for a specific player (0 or 1).
    // Handles combo triggers and cross-player sabotage internally.
    void feedInput(int playerId, Direction d);

    // --- Queries ---
    bool         isOver()  const { return gameOver_; }
    int          winner()  const { return winner_; }  // -1 until ovep
    PlayerState& player(int id)  { return (id == 0) ? p1_ : p2_; }

    // Render both columns side by side (CLI version)
    void render() const;

private:
    PlayerState p1_;
    PlayerState p2_;

    bool gameOver_ = false;
    int  winner_   = -1;

    // Global speed escalation
    int   globalTick_         = 0;
    int   speedEscalateEvery_ = 20; // ticks between global speed bumps
    float globalSpeedMult_    = 1.0f;

    // max height: how many ticks before a string reaches the bottom
    int maxStringAge_ = 20; 

    // Penalty string length for standard combo sabotage
    static constexpr int PENALTY_LENGTH = 8;
    // Speed debuff for mega combo
    static constexpr float MEGA_DEBUFF_MULT  = 1.3f;
    static constexpr int   MEGA_DEBUFF_TICKS = 30; // ~3 sec at 10 ticks/sec

    void handleComboTrigger(int attackerId, int newCombo);
    std::vector<Direction> generatePenaltySequence(int length);
};
