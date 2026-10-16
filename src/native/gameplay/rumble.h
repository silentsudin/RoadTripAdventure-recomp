// Authored rumble/haptics model (PS2 collision-rumble grounding).
//
// Grounding source: the original game's rumble primitive at ELF 0x0020ACB8
// (gate + strength clamp + effect table), the per-frame script interpreter at
// 0x0020A660, the pattern pointer table at 0x0029AD60 and the 8-byte effect
// records at 0x2EA3D0..0x2EA410. Each event site below cites its original
// call site; the full CONFIRMED/INFERRED/OPEN ledger lives in
// docs/native-reference/rumble-haptics.md.
//
// This module expresses the decoded *envelope semantics* of those records
// (motor classes, holds, totals) — no verbatim game bytes are stored. It is
// pure host-side C++: no SDL, no guest memory, no device calls; the document
// above carries the SDL3 glue snippet main.cpp should add.
#pragma once
#include <cstddef>
#include <cstdint>

namespace rt::native::gameplay {
namespace rumble {

// ---------------------------------------------------------------------------
// Settings gate — mirrors the original option semantics.
//
// The options screen writes the master rumble byte at 0x0177AC50+0xE with the
// values 0x00 / 0x02 / 0x04 (ELF 0x00208E80..0x00208E94) and the primitive
// gates every trigger on (playerByte & masterByte) != 0, where the per-player
// bytes live at 0x0177AC50+0xC/+0xD (ELF 0x0020ACBC..0x0020ACC8).
// Which UX level masks 0x02 vs 0x04 correspond to remains OPEN; the gate
// arithmetic itself is CONFIRMED.
// ---------------------------------------------------------------------------

constexpr std::uint8_t kMasterOff    = 0x00;  // options-screen "off" write
constexpr std::uint8_t kMasterMaskA = 0x02;   // options-screen "on" write A
constexpr std::uint8_t kMasterMaskB = 0x04;   // options-screen "on" write B

// CONFIRMED gate: rumble is allowed only when (playerByte & masterByte) != 0.
bool gateOpen(std::uint8_t masterByte, std::uint8_t playerByte);

// ---------------------------------------------------------------------------
// Strength grading — CONFIRMED formulas from the ORIGINAL call sites.
// ---------------------------------------------------------------------------

// 0x00217870..0x00217880: collision strength = |dv| * 1000.0f (0x447A0000),
// then cvt.w.s at 0x00217930. Range semantics per the primitive: 0 = silent,
// >= 0x100 clamps to 0xFF (0x0020ACD4 beqz / 0x0020ACF4 movz).
constexpr float kCollisionStrengthScale = 1000.0f;

// 0x0021B33C: landing rumble requires a raw fall speed >= 0xB3 (179).
// 0x0021B358: strength = speed >> 4. Units match CarState::fallSpeed's raw
// per-tick gravity accumulation (kGravityPerTick = 88: ~3 airborne ticks).
constexpr int kLandingSpeedGate  = 179;  // slti $v0, $a3, 0xb3
constexpr int kLandingShift      = 4;    // sra $a3, $a3, 4
// 0x00253CA0: car<->object collisions use a fixed 0x40 strength.
constexpr std::uint8_t kObjectCollisionStrength = 0x40;
// 0x0020ACF4: strengths saturate at 0xFF.
constexpr int kStrengthMax = 255;

// 0x0021D358 GetSpeedFromF15Vector: display speed = vector length * 216.0.
// Glue converts a host display-unit relative speed to the |dv| the grading
// expects (closingSpeed = displaySpeed / 216.0f).
constexpr float kVelocityToDisplayScale = 216.0f;

// Collision grade from |dv| in original float units: round(|dv| * 1000),
// clamped to 0..kStrengthMax. Grade 0 is silence (the primitive's beqz).
std::uint8_t collisionGrade(float closingSpeed);
// Landing grade from a raw per-point fall speed: 0 below the 179 gate, else
// speed >> 4 (arithmetically clamped to 255 like the primitive does).
std::uint8_t landingGrade(float rawFallSpeed);
// AUTHORED (not in the original): scrapes are graded at one quarter of the
// collision grade (>>2) so a wall grind stays under real impact strength.
std::uint8_t scrapeGrade(float closingSpeed);

// ---------------------------------------------------------------------------
// Effect patterns — decoded from the pointer table at 0x0029AD60 and the
// envelope records at 0x2EA3D0. The interpreter opcodes are 0x80 small-motor
// on / 0x81 off / 0x82 <lit> large-motor set / 0x83 large-motor off /
// 0x84 large-motor = event strength, literals 0x01..0x7F are frame holds,
// 0xFF ends the effect with all motors off (0x0020A660..0x0020A878).
// ---------------------------------------------------------------------------

enum class Pattern : std::uint8_t {
    Stop        = 0,  // 0xFF first          -> cancel everything
    Landing     = 1,  // 0x0021B360          wheel landing
    ImpactShort = 2,  // no caller found     short scaled burst
    CrashClick  = 3,  // impulse codes 2,8,10
    CrashPunch  = 4,  // impulse codes 1,4,5
    CrashBuzz   = 5,  // impulse codes 3,6,7,9
    Collision   = 6,  // 0x0021793C/A0, 0x00253C9C
    MenuTest    = 7,  // 0x00208EA0/B4        options-screen test buzz
};

// Aggregate envelope of one pattern, decoded from the effect records:
//   frames     — total 60Hz hold length (CONFIRMED per pattern),
//   smallMotor — whether the small/high-frequency motor participates,
//   scaledLarge— large/low-frequency motor level = event strength / 255,
//   fixedLarge — large-motor level when !scaledLarge (127/255 for MenuTest,
//                1.0 = 0xFF for the crash family).
constexpr struct Envelope {
    int  frames;
    bool smallMotor;
    bool scaledLarge;
    float fixedLarge;
} kEnvelopeStop        = {0,  false, false, 0.0f},            // 0x2EA3D0
  kEnvelopeLanding     = {8,  true,  true,  0.0f},            // 0x2EA3D8
  kEnvelopeImpactShort = {4,  true,  true,  0.0f},            // 0x2EA3E0
  kEnvelopeCrashClick  = {3,  true,  false, 1.0f},            // 0x2EA3E8
  kEnvelopeCrashPunch  = {2,  true,  false, 1.0f},            // 0x2EA3F0
  kEnvelopeCrashBuzz   = {2,  true,  false, 1.0f},            // 0x2EA3F8
  kEnvelopeCollision   = {8,  true,  true,  0.0f},            // 0x2EA400
  kEnvelopeMenuTest    = {30, false, false, 127.0f / 255.0f};  // 0x2EA408

// The impulse dispatcher at 0x219C3C jumps through the 11-entry table at
// 0x2EAE60 (codes 0..10; 0x219C2C sltiu bounds the dispatch to < 0xb).
// CONFIRMED code classes — rumble arms 0x219C50/0x219C60/0x219C80 set
// patterns 4 / 3 / 5 respectively, all at 0xFF strength:
//   0 -> no rumble;  1,4,5 -> CrashPunch;  2,8,10 -> CrashClick;
//   3,6,7,9 -> CrashBuzz. Code 11+ (the "code 15 crash" note in
// functions.tsv is [inferred]) does not reach the rumble dispatch.
constexpr Pattern crashPatternForCode(int collisionCode) {
    switch (collisionCode) {
        case 1: case 4: case 5:  return Pattern::CrashPunch;
        case 2: case 8: case 10: return Pattern::CrashClick;
        case 3: case 6: case 7: case 9: return Pattern::CrashBuzz;
        default: return Pattern::Stop;  // code 0 / out-of-range: no rumble
    }
}

// ---------------------------------------------------------------------------
// Per-device channel.
// ---------------------------------------------------------------------------

// One 60Hz frame = 1.0 tick. Envelope durations are in frames.
constexpr float kRumbleFrameHz = 60.0f;

enum class Side { Left, Right };

// Device burst. Emitted for exactly one tick when an effect starts; later
// ticks of the same effect emit an inactive Output (intensities 0) so a host
// that can only "set and timeout" (SDL_RumbleGamepad) commands it once.
struct Output {
    float leftIntensity  = 0.0f;  // low-frequency / large motor, 0..1
    float rightIntensity = 0.0f;  // high-frequency / small motor, 0..1
    int   durationFrames = 0;     // envelope length; 0 = no command
    bool  active() const { return durationFrames > 0; }
    float durationSeconds() const { return durationFrames / kRumbleFrameHz; }
};

// A lateral contact normal in car-local space for side weighting:
// (lateralRight, forward) is the direction the car is PUSHED by the contact
// (from obstacle into car), x = right, z = forward, any length; a hit on the
// car's LEFT pushes +x and biases the LEFT motor. The channel normalizes it
// internally; events without lateral information (landing, crash, test buzz)
// use the un-tilted envelope levels.
//
// The left/right bias itself is AUTHORED-for-native: the PS2 DualShock has
// one small + one large motor with no side channels; the grading (strength
// formulas, patterns, gates, durations) stays CONFIRMED. Side gain floor
// keeps the opposite motor alive (never fully dark during an impact).
constexpr float kSideGainFloor = 0.35f;

class Channel {
public:
    // Gate state. Pass the native settings-store rumble option byte(s) each
    // time they change (or once per frame); values mirror the original
    // master encodings {0x00, 0x02, 0x04}. See gateOpen().
    void setOptions(std::uint8_t masterByte, std::uint8_t playerByte);
    // Global suppression flag (0x0177AC50+0x28 bit 0x40): when set, new
    // triggers are refused. Active effects still play out, exactly as the
    // original per-trigger read behaves.
    void setSuppressed(bool suppressed);
    bool enabled() const;

