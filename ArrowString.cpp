#include "ArrowString.h"
#include <cstdlib>
#include <sstream>

// --- Constructors ---

ArrowString::ArrowString(int length) {
    for (int i = 0; i < length; ++i) 
        sequence_.push_back(static_cast<Direction>(rand() % 4));
}

ArrowString::ArrowString(std::vector<Direction> sequence)
    : sequence_(std::move(sequence)) {}

// --- Input handling ---

bool ArrowString::feedInput(Direction d) {
    if (isComplete()) return false;
    if (d == sequence_[pointer_]) {
        ++pointer_;
        return true;
    } else {
        dirty_ = true; // typo, combo broken, but keep going
        ++typos_;       // counted for sudden-death check in PlayerState
        return false;
    }
}

// --- State queries ---

bool ArrowString::isComplete() const {
    return pointer_ >= static_cast<int>(sequence_.size());
}

bool ArrowString::wasClean() const {
    return (isComplete() && !dirty_); // do i need to call isComplete() here?
}

int ArrowString::currentIndex() const { return pointer_; }
int ArrowString::length() const { return static_cast<int>(sequence_.size()); }

// --- Age tracking ---

void  ArrowString::tick(float ageDelta) { age_ += ageDelta; }
float ArrowString::age() const { return age_; }

// --- Display ---

std::string ArrowString::display() const {
    std::ostringstream oss;
    for (int i = 0; i < static_cast<int>(sequence_.size()); ++i) {
        if (i == pointer_ && !isComplete()) { // if current
            oss << ">>" << dirToStr(sequence_[i]) << "<<"; // >>[^]<<
        } else if (i < pointer_) { // if hogya
            oss << " \u2713  "; // show checkmark (doesnt work rn)
        } else { 
            oss << " " << dirToStr(sequence_[i]) << " ";
        }
    }
    return oss.str();
}
