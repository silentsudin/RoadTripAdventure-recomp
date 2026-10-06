#pragma once

// The second screen (Android: the AYN Thor's lower display): the game's map, enlarged, and touch
// buttons for the in-game menu and quick settings, in Road Trip's style. Drawn with a second ImGui
// context (sharing the fonts) into the presenter's second swapchain.

class PS2Runtime;

namespace rt::ui
{
    // Every host frame, inside the presenter's UI callback (outside the main ImGui frame).
    void updateSecondScreen(PS2Runtime &runtime);
}
