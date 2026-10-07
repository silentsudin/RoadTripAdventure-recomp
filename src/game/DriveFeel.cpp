#include "game/DriveFeel.h"

#include <algorithm>
#include <cmath>

namespace rt::game::feel
{
    namespace
    {
        float clamp01(float v) { return std::clamp(v, 0.0f, 1.0f); }
        constexpr float kTwoPi = 6.2831853f;
    }

    bool Pwm::step(float duty)
    {
        duty = clamp01(duty);
        if (period <= 1)
        {
            acc += duty;
            if (acc >= 0.5f)
            {
                acc -= 1.0f;
                return true;
            }
            return false;
        }
        // A cycle's on-time is fixed at its start, the rounding error carried to the next.
        if (phase == 0)
        {
            const float want = duty * period + acc;
            on = static_cast<int>(want + 0.5f);
            on = on < 0 ? 0 : on > period ? period : on;
            acc = want - on;
        }
        const bool b = phase < on;
        phase = (phase + 1) % period;
        return b;
    }

    float gasDuty(float value) { return 0.5f + 0.5f * clamp01(value); }

    bool Throttle::step(float value, uint32_t speed)
    {
        const float target = clamp01(value) * static_cast<float>(top);
        if (static_cast<float>(speed) >= target)
        {
            pwm.acc = 0;
            return false;
        }
        return pwm.step(gasDuty(value));
    }

    int brakeRampLimit(float value)
    {
        return std::clamp(static_cast<int>(std::ceil(clamp01(value) * 32.0f)), 1, 32);
    }

    float Synth::noise()
    {
        m_seed = m_seed * 1664525u + 1013904223u;
        return static_cast<float>(m_seed >> 8) / static_cast<float>(1u << 24);
    }

    void Synth::event(int pattern, int strength)
    {
        const float s = strength > 0 ? clamp01(strength / 255.0f) : 1.0f;
        switch (pattern)
        {
        case 1: // a bump: the game's strengths are small (11..41)
            m_bump = std::max(m_bump, clamp01(strength / 48.0f));
            break;
        case 3:
        case 4:
        case 5: // rough ground under a wheel
            m_rough = 10;
            m_roughKind = pattern;
            break;
        case 6: // a collision
            m_knock = std::max(m_knock, 0.3f + 0.7f * s);
            break;
        default:
            m_knock = std::max(m_knock, 0.35f + 0.65f * s);
            break;
        }
    }

    Motors Synth::step(const CarSample &car)
    {
        ++m_frame;
        Motors m;
        const float rpm = clamp01(car.rpm / 9000.0f);
        const float speed = clamp01(car.speed / 20000.0f);
        const bool moving = car.speed > 200;

        // Engine: a light hum on the fast motor that rises with the revs, beating a little
        // faster as they rise; stronger on the gas.
        m_phase += kTwoPi * (2.0f + 7.0f * rpm) / 60.0f;
        if (m_phase > kTwoPi)
            m_phase -= kTwoPi;
        if (car.rpm > 0)
            m.high = (car.gas ? 0.06f + 0.16f * rpm : 0.03f + 0.07f * rpm) * (0.8f + 0.2f * std::sin(m_phase));

        // Road: a low rumble that grows with speed; nothing in the air.
        if (moving && !car.airborne)
            m.low = speed * (0.04f + 0.05f * noise());
        // Rough ground (the game says so every few frames): each kind its own grain.
        if (m_rough > 0)
        {
            --m_rough;
            const float sp = 0.3f + 0.7f * speed;
            if (m_roughKind == 3) // deep thuds
                m.low = std::max(m.low, sp * (0.20f + 0.30f * noise()));
            else if (m_roughKind == 4) // fine grain
            {
                m.low = std::max(m.low, sp * (0.08f + 0.10f * noise()));
                m.high += sp * (0.12f + 0.18f * noise());
            }
            else // both
            {
                m.low = std::max(m.low, sp * (0.14f + 0.20f * noise()));
                m.high += sp * (0.08f + 0.12f * noise());
            }
            m.left = std::max(m.left, 0.08f * sp);
            m.right = std::max(m.right, 0.08f * sp);
        }
        // Skidding tyres: a buzz on the fast motor.
        if (car.skid > 0 && moving)
            m.high += 0.18f * clamp01(car.skid);
        // Suspension bumps.
        m.low = std::max(m.low, m_bump * 0.6f);
        m.high = std::max(m.high, m_bump * 0.25f);
        m_bump *= 0.6f;

        // Gear changes: a thump, firmer going up under power.
        if (m_gear != 0 && car.gear != m_gear && moving)
            m_kick = car.gear > m_gear ? (car.gas ? 0.45f : 0.30f) : 0.25f;
        m_gear = car.gear;
        m.low = std::max(m.low, m_kick);
        m_kick *= 0.55f;

        // Landing after a jump (at least a quarter of a second in the air): a thump on both.
        if (car.airborne)
            ++m_airFrames;
        else
        {
            if (m_wasAirborne && m_airFrames >= 15)
                m_landing = std::min(1.0f, 0.4f + m_airFrames / 90.0f);
            m_airFrames = 0;
        }
        m_wasAirborne = car.airborne;
        m.low = std::max(m.low, m_landing);
        m.high = std::max(m.high, m_landing * 0.6f);
        m_landing *= 0.75f;

        // Brakes: the left trigger pushes back as the brakes bite (the game's ramp), pulsing like
        // ABS when braking hard at speed; a little body shake as well.
        if (car.brake && car.brakeRamp > 0 && moving)
        {
            const float bite = car.brakeRamp / 32.0f;
            float l = 0.15f + 0.45f * bite * (0.4f + 0.6f * speed);
            if (bite >= 1.0f && speed > 0.35f && (m_frame / 3) % 2)
                l *= 0.35f;
            m.left = l;
            m.low = std::max(m.low, 0.12f * bite * speed);
        }
        // Gas: the right trigger hums with the engine under load, buzzing at the limiter.
        if (car.gas && car.rpm > 0)
        {
            float r = 0.06f + 0.22f * rpm;
            if (car.rpm > 8700 && (m_frame / 2) % 2)
                r += 0.15f;
            m.right = r;
        }

        // The game's knocks (collisions, landings it signals): both motors, fading in about 0.2 s.
        m.low = std::max(m.low, m_knock);
        m.high = std::max(m.high, m_knock * 0.7f);
        m.left = std::max(m.left, m_knock * 0.4f);
        m.right = std::max(m.right, m_knock * 0.4f);
        m_knock *= 0.8f;
        if (m_knock < 0.02f)
            m_knock = 0;

        m.low = clamp01(m.low);
        m.high = clamp01(m.high);
        m.left = clamp01(m.left);
        m.right = clamp01(m.right);
        return m;
    }
}
