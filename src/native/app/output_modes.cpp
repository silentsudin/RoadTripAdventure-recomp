// Output-mode policy, implementation.  See output_modes.h for the design
// summary and docs/native-reference/output-modes.md for the evidence ledger.
// This translation unit holds the RT_APP_RESOLUTION grammar; the arithmetic
// contracts live in the header as constexpr code so callers can evaluate
// them at compile time.

#include "app/output_modes.h"

namespace rt::native::app {
namespace {

// ASCII helpers shared by the preset and WxH paths.
constexpr bool isSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

// Case-insensitive ASCII equality (preset names are pure ASCII literals).
constexpr bool namesEqual(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        const char ca = a[i] >= 'A' && a[i] <= 'Z' ? static_cast<char>(a[i] - 'A' + 'a') : a[i];
        const char cb = b[i] >= 'A' && b[i] <= 'Z' ? static_cast<char>(b[i] - 'A' + 'a') : b[i];
        if (ca != cb) return false;
    }
    return true;
}

constexpr std::string_view trim(std::string_view text) {
    std::size_t begin = 0;
    std::size_t end = text.size();
    while (begin < end && isSpace(text[begin])) ++begin;
    while (end > begin && isSpace(text[end - 1])) --end;
    return text.substr(begin, end - begin);
}

// Result of parsing one unsigned decimal field. value <= 32768 on success.
struct ParsedNumber {
    int value = 0;
    std::size_t consumed = 0;  // digits consumed (0 == parse failure)
    bool ok = false;
};
constexpr ParsedNumber parseUnsigned(std::string_view text) {
    ParsedNumber out;
    constexpr int kMaxDimension = 32768;
    // Leading zeros are tolerated ("01280"); a bare "0" parses but the
    // dimension-range check in parseResolution rejects it.
    while (out.consumed < text.size() && text[out.consumed] >= '0' && text[out.consumed] <= '9') {
        const int digit = text[out.consumed] - '0';
        if (out.value > (kMaxDimension - digit) / 10) {
            out.value = kMaxDimension + 1;  // saturation beats wraparound
            out.ok = true;                  // digits consumed; range rejects below
            ++out.consumed;
            continue;
        }
        out.value = out.value * 10 + digit;
        ++out.consumed;
        out.ok = true;
    }
    return out;
}

constexpr bool withinDimensionRange(int value) {
    constexpr int kMaxDimension = 32768;
    return value >= 1 && value <= kMaxDimension;
}

// Preset resolution by token, including the documented aliases.  Returns
// {0,0} (invalid) when the token names none of them; the "native" /
// "desktop" sentinel is still {0,0} so callers must use isNativeDesktop()
// to tell sentinel from failure after checking the parse shape.
constexpr Resolution presetFor(std::string_view token) {
    for (const OutputPreset& preset : kOutputPresets) {
        if (namesEqual(token, preset.name)) return preset.resolution;
    }
    if (namesEqual(token, "desktop")) return Resolution{};   // -> native sentinel
    if (namesEqual(token, "uhd")) return Resolution{3840, 2160};  // -> 2160p/4k
    return Resolution{};
}

}  // namespace

bool isPresetName(std::string_view text) {
    const std::string_view token = trim(text);
    if (token.empty()) return false;
    if (namesEqual(token, "native") || namesEqual(token, "desktop")) return true;
    return valid(presetFor(token));
}

Resolution parseResolution(std::string_view text) {
    const std::string_view token = trim(text);
    if (token.empty()) return {};

    // "WxH" (or "WXH"): two unsigned fields separated by one separator,
    // nothing else.  Reject before the preset lookup so "720p"-shaped junk
    // can never masquerade as a preset, and so a pure number never parses.
    const std::size_t separator = token.find_first_of("xX");
    if (separator != std::string_view::npos) {
        const ParsedNumber w = parseUnsigned(token.substr(0, separator));
        const ParsedNumber h = parseUnsigned(token.substr(separator + 1));
        if (w.ok && withinDimensionRange(w.value) && h.ok && withinDimensionRange(h.value) &&
            w.consumed == separator && h.consumed == token.size() - separator - 1) {
            return {w.value, h.value};
        }
        return {};  // had an "x" but the shape is wrong: not a preset either
    }

    // Preset token: canonical names and aliases map through presetFor().
    // The native/desktop sentinel and a failed parse both return {0,0};
    // callers separate them with isPresetName().
    const Resolution preset = presetFor(token);
    return valid(preset) || isPresetName(token) ? preset : Resolution{};
}

}  // namespace rt::native::app
