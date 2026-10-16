// Pins the ELF-grounded rumble model (gameplay/rumble.{h,cpp}) and its
// AUTHORED native policies. Deterministic, headless, no game data.
// Evidence ledger: docs/native-reference/rumble-haptics.md.
#include "../../src/native/gameplay/rumble.h"

#include <cstdio>

using namespace rt::native::gameplay;
using namespace rt::native::gameplay::rumble;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

static bool nearly(float a, float b) { float d = a - b; if (d < 0) d = -d; return d < 1e-6f; }

static Channel testChannel() {
    Channel ch;
    ch.setOptions(kMasterMaskA, kMasterMaskA);
    return ch;
}

// ---- gate: CONFIRMED option encodings ------------------------------------

static void testGate() {
    // Gate arithmetic: (playerByte & masterByte) != 0 (ELF 0x0020ACB8).
    CHECK(!gateOpen(kMasterOff, 0x00));
    CHECK(!gateOpen(kMasterOff, 0x06));
    CHECK(!gateOpen(kMasterMaskA, 0x00));
    CHECK(gateOpen(kMasterMaskA, kMasterMaskA));
    CHECK(gateOpen(kMasterMaskB, 0x06));
    CHECK(gateOpen(0x06, kMasterMaskA));
    CHECK(gateOpen(0x06, kMasterMaskB));
    CHECK(!gateOpen(kMasterMaskA, kMasterMaskB));

    // Options UI encodings {0x00, 0x02, 0x04} against both per-player bytes.
    CHECK(kMasterMaskA == 0x02);
    CHECK(kMasterMaskB == 0x04);

    // Gate off -> all submissions are refused -> all-zero outputs.
    Channel off;
    off.setOptions(kMasterMaskA, kMasterMaskA);
    off.setOptions(kMasterOff, kMasterMaskA);
    off.submitCollision(0.0f, 1.0f, 1.0f);
    off.submitLanding(nullptr, 0);
    off.submitScrape(Side::Left, 1.0f);
    off.submitCrash(2);
    off.submitTestBuzz();
    Output o = off.tick();
    CHECK(!o.active() && o.leftIntensity == 0.0f && o.rightIntensity == 0.0f && o.durationFrames == 0);
    o = off.tick();
    CHECK(!o.active() && o.leftIntensity == 0.0f);

    // Default-constructed channel is disabled (master 0x00).
    Channel fresh;
    fresh.submitCollision(0.0f, 1.0f, 1.0f);
    CHECK(!fresh.tick().active());
    CHECK(!fresh.enabled());

    // Per-player byte decides alone when both mask bits are on.
    Channel perPlayer;
    perPlayer.setOptions(0x06, 0x02);
    perPlayer.submitCollision(0.0f, 1.0f, 1.0f);
    CHECK(perPlayer.tick().active());
    perPlayer.setOptions(0x06, 0x00);
    perPlayer.submitCollision(0.0f, 1.0f, 1.0f);
    CHECK(!perPlayer.tick().active());
}

// ---- CONFIRMED strength grading -------------------------------------------

static void testCollisionGrade() {
    // 0x00217870: |dv| * 1000.0f, cvt.w.s (round-to-nearest) at 0x00217930.
    CHECK(collisionGrade(0.0f) == 0);            // 0 -> silent (0x0020ACD4)
    CHECK(collisionGrade(0.0004f) == 0);         // 0.4 rounds to 0
    CHECK(collisionGrade(0.001f) == 1);
    CHECK(collisionGrade(0.25f) == 250);
    CHECK(collisionGrade(0.255f) == 255);        // 254.99999 rounds to 255
    CHECK(collisionGrade(0.256f) == 255);        // 256 clamps to 0xFF (0x0020ACF4)
    CHECK(collisionGrade(1.0f) == 255);
    CHECK(collisionGrade(-0.25f) == 250);        // magnitude only
    // Monotonic in closing speed.
    const float speeds[] = {0.0004f, 0.005f, 0.05f, 0.10f, 0.15f, 0.25f, 0.255f, 1.0f};
    std::uint8_t prev = 0;
    for (float s : speeds) {
        std::uint8_t g = collisionGrade(s);
        CHECK(g >= prev);
        prev = g;
    }
    CHECK(prev == 255);
}

