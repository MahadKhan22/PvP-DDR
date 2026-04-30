#include "Renderer.h"
#include <algorithm>
#include <iostream>

// ---------------------------------------------------------------------------
// Layout
// ---------------------------------------------------------------------------
// The window is divided into two equal-width columns separated by a thin
// divider. Each column has a fixed "play area" extending from columnTopY_
// down to columnBottomY_ — that's the vertical range an arrow string can
// occupy as it ages from 0 to maxStringAge.
//
// HUD takes the top ~80px of each column (player label, combo, speed).
// The bottom ~40px of the window is reserved for global stats.
// ---------------------------------------------------------------------------

Renderer::Renderer(unsigned windowWidth, unsigned windowHeight)
    : winW_(windowWidth), winH_(windowHeight)
{
    columnWidth_   = static_cast<float>(winW_) / 2.0f;
    p1ColumnX_     = columnWidth_ / 2.0f;
    p2ColumnX_     = columnWidth_ / 2.0f + columnWidth_;
    columnTopY_    = 100.0f;                           // below the per-column HUD
    columnBottomY_ = static_cast<float>(winH_) - 60.0f; // above the global footer
}

// ---------------------------------------------------------------------------
// Font loading
// ---------------------------------------------------------------------------
// SFML refuses to draw any sf::Text unless a font has been loaded. We try
// a list of common paths so the project works on any of the team's machines
// without requiring an asset directory.
// ---------------------------------------------------------------------------

bool Renderer::loadFont() {
    const char* candidates[] = {
        "assets/font.ttf",                          // local override
        "C:/Windows/Fonts/consola.ttf",             // Windows Consolas
        "C:/Windows/Fonts/arial.ttf"                // Windows Arial fallback
    };
    for (const char* path : candidates) {
        if (font_.loadFromFile(path)) {
            fontLoaded_ = true;
            std::cout << path << std::endl;         // see which
            return true;
        }
    }
    std::cerr << "[Renderer] FATAL: could not load any font. "
                 "Drop a TTF at ./assets/font.ttf and try again.\n";
    return false;
}

// ---------------------------------------------------------------------------
// Per-frame animation tick (called every real frame, not every game tick)
// ---------------------------------------------------------------------------

void Renderer::updateAnimations(float dtSeconds) {
    p1FlashTime_ = std::max(0.0f, p1FlashTime_ - dtSeconds);
    p2FlashTime_ = std::max(0.0f, p2FlashTime_ - dtSeconds);
}

void Renderer::notifyComboFlash(int playerId) {
    if (playerId == 0) p1FlashTime_ = FLASH_DURATION;
    else               p2FlashTime_ = FLASH_DURATION;
}

// ---------------------------------------------------------------------------
// Top-level draw
// ---------------------------------------------------------------------------

void Renderer::draw(sf::RenderWindow& window, const GameState& game, float tickProgress) {
    // Clamp tickProgress defensively — if main.cpp passes a wonky value,
    // we'd rather draw stale than draw garbage off-screen.
    tickProgress = std::max(0.0f, std::min(1.0f, tickProgress));

    // Background
    window.clear(sf::Color(15, 15, 25));

    // Compute flash strengths (0.0 = no flash, 1.0 = full flash)
    float p1Flash = p1FlashTime_ / FLASH_DURATION;
    float p2Flash = p2FlashTime_ / FLASH_DURATION;

    // Draw each column
    drawPlayerColumn(window, game.player(0), p1ColumnX_,
                     game.maxStringAge(), tickProgress, p1Flash);
    drawPlayerColumn(window, game.player(1), p2ColumnX_,
                     game.maxStringAge(), tickProgress, p2Flash);

    // Vertical divider between columns
    sf::RectangleShape divider({2.0f, static_cast<float>(winH_)});
    divider.setPosition(static_cast<float>(winW_) / 2.0f - 1.0f, 0.0f);
    divider.setFillColor(sf::Color(70, 70, 90));
    window.draw(divider);

    // Floor line: visualises where age == maxStringAge (death)
    sf::RectangleShape floorLine({static_cast<float>(winW_), 3.0f});
    floorLine.setPosition(0.0f, columnBottomY_);
    floorLine.setFillColor(sf::Color(180, 50, 50));
    window.draw(floorLine);

    // HUD (global stats footer)
    drawHud(window, game);

    // Game-over overlay (drawn last so it's on top)
    if (game.isOver()) {
        drawGameOver(window, game);
    }
}

// ---------------------------------------------------------------------------
// drawPlayerColumn
// ---------------------------------------------------------------------------
// Renders one player's slice of the window. Iterates the player's queue
// from front (active, bottommost) to back (newest, topmost), positioning
// each ArrowString by its age.
// ---------------------------------------------------------------------------