    // -- Frame input sites -------------------------------------------------
    // Car<->car collision (0x0021793C): grading CONFIRMED, |dv| in original
    // float units (closingSpeed = displaySpeed / 216.0f for a host speed).
    void submitCollision(float lateralRight, float forward, float closingSpeed);
    // Car<->object collision (0x00253C9C): pattern 6 with the CONFIRMED
    // fixed 0x40 strength (closing speed not graded by the original).
    void submitObjectCollision(float lateralRight, float forward);
    // Per-point landing speeds in raw fall-speed units (CarState::fallSpeed).
    // The original triggers per landing wheel (last wins); we aggregate to
    // the strongest point per frame for call-order independence (AUTHORED).
    void submitLanding(const float* pointFallSpeeds, std::size_t pointCount);
    // Sustained wall scrape. AUTHORED grading (quarter of collision grade)
    // over the CONFIRMED scaled short-burst envelope (pattern 2), the
    // table record with no observed caller; side biases the motors.
    void submitScrape(Side side, float closingSpeed);
    // Crash impulse codes 0..10 from ApplyCollisionImpulse; full 0xFF
    // strength per pattern class (CONFIRMED).
    void submitCrash(int collisionCode);
    // Options-screen test buzz (pattern 7): 127/255 for 30 frames.
    void submitTestBuzz();
    // Immediately cancel any active effect (pattern 0 / 0xFF-end semantics).
    void stop();

    // Arithmetic for one 60Hz frame: applies the best accepted submission
    // of the frame and emits its burst, or advances/stems the active one.
    Output tick();

    // Introspection for glue/tests.
    bool    isActive() const { return remainingFrames_ > 0; }
    Pattern activePattern() const { return active_.pattern; }
    Output  lastBurst() const { return lastBurst_; }

private:
    struct Request {
        Pattern      pattern = Pattern::Stop;
        int          priority = 0;          // higher wins arbitration
        std::uint8_t strength = 0;          // 0 -> deadzone: no event
        float        leftGain  = 1.0f;
        float        rightGain = 1.0f;
    };

    void offer(const Request& r);
    Output emit(const Envelope& env, const Request& r);

    std::uint8_t masterByte_ = kMasterOff;
    std::uint8_t playerByte_ = 0;
    bool         suppressed_ = false;

    Request pending_;        // best accepted submission of the current frame
    bool    hasPending_ = false;
    Request active_;         // currently playing effect
    int     remainingFrames_ = 0;
    Output  lastBurst_;
};

}  // namespace rumble
}  // namespace rt::native::gameplay