static void testLandingGrade() {
    // 0x0021B33C: raw speed >= 0xB3 (179) or no rumble; 0x0021B358: >> 4.
    CHECK(landingGrade(0.0f) == 0);
    CHECK(landingGrade(88.0f) == 0);             // one gravity tick: silent
    CHECK(landingGrade(178.0f) == 0);
    CHECK(landingGrade(179.0f) == 11);           // 179 >> 4
    CHECK(landingGrade(192.0f) == 12);
    CHECK(landingGrade(264.0f) == 16);           // 88 * 3
    CHECK(landingGrade(1000.0f) == 62);
    CHECK(landingGrade(4080.0f) == 255);         // 4080 >> 4 == 0xFF
    CHECK(landingGrade(4096.0f) == 255);         // saturates like the primitive
    // Monotonic across the gate.
    const float raws[] = {0.0f, 88.0f, 179.0f, 300.0f, 800.0f, 2000.0f, 5000.0f};
    std::uint8_t prev = 0;
    for (float r : raws) {
        std::uint8_t g = landingGrade(r);
        CHECK(g >= prev);
        prev = g;
    }
    CHECK(prev == 255);
}

static void testScrapeGrade() {
    // AUTHORED: quarter of the collision grade.
    CHECK(scrapeGrade(0.0f) == 0);
    CHECK(scrapeGrade(0.001f) == 0);             // 1 >> 2
    CHECK(scrapeGrade(0.25f) == 62);             // 250 >> 2
    CHECK(scrapeGrade(1.0f) == 63);              // 255 >> 2
}

static void testCrashPatternForCode() {
    // CONFIRMED dispatch classes (jump table at 0x2EAE60).
    CHECK(crashPatternForCode(0) == Pattern::Stop);           // no rumble
    CHECK(crashPatternForCode(1) == Pattern::CrashPunch);
    CHECK(crashPatternForCode(4) == Pattern::CrashPunch);
    CHECK(crashPatternForCode(5) == Pattern::CrashPunch);
    CHECK(crashPatternForCode(2) == Pattern::CrashClick);
    CHECK(crashPatternForCode(8) == Pattern::CrashClick);
    CHECK(crashPatternForCode(10) == Pattern::CrashClick);
    CHECK(crashPatternForCode(3) == Pattern::CrashBuzz);
    CHECK(crashPatternForCode(6) == Pattern::CrashBuzz);
    CHECK(crashPatternForCode(7) == Pattern::CrashBuzz);
    CHECK(crashPatternForCode(9) == Pattern::CrashBuzz);
    CHECK(crashPatternForCode(11) == Pattern::Stop);         // out of table
    CHECK(crashPatternForCode(-1) == Pattern::Stop);
    CHECK(crashPatternForCode(15) == Pattern::Stop);
}

// ---- CONFIRMED envelope grading -> per-device motor outputs ----------------

static void testCollisionBurst() {
    // Pattern 6 (0x2EA400): large = strength for 8 frames, small-motor click.
    Channel ch = testChannel();
    ch.submitCollision(0.0f, 1.0f, 0.25f);       // head-on, grade 250
    Output o = ch.tick();
    CHECK(o.active());
    CHECK(o.durationFrames == 8);               // CONFIRMED envelope total
    CHECK(nearly(o.leftIntensity, 250.0f / 255.0f));   // head-on: full gain
    CHECK(nearly(o.rightIntensity, 1.0f));             // small motor is binary
    CHECK(ch.isActive());
    CHECK(ch.activePattern() == Pattern::Collision);
    // The burst is emitted once; rest of the envelope emits zero commands.
    for (int i = 0; i < 3; ++i) {
        Output quiet = ch.tick();
        CHECK(!quiet.active() && quiet.leftIntensity == 0.0f);
        CHECK(ch.isActive());
    }
    // Monotonic in speed: a harder hit within the same window replaces.
    ch.submitCollision(0.0f, 1.0f, 0.50f);       // grade 500 -> clamps to 255
    Output harder = ch.tick();
    CHECK(harder.active() && harder.leftIntensity > o.leftIntensity);
    CHECK(nearly(harder.leftIntensity, 1.0f));
    // After the envelope expires the channel goes idle.
    for (int i = 0; i < 8; ++i) ch.tick();
    CHECK(!ch.isActive());
}

