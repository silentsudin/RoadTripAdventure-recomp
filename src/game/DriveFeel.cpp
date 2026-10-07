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

    Surface surfaceOf(uint32_t contact)
    {
        if (contact == 0xFFFFFFFFu)
            return Surface::Other;
        switch ((contact >> 8) & 7)
        {
        case 0: return Surface::Asphalt;
        case 1: return Surface::Dirt;
        case 3: return Surface::Grass;
        case 4: return Surface::Snow;
        case 5: return (contact & 0xF) == 4 ? Surface::Ice : Surface::Boards;
        case 6: return Surface::Water;
        default: return Surface::Other;
        }
    }

    const char *surfaceName(Surface s)
    {
        static const char *const names[] = {"asphalt", "dirt", "grass", "snow", "ice", "boards", "water", "other"};
        return names[static_cast<int>(s)];
    }

    float landingStrength(int fall)
    {
        return std::clamp(0.3f + 0.7f * (static_cast<float>(fall) - 600.0f) / 4000.0f, 0.3f, 1.0f);
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
        case 1: // a ground point landing: 11..50 bumps, more after a jump (the landing thump is step's)
            m_bump = std::max(m_bump, clamp01(strength / 48.0f));
            break;
        case 3: // scraping a wall: right side, left side, front (renewed every few frames)
        case 4:
        case 5:
            m_scrape[pattern == 4 ? 0 : pattern == 3 ? 1 : 2] = 8;
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

        // The ground, each kind its own grain, growing with speed; nothing in the air.
        const bool air = car.airborne();
        m_smooth += (noise() - m_smooth) * 0.15f;
        if (moving && !air)
        {
            const float n = noise();
            switch (surfaceOf(car.ground))
            {
            case Surface::Asphalt:
            case Surface::Other: // smooth: a faint road hum
                m.low = speed * (0.04f + 0.05f * n);
                break;
            case Surface::Dirt: // loose and gritty: both motors, coarse
                m.low = (0.2f + 0.8f * speed) * (0.10f + 0.14f * n);
                m.high += (0.2f + 0.8f * speed) * (0.06f + 0.10f * noise());
                break;
            case Surface::Grass: // soft and uneven: a slow, padded rumble
                m.low = (0.2f + 0.8f * speed) * (0.08f + 0.10f * m_smooth + 0.04f * n);
                break;
            case Surface::Snow: // muffled crunch
                m.low = (0.2f + 0.8f * speed) * (0.05f + 0.05f * n);
                m.high += (0.2f + 0.8f * speed) * 0.03f * noise();
                break;
            case Surface::Ice: // next to nothing under the wheels: a thin hiss
                m.high += speed * (0.03f + 0.03f * n);
                break;
            case Surface::Boards: // joints: a tick every so far travelled
            {
                m_plank += static_cast<float>(car.speed) / 20000.0f;
                m.low = speed * 0.04f;
                if (m_plank >= 0.35f)
                {
                    m_plank = 0;
                    m.low += 0.10f + 0.12f * speed;
                }
                break;
            }
            case Surface::Water: // drag and slosh: heavy and slow on the large motor
                m.low = (0.3f + 0.7f * speed) * (0.16f + 0.16f * m_smooth);
                m.high += 0.04f * speed * n;
                break;
            }
        }
        // Scraping a wall: a rasp on the fast motor, and on the trigger on that side (both at the front).
        for (int side = 0; side < 3; ++side)
        {
            if (m_scrape[side] <= 0)
                continue;
            --m_scrape[side];
            const float sp = 0.4f + 0.6f * speed;
            const float rasp = sp * (0.18f + 0.22f * noise());
            m.high = std::max(m.high, rasp);
            m.low = std::max(m.low, sp * 0.12f);
            const float pull = sp * (0.30f + 0.30f * noise()); // the side's trigger, stronger than the engine's hum
            if (side != 1)
                m.left = std::max(m.left, side == 0 ? pull : rasp);
            if (side != 0)
                m.right = std::max(m.right, side == 1 ? pull : rasp);
        }
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

        // Landing after time in the air (all three ground points falling for 5+ frames): a thump
        // on both motors and both triggers, by how fast the car came down, longer for harder ones.
        if (air)
        {
            ++m_airFrames;
            m_peakFall = std::max({m_peakFall, car.fall[0], car.fall[1], car.fall[2]});
        }
        else
        {
            if (m_airFrames >= 5)
            {
                m_landing = std::max(m_landing, landingStrength(m_peakFall));
                m_landingDecay = 0.70f + 0.18f * m_landing;
            }
            m_airFrames = 0;
            m_peakFall = 0;
        }
        m.low = std::max(m.low, m_landing);
        m.high = std::max(m.high, m_landing * 0.6f);
        m.left = std::max(m.left, m_landing * 0.5f);
        m.right = std::max(m.right, m_landing * 0.5f);
        m_landing *= m_landingDecay;
        if (m_landing < 0.02f)
            m_landing = 0;

        // Brakes: the left trigger pushes back as the brakes bite (the game's ramp), pulsing like
        // ABS when braking hard at speed; a little body shake as well.
        if (car.brake && car.brakeRamp > 0 && moving)
        {
            const float bite = car.brakeRamp / 32.0f;
            float l = 0.15f + 0.45f * bite * (0.4f + 0.6f * speed);
            if (bite >= 1.0f && speed > 0.35f && (m_frame / 3) % 2)
                l *= 0.35f;
            m.left = std::max(m.left, l);
            m.low = std::max(m.low, 0.12f * bite * speed);
        }
        // Gas: the right trigger hums with the engine under load, buzzing at the limiter.
        if (car.gas && car.rpm > 0)
        {
            float r = 0.06f + 0.22f * rpm;
            if (car.rpm > 8700 && (m_frame / 2) % 2)
                r += 0.15f;
            m.right = std::max(m.right, r);
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
