#pragma once
#include "ArrowString.h"
#include <queue>
#include <string>

// All state belonging to one player
class PlayerState {
public:
    explicit PlayerState(int playerId);

    // --- Per-tick update ---
    // Ages all strings, checks if front string hit the bottom.
    // Returns true if the player just LOST (string reached bottom).
    bool tick(int maxAge);

    // --- Input ---
    // Feed a directional input. Operates on the front (bottommost) string.
    // Returns true if input matched, false = typo.
    // If the front string completes, handles combo logic and queue pop.
    // outCombo: the new combo value after this input (useful for caller to check triggers).
    bool feedInput(Direction d, int& outCombo);

    // --- Queue management ---
    void addString(int length);                          // add random string
    void injectString(std::vector<Direction> sequence);  // inject penalty string
    int  queueSize() const;

    // --- Debuff ---
    void applySpeedDebuff(float multiplier, int durationTicks);
    void tickDebuff();         // call each tick, removes debuff when expired
    float currentSpeed() const; // baseSpeed_ * debuffMult_

    // --- Display ---
    std::string renderQueue() const; // shows all strings in queue, oldest at bottom

    // --- Getters ---
    int  id()      const { return id_; }
    int  combo()   const { return combo_; }
    bool hasLost() const { return lost_; }

    static constexpr int COMBO_STANDARD = 5;   // inject long string
    static constexpr int COMBO_MEGA     = 10;  // speed debuff

private:
    int  id_;
    std::deque<ArrowString> queue_; // front = bottommost = active string
    int  combo_   = 0;
    bool lost_    = false;

    // Speed
    float baseSpeed_      = 1.0f;
    float debuffMult_     = 1.0f;
    int   debuffTicksLeft_= 0;

    void onStringCleared(bool clean); // handles combo increment/reset + trigger check (internal)
};
