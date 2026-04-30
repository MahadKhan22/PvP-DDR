#pragma once
#include "ArrowString.h"
#include <queue>
#include <deque>
#include <string>

// All state belonging to one player
class PlayerState {
public:
    explicit PlayerState(int playerId);

    // --- Per-tick update ---
    // Ages all strings, checks if front string hit the bottom.
    // Returns true if the player just LOST (string reached bottom).
    // globalSpeedMult is multiplied with the player's currentSpeed() to
    // determine how much each string ages this tick.
    bool tick(int maxAge, float globalSpeedMult);

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

    // --- Renderer accessors (added for SFML rendering; read-only) ---
    const std::deque<ArrowString>& queue() const { return queue_; }
    bool  isDebuffed()       const { return debuffTicksLeft_ > 0; }
    int   debuffTicksLeft()  const { return debuffTicksLeft_; }
    float debuffMult()       const { return debuffMult_; }

    static constexpr int COMBO_STANDARD = 5;   // inject long string
    static constexpr int COMBO_MEGA     = 10;  // speed debuff

    // Minimum age (in base ticks) the topmost string must reach before another
    // string is allowed to spawn at the top. Stops new strings from stacking
    // on top of the previous one and hiding the active cursor.
    static constexpr int SPAWN_SPACING  = 20;

    // Sudden-death: if this many typos accumulate within a SINGLE active
    // string, the player loses immediately. Resets per string (typo count
    // lives on the ArrowString), so spamming all 4 inputs to brute-force
    // a clear is no longer viable.
    static constexpr int TYPO_LIMIT     = 5;

private:
    int  id_;
    std::deque<ArrowString> queue_; // front = bottommost = active string
    // Penalty strings waiting for room at the top of the column.
    // injectString appends here; tryRefill drains them through the same
    // spawn-spacing gate as normal refills, so a penalty never lands on
    // top of a freshly-spawned string.
    std::deque<std::vector<Direction>> pendingPenalties_;
    int  combo_   = 0;
    bool lost_    = false;

    // Speed
    float baseSpeed_      = 1.0f;
    float debuffMult_     = 1.0f;
    int   debuffTicksLeft_= 0;

    void onStringCleared(bool clean); // handles combo increment/reset + trigger check (internal)
    void tryRefill();                 // spawn-spacing-aware refill helper
};
