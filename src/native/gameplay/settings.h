#pragma once
// Persisted user settings + options-menu logic for the native Road Trip app.
//
// Pure C++: no SDL, no Vulkan, no guest/disc data, no clocks. The host injects
// the data directory (no hardcoded paths); tests drive the store, the catalog
// and the navigator deterministically.
//
// REFERENCE PRECEDENT (CLAUDE.md:87): the sibling recomp app owns a
// controller-navigable pause menu (Display/Graphics/Controllers/Sound pages)
// with settings.toml persistence; rows a device cannot use, or that aren't
// built yet, are hidden. This module is AUTHORED-for-native: it reproduces that
// *shape* — a flat persisted file, option rows mapped to behaviour flags, rows
// hidden by availability — against the flags the native app actually has today
// (the RT_APP_* env vars enumerated in docs/native-reference/options-menu.md),
// not the sibling's code or its TOML tables.
//
// Division of labour:
//   SettingsStore  - a flat key=value text file at a caller-supplied path with
//                    typed getters (default + validation + clamping), unknown
//                    keys preserved round-trip, and corrupt-line tolerance.
//   OptionsModel   - the catalog: every user option mapped to the env flag(s)
//                    it stands in for, with its page, value domain, default,
//                    availability predicate and honest wired/preview status.
//   MenuNavigator  - pages + focus state machine: up/down move focus (skipping
//                    unavailable rows, wrapping per page), left/right adjust a
//                    value domain, enter/exit/back pages, deterministic initial
//                    state, and current-focus metadata for the renderer.

#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace rt::native::gameplay {

// ---------------------------------------------------------------------------
// SettingsStore
//
// File format (flat, line based):
//   * blank lines and lines whose first non-space char is '#' are kept verbatim
//   * every other line is "key = value": split at the FIRST '=', both sides
//     trimmed; the key is a non-empty identifier; the value is the raw text
//   * a line with no '=' (or an empty key) is corrupt: skipped for reads but
//     preserved verbatim so a re-save never destroys a user's file
//   * duplicate keys: the first occurrence wins for reads; setting a key
//     removes later duplicates
//
// ONE WRITER AT A TIME: a SettingsStore owns one file and is not thread-safe.
// One owner (the app's main/menu thread) performs every mutation and save; any
// concurrent access must be serialised by the caller. save() is atomic
// (temp file + rename) so a reader never sees a half-written file.
//
// Unknown keys (anything the getters don't know about — a newer build's option,
// a hand edit, a comment) survive load -> save byte-for-byte in order.
class SettingsStore {
public:
    struct Entry {
        std::string key;
        std::string value;
    };

    // `path` is injected by the caller (never a hardcoded location). An empty
    // path makes load()/save() no-ops that return false.
    explicit SettingsStore(std::filesystem::path path = {});

    [[nodiscard]] const std::filesystem::path& path() const { return path_; }
    void setPath(std::filesystem::path path) { path_ = std::move(path); }

    // Reads the file at path(). Returns true when the file was opened (even if
    // some lines were corrupt); false when it is absent or unreadable. Resets
    // the in-memory contents first, so calling load() twice is deterministic.
    bool load();

    // Atomically writes the current contents (comments/blank/corrupt lines
    // included) to path(). Creates the parent directory. Returns true on
    // success. No-op returning false when path() is empty.
    bool save() const;

    // Drops every in-memory line (skipped-line count resets too).
    void clear();

    // Lines skipped because they were not "key = value" (diagnostic).
    [[nodiscard]] int skippedLines() const { return skippedLines_; }
    [[nodiscard]] std::size_t entryCount() const;
    [[nodiscard]] bool contains(std::string_view key) const;
    [[nodiscard]] std::vector<Entry> entries() const;

    // Raw string value or `fallback` when the key is absent.
    [[nodiscard]] std::string getString(std::string_view key, std::string fallback) const;

    // true/false/1/0/yes/no/on/off (case-insensitive); anything else ->
    // `fallback` (a present-but-unparseable key never silently becomes "on").
    [[nodiscard]] bool getBool(std::string_view key, bool fallback) const;

    // Decimal integer clamped to [lo, hi] (swapped if lo > hi). A present but
    // unparseable value falls back to the clamped `fallback`.
    [[nodiscard]] int getInt(std::string_view key, int fallback, int lo, int hi) const;

    // Decimal float clamped to [lo, hi].
    [[nodiscard]] float getFloat(std::string_view key, float fallback, float lo, float hi) const;

    // Enum-string: returns the matching entry of `allowed` (case-insensitive on
    // input, canonical spelling on output). An invalid/absent value returns
    // `fallback` — the current setting is kept, never coerced by accident.
    [[nodiscard]] std::string getEnum(std::string_view key, std::string fallback,
                                      const std::vector<std::string>& allowed) const;

