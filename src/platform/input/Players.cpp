#include "Players.h"

#include <algorithm>

namespace rt::input
{
    Players::Slots Players::assign(const std::vector<std::string> &connected)
    {
        Slots slots;
        std::vector<std::string> undecided;
        for (const std::string &id : connected)
        {
            int player = -2; // undecided
            if (auto m = m_manual.find(id); m != m_manual.end())
                player = m->second;
            else if (auto r = m_remembered.find(id); r != m_remembered.end())
                player = r->second;
            else if (id == kKeyboardId)
                player = 0;
            if (player >= 0 && player < kPlayers)
                slots[player].push_back(id);
            else if (player == -2)
                undecided.push_back(id);
        }
        auto hasPad = [&](int player) {
            return std::any_of(slots[player].begin(), slots[player].end(),
                               [](const std::string &id) { return id != kKeyboardId; });
        };
        for (const std::string &id : undecided)
        {
            const int player = !hasPad(0) ? 0 : slots[1].empty() ? 1 : -1;
            if (player < 0)
                continue; // waits for a free slot (or the Controllers window)
            m_remembered[id] = player;
            slots[player].push_back(id);
        }
        return slots;
    }
}