void Renderer::drawPlayerColumn(sf::RenderWindow& window,
                                const PlayerState& player,
                                float columnCenterX,
                                int   maxStringAge,
                                float tickProgress,
                                float flashStrength) const
{
    // --- Column background tint ---
    // Three states layered:
    //   1. Base tint based on player id (P1 blue-ish, P2 red-ish)
    //   2. Combo flash overlay
    //   3. Debuff overlay (purple) when speed-debuffed
    sf::RectangleShape colBg;
    colBg.setSize({columnWidth_, static_cast<float>(winH_)});
    colBg.setPosition(columnCenterX - columnWidth_ / 2.0f, 0.0f);

    sf::Color baseColor = (player.id() == 0)
        ? sf::Color(20, 30, 50)    // P1: cool blue
        : sf::Color(50, 25, 30);   // P2: warm red

    if (flashStrength > 0.0f) {
        // Tween toward bright yellow during combo flash
        sf::Uint8 flashR = static_cast<sf::Uint8>(baseColor.r + (255 - baseColor.r) * flashStrength);
        sf::Uint8 flashG = static_cast<sf::Uint8>(baseColor.g + (215 - baseColor.g) * flashStrength);
        sf::Uint8 flashB = static_cast<sf::Uint8>(baseColor.b + (50  - baseColor.b) * flashStrength);
        baseColor = sf::Color(flashR, flashG, flashB);
    }
    colBg.setFillColor(baseColor);
    window.draw(colBg);

    if (player.isDebuffed()) {
        // Translucent purple wash to indicate the speed debuff
        sf::RectangleShape debuffWash;
        debuffWash.setSize({columnWidth_, static_cast<float>(winH_)});
        debuffWash.setPosition(columnCenterX - columnWidth_ / 2.0f, 0.0f);
        debuffWash.setFillColor(sf::Color(120, 50, 180, 60));
        window.draw(debuffWash);
    }

    // --- Per-column HUD ---
    std::string label = "PLAYER " + std::to_string(player.id() + 1);
    sf::Text title = makeText(label, 28, sf::Color(220, 220, 220),
                              columnCenterX, 20.0f, true);
    window.draw(title);

    // Combo display — turns green near the standard combo threshold,
    // gold near the mega threshold
    sf::Color comboColor = sf::Color(200, 200, 200);
    if (player.combo() >= PlayerState::COMBO_MEGA)        comboColor = sf::Color(255, 200, 50);
    else if (player.combo() >= PlayerState::COMBO_STANDARD) comboColor = sf::Color(120, 220, 120);

    sf::Text combo = makeText("Combo: " + std::to_string(player.combo()),
                              20, comboColor,
                              columnCenterX - 90.0f, 55.0f);
    window.draw(combo);

    // Speed multiplier
    char speedBuf[32];
    std::snprintf(speedBuf, sizeof(speedBuf), "Speed: x%.1f", player.currentSpeed());
    sf::Text speed = makeText(speedBuf, 20,
                              player.isDebuffed() ? sf::Color(220, 120, 220) : sf::Color(200, 200, 200),
                              columnCenterX + 10.0f, 55.0f);
    window.draw(speed);

    // Debuff countdown (when active)
    if (player.isDebuffed()) {
        std::string dbText = "DEBUFF! " + std::to_string(player.debuffTicksLeft()) + "t";
        sf::Text db = makeText(dbText, 16, sf::Color(255, 100, 255),
                               columnCenterX, 80.0f, true);
        window.draw(db);
    }

    // --- Arrow strings ---
    // Walk the queue front-to-back. Front = active, oldest (largest age),
    // so it has the largest y. Newer strings are higher up.
    const auto& queue = player.queue();
    bool isFirst = true;
    for (const auto& str : queue) {
        // Smooth y interpolation: age + tickProgress so arrows fall
        // continuously between game ticks.
        float effectiveAge = static_cast<float>(str.age()) + tickProgress;
        float t = effectiveAge / static_cast<float>(maxStringAge);
        if (t > 1.0f) t = 1.0f; // shouldn't happen — game ends at age==maxAge
        if (t < 0.0f) t = 0.0f;
        float y = columnTopY_ + t * (columnBottomY_ - columnTopY_);

        drawArrowString(window, str, columnCenterX, y, isFirst);
        isFirst = false;
    }
}

// ---------------------------------------------------------------------------
// drawArrowString
// ---------------------------------------------------------------------------
// Draws one sequence as a row of arrow tiles centered on (centerX, centerY).
// Each tile is 50x50 with a 6px gap. Already-matched arrows are dim, the
// cursor arrow is highlighted, future arrows are bright.
// ---------------------------------------------------------------------------