static void testLandingBurst() {
    // Pattern 1 (0x2EA3D8): small + scaled large, 8 frames.
    Channel ch = testChannel();
    const float points[] = {50.0f, 264.0f, 90.0f};
    ch.submitLanding(points, 3);
    Output o = ch.tick();
    CHECK(o.active());
    CHECK(o.durationFrames == 8);
    CHECK(nearly(o.leftIntensity, 16.0f / 255.0f));  // strongest point: 264 >> 4
    CHECK(nearly(o.rightIntensity, 1.0f));
    // Below the 179 gate: no event even with several points.
    const float soft[] = {0.0f, 88.0f, 178.0f};
    ch.submitLanding(soft, 3);
    CHECK(!ch.tick().active());
    CHECK(ch.lastBurst().active());  // no re-command: previous burst untouched
    // nullptr / empty: never an event.
    Channel empty = testChannel();
    empty.submitLanding(nullptr, 0);
    CHECK(!empty.tick().active());
    const float none[] = {1.0f};
    empty.submitLanding(none, 0);
    CHECK(!empty.tick().active());
}

static void testObjectCollisionBurst() {
    // 0x00253C9C: pattern 6 at the CONFIRMED fixed 0x40 strength.
    Channel ch = testChannel();
    ch.submitObjectCollision(0.0f, 1.0f);
    Output o = ch.tick();
    CHECK(o.active() && o.durationFrames == 8);
    CHECK(nearly(o.leftIntensity, 64.0f / 255.0f));
    CHECK(nearly(o.rightIntensity, 1.0f));
}

static void testSideAssignment() {
    // AUTHORED lateral balance over the CONFIRMED envelope levels.
    Channel ch = testChannel();
    ch.submitCollision(1.0f, 0.0f, 0.25f);        // pushed +x: hit came from the LEFT
    Output left = ch.tick();
    CHECK(left.active());
    CHECK(nearly(left.leftIntensity, 250.0f / 255.0f));      // struck side full
    CHECK(nearly(left.rightIntensity, kSideGainFloor));     // opposite dips

    ch.submitCollision(-1.0f, 0.0f, 0.25f);       // pushed -x: RIGHT-side hit
    Output right = ch.tick();
    CHECK(right.active());
    CHECK(nearly(right.rightIntensity, 1.0f));
    CHECK(nearly(right.leftIntensity, 250.0f / 255.0f * kSideGainFloor));
    CHECK(right.leftIntensity < right.rightIntensity);
    CHECK(left.rightIntensity < left.leftIntensity);

    // Unnormalized diagonal inputs tilt proportionally.
    Channel diag = testChannel();
    diag.submitCollision(2.0f, -2.0f, 0.25f);     // 45 degrees: tilt 0.707...
    Output mid = diag.tick();
    CHECK(mid.active());
    CHECK(nearly(mid.leftIntensity, 250.0f / 255.0f));   // struck side keeps full level
    CHECK(mid.rightIntensity > kSideGainFloor);           // not floor
    CHECK(mid.rightIntensity < 1.0f);                     // not full either
    CHECK(mid.rightIntensity < mid.leftIntensity);

    // Scrape sides.
    Channel scr = testChannel();
    scr.submitScrape(Side::Left, 0.25f);          // grade 62
    Output sl = scr.tick();
    CHECK(sl.active() && sl.durationFrames == 4);  // scaled short-burst envelope
    CHECK(nearly(sl.leftIntensity, 62.0f / 255.0f));
    CHECK(nearly(sl.rightIntensity, kSideGainFloor));
    scr.submitScrape(Side::Right, 0.25f);
    Output sr = scr.tick();
    CHECK(nearly(sr.leftIntensity, 62.0f / 255.0f * kSideGainFloor));
    CHECK(nearly(sr.rightIntensity, 1.0f));
}

static void testCrashBurst() {
    // CONFIRMED: the crash family ignores the event strength (fixed 0xFF)
    // and its envelopes are the short-record totals {3, 2, 2} frames.
    Channel ch = testChannel();
    ch.submitCrash(2);                            // CrashClick
    Output click = ch.tick();
    CHECK(click.active());
    CHECK(click.durationFrames == 3);
    CHECK(nearly(click.leftIntensity, 1.0f) && nearly(click.rightIntensity, 1.0f));
    CHECK(ch.activePattern() == Pattern::CrashClick);

    ch.submitCrash(1);                            // CrashPunch
    Output punch = ch.tick();
    CHECK(punch.active() && punch.durationFrames == 2);

    ch.submitCrash(3);                            // CrashBuzz
    Output buzz = ch.tick();
    CHECK(buzz.active() && buzz.durationFrames == 2);
    CHECK(ch.activePattern() == Pattern::CrashBuzz);

    // Code 0 / invalid codes never trigger (CONFIRMED).
    Channel none = testChannel();
    none.submitCrash(0);
    CHECK(!none.tick().active());
    none.submitCrash(15);
    CHECK(!none.tick().active());
}