    // Typed setters funnel to setRaw(); the canonical text is what a later
    // getter reads back.
    void setBool(std::string_view key, bool value);
    void setInt(std::string_view key, int value);
    void setFloat(std::string_view key, float value);
    void setString(std::string_view key, std::string_view value);
    void setRaw(std::string_view key, std::string_view value);

    // Removes every entry with `key`. Returns true when one was removed.
    bool erase(std::string_view key);

    [[nodiscard]] static bool parseBool(std::string_view text, bool& out);
    [[nodiscard]] static std::string formatFloat(float value);

private:
    enum class LineKind { Blank, Comment, Entry, Invalid };
    struct Line {
        LineKind kind = LineKind::Blank;
        std::string raw;    // Blank/Comment/Invalid: the line as read
        std::string key;    // Entry only
        std::string value;  // Entry only
    };

    [[nodiscard]] int findFirst(std::string_view key) const;

    std::filesystem::path path_;
    std::vector<Line> lines_;
    int skippedLines_ = 0;
};

// The persistence path contract: a flat "settings.toml" in the app's data
// directory (the app injects its data dir; tests inject a temp dir).
[[nodiscard]] std::filesystem::path defaultSettingsPath(const std::filesystem::path& dataDirectory);

// ---------------------------------------------------------------------------
// OptionsModel

enum class OptionPage : int { Display = 0, Graphics, Controllers, Sound, Save, Count };
enum class OptionType : int { Toggle, Enum, Integer, Float, String, Action };

// WiredNow     = the row's value already has a consumer in the native app today
//                (an RT_APP_* env flag it stands in for).
// GpuWavePending = the row is present but honest about its state: the value is
//                persisted and shown, but nothing consumes it yet (the sibling
//                "preview" row). The note string carries the reason.
enum class OptionStatus : int { WiredNow, GpuWavePending };

// When a row may be shown. Predicates are evaluated against a MenuContext so a
// device fact (no gamepad), a capability (no second screen) or a session state
// (not in an Adventure) hides the row rather than showing a dead control.
enum class OptionAvailability : int {
    Always,
    RequiresGamepad,        // triggers/rumble need a pad in hand
    RequiresSecondScreen,   // no presentation display -> hidden
    RequiresTerrain,        // disc terrain off -> textures/stream/file rows hidden
    RequiresStream,         // streaming on -> step/row/position rows shown
    RequiresNav,            // nav overlay on -> town-graph row shown
    RequiresRoute,          // route follower on -> town-route row shown
    RequiresInterp,         // frame interp on -> cap/rate rows shown
    RequiresAudio,          // audio on -> BGM rows shown
    RequiresAdventure,      // radio: Adventure only (sibling rule)
    RequiresSaveState,      // save/load: Adventure or the player's race, no movie
};

struct OptionSpec {
    std::string id;                     // stable settings-file key (also the row id)
    std::string label;                  // UI label
    OptionPage page = OptionPage::Display;
    OptionType type = OptionType::Toggle;
    OptionStatus status = OptionStatus::WiredNow;
    OptionAvailability availability = OptionAvailability::Always;
    std::vector<std::string> domain;    // Enum: allowed tokens (domain[0] = default)
    std::string defaultRaw;             // default value as stored text
    double min = 0.0;                   // Integer/Float domain lower bound
    double max = 0.0;                   // Integer/Float domain upper bound
    double step = 1.0;                  // Integer/Float adjust step
    std::vector<std::string> envFlags;  // RT_APP_* (or RT_DISC) rows this stands in for
    std::string note;                   // honest status note (e.g. preview reason)
};

// The facts an availability predicate needs beyond the settings themselves
// (injected: no globals, no SDL). Session-state fields mirror the sibling rule
// for save states (CLAUDE.md:90).
struct MenuContext {
    bool gamepadPresent = false;        // a controller with triggers is attached
    bool secondScreenSupported = false; // a presentation display exists
    bool adventure = false;             // SessionState::EXPLORE (an Adventure)
    bool race = false;                  // SessionState::RACE (the player's race)
    bool playerRace = true;             // the race belongs to the player
    bool moviePlaying = false;          // attract demo / movie: no save states
    bool gsCanSaveStates = true;        // the renderer can snapshot (sibling Capabilities)
    // Derived from the store at evaluation time (see MenuNavigator::effectiveContext):
    bool terrainEnabled = false;
    bool streamEnabled = false;
    bool navEnabled = false;
    bool routeEnabled = false;
    bool interpEnabled = false;
    bool audioAvailable = true;
};

[[nodiscard]] bool optionAvailable(const OptionSpec& spec, const MenuContext& context);

[[nodiscard]] const char* optionPageName(OptionPage page);
[[nodiscard]] const char* optionTypeName(OptionType type);
[[nodiscard]] const char* optionStatusName(OptionStatus status);

class OptionsModel {
public:
    OptionsModel();

    [[nodiscard]] const std::vector<OptionSpec>& options() const { return options_; }
    [[nodiscard]] const OptionSpec* find(std::string_view id) const;

