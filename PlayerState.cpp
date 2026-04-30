#include "PlayerState.h"
#include <sstream>
#include <iostream>

PlayerState::PlayerState(int playerId) {
    id_ = playerId;
    // Start with a single string. tryRefill() (called from tick) will add
    // more once SPAWN_SPACING ticks have elapsed, so the active string is
    // never visually hidden behind a fresh spawn at the same y.
    addString(3);
}

// --- Per-tick update ---

bool PlayerState::tick(int maxAge, float globalSpeedMult) {
    if (lost_) return true;

    tickDebuff();

    // Age all strings in queue. The amount of aging per tick is the player's
    // effective speed (debuff * global escalation), so a debuffed player's
    // strings actually fall faster instead of just being labeled "DEBUFF".
    float ageDelta = currentSpeed() * globalSpeedMult;
    for (auto& s : queue_) // for all s in queue_ (type=ArrowString),
        s.tick(ageDelta);

    // Check if the front string hit the bottom
    if (!queue_.empty() && queue_.front().age() >= static_cast<float>(maxAge)) {
        lost_ = true;
        return true;
    }

    tryRefill();

    return false;
}

// --- Input ---

bool PlayerState::feedInput(Direction d, int& outCombo) {
    if (lost_ || queue_.empty()) { 
        outCombo = combo_; 
        return false; 
    }

    bool matched = queue_.front().feedInput(d); // ArrowString wala feedInput

    if (queue_.front().isComplete()) {
        bool clean = queue_.front().wasClean();
        queue_.pop_front();
        onStringCleared(clean);

        // Refill (spawn-spacing-aware) so input between ticks doesn't fall
        // into an empty queue. tryRefill may add 0 strings if the back is
        // still too close to the top — tick() will pick it up later.
        tryRefill();

    } else if (!matched) {
        // Typo but string not done yet, reset combo
        combo_ = 0;
        // Sudden-death: too many typos in this single string = instant loss.
        // Stops the "spam all 4 keys" exploit since brute-forcing burns
        // through the typo budget long before the string completes.
        if (queue_.front().typoCount() >= TYPO_LIMIT) {
            lost_ = true;
        }
    }

    outCombo = combo_;
    return matched;
}

void PlayerState::onStringCleared(bool clean) {
    if (clean) {combo_++; }
    else       {combo_ = 0; } // any typo in the string breaks the combo
}

// --- Queue management ---

void PlayerState::addString(int length) {
    queue_.emplace_back(length);
}

void PlayerState::tryRefill() {
    // Helper: is there room at the top for a new string?
    auto roomAtTop = [&]() {
        return queue_.empty()
            || queue_.back().age() >= static_cast<float>(SPAWN_SPACING);
    };

    // Drain pending penalty strings first — they take priority over filler
    // strings, so as soon as the top of the column is clear they slot in.
    while (!pendingPenalties_.empty() && roomAtTop()) {
        queue_.emplace_back(std::move(pendingPenalties_.front()));
        pendingPenalties_.pop_front();
    }

    // Then top up to 3 with random filler strings, respecting the same
    // spacing rule so freshly-spawned strings never share a y-position.
    while (queue_.size() < 3 && roomAtTop()) {
        addString(3 + rand() % 5);
    }
}

void PlayerState::injectString(std::vector<Direction> sequence) { // penalty
    // Defer until tryRefill can place it without overlapping the topmost
    // existing string. tryRefill is called from tick() each tick, so the
    // penalty appears within at most SPAWN_SPACING ticks (~2s).
    pendingPenalties_.push_back(std::move(sequence));
    tryRefill();
}

int PlayerState::queueSize() const { return static_cast<int>(queue_.size()); }

// --- Debuff ---

void PlayerState::applySpeedDebuff(float multiplier, int durationTicks) {
    debuffMult_      = multiplier;
    debuffTicksLeft_ = durationTicks;
}

void PlayerState::tickDebuff() {
    if (debuffTicksLeft_ > 0) {
        --debuffTicksLeft_;
        if (debuffTicksLeft_ == 0) debuffMult_ = 1.0f; // reset speed at end
    }
}

float PlayerState::currentSpeed() const {
    return baseSpeed_ * debuffMult_;
}

// --- Display ---

std::string PlayerState::renderQueue() const {
    std::ostringstream oss;
    oss << "=== Player " << (id_ + 1) << " | Combo: " << combo_
        << " | Speed: x" << currentSpeed() << " ===\n";

    // Show from back (top of column) to front (active, bottom)
    int idx = static_cast<int>(queue_.size()) - 1;
    for (auto it = queue_.rbegin(); it != queue_.rend(); ++it, --idx) {
        if (idx == 0) 
            oss << "  [ACTIVE] " << it->display() << "  (age: " << it->age() << ")\n";
        else // non-active strings also have active because of display()
            oss << "           " << it->display() << "\n"; // so that they align
    }
    return oss.str();
}
