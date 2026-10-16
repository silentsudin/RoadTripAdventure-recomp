// Authored rumble/haptics model: implementation.
//
// Evidence ledger, call sites and envelope decoding:
// docs/native-reference/rumble-haptics.md. Everything marked CONFIRMED below
// is ELF-grounded (SLUS_203.98, vaddr = fileOffset + 0x1ff000); AUTHORED
// parts are native policy choices the original cannot answer (no side
// channels on a DualShock, no explicit scrape event, tie behavior beyond the
// primitive's unconditional replace).
#include "rumble.h"

#include <cmath>

namespace rt::native::gameplay {
namespace rumble {

// Original gate: (playerByte & masterByte) != 0 (0x0020ACBC..0x0020ACC8:
// lbu a1,+0xE ; lbu v0,0xC(a0+player) ; and ; beqz -> return).
bool gateOpen(std::uint8_t masterByte, std::uint8_t playerByte) {
    return (playerByte & masterByte) != 0;
}

std::uint8_t collisionGrade(float closingSpeed) {
    // CONFIRMED 0x00217870..0x00217930: strength = int(|dv| * 1000.0f),
    // cvt.w.s under the EE default round-to-nearest mode.
    const float scaled = std::fabs(closingSpeed) * kCollisionStrengthScale;
    const long  grade  = std::lrintf(scaled);  // ties-to-even, like cvt.w.s
    if (grade <= 0) return 0;                 // CONFIRMED 0x0020ACD4: 0 is silent
    // CONFIRMED 0x0020ACF4: >= 0x100 saturates to 0xFF.
    if (grade >= (kStrengthMax + 1)) return static_cast<std::uint8_t>(kStrengthMax);
    return static_cast<std::uint8_t>(grade);
}

std::uint8_t landingGrade(float rawFallSpeed) {
    // CONFIRMED 0x0021B33C/0x0021B358: require speed >= 0xB3 (179), then
    // strength = speed >> 4 on the integer raw value.
    const long raw = static_cast<long>(std::fabs(rawFallSpeed));
    if (raw < kLandingSpeedGate) return 0;
    const long shifted = raw >> kLandingShift;  // min 11 at the gate
    // The primitive saturates anything >= 0x100 to 0xFF.
    if (shifted > kStrengthMax) return static_cast<std::uint8_t>(kStrengthMax);
    return static_cast<std::uint8_t>(shifted);
}

// AUTHORED: one quarter of the collision grade, so sustained scraping reads
// under any real impact. Marked AUTHORED because the original has no scrape
// event (walls re-fire full-strength impulse patterns; see the ledger).
std::uint8_t scrapeGrade(float closingSpeed) {
    return static_cast<std::uint8_t>(collisionGrade(closingSpeed) >> 2);
}

namespace {

constexpr Envelope envelopeOf(Pattern p) {
    switch (p) {
        case Pattern::Landing:     return kEnvelopeLanding;
        case Pattern::ImpactShort: return kEnvelopeImpactShort;
        case Pattern::CrashClick:  return kEnvelopeCrashClick;
        case Pattern::CrashPunch:  return kEnvelopeCrashPunch;
        case Pattern::CrashBuzz:   return kEnvelopeCrashBuzz;
        case Pattern::Collision:   return kEnvelopeCollision;
        case Pattern::MenuTest:    return kEnvelopeMenuTest;
        case Pattern::Stop:
        default:                   return kEnvelopeStop;
    }
}

float clamp01(float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }

// Side weighting from the car-local push normal (AUTHORED, see header).
// lateralTilt in [-1, 1]: +1 = pushed fully rightward (hit came from the
// LEFT side) => struck-side motor keeps full gain while the opposite motor
// dips to kSideGainFloor. 0 (head-on / pure-forward) = both motors full,
// so centered contacts preserve the CONFIRMED envelope levels exactly.
void sideGains(float lateralTilt, float& leftGain, float& rightGain) {
    if (lateralTilt > 1.0f) lateralTilt = 1.0f;
    if (lateralTilt < -1.0f) lateralTilt = -1.0f;
    const float dip = 1.0f - kSideGainFloor;
    // Pushed +x (left-side hit): left motor unaffected, right motor dips.
    leftGain  = 1.0f - dip * (lateralTilt < 0.0f ? -lateralTilt : 0.0f);
    rightGain = 1.0f - dip * (lateralTilt > 0.0f ?  lateralTilt : 0.0f);
}

// Normalizes the horizontal push normal and produces the motor gains.
// A pure +x push (hit from the left) is tilt +1, a head-on/rear contact
// (x == 0) is tilt 0, and diagonals tilt proportionally.
void gainsFromNormal(float lateralRight, float forward, bool hasSide,
                     float& leftGain, float& rightGain) {
    if (!hasSide) {
        leftGain = 1.0f;
        rightGain = 1.0f;
        return;
    }
    const float len = std::sqrt(lateralRight * lateralRight + forward * forward);
    const float tilt = len > 0.0f ? lateralRight / len : 0.0f;
    sideGains(tilt, leftGain, rightGain);
}

// Arbitration priorities (AUTHORED over the CONFIRMED primitive, which
// replaces unconditionally — last caller wins). Higher beats lower; equal
// ranks replace (mirroring the original replace-on-new-event), so a new
// landing overrides a scrape while both are active, but a scrape can never
// interrupt a landing/collision burst.
constexpr int kPrioScrape    = 0;
constexpr int kPrioLanding   = 1;
constexpr int kPrioTestBuzz  = 1;
constexpr int kPrioCollision = 2;
constexpr int kPrioCrash     = 3;

}  // namespace

void Channel::setOptions(std::uint8_t masterByte, std::uint8_t playerByte) {
    masterByte_ = masterByte;
    playerByte_ = playerByte;
}

void Channel::setSuppressed(bool suppressed) { suppressed_ = suppressed; }

bool Channel::enabled() const {
    return gateOpen(masterByte_, playerByte_);
}

Output Channel::emit(const Envelope& env, const Request& r) {
    if (env.frames <= 0 || r.strength == 0) return Output{};  // nothing to send
    Output out;
    const float strength01 = static_cast<float>(r.strength) / 255.0f;
    const float large01 = env.scaledLarge ? strength01 : env.fixedLarge;
    out.leftIntensity   = clamp01(large01 * r.leftGain);
    out.rightIntensity  = env.smallMotor ? clamp01(r.rightGain) : 0.0f;
    out.durationFrames  = env.frames;
    return out;
}

void Channel::offer(const Request& r) {
    // The original primitive overwrites the active effect unconditionally;
    // native policy keeps that for equal/higher priorities and shields the
    // active burst from lower-priority events (see kPrio*).
    const int incumbent = hasPending_ ? pending_.priority
                         : (remainingFrames_ > 0 ? active_.priority : -1);
    if (static_cast<int>(r.priority) < incumbent) return;
    pending_ = r;
    hasPending_ = true;
}

void Channel::submitCollision(float lateralRight, float forward,
                              float closingSpeed) {
    if (suppressed_ || !enabled()) return;
    const std::uint8_t strength = collisionGrade(closingSpeed);
    if (strength == 0) return;  // CONFIRMED deadzone: grade 0 never rumbles
    Request r;
    r.pattern = Pattern::Collision;
    r.priority = kPrioCollision;
    r.strength = strength;
    gainsFromNormal(lateralRight, forward, true, r.leftGain, r.rightGain);
    offer(r);
}

void Channel::submitObjectCollision(float lateralRight, float forward) {
    if (suppressed_ || !enabled()) return;
    Request r;
    r.pattern = Pattern::Collision;
    r.priority = kPrioCollision;
    r.strength = kObjectCollisionStrength;  // CONFIRMED fixed 0x40 @0x00253CA0
    gainsFromNormal(lateralRight, forward, true, r.leftGain, r.rightGain);
    offer(r);
}

void Channel::submitLanding(const float* pointFallSpeeds, std::size_t pointCount) {
    if (suppressed_ || !enabled() || pointFallSpeeds == nullptr) return;
    float best = 0.0f;
    for (std::size_t i = 0; i < pointCount; ++i) {
        if (pointFallSpeeds[i] > best) best = pointFallSpeeds[i];
    }
    const std::uint8_t strength = landingGrade(best);
    if (strength == 0) return;  // CONFIRMED 0xB3 gate
    Request r;
    r.pattern = Pattern::Landing;
    r.priority = kPrioLanding;
    r.strength = strength;
    // No lateral information: un-tilted envelope levels.
    offer(r);
}

void Channel::submitScrape(Side side, float closingSpeed) {
    if (suppressed_ || !enabled()) return;
    const std::uint8_t strength = scrapeGrade(closingSpeed);
    if (strength == 0) return;
    Request r;
    // Pattern 2 (0x2EA3E0): small motor + strength-scaled large motor, held
    // 4 frames — the only scaled short-burst envelope in the CONFIRMED table,
    // and the natural fit for a sustained wall grind that re-fires per frame.
    // Its table record has no observed jal caller (see the ledger); adopting
    // it for scrapes is AUTHORED.
    r.pattern = Pattern::ImpactShort;
    r.priority = kPrioScrape;
    r.strength = strength;
    sideGains(side == Side::Left ? 1.0f : -1.0f, r.leftGain, r.rightGain);
    offer(r);
}

void Channel::submitCrash(int collisionCode) {
    if (suppressed_ || !enabled()) return;
    const Pattern p = crashPatternForCode(collisionCode);
    if (p == Pattern::Stop) return;  // CONFIRMED: code 0 / out-of-range never rumble
    Request r;
    r.pattern = p;
    r.priority = kPrioCrash;
    r.strength = static_cast<std::uint8_t>(kStrengthMax);  // CONFIRMED 0xFF
    offer(r);
}

void Channel::submitTestBuzz() {
    if (suppressed_ || !enabled()) return;
    Request r;
    r.pattern = Pattern::MenuTest;
    r.priority = kPrioTestBuzz;
    // Pattern 7 carries its own fixed 127/255 level; the original passes
    // strength 1, which the envelope ignores (literal, not 0x84-scaled).
    r.strength = 1;
    offer(r);
}

void Channel::stop() {
    pending_ = Request{};
    hasPending_ = false;
    active_ = Request{};
    remainingFrames_ = 0;
    // CONFIRMED 0xFF-end semantics: motors off. Emit nothing further; a
    // stopped channel reports inactive and tick() returns zeros.
    lastBurst_ = Output{};
}

Output Channel::tick() {
    if (hasPending_) {
        // Highest accepted submission of the frame becomes the active effect
        // (CONFIRMED: a new event replaces the playing one; the priority
        // shield above is the AUTHORED refinement).
        active_ = pending_;
        pending_ = Request{};
        hasPending_ = false;
        remainingFrames_ = envelopeOf(active_.pattern).frames;
        lastBurst_ = emit(envelopeOf(active_.pattern), active_);
        return lastBurst_;
    }
    if (remainingFrames_ > 0) {
        remainingFrames_ -= 1;
        if (remainingFrames_ == 0) {
            active_ = Request{};   // envelope end == 0xFF: motors off
            lastBurst_ = Output{};
        }
    }
    return Output{};
}

}  // namespace rumble
}  // namespace rt::native::gameplay
