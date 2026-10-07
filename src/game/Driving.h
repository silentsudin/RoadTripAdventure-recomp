#pragma once

// Driving controls and feel (config/game_state.toml [controls]):
//  - the game's own button setup: its 9 driving actions, each a DS2 button mask in the progress
//    block (0x177F760 + port * 0x3448 + 0x12E8..), saved with the game. The app's Driving
//    controls page edits them here.
//  - analogue triggers: the player controller 0x21A820 (task, sys, car) is wrapped, and its
//    control word's Gas bit is pulsed by how far the Gas trigger is pressed, its Brake ramp held
//    short by how far the Brake trigger is (DriveFeel.h).
//  - dynamic vibration: the car's state each frame and the game's rumble calls (0x20ACB8) become
//    motor and trigger speeds for the input layer.
// Game-thread hooks; the rest is any thread.

#include <cstdint>
#include <string>

class PS2Runtime;

namespace rt::game
{
    enum class DriveAction { Gas, Brake, Reverse, Jet, WingUp, WingDown, HornLights, View, Navigator, Count };
    constexpr int kDriveActions = static_cast<int>(DriveAction::Count);
    const char *driveActionName(DriveAction a); // "Gas", "Brake", ...
    const char *driveActionKey(DriveAction a);  // "gas", "brake", ... (test socket)

    // The DS2 button (rt::input::Ps2Button index) an action is on for a port, or -1 when the
    // game has no button setup in memory yet (before the title).
    int actionButton(PS2Runtime &runtime, int port, DriveAction a);
    // Whether the game's button setup offers this DS2 button (its table at 0x29C538: the face
    // buttons, the shoulders, D-pad up and down, Select).
    bool actionButtonAllowed(PS2Runtime &runtime, int ps2Button);
    // Puts an action on a button; an action already there takes this one's old button. False if
    // not allowed or there is no setup yet.
    bool setActionButton(PS2Runtime &runtime, int port, DriveAction a, int ps2Button);
    // Control schemes (settings [controls] scheme): the layout each puts the 9 actions on, as DS2
    // buttons (rt::input::Ps2Button) in DriveAction order.
    //  Modern: R2 gas, L2 brake (Modern and Custom: hold it at a standstill to reverse), Square reverse, Cross jet,
    //    R1 / L1 wing up / down, Circle horn & lights, Triangle view, Select navigator. A player
    //    with only a keyboard keeps Classic (its keys follow the game's layout).
    //  Classic: the game's own (Cross gas, Square brake, Circle reverse, R2 jet, L1 / L2 wing
    //    up / down, R1 horn, Triangle view, Select navigator).
    //  Custom: the player's (settings customControls), edited on the Driving controls page.
    // The scheme's layout is written into the game's setup (both ports) while driving, so it is
    // what the game uses and saves; never under virtual time or with a movie or script (they
    // replay the game's own layout) unless RT_CONTROL_SCHEME=modern|classic|custom says so.
    bool controlSchemeActive();
    // The layout of a scheme (0 Modern, 1 Classic, 2 Custom; Custom falls back to Classic).
    void schemeLayout(int scheme, int (&buttons)[kDriveActions]);
    // Writes the current scheme's layout for both ports now (after a change in the menu).
    bool applyControlScheme(PS2Runtime &runtime);
    // Custom: the game's current layout (port 0) as settings text ("r2 l2 square ...").
    std::string layoutText(PS2Runtime &runtime);
    // How a port is played (the input layer): 0 keyboard only (Modern falls back to Classic), or
    // the label family of its first controller: 1 PlayStation, 2 Xbox (and unnamed pads), 3 Nintendo.
    // The game's one line of dialogue naming driving buttons (the wing set's L1 / L2, 0x326EAD)
    // is rewritten to name player 1's buttons.
    void setPortFamily(int port, int family);

    // Analogue gas and brake for a port, from the input layer (render thread, every frame): how far
    // the controls on the game's Gas / Brake buttons are pressed (0..1; axes only), and whether a
    // digital control presses them as well.
    struct AnalogInput
    {
        float gas = 0, brake = 0;
        bool gasDigital = false, brakeDigital = false;
    };
    void setAnalogInput(int port, const AnalogInput &in);
    // Whether analogue gas and brake are used: the setting, and never under virtual time or with a
    // movie or script driving the pads (they replay buttons only). RT_ANALOG_TRIGGERS=0|1 overrides.
    bool analogTriggersActive();
    // The DS2 buttons the game reads as Gas and Brake on a port (-1 = unknown), for the input layer.
    int gasButton(int port);
    int brakeButton(int port);

    // Dynamic vibration for a port (motor and trigger speeds 0..1) while driving; false when the
    // car has not been driven for a few frames (menus: the game's own vibration plays).
    struct RumbleOut
    {
        float low = 0, high = 0, left = 0, right = 0;
    };
    bool dynamicRumble(int port, RumbleOut &out);

    void installDrivingHooks(PS2Runtime &runtime);

    // Test socket {"cmd":"driving"[,"port":N][,"action":"gas","button":"r2"][,"scheme":"modern"|"classic"|"custom"]
    //   [,"analog":1,"gas":0.5,"brake":0]}: sets, then reports the actions' buttons, the analogue
    // state and the dynamic vibration per port.
    std::string drivingCommand(PS2Runtime &runtime, const std::string &line);
}
