"""Small fail-closed checks; no ELF or live gen tree needed."""
import unittest
import tempfile
from pathlib import Path
from unittest.mock import patch

from callargs import call_states, integer_params, reassigned, signature
from transliterate import fill_callargs, load_callargs, new_report


class CallArgsTest(unittest.TestCase):
    def test_float_does_not_consume_a0(self):
        self.assertEqual(integer_params(["float param_1", "int param_2"]), ["param_2"])
        self.assertEqual(signature("// comment\nvoid f(int param_1)\n{"), ("f", ["int param_1"]))

    def test_delay_slot_and_clobbers(self):
        code = [(0, "move", "$16, $4"), (4, "jal", "0x100"),
                (8, "move", "$5, $16"), (12, "jal", "0x104"), (16, "nop", "")]
        states = call_states(code, ["param_1", "param_2"])
        self.assertEqual(states[0], (4, 0x100, ["param_1", "param_1", None, None]))
        self.assertEqual(states[1][2], [None] * 4)

    def test_written_nonparam_branch_and_unknown_skip(self):
        for op, args in [("addiu", "$5, $zero, 100"), ("beqz", "$2, 0x100"),
                         ("<unknown>", "")]:
            code = [(0, op, args), (4, "jal", "0x100"), (8, "nop", "")]
            self.assertIsNone(call_states(code, ["param_1", "param_2"])[0][2][1])

    def test_fill_only_exact_occurrence_and_arity(self):
        body = "f(x); f(g(x, y));"
        rows = [("f", 1, 1, ["param_2"], "0x1234")]
        report = new_report()
        self.assertEqual(fill_callargs(body, "00123456", rows, report),
                         "f(x); f(g(x, y), param_2);")
        self.assertEqual(report["argfill"], [{"caller": "00123456", "callee": "f", "jal": "0x1234"}])
        self.assertEqual(fill_callargs(body, "00123456", [], report), body)
        self.assertEqual(fill_callargs("f(x, y);", "00123456", rows, report), "f(x, y);")

    def test_missing_table_is_noop_and_skip_rows_are_not_loaded(self):
        with tempfile.TemporaryDirectory() as directory, patch("transliterate.ROOT", Path(directory)):
            self.assertEqual(load_callargs(), {})
            path = Path(directory) / "build/decomp/callargs.tsv"
            path.parent.mkdir(parents=True)
            path.write_text("caller\tcallee\toccurrence\targc\tdeclared\tjal\tverdict\n"
                            "00200000\tf\t0\t0\t1\t0x00200010\tskip=written\n"
                            "00200000\tg\t0\t0\t1\t0x00200020\tfill=param_1\n")
            self.assertEqual(dict(load_callargs()),
                             {"00200000": [("g", 0, 0, ["param_1"], "0x00200020")]})

    def test_comments_and_quoted_commas_are_not_calls_or_args(self):
        body = '// f(fake);\nf("a,b) f(fake)");'
        rows = [("f", 0, 1, ["param_2"], "0x1234")]
        self.assertEqual(fill_callargs(body, "00123456", rows, new_report()),
                         '// f(fake);\nf("a,b) f(fake)", param_2);')
        self.assertEqual(fill_callargs("f(')');", "00123456", rows, new_report()),
                         "f(')', param_2);")

    def test_reassigned_param_is_not_an_entry_value(self):
        for body in ["param_1 = x;", "param_1 *= x;", "param_1 <<= 1;", "++param_1;", "param_1--;"]:
            self.assertTrue(reassigned(body, "param_1"), body)
        self.assertFalse(reassigned('param_1 == x; // param_1 = y;\nf("param_1++");', "param_1"))

    def test_fp_is_a_gpr_not_a_float_register(self):
        code = [(0, "move", "$fp, $4"), (4, "addiu", "$fp, $fp, 4"),
                (8, "move", "$4, $fp"), (12, "jal", "0x100"), (16, "nop", "")]
        self.assertIsNone(call_states(code, ["param_1"])[0][2][0])


if __name__ == "__main__":
    unittest.main()