static void testMenuTestBuzz() {
    // 0x00208EA0: pattern 7 — fixed 127/255 large motor for 30 frames; the
    // passed strength 1 is cosmetic (literal envelope, not 0x84-scaled).
    Channel ch = testChannel();
    ch.submitTestBuzz();
    Output o = ch.tick();
    CHECK(o.active());
    CHECK(o.durationFrames == 30);
    CHECK(nearly(o.leftIntensity, 127.0f / 255.0f));
    CHECK(nearly(o.rightIntensity, 0.0f));
}

// ---- AUTHORED arbitration: per-event priority ------------------------------

static void testPriority() {
    // A new landing overrides a scrape while both are active.
    Channel ch = testChannel();
    ch.submitScrape(Side::Right, 0.25f);
    Output scrape = ch.tick();
    CHECK(scrape.active() && scrape.durationFrames == 4);
    CHECK(ch.isActive());
    ch.submitScrape(Side::Right, 0.25f);          // scrape continues across frames
    CHECK(ch.tick().leftIntensity > 0.0f);        // still emitting per burst
    CHECK(ch.isActive());
    ch.submitLanding(nullptr, 0);
    ch.submitCrash(0);
    CHECK(!ch.tick().active());                    // no valid landing/crash yet
    CHECK(ch.isActive());
    const float fall[] = {300.0f};
    ch.submitLanding(fall, 1);                    // landing beats scrape
    Output land = ch.tick();
    CHECK(land.active() && land.durationFrames == 8);
    CHECK(ch.activePattern() == Pattern::Landing);

    // While a landing is active, scrapes are refused (lower priority).
    ch.submitScrape(Side::Left, 1.0f);
    CHECK(!ch.tick().active());
    CHECK(ch.activePattern() == Pattern::Landing);

    // Collision outranks landing; crash outranks collision.
    ch.submitCollision(0.5f, 0.5f, 0.20f);
    Output col = ch.tick();
    CHECK(col.active() && ch.activePattern() == Pattern::Collision);
    ch.submitScrape(Side::Left, 1.0f);
    CHECK(!ch.tick().active());
    ch.submitCrash(6);
    Output crash = ch.tick();
    CHECK(crash.active() && ch.activePattern() == Pattern::CrashBuzz);

    // Same priority replaces (the CONFIRMED primitives "last caller wins");
    // a stronger new landing replaces an active weaker landing.
    Channel land2 = testChannel();
    const float soft[] = {200.0f};
    const float hard[] = {600.0f};
    land2.submitLanding(soft, 1);
    Output first = land2.tick();
    CHECK(first.active() && nearly(first.leftIntensity, 12.0f / 255.0f));
    land2.submitLanding(hard, 1);
    Output second = land2.tick();
    CHECK(second.active() && nearly(second.leftIntensity, 37.0f / 255.0f));

    // Within one frame, order does not matter: submit scrape then landing ==
    // landing then scrape (same tick outcome).
    Channel a = testChannel(), b = testChannel();
    a.submitScrape(Side::Left, 0.25f);
    a.submitLanding(hard, 1);
    const float hardPts[] = {600.0f};
    b.submitLanding(hardPts, 1);
    b.submitScrape(Side::Left, 0.25f);
    Output oa = a.tick(), ob = b.tick();
    CHECK(oa.active() && ob.active());
    CHECK(nearly(oa.leftIntensity, ob.leftIntensity));
    CHECK(oa.durationFrames == 8 && ob.durationFrames == 8);
}

