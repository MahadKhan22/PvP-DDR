# PvP-DDR
CS-107 semester 2 C++ project — PvP Dance Dance Revolution

A real-time two-player competitive game where players race to clear falling arrow sequences before they reach the bottom. Built in C++ with **SFML 2.6** for graphics and input.

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
| Restart (after game over) | `R` | `R` | |
| Quit | `Esc` | `Esc` | |

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
├── Arrow.h / Arrow.cpp              # Direction enum, key-to-direction mapping
├── ArrowString.h / ArrowString.cpp  # One falling sequence (data + input tracking)
├── PlayerState.h / PlayerState.cpp  # Queue, combo counter, debuff state per player
├── GameState.h / GameState.cpp      # Top-level coordinator, cross-player sabotage
├── Renderer.h / Renderer.cpp        # SFML rendering layer (NEW)
├── main.cpp                         # SFML window + event loop + fixed-timestep tick
└── Makefile                         # Build system
```

### How the classes relate

```
GameState
├── PlayerState (P1)
│   └── deque<ArrowString>
└── PlayerState (P2)
    └── deque<ArrowString>
            └── vector<Direction>  ← defined in Arrow.h

Renderer (read-only consumer of GameState)
```

`GameState` is the only class that sees both players. `Renderer` is a pure consumer of state — it never mutates anything. Each layer only knows about the layer directly below it. The same architecture from the CLI version carried over to SFML untouched; only `main.cpp` was rewritten and `Renderer.*` was added.

---

## Building and Running

### Windows (MSYS2 / MinGW — UCRT64 environment recommended)

Install the toolchain and SFML:
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-make \
          mingw-w64-ucrt-x86_64-sfml
```

Build and run:
```bash
mingw32-make
./game.exe
```


---

## How the rendering works

- The window is 1200×800, divided into two equal columns separated by a vertical bar.
- Each falling arrow string is drawn at a y-position derived from its `age` (0 → top, 20 → bottom = death).
- Game logic ticks at a fixed **10 Hz**. The window renders at **60 FPS**, with the renderer interpolating arrow positions between ticks for smooth falling motion.
- The bottommost (active) string of each column gets a highlighted backdrop with a yellow cursor on the next-arrow-to-press.
- Combo triggers (5 or 10 clean clears) flash the player's column yellow.
- Speed debuffs tint the affected column purple and show a "DEBUFF! Nt" countdown.
- A red floor line marks the danger zone where strings cause game-over on contact.

---

## Roadmap

- [x] Core game logic
- [x] Arrow sequence generation
- [x] Input matching and typo tracking
- [x] Combo counter and sabotage triggers
- [x] Speed debuff system
- [x] Win/loss detection
- [x] SFML window and rendering
- [x] Visual falling animation (smoothly interpolated between ticks)
- [x] Combo flash effects and debuff indicator
- [x] Restart screen
- [ ] Sound effects
- [ ] Main menu

---

## References

- Dance Dance Revolution — Konami (1998)
- ztype — Phoboslab (2012) — https://zty.pe
- SFML 2.6 Documentation — https://www.sfml-dev.org/documentation/2.6.2/
