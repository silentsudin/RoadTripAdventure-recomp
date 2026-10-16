"""Keep Android's abstract roadtrip-test endpoint compatible with adb forward."""

from pathlib import Path


def test_abstract_socket_length_excludes_the_at_prefix():
    source = (Path(__file__).resolve().parents[2] / "src/native/runtime/testsocket.cpp").read_text()
    assert "sizeof(sa_family_t) + spec.size() - 1" in source