void Renderer::drawArrowString(sf::RenderWindow& window,
                               const ArrowString& str,
                               float centerX,
                               float centerY,
                               bool  isActive) const
{
    constexpr float TILE = 50.0f;
    constexpr float GAP  = 6.0f;
    constexpr float STRIDE = TILE + GAP;

    const auto& seq = str.sequence();
    const int n = static_cast<int>(seq.size());
    const int cursor = str.currentIndex();

    float totalWidth = n * TILE + (n - 1) * GAP;
    float startX = centerX - totalWidth / 2.0f;

    // Optional active-string backdrop (subtle pill behind the row)
    if (isActive) {
        sf::RectangleShape backdrop;
        backdrop.setSize({totalWidth + 24.0f, TILE + 16.0f});
        backdrop.setOrigin(backdrop.getSize().x / 2.0f, backdrop.getSize().y / 2.0f);
        backdrop.setPosition(centerX, centerY);
        backdrop.setFillColor(sf::Color(40, 40, 60, 200));
        backdrop.setOutlineColor(str.hasTypo() ? sf::Color(220, 80, 80) : sf::Color(120, 200, 255));
        backdrop.setOutlineThickness(2.0f);
        window.draw(backdrop);
    }

    for (int i = 0; i < n; ++i) {
        float tileX = startX + i * STRIDE;
        float tileY = centerY - TILE / 2.0f;

        // Tile colour state machine
        sf::Color fill, outline;
        if (i < cursor) {
            // already matched
            fill    = sf::Color(45, 90, 45);
            outline = sf::Color(80, 160, 80);
        } else if (i == cursor && isActive && !str.isComplete()) {
            // current cursor target
            fill    = sf::Color(220, 200, 80);
            outline = sf::Color(255, 255, 120);
        } else {
            // upcoming
            fill    = sf::Color(60, 60, 80);
            outline = sf::Color(120, 120, 150);
        }

        sf::RectangleShape tile({TILE, TILE});
        tile.setPosition(tileX, tileY);
        tile.setFillColor(fill);
        tile.setOutlineColor(outline);
        tile.setOutlineThickness(2.0f);
        window.draw(tile);

        // Arrow glyph centered in the tile
        std::string glyph(1, dirToChar(seq[i]));
        sf::Color glyphColor = (i < cursor) ? sf::Color(180, 220, 180)
                                            : sf::Color(255, 255, 255);
        sf::Text t = makeText(glyph, 32, glyphColor,
                              tileX + TILE / 2.0f, tileY + TILE / 2.0f, true);
        window.draw(t);
    }
}

// ---------------------------------------------------------------------------
// drawHud — global footer: tick count, global speed multiplier
// ---------------------------------------------------------------------------

void Renderer::drawHud(sf::RenderWindow& window, const GameState& game) const {
    char buf[128];
    std::snprintf(buf, sizeof(buf),
                  "Tick: %d   |   Global Speed: x%.1f",
                  game.globalTick(), game.globalSpeedMult());

    sf::Text hud = makeText(buf, 18, sf::Color(170, 170, 200),
                            static_cast<float>(winW_) / 2.0f,
                            static_cast<float>(winH_) - 30.0f, true);
    window.draw(hud);
}

// ---------------------------------------------------------------------------
// drawGameOver — banner overlay shown when isOver() returns true
// ---------------------------------------------------------------------------

void Renderer::drawGameOver(sf::RenderWindow& window, const GameState& game) const {
    // Dim everything behind
    sf::RectangleShape veil;
    veil.setSize({static_cast<float>(winW_), static_cast<float>(winH_)});
    veil.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(veil);

    std::string msg;
    if (game.winner() == 0)      msg = "PLAYER 1 WINS!";
    else if (game.winner() == 1) msg = "PLAYER 2 WINS!";
    else                         msg = "DRAW!";

    sf::Text banner = makeText(msg, 60, sf::Color(255, 230, 100),
                               static_cast<float>(winW_) / 2.0f,
                               static_cast<float>(winH_) / 2.0f - 30.0f,
                               true);
    window.draw(banner);

    sf::Text hint = makeText("Press R to restart   |   Esc to quit",
                             22, sf::Color(220, 220, 220),
                             static_cast<float>(winW_) / 2.0f,
                             static_cast<float>(winH_) / 2.0f + 40.0f,
                             true);
    window.draw(hint);
}

// ---------------------------------------------------------------------------
// makeText — small convenience wrapper. SFML 2's sf::Text constructor
// is a little awkward (no positioning args), so we wrap it.
// ---------------------------------------------------------------------------

sf::Text Renderer::makeText(const std::string& s,
                            unsigned size,
                            sf::Color color,
                            float x, float y,
                            bool centerOrigin) const
{
    sf::Text t;
    if (fontLoaded_) t.setFont(font_);
    t.setString(s);
    t.setCharacterSize(size);
    t.setFillColor(color);
    if (centerOrigin) {
        sf::FloatRect b = t.getLocalBounds();
        // SFML quirk: getLocalBounds includes a top/left offset that's not zero
        // for many fonts. Subtract it so the origin is the *visual* center.
        t.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
    }
    t.setPosition(x, y);
    return t;
}
