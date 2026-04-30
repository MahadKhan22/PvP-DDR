#pragma once
#include "Arrow.h"
#include <vector>

// Represents one falling arrow sequence that a player must match.
class ArrowString {
public:
    // Constructs a random sequence of 'length' arrows
    explicit ArrowString(int length);

    // Constructs a specific sequence (used for penalty string injection)
    explicit ArrowString(std::vector<Direction> sequence);

    // --- Input handling ---

    // Feed the next player input. Returns true if key matched, false = typo.
    // After a typo the pointer does NOT reset — player still finishes the string,
    // but the combo flag is permanently tainted for this string.
    bool feedInput(Direction d);

    // --- State queries ---
    bool isComplete() const;   // all arrows matched
    bool wasClean()   const;     // completed with zero typos

    int  currentIndex() const; // how far through the sequence we are
    int  length()       const;

    // Render: shows the sequence, highlights current position
    // e.g.  [^] [v] >>[<]<< [>]
    std::string display() const;

    // Advance age by ageDelta (default 1.0 = one tick at base speed).
    // Caller passes the player's effective speed so debuffs/global scaling
    // actually translate into a faster fall.
    void  tick(float ageDelta = 1.0f);
    float age() const;

    // --- Renderer accessors (added for SFML rendering; read-only) ---
    const std::vector<Direction>& sequence() const { return sequence_; }
    bool hasTypo()    const { return dirty_; }
    int  typoCount() const { return typos_; }

private:
    std::vector<Direction> sequence_;
    int   pointer_ = 0;     // index of next arrow to match
    bool  dirty_   = false; // true if any typo occurred (kept for combo logic)
    int   typos_   = 0;     // total typo count for this string (sudden-death)
    float age_     = 0.0f;  // float so speed multipliers can scale aging fractionally
};
