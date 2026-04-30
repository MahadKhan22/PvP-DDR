#include "GameState.h"
#include <iostream>
#include <sstream>

GameState::GameState() : p1_(0), p2_(1) {}

bool GameState::tick() {
    if (gameOver_) return true;

    ++globalTick_;

    // Global speed escalation
    if (globalTick_ % speedEscalateEvery_ == 0) {
        globalSpeedMult_ += 0.1f;
        std::cout << "[GAME] Speed escalated! Global multiplier: x"
                  << globalSpeedMult_ << "\n";
        // In SFML: adjust the fall velocity of all strings
    }

    bool p1Lost = p1_.tick(maxStringAge_, globalSpeedMult_);
    bool p2Lost = p2_.tick(maxStringAge_, globalSpeedMult_);

    if (p1Lost && p2Lost) {
        gameOver_ = true;
        winner_   = -1; // draw (rare edge case)
    } else if (p1Lost) {
        gameOver_ = true;
        winner_   = 1; // player 2 wins
    } else if (p2Lost) {
        gameOver_ = true;
        winner_   = 0; // player 1 wins
    }

    return gameOver_;
}

void GameState::feedInput(int playerId, Direction d) {
    if (gameOver_) return;

    PlayerState& attacker = player(playerId);
    int comboBefore = attacker.combo();
    int newCombo = 0;
    attacker.feedInput(d, newCombo);

    // Only fire on the input that CROSSED the threshold. Without this,
    // every matched-but-not-completing key while combo is still at 5
    // (mid next string) re-triggers the penalty injection.
    if (newCombo > comboBefore) {
        handleComboTrigger(playerId, newCombo);
    }
}

void GameState::handleComboTrigger(int attackerId, int newCombo) {
    PlayerState& victim = player(1 - attackerId); // the other player

    if (newCombo == PlayerState::COMBO_MEGA) {
        // Mega combo: speed debuff on opponent
        victim.applySpeedDebuff(MEGA_DEBUFF_MULT, MEGA_DEBUFF_TICKS);
        std::cout << "[COMBO] Player " << (attackerId + 1)
                  << " hit MEGA COMBO! Player " << (2 - attackerId)
                  << " speed debuffed for " << MEGA_DEBUFF_TICKS << " ticks.\n";
                  
    } else if (newCombo == PlayerState::COMBO_STANDARD) {
        // Standard combo: inject long penalty string
        victim.injectString(generatePenaltySequence(PENALTY_LENGTH));
        std::cout << "[COMBO] Player " << (attackerId + 1)
                  << " hit COMBO x5! Injecting penalty string into Player "
                  << (2 - attackerId) << "'s queue.\n";
    }
}

std::vector<Direction> GameState::generatePenaltySequence(int length) {
    std::vector<Direction> seq;
    for (int i = 0; i < length; ++i)
        seq.push_back(static_cast<Direction>(rand() % 4));
    return seq;
}

void GameState::render() const {
    std::cout << "\n";
    std::cout << "Tick: " << globalTick_
              << " | Global speed: x" << globalSpeedMult_ << "\n";
    std::cout << std::string(60, '-') << "\n";
    std::cout << p1_.renderQueue();
    std::cout << std::string(60, '-') << "\n";
    std::cout << p2_.renderQueue();
    std::cout << std::string(60, '-') << "\n";
}
