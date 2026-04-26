# PvP-DDR
CS-107 semester 2 C++ project - PvP Dance Dance Revolution

A real-time two-player competitive game where players race to clear falling arrow sequences before they reach the bottom. Built in C++ — currently in CLI form, with SFML rendering planned.

**Group Members**
- Mahad Khan — 542488
- Arfa Khattak — 546245
- Marium Saleem — 540134

*BEE-17D | Computer Programming CS-107*

---

## What is this game?

Two players share one keyboard. Each player has their own column of falling arrow sequences. You must match the bottommost sequence in your column — key by key, in order — before it reaches the bottom. Miss too many strings and you lose.

Playing cleanly isn't just about survival: landing combos actively sabotages your opponent.

---

## Controls

| Action | Player 1 | Player 2 | Logic |
|---|---|---|---|
| Up | `W` | `I` | `↑` |
| Down | `S` | `K` | `↓` |
| Left | `A` | `J` | `←` |
| Right | `D` | `L` | `→` |

---

## Core Mechanics

**Clearing strings**
Each string is a randomly generated sequence of 3–8 directional arrows. You always work on the bottommost string first. Press the correct keys in order to clear it. A wrong key counts as a typo — you keep going, but your combo is broken.

**Losing**
If any string in your column reaches the bottom without being cleared, you lose immediately.

**Speed escalation**
Every 30 seconds, the fall speed increases for both players. Early game is readable; late game is reflexive. Matches always reach a conclusion.

**Combo system**

| Combo | Requirement | Effect on opponent |
|---|---|---|
| Standard | 5 strings, zero typos | A long penalty string injected into their queue |
| Mega | 10 strings, zero typos | Fall speed increased by 30% for 3 seconds |

A single typo resets your combo counter to zero. The string still must be completed.

---

## Project Structure

```
game/
├── Arrow.h / Arrow.cpp          # Direction enum, key-to-direction mapping
├── ArrowString.h / ArrowString.cpp  # One falling sequence (data + input tracking)
├── PlayerState.h / PlayerState.cpp  # Queue, combo counter, debuff state per player
├── GameState.h / GameState.cpp      # Top-level coordinator, cross-player sabotage
├── main.cpp                         # CLI test harness
└── Makefile                         # Build system (Linux/Mac)
```

### How the classes relate

```
GameState
├── PlayerState (P1)
│   └── deque<ArrowString>
└── PlayerState (P2)
    └── deque<ArrowString>
            └── vector<Direction>  ← defined in Arrow.h
```

`GameState` is the only class that sees both players — it's where sabotage logic lives. `PlayerState` knows nothing about its opponent. `ArrowString` knows nothing about players. Each layer only knows about the layer directly below it.

---

## Building and Running

**Linux / Mac**
```bash
make
./game_cli
```

**Windows (PowerShell)**
```powershell
g++ main.cpp Arrow.cpp ArrowString.cpp PlayerState.cpp GameState.cpp -o game
./game.exe
```

---

## Reading the CLI Display

```
=== Player 1 | Combo: 0 | Speed: x1 ===
           [^]  [v] >>[<]<< [>]         ← queued string (falling next)
  [ACTIVE] >>[^]<< [>]  [v]  (age: 3)  ← current string to type
```

- `[ACTIVE]` — the bottommost string; this is what you must type right now
- `>>[^]<<` — the arrow the cursor is currently pointing at (next key to press)
- `✓` — an arrow you've already matched correctly
- `age: N` — how many ticks this string has been alive; reaches 20 = you lose

---

## Roadmap

- [x] Core game logic (CLI)
- [x] Arrow sequence generation
- [x] Input matching and typo tracking
- [x] Combo counter and sabotage triggers
- [x] Speed debuff system
- [x] Win/loss detection
- [ ] SFML window and rendering
- [ ] Visual falling animation
- [ ] Combo flash effects and debuff indicator
- [ ] Sound effects
- [ ] Main menu and restart screen

---

## References

- Dance Dance Revolution — Konami (1998)
- ztype — Phoboslab (2012) — https://zty.pe
- SFML Documentation — https://www.sfml-dev.org
