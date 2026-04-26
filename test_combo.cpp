// Test the EXACT logic in PlayerState::feedInput by inlining it with known
// strings.
#include "ArrowString.h"
#include <iostream>
#include <deque>

struct Mini {
    std::deque<ArrowString> queue;
    int combo = 0;

    bool feedInput(Direction d) {
        if (queue.empty()) return false;
        bool matched = queue.front().feedInput(d);

        if (queue.front().isComplete()) {
            bool clean = queue.front().wasClean();
            queue.pop_front();
            if (clean) combo++;
            else combo = 0;
        } else if (!matched) {
            combo = 0;
        }
        return matched;
    }
};

int main() {
    Mini m;
    // Push 3 strings: clean, dirty, clean
    m.queue.emplace_back(std::vector<Direction>{Direction::UP, Direction::DOWN});
    m.queue.emplace_back(std::vector<Direction>{Direction::LEFT, Direction::RIGHT});
    m.queue.emplace_back(std::vector<Direction>{Direction::UP, Direction::UP});

    std::cout << "Initial combo: " << m.combo << "\n";

    // String 1 - clean
    m.feedInput(Direction::UP);
    m.feedInput(Direction::DOWN);
    std::cout << "After clean string 1: combo=" << m.combo << "\n";

    // String 2 - typo on first arrow, then complete
    m.feedInput(Direction::UP);     // typo on LEFT
    std::cout << "  after typo: combo=" << m.combo << "\n";
    m.feedInput(Direction::LEFT);   // correct
    m.feedInput(Direction::RIGHT);  // correct, complete
    std::cout << "After dirty string 2: combo=" << m.combo << "\n";

    // String 3 - clean
    m.feedInput(Direction::UP);
    m.feedInput(Direction::UP);
    std::cout << "After clean string 3: combo=" << m.combo << "\n";

    return 0;
}
