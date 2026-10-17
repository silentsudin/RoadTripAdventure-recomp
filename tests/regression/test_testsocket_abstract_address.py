"""Keep Android's abstract roadtrip-test endpoint compatible with adb forward.

Pinned against the real device failure (Thor, 2026-10-17): the previous
length `sizeof(sa_family_t) + spec.size() - 1` dropped the final character
of the abstract name, so the listener bound as "@roadtrip-tes" while every
adb forward to "roadtrip-test" was refused.  /proc/net/unix confirmed the
truncated name live; the old assertion here had pinned the buggy expression
(a derivation-only contract), which is exactly the failure mode this suite
must not repeat: when the module changes because of device evidence, the
regression must encode the DEVICE-VERIFIED contract.
"""

from pathlib import Path

SOURCE = Path(__file__).resolve().parents[2] / "src/native/runtime/testsocket.cpp"


def test_abstract_socket_length_includes_the_full_name():
    source = SOURCE.read_text()
    # Abstract name bytes = NUL + every character after '@'.
    assert "sizeof(sa_family_t) + 1 + nameLen" in source
    assert "sizeof(sa_family_t) + spec.size() - 1" not in source


def test_abstract_name_is_copied_by_exact_length():
    source = SOURCE.read_text()
    assert "std::memcpy(a.sun_path + 1, spec.c_str() + 1, nameLen)" in source
    # Over-long names must fail closed instead of truncating.
    assert "nameLen > sizeof(a.sun_path) - 1" in source