    // Rows of `page` whose availability predicate holds, in catalog order.
    [[nodiscard]] std::vector<const OptionSpec*> pageRows(OptionPage page,
                                                          const MenuContext& context) const;
    [[nodiscard]] bool pageAvailable(OptionPage page, const MenuContext& context) const;
    [[nodiscard]] std::vector<OptionPage> availablePages(const MenuContext& context) const;

    // Current value of a row as display/storage text (validated + clamped).
    [[nodiscard]] std::string valueOf(const OptionSpec& spec, const SettingsStore& store) const;

    // Validates `raw` against the row's type/domain and stores canonical text.
    // Returns false (and changes nothing) for an invalid value: Enum tokens
    // outside the domain are rejected, keeping the current value; the same for
    // a malformed number.
    bool setValue(const OptionSpec& spec, SettingsStore& store, std::string_view raw) const;

    // Moves an Enum by `direction` through its domain (wrapping), or an
    // Integer/Float by step*direction (clamped). Toggle flips. String/Action do
    // nothing. Returns true when the stored value changed.
    bool adjustValue(const OptionSpec& spec, SettingsStore& store, int direction) const;

private:
    std::vector<OptionSpec> options_;
};

// ---------------------------------------------------------------------------
// MenuNavigator
//
// Gamepad-only navigation contract: every transition is reachable with a D-pad
// and a confirm/cancel button — no pointer or mouse is required (the renderer
// may also accept them, but nothing here depends on a cursor).
//   up/down    : move focus (skip unavailable rows; wrap per page)
//   left/right : adjust the focused row's value domain
//   confirm    : enter the focused page / toggle / advance an Enum / trigger an Action
//   cancel     : back out (page -> page list -> closed)
//
// Two levels: a page list (root) and one page. open() yields the deterministic
// initial state (page list, cursor on the first available page). Focus memory
// per page means re-entering a page lands on the row it was left on (clamped to
// an available row when that row has since been hidden).
class MenuNavigator {
public:
    // Metadata the renderer needs for one row, per frame.
    struct FocusRow {
        bool isPageRow = false;  // true at the page-list level
        OptionPage page = OptionPage::Display;
        std::string id;
        std::string label;
        std::string value;       // display text ("" for Action/page rows)
        OptionType type = OptionType::Toggle;
        OptionStatus status = OptionStatus::WiredNow;
        bool available = true;
        std::vector<std::string> domain;
        std::string note;
    };

    MenuNavigator(const OptionsModel& model, SettingsStore& store, MenuContext context = {});

    void setContext(const MenuContext& context) { context_ = context; }
    [[nodiscard]] const MenuContext& context() const { return context_; }
    // context_ with the store-driven flags (terrain/stream/interp/audio) refreshed.
    [[nodiscard]] MenuContext effectiveContext() const;

    bool open();
    bool close();
    [[nodiscard]] bool isOpen() const { return open_; }
    [[nodiscard]] bool inPageList() const { return open_ && pageList_; }

    // up/down. Returns true when the focus moved.
    bool moveFocus(int direction);
    // left/right on the focused row. Returns true when the value changed.
    bool adjust(int direction);
    // confirm button. Returns true when the state changed (page entered, value
    // changed, or an Action fired).
    bool confirm();
    // cancel/back button. Returns true when the state changed.
    bool back();
    // Enters `page` directly (only when it has an available row).
    bool enterPage(OptionPage page);

    [[nodiscard]] OptionPage page() const { return page_; }
    [[nodiscard]] std::size_t focusIndex() const { return focusIndex_; }
    [[nodiscard]] std::size_t pageCursor() const { return pageCursor_; }
    [[nodiscard]] const OptionsModel& model() const { return model_; }
    [[nodiscard]] SettingsStore& store() const { return store_; }

    // Available rows of the current page (empty at the page-list level).
    [[nodiscard]] std::vector<const OptionSpec*> visibleRows() const;
    // Current focus metadata for the renderer.
    [[nodiscard]] FocusRow focusedRow() const;
    // The id of the most recent Action row confirmed ("" until one fires).
    [[nodiscard]] const std::string& lastAction() const { return lastAction_; }

private:
    [[nodiscard]] std::vector<OptionPage> pages() const;
    [[nodiscard]] int firstAvailableRowIndex(OptionPage page, int from) const;
    [[nodiscard]] int rowCount(OptionPage page) const;
    void clampPageCursor(std::size_t cursor);

    const OptionsModel& model_;
    SettingsStore& store_;
    MenuContext context_;

    bool open_ = false;
    bool pageList_ = true;
    OptionPage page_ = OptionPage::Display;
    std::size_t focusIndex_ = 0;     // index into the current page's available rows
    std::size_t pageCursor_ = 0;     // index into pages()
    std::string lastAction_;
    std::vector<std::size_t> focusMemory_;  // per-page remembered focus index
};

}  // namespace rt::native::gameplay