static void testStopAndSuppression() {
    // stop() is the pattern-0 / 0xFF-end semantics: immediate silence.
    Channel ch = testChannel();
    ch.submitCollision(0.0f, 1.0f, 0.25f);
    CHECK(ch.tick().active());
    ch.stop();
    CHECK(!ch.isActive());
    Output o = ch.tick();
    CHECK(!o.active() && o.leftIntensity == 0.0f && o.rightIntensity == 0.0f);

    // Suppression (0x0177AC50+0x28 bit 0x40): refuses new triggers,
    // lets the active effect play out (the original gate reads per trigger).
    Channel s = testChannel();
    s.submitCollision(0.0f, 1.0f, 0.25f);
    CHECK(s.tick().active());
    s.setSuppressed(true);
    s.submitCollision(0.0f, 1.0f, 1.0f);
    CHECK(!s.tick().active());
    CHECK(s.isActive());                          // original keeps playing
    s.setSuppressed(false);
    // Post-release triggers work: the active collision outranks a landing,
    // so demonstrate with a crash (priority 3 > collision 2) — the release
    // itself is what unblocks the trigger, not a priority accident.
    s.submitLanding(nullptr, 0);  // still gated to idleness: not a valid event
    CHECK(!s.tick().active());
    CHECK(s.isActive());
    s.submitCrash(6);
    CHECK(s.tick().active());
    CHECK(s.activePattern() == Pattern::CrashBuzz);
}

static void testClampAndDeadzoneEdges() {
    Channel ch = testChannel();
    // Deadzones: collisions at grade 0 and tiny scrapes never fire.
    ch.submitCollision(0.0f, 1.0f, 0.0001f);
    CHECK(!ch.tick().active());
    ch.submitScrape(Side::Left, 0.001f);
    CHECK(!ch.tick().active());
    ch.submitLanding(nullptr, 0);
    CHECK(!ch.tick().active());
    // Clamp: enormous energies saturate at 255 and intensities stay in 0..1.
    ch.submitCollision(7.0f, 0.0f, 10000.0f);
    Output o = ch.tick();
    CHECK(o.active());
    CHECK(nearly(o.leftIntensity, 1.0f));
    CHECK(o.rightIntensity >= 0.0f && o.rightIntensity <= 1.0f);
    // Clamped landing on an idle channel (landings do not outrank collisions).
    Channel lp = testChannel();
    const float fall[] = {999999.0f};
    lp.submitLanding(fall, 1);
    Output l = lp.tick();
    CHECK(l.active() && nearly(l.leftIntensity, 1.0f));
    // Clamped scrape on an idle channel.
    Channel sp = testChannel();
    sp.submitScrape(Side::Right, 1000.0f);
    Output sc = sp.tick();
    CHECK(sc.active() && sc.leftIntensity <= 1.0f);
    CHECK(nearly(sc.leftIntensity, 63.0f / 255.0f * kSideGainFloor));
    // Envelope totals hold at the clamp.
    Channel cp = testChannel();
    cp.submitCrash(4);
    Output c = cp.tick();
    CHECK(c.active() && c.durationFrames == 2 && nearly(c.leftIntensity, 1.0f));
}

static void testEnvelopeDurations() {
    // CONFIRMED decoded totals per pattern record @0x2EA3D0..0x2EA410.
    CHECK(kEnvelopeLanding.frames == 8);
    CHECK(kEnvelopeImpactShort.frames == 4);
    CHECK(kEnvelopeCrashClick.frames == 3);
    CHECK(kEnvelopeCrashPunch.frames == 2);
    CHECK(kEnvelopeCrashBuzz.frames == 2);
    CHECK(kEnvelopeCollision.frames == 8);
    CHECK(kEnvelopeMenuTest.frames == 30);
    CHECK(!kEnvelopeStop.frames);
    CHECK(kEnvelopeMenuTest.smallMotor == false);
    CHECK(nearly(kEnvelopeMenuTest.fixedLarge, 127.0f / 255.0f));
    CHECK(kEnvelopeCrashClick.scaledLarge == false);
    CHECK(kEnvelopeCollision.scaledLarge == true);
    // durations/seconds helpers for the SDL glue.
    Channel ch = testChannel();
    ch.submitTestBuzz();
    Output o = ch.tick();
    CHECK(nearly(o.durationSeconds(), 0.5f));  // 30 frames @60Hz
}

int main() {
    testGate();
    testCollisionGrade();
    testLandingGrade();
    testScrapeGrade();
    testCrashPatternForCode();
    testCollisionBurst();
    testLandingBurst();
    testObjectCollisionBurst();
    testSideAssignment();
    testCrashBurst();
    testMenuTestBuzz();
    testPriority();
    testStopAndSuppression();
    testClampAndDeadzoneEdges();
    testEnvelopeDurations();
    if (g_fail) { std::fprintf(stderr, "%d failure(s)\n", g_fail); return 1; }
    std::puts("rumble: all checks passed");
    return 0;
}
