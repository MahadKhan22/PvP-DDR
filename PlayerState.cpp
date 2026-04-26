#include "PlayerState.h"
#include <sstream>
#include <iostream>

PlayerState::PlayerState(int playerId) {
    id_ = playerId;
    // Start with 2 strings in the queue
    addString(3);
    addString(4);
}

// --- Per-tick update ---

bool PlayerState::tick(int maxAge) {
    if (lost_) return true;

    tickDebuff();

    // Age all strings in queue
    for (auto& s : queue_) // for all s in queue_ (type=ArrowString),
        s.tick();          

    // Check if the front string hit the bottom
    if (!queue_.empty() && queue_.front().age() >= maxAge) {
        lost_ = true;
        return true;
    }

    // taken care of in PlayerState::feedInput()

    // // Refill queue so there's always something to do
    // while (queue_.size() < 3)
    //     addString(3 + rand() % 5);

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

        // Ensure queue never runs empty
        while (queue_.size() < 3)
            addString(3 + rand() % 5);

    } else if (!matched) {
        // Typo but string not done yet, reset combo
        combo_ = 0;
    }

    outCombo = combo_;
    return matched;
}

void PlayerState::onStringCleared(bool clean) {
<<<<<<< HEAD
    if (clean) {combo_++; std::cout<<"clean\n";}
    else       {combo_ = 0; std::cout<<"dirt\n";} // any typo in the string breaks the combo
=======
    if (clean) {combo_++;}
    else       {combo_ = 0;} // any typo in the string breaks the combo
>>>>>>> 89fcc8989879066325dd955c2f2be1d08c265877
}

// --- Queue management ---

void PlayerState::addString(int length) {
    queue_.emplace_back(length);
}

void PlayerState::injectString(std::vector<Direction> sequence) { // penalty
    queue_.emplace_back(std::move(sequence)); // goes to the back
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
