#pragma once

// How driving feels on a modern pad: analogue gas and brake on a game that reads only buttons,
// and vibration made from the car's state. Pure: no runtime, no SDL (unit-tested in
// tests/drive_feel_test.cpp). The hooks that feed it are in src/game/Driving.cpp.

#include <cstdint>

namespace rt::game::feel
{
    // Below this a trigger is at rest (the game's own button, or nothing, decides).
    constexpr float kTriggerRest = 0.04f;
    // At and above this it is fully pressed.
    constexpr float kTriggerFull = 0.97f;

    // Gas: the game accelerates while its Gas bit is set, so a partly pressed trigger sets the bit
    // on that fraction of frames, spread as evenly as possible (error diffusion).
    struct Pwm
    {
        float acc = 0;
        int period = 1;  // frames per cycle: on for duty * period of them at its start
        int phase = 0;
        int on = 0;      // frames on in this cycle
        bool step(float duty);
    };

    // The Gas bit's duty for a trigger value. The game's engine hardly moves the car below half
    // duty (its revs fall away between pulses) and responds from there to full (measured at
    // Peach Raceway, 1.5 s from a standstill: 0.5 -> ~500, 0.65 -> ~1300, 1.0 -> ~4300), so a
    // trigger just past rest creeps and the rest of its travel spans the engine's range.
    float gasDuty(float value);

    // Analogue gas, as a speed the trigger asks for and how hard to get there: below
    // value x the car's top speed the Gas bit is pulsed at gasDuty(value), at or above it the bit
    // stays off. The game's response to the pulses alone depends on the car (some cars accelerate
    // as fast at 65% duty as at 100%: grip, not the engine, limits them), so the speed cap is what
    // makes a partly pressed trigger mean less speed on every car. The top speed is learned from
    // the fastest the car goes on full gas (20000, the starter car's, until then).
    struct Throttle
    {
        Pwm pwm;
        uint32_t top = 20000;
        bool step(float value, uint32_t speed);
        // Full gas this frame (button or trigger all the way): learns the top speed.
        void full(uint32_t speed) { top = speed > top ? speed : top; }
    };

    // Brake: the game ramps braking force up over 32 frames while the bit is held (car + 0x1FE,
    // force = curve[ramp - 1]); a partly pressed trigger stops the ramp at this many frames (1..32).
    int brakeRampLimit(float value);

    // The car, once per game frame (car object 0x177AC50 + k * 0x270).
    struct CarSample
    {
        int rpm = 0;          // +0x1D0 u16, 0 .. about 9000
        uint32_t speed = 0;   // +0x1D8 u32, |speed| (about 20000 flat out in 5th on the starter car)
        int gear = 1;         // +0x1FF, 1..5
        int brakeRamp = 0;    // +0x1FE, 0..32
        bool gas = false, brake = false, reverse = false; // the control word the game got
        bool airborne = false; // no contact point on the ground (+0x19C.. seven words, all -1)
        float skid = 0;       // tyres skidding, 0..1 (+0x1C4 u8, 0..180; inferred)
    };

    // Motor speeds, 0..1: low = the large, slow motor; high = the small, fast one; left/right =
    // the trigger motors (Xbox One and later pads, DualSense through SDL).
    struct Motors
    {
        float low = 0, high = 0, left = 0, right = 0;
    };

    class Synth
    {
    public:
        // The game asked for vibration (0x20ACB8 pattern, strength). Seen while driving:
        //  1 from 0x21B360: the suspension hitting a bump, strength 11..41 (graded);
        //  3, 4, 5 from 0x219C6C / 0x219C8C: a wheel on rough ground, by kind of ground (the
        //    contact's surface type: 4 for 1/4/5, 3 for 2/8/10, 5 for 3/6/7/9), every few frames;
        //  6 from 0x21793C (in 0x217830): a collision, strength = how hard (125 into a car);
        // others: a knock of that strength.
        void event(int pattern, int strength);
        // One game frame of driving.
        Motors step(const CarSample &car);
        void reset() { *this = Synth{}; }

    private:
        float m_knock = 0;      // decaying impact
        float m_bump = 0;       // decaying suspension bump
        int m_rough = 0;        // frames of rough ground left (renewed by the game's calls)
        int m_roughKind = 0;    // 3, 4 or 5
        float m_kick = 0;       // decaying gear-change thump
        float m_landing = 0;    // decaying landing thump
        float m_phase = 0;      // engine beat
        uint32_t m_seed = 0x2545F491u;
        int m_gear = 0;
        bool m_wasAirborne = false;
        int m_airFrames = 0;
        uint32_t m_frame = 0;
        float noise();
    };
}
