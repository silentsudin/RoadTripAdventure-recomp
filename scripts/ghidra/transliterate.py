#!/usr/bin/env python3
"""Transliterate Ghidra pseudocode into compilable C++ (compiled-game program, W10+).

    python3 scripts/ghidra/transliterate.py --decls                 # forward-decl header
    python3 scripts/ghidra/transliterate.py FILE [FILE...] --out DIR [--report R.json]

Each build/decomp/fn/<addr>.c becomes a TU in namespace guest: pointers become u32
guest addresses (RAM stays byte-identical to the emulator), dereferences route
through rtguest::G(), *(code*) calls become rtCall(), and every function registers
itself in the indirect-call table. Data globals are typed per SITE from the
decompiler's own choice (build/decomp/globals.tsv, written by RtExport) — the
site type is what the guest instruction was, so widths stay bit-exact. Anything
the regex pass cannot rewrite (structs, odd register vars, unpaired patterns) is
REPORTED for the package agent's fix pass — never silently mangled.
See docs/native-reference/compiled-game-plan.md.
"""
from __future__ import annotations
import argparse, ast, json, re, sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FN = ROOT / "build/decomp/fn"
GLOBALS = ROOT / "build/decomp/globals.tsv"
SYMBOLS = ROOT / "build/decomp/symbols.tsv"

IDENT = r"[A-Za-z_]\w*"
# *(TYPE **)(  /  *(TYPE *)EXPR — the cast must carry '*': Ghidra writes pointer
# casts as (T *) only, so "x * (float)y" is multiplication, never a deref.
DEREF = re.compile(r"(\*{1,3})\((%s|code)\s*(\*+)\s*\)(?!\s*rtguest::)" % IDENT)
CALL_CODE = re.compile(r"(?<![\w)\]])\((\*+)\((code)\s*(\*+)\s*\)")
CALL_VAR = re.compile(r"(?<![\w)\]])\((\*{1,2})(%s)\)\s*\(" % IDENT)
# ***((T **)(expr)): stacked pointer loads (double-indirection tables)
DEREF2 = re.compile(r"(?<![\w)\]])\*{1,3}\(\((%s|code)\s*\*{2,}\s*\)" % IDENT)

REPORT_KEYS = ("unpaired", "no_signature", "no_body", "needs_struct",
               "raw_ptr_cast", "unknown_type", "mmio_global", "call_var",
               "vu0_macro", "names", "field_select", "ee_reg", "varargs",
               "tok_hex", "lab_value", "tok_str", "sig_param", "cast_subscript",
               "dup_def", "stage_collision", "skip_packed_vec", "skip_named_field",
               "skip_wide_field", "strlit", "strlit_unresolved", "stack_redecl",
               "stack_frame_ref", "deep_deref", "skip_arrow_field",
               "skip_array_cast", "hex_subscript", "amp_local")

def new_report() -> dict:
    return {k: ([] if k != "names" else {}) for k in REPORT_KEYS}

# ---------------------------------------------------------------- globals types
def load_globals() -> tuple[dict[str, dict[int, tuple[str, int]]], dict[int, tuple[str, int]]]:
    """(per-function symaddr->(type,size), dominant symaddr->(type,size))."""
    per_fn: dict[str, dict[int, tuple[str, int]]] = defaultdict(dict)
    votes: dict[int, Counter] = defaultdict(Counter)
    if GLOBALS.exists():
        for line in GLOBALS.read_text().splitlines():
            p = line.split("\t")
            if len(p) < 5:
                continue
            fn, sym, name, typ, size = p[0], int(p[1], 16), p[2], p[3], int(p[4])
            if sym == 0:  # RtExport leaves symaddr 0 for unnamed Ghidra data labels
                m = re.match(r"(?:DAT|UNK|EXT)_([0-9a-fA-F]{6,8})$", name)
                if not m:
                    continue
                sym = int(m.group(1), 16)
            per_fn[fn][sym] = (typ, size)
            votes[sym][(typ, size)] += 1
    dom = {sym: c.most_common(1)[0][0] for sym, c in votes.items()}
    return per_fn, dom

def load_named_data() -> dict[str, int]:
    """name -> addr for data labels that print bare in pseudocode but carry no
    C++ symbol: our applied names (symbols.tsv) plus every named globals.tsv row
    (Ghidra-named globals print as bare identifiers; sym-0 rows carry their
    address as a trailing hex in the label name)."""
    out = {}
    if SYMBOLS.exists():
        for line in SYMBOLS.read_text().splitlines():
            p = line.split("\t")
            if len(p) >= 3 and p[1] == "data":
                out[p[2]] = int(p[0], 16)
    if GLOBALS.exists():
        for line in GLOBALS.read_text().splitlines():
            p = line.split("\t")
            if len(p) < 5:
                continue
            name = p[2]
            if name in ("null", "") or re.fullmatch(r"(?:DAT|UNK|EXT)_[0-9a-fA-F]{6,8}", name):
                continue
            if p[1] != "0":
                out.setdefault(name, int(p[1], 16))
            else:
                m = re.search(r"([0-9a-fA-F]{6,8})$", name)
                if m:
                    out.setdefault(name, int(m.group(1), 16))
    return out

def ctype(typ: str, size: int) -> str | None:
    """Ghidra global data type -> C++ element type (None = report, leave alone)."""
    t = typ.strip()
    if "*" in t or t in ("pointer", "code"):
        return "uint32_t"                       # the value stored IS a guest address
    m = {"undefined": "uint8_t", "undefined1": "uint8_t", "uchar": "uint8_t",
         "byte": "int8_t", "char": "int8_t",
         "undefined2": "uint16_t", "ushort": "uint16_t", "short": "int16_t",
         "undefined3": "uint32_t", "undefined4": "uint32_t", "uint": "uint32_t", "int": "int32_t",
         "undefined5": "uint64_t", "undefined6": "uint64_t", "undefined7": "uint64_t",
         "undefined8": "uint64_t", "ulong": "uint64_t", "long": "int64_t",
         "float": "float", "double": "double"}
    if t in m:
        return m[t]
    return {1: "uint8_t", 2: "uint16_t", 4: "uint32_t", 8: "uint64_t"}.get(size)

# ---------------------------------------------------------------- expression rewrite
def match_bal(text: str, start: int) -> int:
    """Index just past the paren group starting at text[start] == '('. -1 if unbalanced."""
    depth, i = 0, start
    while i < len(text):
        if text[i] == "(":
            depth += 1
        elif text[i] == ")":
            depth -= 1
            if depth == 0:
                return i + 1
        i += 1
    return -1

def split_args(argstr: str) -> list[str]:
    args, depth, cur = [], 0, ""
    for ch in argstr:
        if ch == "," and depth == 0:
            args.append(cur); cur = ""
        else:
            if ch in "([": depth += 1
            elif ch in ")]": depth -= 1
            cur += ch
    if cur.strip():
        args.append(cur)
    return [a.strip() for a in args]

def sig_name_args(sig: str) -> tuple[str, str]:
    """Flattened definition signature (multi-line, brace-terminated ok) ->
    (fn name, arg-list text). ('', '') when it has no balanced final parens."""
    s = " ".join(sig.replace("{", " ").split())
    if not s.endswith(")"):
        return "", ""
    depth, i = 0, len(s) - 1
    while i >= 0:
        if s[i] == ")":
            depth += 1
        elif s[i] == "(":
            depth -= 1
            if depth == 0:
                break
        i -= 1
    if depth != 0 or i <= 0:
        return "", ""
    pre = s[:i].split()
    return (pre[-1] if pre else ""), s[i + 1:-1]

def scan_operand(text: str, pos: int) -> tuple[str, int]:
    """One deref operand starting at pos: a balanced paren group, or one token
    (identifier, optionally indexed, or a number). Returns (operand, index past it)."""
    while pos < len(text) and text[pos] in " \t\r\n":
        pos += 1  # Ghidra wraps long lines between the cast and its operand
    if pos < len(text) and text[pos] == "(":
        end = match_bal(text, pos)
        if end < 0:
            return "", -1
        return text[pos:end], end
    m = re.match(r"@@\d+@@|[A-Za-z_]\w*(\[[^()\[\]]*\])?|0x[0-9a-fA-F]+|\d+", text[pos:])
    if not m:
        return "", -1
    end = pos + m.end()
    while end < len(text) and text[end] == "(":          # call chain, e.g. taskAlloc()
        e2 = match_bal(text, end)
        if e2 < 0:
            break
        end = e2
    return text[pos:end], end

_ELF: bytes | None = None
_ELF_MISS: set[bytes] = set()
_ELF_CACHE: dict[bytes, int] = {}

def strlit_addr(s: str) -> int | None:
    """Ghidra renders boot-ELF string literals as C quotes; find the bytes in the
    ELF and give their vaddr (file offset + 0x1FF000)."""
    global _ELF
    if _ELF is None:
        p = ROOT / "build/rom/SLUS_203.98"
        _ELF = p.read_bytes() if p.exists() else b""
    if not _ELF:
        return None
    b = s.encode("latin1", "replace") + b"\0"
    if b in _ELF_CACHE:
        return _ELF_CACHE[b]
    if b in _ELF_MISS:
        return None
    o = _ELF.find(b)
    if o < 0:
        b = b[:-1]        # fixed-field copies are not NUL-terminated ("UnnamedR" in char[8])
        o = _ELF.find(b)
        if o < 0:
            _ELF_MISS.add(b)
            return None
    _ELF_CACHE[b] = o + 0x1FF000
    return _ELF_CACHE[b]

def deref_emit(typ: str, stars: str, expr: str, lead: str = "*") -> str:
    # extra leading stars = chained pointer loads before the final typed one
    depth = max(1, len(lead))
    if typ == "code" or len(stars) > depth:
        elem = "uint32_t"     # reading a pointer slot
    else:
        elem = ctype(typ, 4) or "uint32_t"
    t = f"(uint32_t){expr}"
    for _ in range(depth - 1):
        t = f"(*(uint32_t*)rtguest::G((uint32_t){t}))"
    return f"(*({elem}*)rtguest::G((uint32_t){t}))"

def rewrite_exprs(text: str, report: dict, addr: str) -> str:
    """*(T*)EXPR derefs and (*(code*)EXPR)(args) calls -> G()/rtCall()."""
    out, i = [], 0
    while True:
        m1, m2, m3, m4 = (DEREF.search(text, i), CALL_CODE.search(text, i),
                          CALL_VAR.search(text, i), DEREF2.search(text, i))
        cands = [(m.start(), k, m) for m, k in ((m1, 1), (m2, 2), (m3, 3), (m4, 4)) if m]
        cands.sort(key=lambda t: (t[0], t[1]))
        hit = next(((m, k) for s, k, m in cands if s >= i), None)
        if not hit:
            out.append(text[i:])
            break
        m, kind = hit
        out.append(text[i:m.start()])
        if kind == 3:                                    # (*fnPtrVar)(args)
            var = m.group(2)
            if m.group(1) == "**":                       # (**ppVar)(args): slot -> slot -> fn
                var = f"(*(uint32_t*)rtguest::G((uint32_t){var}))"
            a2 = m.end() - 1
            end2 = match_bal(text, a2)
            args = "" if end2 < 0 else ", ".join(rewrite_exprs(a, report, addr)
                                            for a in split_args(text[a2 + 1:end2 - 1]))
            report["call_var"].append(f"{addr}:{var[:30]}")
            out.append(f"rtCall({var}{', ' if args else ''}{args})")
            i = end2 if end2 > 0 else m.end()
            continue
        if kind == 4:                                    # ***((T **)(expr)) deep loads
            lead = re.match(r"\*+", m.group(0)).group(0)
            typ, istars = m.group(2), m.group(3)
            expr, pos = scan_operand(text, m.end())
            if not expr:
                report["unpaired"].append(f"{addr}:{text[m.start():m.start() + 40]}")
                i = m.end()
                continue
            expr = rewrite_exprs(expr, report, addr)
            depth = len(lead) + len(istars)
            report["deep_deref"].append(f"{addr}:{depth}")
            inner = expr
            for _ in range(depth - 1):
                inner = f"(*(uint32_t*)rtguest::G((uint32_t){inner}))"
            elem = "uint32_t" if typ == "code" else (ctype(typ, 4) or "uint32_t")
            target = f"(*({elem}*)rtguest::G((uint32_t){inner}))"
            m5 = re.match(r"\s*\)\s*\(", text[pos:])      # call site: (**(code**)(t))(args)
            if m5:
                a2 = pos + m5.end() - 1
                end2 = match_bal(text, a2)
                args = "" if end2 < 0 else ", ".join(rewrite_exprs(a, report, addr)
                                            for a in split_args(text[a2 + 1:end2 - 1]))
                out.append(f"rtCall({target}{', ' if args else ''}{args})")
                i = end2 if end2 > 0 else pos
                continue
            out.append("(" + target + ")")               # caller's group '(' stays literal
            i = pos
            continue
        lead, typ, stars = m.group(1), m.group(2), m.group(3)
        expr, pos = scan_operand(text, m.end())
        if not expr:
            report["unpaired"].append(f"{addr}:{text[m.start():m.start() + 40]}")
            i = m.end()
            continue
        expr = rewrite_exprs(expr, report, addr)   # nested derefs were consumed with the operand
        if kind == 2:                                    # (**(code **)(tbl+n))(args)
            m4 = re.match(r"\s*\)\s*\(", text[pos:])     # close of the leading '(' + args '('
            if m4:
                a2 = pos + m4.end() - 1
                end2 = match_bal(text, a2)
                args = "" if end2 < 0 else ", ".join(rewrite_exprs(a, report, addr)
                                            for a in split_args(text[a2 + 1:end2 - 1]))
                target = deref_emit(typ, stars, expr, lead)
                out.append(f"rtCall({target}{', ' if args else ''}{args})")
                i = end2 if end2 > 0 else pos
                continue
        out.append("(" if kind == 2 else "")   # CALL_CODE consumed the group '('; keep it balanced
        out.append(deref_emit(typ, stars, expr, lead))
        i = pos
    return "".join(out)

def rewrite_named(text: str, ptvars: dict[str, str], keep, report: dict) -> str:
    if not ptvars:
        return text
    names = "|".join(map(re.escape, sorted(ptvars, key=len, reverse=True)))
    def deref_sub(m):
        name = m.group(1)
        return f"(*({ptvars[name]}*)rtguest::G((uint32_t){name}))"
    def deref2_sub(m):
        name = m.group(1)
        return (f"(*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t){name}))))")
    def deref3_sub(m):
        name = m.group(1)
        inner = f"(*(uint32_t*)rtguest::G((uint32_t){name}))"
        for _ in range(2):
            inner = f"(*(uint32_t*)rtguest::G((uint32_t){inner}))"
        return inner
    # call syntax first so the star passes below never see it:
    # (*fnVar)(args) -> rtCall(fnVar, args); (**ppfn)(args) loads twice first
    def call_var_sub(m):
        stars, name = m.group(1), m.group(2)
        target = name if stars == "*" else f"(*(uint32_t*)rtguest::G((uint32_t){name}))"
        if m.group(3) != "(":                      # (*fn)(): empty args
            return keep(f"rtCall({target})")
        return keep(f"rtCall({target}, ")          # the '(' stays replaced; ')' closes rtCall
    text = re.sub(r"(?<![\w)\]])\((\*{1,2})(%s)\)(\(\s*\)|\()" % names, call_var_sub, text)
    text = re.sub(r"(?<![\w*])\*\*\*(%s)\b" % names, deref3_sub, text)
    text = re.sub(r"(?<![\w*])\*\*(%s)\b" % names, deref2_sub, text)
    text = re.sub(r"(?<![\w*])\*(%s)\b" % names, deref_sub, text)
    # &ptvar: the pointer variable's own frame slot (address-of a local): scratch
    # per site; shortcut: site slots do not alias the guest frame — W11 reworks.
    def amp_local(m):
        report["amp_local"].append(f"{m.string[m.start() - 40:m.end()]}")
        return keep(f"rtguest::allocFrame(0x20)")
    text = re.sub(r"&\b(%s)\b\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]" % names,
                  lambda m: keep(f"((uint32_t){m.group(1)} + (uint32_t)({m.group(2)})"
                                 f" * {CSZ.get(ptvars[m.group(1)], 4)})"), text)
    text = re.sub(r"&\b(%s)\b(?!\s*\[)" % names, amp_local, text)
    def deref_add_sub(m):
        name, off = m.group(1), m.group(2)
        t = ptvars[name]
        return f"(*({t}*)rtguest::G((uint32_t){name} + (uint32_t)({off})))"
    # shortcut: Ghidra's export shows byte-scaled offsets for these; upgrade if a
    # state-hash diff shows an element-scale site.
    text = re.sub(r"\*\((%s) \+ ([^()]*)\)" % names, deref_add_sub, text)
    def idx_sub(m, depth=0):
        name, idx = m.group(1), m.group(2)
        if depth < 2:      # a[b[c]] sites: resolve inner subscripts too (2 levels)
            idx = re.sub(r"\b(%s)\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]" % names,
                         lambda mm: idx_sub(mm, depth + 1), idx)
        t = ptvars[name]
        return f"(*({t}*)rtguest::G((uint32_t){name} + (uint32_t)({idx}) * sizeof({t})))"
    # shortcut: inner subscripts to depth 2; deeper -> sweep triage
    text = re.sub(r"\b(%s)\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]" % names,
                  lambda m: keep(idx_sub(m)), text)
    return text

# libc memory/string entry points: which arg positions want a HOST pointer.
LIBC_PTR = {"memset": (0,), "memcpy": (0, 1), "memmove": (0, 1), "memcmp": (0, 1),
            "strlen": (0,), "strcpy": (0, 1), "strncpy": (0, 1), "strcat": (0, 1),
            "strcmp": (0, 1), "strncmp": (0, 1), "strchr": (0,), "strrchr": (0,),
            "sprintf": (0, 1), "snprintf": (0, 1), "vsprintf": (0, 1),
            "puts": (0,), "fputs": (0,), "printf": (0,), "vprintf": (0,),
            "vsnprintf": (0, 1)}
# these return pointers: as guest u32 they take the host value (compiles; the copy
# itself reads guest RAM only after W11 re-implements them against G())
PTRRET = {"memset", "memcpy", "memmove", "strcpy", "strncpy", "strcat",
          "strchr", "strrchr"}

def libc_wrap(body: str) -> str:
    # Ghidra prints gcc's strncpy as bare builtin_strncpy; the host libc is fine
    body = re.sub(r"(?<![:\w.])builtin_strncpy\(", "strncpy(", body)
    for fn, positions in LIBC_PTR.items():
        i = 0
        while True:
            m = re.search(r"(?<![\w.])%s\(" % fn, body[i:])
            if not m:
                break
            ms, a2 = i + m.start(), i + m.end() - 1
            end2 = match_bal(body, a2)
            if end2 < 0:
                i = ms + len(fn) + 1
                continue
            args = split_args(body[a2 + 1:end2 - 1])
            for p in positions:
                if p < len(args):
                    args[p] = f"(char *)rtguest::G((uint32_t)({args[p]}))"
            if fn in ("vsprintf", "vsnprintf") and len(args) > 2:
                args[2] = f"(va_list)(uintptr_t)({args[2]})"   # u32 va-handle stand-in
            call_txt = "::" + fn + "(" + ", ".join(args) + ")"
            if fn in PTRRET:
                call_txt = f"(uint32_t)(uintptr_t)({call_txt})"
            body = body[:ms] + call_txt + body[end2:]
            i = ms + len(call_txt)       # never re-wrap the same call
    # printf/sprintf: newlib stubs in the ELF would shadow host libc inside namespace guest
    body = re.sub(r"(?<![:\w.])v?(?:printf|sprintf|snprintf|vsprintf|puts|fputs)\(",
                  lambda m: ("rtguest::rtPrintf(" if m.group(0) == "printf("
                             else "::" + m.group(0)), body)
    return body

DAT_RE = re.compile(r"&(FUN|DAT|UNK|EXT)_([0-9a-fA-F]{6,8})(?:_[0-9]+)?")

CSZ = {"int8_t": 1, "uint8_t": 1, "int16_t": 2, "uint16_t": 2, "int32_t": 4,
       "uint32_t": 4, "float": 4, "int64_t": 8, "uint64_t": 8, "double": 8}

def transliterate(path: Path, report: dict,
                  fn_globals: dict[int, tuple[str, int]],
                  dom: dict[int, tuple[str, int]],
                  named_data: dict[str, int],
                  def_alias: str = "") -> tuple[str, str, str]:
    text = path.read_text()
    addr = path.stem
    lines = text.splitlines(keepends=True)

    def skip(n: int) -> int:
        """First line that is not a comment (Ghidra's note headers are multi-line
        /* blocks whose interior lines have no marker) or blank."""
        while n < len(lines):
            s = lines[n].strip()
            if s.startswith("/*"):
                while n < len(lines) and "*/" not in lines[n]:
                    n += 1
                n += 1
                continue
            if not s or s.startswith("//"):
                n += 1
                continue
            return n
        return n
    sig_start = skip(0)
    if sig_start >= len(lines):
        report["no_signature"].append(addr)
        return addr, "", ""
    sig_end = None
    for n in range(sig_start, len(lines)):
        if lines[n].rstrip().endswith("{"):
            sig_end = n; break
    if sig_end is None:
        report["no_body"].append(addr)
        return addr, "", ""
    sig = "".join(lines[sig_start:sig_end + 1]).rstrip()   # includes the opening brace
    body = "".join(lines[sig_end + 1:])

    # Ghidra prints boot-ELF strings as C literals: replace each with its vaddr so
    # char* locals and libc fmt args take plain u32 addresses like everything else.
    def strlit_sub(m):
        try:
            s = ast.literal_eval(m.group(0))
        except (SyntaxError, ValueError):
            report["strlit_unresolved"].append(f"{addr}:{m.group(0)[:30]}")
            return m.group(0)
        a = strlit_addr(s)
        if a is None:
            report["strlit_unresolved"].append(f"{addr}:{m.group(0)[:30]}")
            return m.group(0)
        report["strlit"].append(f"{addr}:0x{a:x}")
        return f"0x{a:x}u"
    body = re.sub(r'"(?:[^"\\]|\\.)*"', strlit_sub, body)

    # Ghidra scalar casts to odd widths (int3 etc.) don't exist host-side; the
    # truncation semantics are W11's to land, the compile is G0's.
    body = re.sub(r"\((?:u?int[1-7]|undefined[1-7]|uint|ushort|byte|ulong)\)", "(uint32_t)", body)

    # newlib varargs stubs: need an HLE decision (host libc vs ported newlib), not
    # a regex transliteration — skip the TU, callers link through the registry.
    if re.search(r"\bva_list\b|__gnuc_va_list", sig + body):
        report.setdefault("varargs", []).append(addr)
        return addr, "", ""

    # W11 hand-port territory (decided with cited evidence, batch 3+4): packed-vector
    # intrinsic ops with vector-temp locals, Ghidra type-library field names
    # (newlib FILE etc., offsets unrecoverable from text, both `.` and `->` forms),
    # 3/5/6/7-byte field-select tokens (no struct model), and array-type casts
    # (`(char (*)[8])`, inexpressible in C++ without typedefs). No TU; synth_decl
    # covers callers — never a silent no-op body.
    for pat, key in ((r"\b_(?:pcpyld|pcpyud|pextlw|pextuw|pcpyh|pclwld|pcswd"
                      r"|paddw|psubw|paddb|psubb|paddh|psubh|maxiw|miniw|maxsw|minsw)\s*\(",
                      "skip_packed_vec"),
                     (r"\._[A-Za-z]", "skip_named_field"),
                     (r"->", "skip_arrow_field"),
                     (r"\._\d+_(?:3|5|6|7)_", "skip_wide_field"),
                     (r"\(%s\s*(?:\(\s*\*\s*\))?\s*\[\d+\]\)" % IDENT, "skip_array_cast")):
        mm = re.search(pat, body)
        if mm:
            report.setdefault(key, []).append(f"{addr}:{mm.group(0)}")
            return addr, "", ""

    # Ghidra's switch-case methods: implicit class scope -> flat C++ names
    # (registry keys stay the addresses, so the rename is call-consistent).
    sig = re.sub(r"\bswitchD_([0-9a-fA-F]+)::", r"switchD_\1_", sig)
    body = re.sub(r"\bswitchD_([0-9a-fA-F]+)::", r"switchD_\1_", body)

    # VU0 macro-instruction code (Ghidra prints the intrinsic names): needs the
    # VU0 host model, not a regex transliteration. Skip the TU; call sites still
    # link via the registry (rtCall) and the decl.
    if re.search(r"\b_(v[a-z0-9]{2,}|lqc2|sqc2)\s*\(", body):
        report["vu0_macro"].append(addr)
        return addr, "", ""

    # pointer variable declarations -> u32 guest addresses (elem type kept in ptvars)
    ptvars: dict[str, str] = {}
    def elem_t(t: str) -> str:
        return ctype(t, 4) or "uint32_t"
    def decl_sub(m):
        t, stars, name = m.group(1), m.group(2), m.group(3)
        if t.split()[-1].startswith(("stru_", "unnamed_", "code_ptr")):
            report["needs_struct"].append(f"{addr}:{m.group(0)}")
            return m.group(0)
        if name in ptvars:
            report["stack_redecl"].append(f"{addr}:{name}")   # re-declared pointer local
            return ""
        ptvars[name] = elem_t(t) if len(stars) == 1 else "uint32_t"
        return f"uint32_t {name};"
    body = re.sub(r"^\s*((?:[A-Za-z_]\w*\s+)*[A-Za-z_]\w*)\s*(\*+)(\w+);", decl_sub,
                  body, flags=re.M)

    # Loose calling convention (G0): every guest fn is DEFINED as
    # (uint32_t p0..pN-1, ...) and the decl header defaults every param, so any
    # call site matches by name whatever Ghidra's printed arity was. Pointer and
    # array params keep their element type in ptvars so *name derefs still work.
    sname, inner = sig_name_args(sig)
    if not sname:
        report["no_signature"].append(addr)
        return addr, "", ""
    out_params: list[str] = []
    param_names: set[str] = set()
    for i, p in enumerate(split_args(inner) if inner.strip() not in ("", "void") else []):
        if "..." in p:
            continue
        fm = re.match(r"^.+\(\s*\*\s*(\w+)\s*\)\s*(\([^()]*\)|\[[^()]*\])?$", p)  # func-ptr/ptr-to-array param
        pm = re.search(r"\*\s*(\w+)\s*(\[[^()]*\])?$", p)       # pointer param
        am = re.match(r"^([A-Za-z_][\w ]*?)\s+(\w+)\s*\[[^()]*\]$", p)  # value-array param
        if fm:
            nm = fm.group(1)
            report["sig_param"].append(f"{addr} {p}")
            ptvars[nm] = "uint32_t"
        elif pm:
            nm = pm.group(1)
            stars = p[:pm.start(1)].count("*")
            base = p[:pm.start(1)].replace("*", " ").strip()
            ptvars[nm] = elem_t(base) if stars == 1 else "uint32_t"
        elif am:
            nm = am.group(2)
            report["sig_param"].append(f"{addr} {p}")
            ptvars[nm] = elem_t(am.group(1))
        else:
            tm = re.search(r"(\w+)\s*$", p)
            if not tm:
                report["no_signature"].append(addr)
                return addr, "", ""
            nm = tm.group(1)
        out_params.append(f"uint32_t {nm}")
        param_names.add(nm)
    params = out_params or ["uint32_t q0"]  # C++ variadics need one named param
    sig = f"uint32_t {def_alias or sname}({', '.join(params)}, ...) {{"
    # EE convention: v0 carries the return value even where Ghidra printed void
    body = re.sub(r"(?<![\w.])return;", "return 0;", body)

    # Stack arrays (auStack_* locals): back them with the guest-RAM frame arena so
    # they hold guest addresses and can be passed to other guest fns.
    # shortcut: unknown element types assume 4 bytes; upgrade if a struct-typed
    # stack array shows up in a state-hash diff.
    # emitted-text sentinels: text already in final G() shape must not be re-scanned
    # by the expression passes (the @@n@@ tokens are inert to every regex here).
    store: list[str] = []
    def keep(text_: str) -> str:
        store.append(text_)
        return f"@@{len(store) - 1}@@"
    # type group must be identifier words only: (.+?) would reach back into
    # assignment lines like "x = auStack_6c[2];" and destroy them.
    TYPE = r"[A-Za-z_]\w*(?:\s*\*+)?(?:\s+[A-Za-z_]\w*(?:\s*\*+)?)*"

    # Early token resolution (before the expression passes): Ghidra's data and
    # label tokens must become hex literals or keep()'d G() refs while they are
    # still token-shaped.
    body = re.sub(r"\bstack0x([0-9a-fA-F]+)\b", lambda m: f"0x{m.group(1)}u", body)
    # named pointer locals first: their loads/subscripts/call syntax must see the raw
    # text, and the token passes below may nest those names in their keep()'d entries
    body = rewrite_named(body, ptvars, keep, report)
    # scalar field-select tokens print uniformly as BASE._OFF_SIZE_ over untyped
    # DATs, named globals, stack slots and PTR slots alike; resolve uniformly by
    # the base's address (&base is fixed by the later &DAT/&named/&stack rewrites).
    def field_sub(m):
        ct = {1: "uint8_t", 2: "uint16_t", 4: "uint32_t", 8: "uint64_t"}.get(int(m.group(3)))
        report["field_select"].append(f"{addr}:{m.group(0)}")
        if not ct:
            return m.group(0)
        return f"(*({ct}*)rtguest::G((uint32_t)&{m.group(1)} + {int(m.group(2))}))"
    body = re.sub(r"(?<![\w.])\b(\w+)\._(\d+)_(\d+)_", field_sub, body)
    # field_sub may print `&local` for a pointer local's slot: scratch frame slot
    if ptvars:
        ptn = "|".join(map(re.escape, sorted(ptvars, key=len, reverse=True)))
        body = re.sub(r"&\b(%s)\b(?!\s*\[)" % ptn,
                      lambda m: keep("rtguest::allocFrame(0x20)"), body)
    # &0x... (field_sub's &base on hex-resolved bases, &stack-slot refs): the slot
    # address is the literal itself. shortcut: real frame-slot semantics -> W11.
    def amp_hex(m):
        report["stack_frame_ref"].append(f"{addr}:{m.group(0)}")
        return m.group(1)
    body = re.sub(r"(?<![\w)\]])&(0x[0-9a-fA-F]+u?)", amp_hex, body)
    # PTR table subscripts, before ptr_sub would strand the '[' on a hex value:
    # (&PTR_X)[i] and bare PTR_X[i] load the u32 slot at X+i*4; &PTR_X[i] is the
    # slot's own address.
    PTR_TOK = r"_*PTR_[\w]*?([0-9a-fA-F]{6,8})(?:_\d+)?(?!\w)"
    PTR_IDX = r"\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]"   # Ghidra wraps lines before '['
    PTR_CH2 = r"((?:\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\])?)"  # `X[i][j]` chain
    def ptr_idx(m, addr_form: bool):
        a, idx = int(m.group(1), 16), m.group(2)
        if addr_form:
            report["tok_hex"].append(f"{addr}:&PTR_{m.group(1)}[i]")
            return f"(0x{a:x}u + (uint32_t)({idx}) * 4)"
        load = (f"(*(uint32_t*)rtguest::G((uint32_t)(0x{a:x}u "
                f"+ (uint32_t)({idx}) * 4)))")
        if m.group(3):    # pointer table: slot i holds a base; element j off it
            load = (f"(*(uint32_t*)rtguest::G((uint32_t){load}"
                    f" + (uint32_t)({m.group(4)}) * 4))")
        return keep(load)
    body = re.sub(r"\(&" + PTR_TOK + r"\)" + PTR_IDX + PTR_CH2,
                  lambda m: ptr_idx(m, False), body)
    body = re.sub(r"&" + PTR_TOK + PTR_IDX, lambda m: ptr_idx(m, True), body)
    body = re.sub(r"(?<![\w.&])" + PTR_TOK + PTR_IDX + PTR_CH2,
                  lambda m: ptr_idx(m, False), body)
    # PTR_<hex> indirect-data slots: bare token = load the stored pointer value,
    # &form = the slot's own address (hex).
    def ptr_sub(m):
        a = int(m.group(2), 16)
        if m.group(1):
            return f"0x{a:x}u"
        return keep(f"(*(uint32_t*)rtguest::G(0x{a:08x}))")
    body = re.sub(r"(&?)_*PTR_[\w]*?([0-9a-fA-F]{6,8})(?:_\d+)?(?!\w)", ptr_sub, body)
    # RAM tokens (uRam/cRam/ram0x + addr): typed load at a fixed guest address.
    def ram_sub(m):
        a = int(m.group(3), 16)
        if m.group(1):
            return f"0x{a:x}u"
        if m.group(2) in ("uRam", "cRam"):
            ct = "uint8_t"
        else:
            tt = dom.get(a)
            ct = (ctype(*tt) if tt else None) or "uint32_t"
        return keep(f"(*({ct}*)rtguest::G(0x{a:08x}))")
    body = re.sub(r"(&?)\b(uRam|cRam|ram0x)([0-9a-fA-F]{6,8})\b", ram_sub, body)
    # LAB_<hex> used as a value (computed switch targets), not goto/label defs.
    def lab_sub(m):
        report["lab_value"].append(f"{addr}:{m.group(0)}")
        return f"0x{int(m.group(2), 16):x}u"
    body = re.sub(r"(&?)(?<!goto )\bLAB_([0-9a-fA-F]{6,8})(?:_\d+)?(?!\w)(?!\s*:)", lab_sub, body)
    # s_<text><hex> string-literal data labels -> address value (bare or &form).
    def s_sub(m):
        report["tok_str"].append(f"{addr}:{m.group(0)}")
        return f"0x{int(m.group(2), 16):x}u"
    body = re.sub(r"(&?)\bs_[\w]*?([0-9a-fA-F]{6,8})(?:_\d+)?(?!\w)(?!\s*:)", s_sub, body)
    body = re.sub(r"\bnull\b", "0u", body)

    stack_var: dict[str, tuple[str, int]] = {}
    scalar_stack: dict[str, tuple[str, int]] = {}
    def stack_decl(m):
        name = m.group(3)
        if name in stack_var or name in scalar_stack or name in ptvars \
                or name in param_names:
            report["stack_redecl"].append(f"{addr}:{name}")
            return ""
        ct = "uint32_t" if m.group(2) else (ctype(m.group(1), 4) or "uint32_t")
        esz = CSZ.get(ct, 4)
        stack_var[name] = (ct, esz)
        return f"uint32_t {name} = rtguest::allocFrame({int(m.group(4)) * esz});"
    body = re.sub(r"^\s*(%s)\s*(\*+)?\s*\b(\w+)\s*\[\s*(\d+)\s*\];" % TYPE,
                  stack_decl, body, flags=re.M)
    if stack_var:
        sn = "|".join(map(re.escape, sorted(stack_var, key=len, reverse=True)))
        def st_base(nm, i):
            ct, esz = stack_var[nm]
            return ct, esz, (f"(*({ct}*)rtguest::G((uint32_t){nm}"
                             f" + (uint32_t)({i}) * {esz}))")
        # pointer arrays: NAME[i][j] indexes through the pointer in slot i,
        # *NAME[i] dereferences it
        body = re.sub(r"\b(%s)\b\s*\[([^\]]*)\]\s*\[([^\]]*)\]" % sn,
                      lambda m: keep(f"(*(uint32_t*)rtguest::G((uint32_t){st_base(m.group(1), m.group(2))[2]}"
                                    f" + (uint32_t)({m.group(3)}) * 4))"), body)
        body = re.sub(r"(?<![\w.])\*\b(%s)\b\s*\[([^\]]*)\]" % sn,
                      lambda m: keep(f"(*(uint32_t*)rtguest::G((uint32_t){st_base(m.group(1), m.group(2))[2]}))"),
                      body)
        body = re.sub(r"&\b(%s)\b\s*\[([^\]]*)\]" % sn,
                      lambda m: f"({m.group(1)} + ({m.group(2)}) * {stack_var[m.group(1)][1]})", body)
        body = re.sub(r"\b(%s)\b\s*\[([^\]]*)\]" % sn,
                      lambda m: keep(st_base(m.group(1), m.group(2))[2]), body)
        body = re.sub(r"&\b(%s)\b" % sn, lambda m: m.group(1), body)

    # scalar Stack_ locals: same arena backing; bare name = value load, &name = slot addr
    # (no initialiser on these declarations in this corpus — proven by grep)
    def scalar_decl(m):
        if m.group(3) in ptvars or m.group(3) in stack_var:
            return m.group(0)     # pointer/array local already handled by its own pass
        ct = "uint32_t" if m.group(2) else (ctype(m.group(1), 4) or "uint32_t")
        scalar_stack[m.group(3)] = (ct, CSZ.get(ct, 4))
        return ""
    body = re.sub(r"^\s*(%s)\s*(\*+)?\s*\b(\w*Stack_[0-9a-fA-F]+)\s*;\n" % TYPE,
                  scalar_decl, body, flags=re.M)
    if scalar_stack:
        sn = "|".join(map(re.escape, sorted(scalar_stack, key=len, reverse=True)))
        # subscripted scalar name: the slot holds a pointer value; index through it
        def sc_load(nm, i):
            return (f"(*(uint32_t*)rtguest::G((uint32_t){nm}"
                    f" + (uint32_t)({i}) * 4))")
        body = re.sub(r"\b(%s)\b\s*\[([^\]]*)\]\s*\[([^\]]*)\]" % sn,
                      lambda m: keep(f"(*(uint32_t*)rtguest::G((uint32_t){sc_load(m.group(1), m.group(2))})"
                                    f" + (uint32_t)({m.group(3)}) * 4))"), body)
        body = re.sub(r"(?<![\w.])\*\b(%s)\b\s*\[([^\]]*)\]" % sn,
                      lambda m: keep(f"(*(uint32_t*)rtguest::G((uint32_t){sc_load(m.group(1), m.group(2))}))"),
                      body)
        body = re.sub(r"&\b(%s)\b\s*\[([^\]]*)\]" % sn,
                      lambda m: f"({m.group(1)} + (uint32_t)({m.group(2)}) * 4)", body)
        body = re.sub(r"\b(%s)\b\s*\[([^\]]*)\]" % sn,
                      lambda m: keep(sc_load(m.group(1), m.group(2))), body)
        body = re.sub(r"&\b(%s)\b" % sn, lambda m: keep(m.group(1)), body)
        body = re.sub(r"(?<![\w.])\b(%s)\b(?!\s*\[)(?!\s*\()" % sn,
                      lambda m: keep(f"(*({scalar_stack[m.group(1)][0]}*)rtguest::G((uint32_t){m.group(1)}))"), body)

    # Ghidra declares locals per scope and re-declares params at first use; the
    # flattened function body needs one declaration per name. Drop repeats.
    declared: set[str] = set(param_names)
    def dedup_decl(m):
        name = m.group(1)
        if name in declared:
            return ""
        declared.add(name)
        return m.group(0)
    # the guard matters: without it "return 0;" reads as a declaration of "0"
    # and every later one in the TU gets dropped (silent case fall-through)
    body = re.sub(r"^[ \t]*(?!(?:return|case|break|continue|goto|default|do|else)\b)"
                  r"[A-Za-z_]\w*(?:\s*\*+)?[ \t]+([A-Za-z_]\w*)[ \t]*;\n",
                  dedup_decl, body, flags=re.M)

    # (&DAT_x)[i] and DAT_x[i]: Ghidra's "array at global" idiom — element type
    # from the site table (dominant vote fallback), stride = its size; must run
    # before &DAT literalization, which would strand the subscript on a hex value.
    def arr_sub(m):
        a = int(m.group(1), 16)
        tt = fn_globals.get(a) or dom.get(a)
        ct = ctype(*tt) if tt else None
        if not ct:
            report["unknown_type"].append(f"{addr}:arr{m.group(1)}")
            ct, esz = "uint32_t", 4
        else:
            esz = CSZ.get(ct, 4)
        return keep(f"(*({ct}*)rtguest::G((uint32_t)0x{a:x} + (uint32_t)({m.group(2)}) * {esz}))")
    body = re.sub(r"\(&\b(?:DAT|UNK|EXT)_([0-9a-fA-F]{6,8})\)\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]",
                  arr_sub, body)
    # `(&DAT_x + off)[i]`: array base expressed as symbol + offset
    def arr_sub_plus(m):
        a = int(m.group(1), 16)
        tt = fn_globals.get(a) or dom.get(a)
        ct = (ctype(*tt) if tt else None) or "uint32_t"
        esz = CSZ.get(ct, 4)
        return keep(f"(*({ct}*)rtguest::G((uint32_t)(0x{a:x}u{m.group(2)})"
                    f" + (uint32_t)({m.group(3)}) * {esz}))")
    body = re.sub(r"\(&\b(?:DAT|UNK|EXT)_([0-9a-fA-F]{6,8})((?:[^()\[\]]|\([^()]*\))*)\)"
                  r"\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]", arr_sub_plus, body)
    body = re.sub(r"(?<![\w.])\b(?:DAT|UNK|EXT)_([0-9a-fA-F]{6,8})(?:_[0-9]+)?\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]",
                  arr_sub, body)

    # &FUN_/&DAT_/&UNK_/&EXT_ -> guest-address literals (bare FUN_ calls stay names;
    # bare DAT_/named data get typed G() refs below from globals.tsv)
    body = DAT_RE.sub(lambda m: f"0x{m.group(2)}", body)
    # FUN_hex used as a VALUE (function pointer into the registry), not called:
    body = re.sub(r"\bFUN_([0-9a-fA-F]{6,8})\b(?!\s*\()", lambda m: "0x" + m.group(1), body)

    # EE memory-mapped register labels (r5900 analyzer symbols, not in the ELF):
    # keep the token and declare it as staging storage in the decls header.
    # shortcut: reads/writes are staged, not modelled; W12 lands the MMIO handlers.
    for reg in sorted(set(re.findall(r"\bREG_[0-9A-Za-z_]+\b", body))):
        report["ee_reg"].append(reg)

    def global_sub(name2addr):
        def sub(m):
            name, a = m.group(0), name2addr[m.group(0)]
            tt = fn_globals.get(a) or dom.get(a)
            if not tt:
                report["unknown_type"].append(f"{addr}:{name}")
                tt = ("uint32_t", 4)      # conservative default; port pass retypes from the ELF
            ct = ctype(*tt)
            if not ct:
                report["unknown_type"].append(f"{addr}:{name}:{tt[0]}")
                ct = "uint32_t"
            if 0x10000000 <= a < 0x20000000:
                report["mmio_global"].append(f"{addr}:{name}")
            # struct-typed global used with field access: leave for the struct
            # pass. Numeric FIELD tokens (._0_4_) are transliterated plumbing,
            # not proof of a struct — only real member syntax counts.
            if re.search(re.escape(name) + r"\s*(\.(?!_?\d)|->)", body):
                report["needs_struct"].append(f"{addr}:{name}")
                return m.group(0)
            return f"(*({ct}*)rtguest::G(0x{a:x}))"
        return sub
    if named_data:
        nd = "|".join(map(re.escape, sorted(named_data, key=len, reverse=True)))
        body = re.sub(r"&(%s)\b(?!\s*\[)" % nd, lambda m: f"0x{named_data[m.group(1)]:x}", body)

    if "->" in re.sub(r"\b\w+\b(?=\s*->)", "", body):  # struct deref sites (not our u32 refs)
        report["needs_struct"].append(addr)

    # named locals first: `*name` / `name[i]` / `(*(code *)*name)()` must see the
    # raw pointer-variable text, not operands already flattened into G() loads
    body = rewrite_exprs(body, report, addr)
    body = libc_wrap(body)

    # (T *)base[i]: base is a u32 pointer value; the cast gives the element type.
    def cast_sub(m):
        ct = ctype(m.group(1), 4) or "uint32_t"
        esz = CSZ.get(ct, 4)
        report["cast_subscript"].append(f"{addr}:{m.group(0)}")
        return keep(f"(*({ct}*)rtguest::G((uint32_t){m.group(2)} "
                    f"+ (uint32_t)({m.group(3)}) * {esz}))")
    body = re.sub(r"\(([A-Za-z_]\w*)\s*\*+\)\s*(@@\d+@@|[A-Za-z_]\w*(?!\s*\())\s*"
                  r"\[((?:[^\[\]]|\[[^\[\]]*\])*)\]", cast_sub, body)
    # ((T *)base)[i]: the same site inside grouping parens
    body = re.sub(r"\(\(([A-Za-z_]\w*)\s*\*+\)\s*(@@\d+@@|[A-Za-z_]\w*(?!\s*\())\)\s*"
                  r"\[((?:[^\[\]]|\[[^\[\]]*\])*)\]", cast_sub, body)

    # typed data refs LAST: their emitted (*(T*)G(0x..)) text must not be re-scanned
    # by the expression passes above (that corrupted the derefs)
    def dat_star(m):
        # `*DAT_x` / `**DAT_x`: loads chained through the global's pointer value
        k = len(m.group(0)) - len(m.group(0).lstrip("*"))
        a = int(m.group(1), 16)
        tt = fn_globals.get(a) or dom.get(a)
        ct = (ctype(*tt) if tt else None) or "uint32_t"
        t = f"(*({ct}*)rtguest::G((uint32_t)0x{a:x}))"
        for _ in range(k):
            t = f"(*(uint32_t*)rtguest::G((uint32_t){t}))"
        return t
    body = re.sub(r"(?<![\w.])\*{1,2}_?(?:DAT|UNK|EXT)_([0-9a-fA-F]{6,8})(?:_[0-9]+)?(?!\w)",
                  dat_star, body)
    body = re.sub(r"(?<![\w.])_?(?:DAT|UNK|EXT)_([0-9a-fA-F]{6,8})(?:_[0-9]+)?(?!\w)",
                  lambda m: global_sub({m.group(0): int(m.group(1), 16)})(m), body)
    if named_data:
        # subscripted named globals: pointer-shaped ones (used as `*NAME`/`NAME + k`
        # in this TU) index through the loaded slot; the rest index at the symbol.
        ptrish = {n for n in named_data
                  if re.search(r"(?<![\w.])%s\s*\[" % re.escape(n), body)
                  and re.search(r"\*\s*%s\b|%s\s*\+\s*[0-9(]" % (re.escape(n), re.escape(n)), body)}
        def nd_base(nm):
            a = named_data[nm]
            tt = fn_globals.get(a) or dom.get(a)
            ct = (ctype(*tt) if tt else None) or "uint32_t"
            esz = CSZ.get(ct, 4)
            b = (f"(uint32_t)(*({ct}*)rtguest::G(0x{a:x}))" if nm in ptrish else f"0x{a:x}")
            return ct, esz, b
        def nd_load(nm, idx):
            ct, esz, b = nd_base(nm)
            return (f"(*({ct}*)rtguest::G({b}"
                    f" + (uint32_t)({idx}) * {esz}))")
        body = re.sub(r"&\b(%s)\b\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]" % nd,
                      lambda m: "({} + (uint32_t)({}) * {})".format(
                          nd_base(m.group(1))[2], m.group(2), nd_base(m.group(1))[1]), body)
        body = re.sub(r"(?<![\w.])(\*{1,2})(%s)\b(?!\s*\[)" % nd,
                      lambda m: ("(*(uint32_t*)rtguest::G((uint32_t){}))"
                                 .format(nd_load(m.group(2), "0"))
                                 if m.group(1) == "*" else
                                 "(*(uint32_t*)rtguest::G((uint32_t)"
                                 "(*(uint32_t*)rtguest::G((uint32_t){}))))".format(nd_load(m.group(2), "0"))),
                      body)
        body = re.sub(r"(?<![\w.])\b(%s)\b\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]"
                      r"((?:\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\])?)(?!\s*\()" % nd,
                      lambda m: (f"(*(uint32_t*)rtguest::G((uint32_t){nd_load(m.group(1), m.group(2))})"
                                 f" + (uint32_t)({m.group(4)}) * 4))" if m.group(3)
                                 else nd_load(m.group(1), m.group(2))), body)
        body = re.sub(r"\b(%s)\b(?!\s*\()(?!\s*\[)" % nd, global_sub(named_data), body)

    # hex value with a left-behind subscript (a token resolved to its address
    # under a `(...)[i]` shape): read the word array at that address. only the
    # balanced forms — a `)` that belongs to an outer group must not be eaten.
    def hex_idx(m, base):
        return keep(f"(*(uint32_t*)rtguest::G((uint32_t){base}"
                    f" + (uint32_t)({m.group(2)}) * 4))")
    body = re.sub(r"\(0x([0-9a-fA-F]{1,8}u?)\)\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]",
                  lambda m: hex_idx(m, f"(uint32_t)0x{m.group(1)}"), body)
    body = re.sub(r"(?<![\w)\]])0x([0-9a-fA-F]{1,8})u?\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]",
                  lambda m: hex_idx(m, f"(uint32_t)0x{m.group(1)}u"), body)
    body = re.sub(r"\((0x[0-9a-fA-F]{1,8}u?(?:[^()\[\]]|\([^()]*\))*)\)"
                  r"\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]",
                  lambda m: hex_idx(m, f"(uint32_t)({m.group(1)})"), body)
    # general `(EXPR)[i]`: Ghidra's commuted subscript on a computed base
    body = re.sub(r"\(((?:[^()\[\]]|\([^()]*\))+)\)\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]",
                  lambda m: hex_idx(m, f"(uint32_t)({m.group(1)})"), body)

    # remaining raw pointer casts -> guest-address casts (pointer variables are u32);
    # skip the (*(T*)rtguest::G(...)) forms we emitted, which must keep their '*'
    if re.search(r"\((?:%s|code)\s*\*+\s*\)(?!\s*rtguest::G)" % IDENT, body):
        report["raw_ptr_cast"].append(addr)
        body = re.sub(r"\((?:%s|code)\s*\*+\s*\)(?!\s*rtguest::G)" % IDENT, "(uint32_t)", body)

    # scalar slots get their arena allocation as the first statements (name defs must
    # come after the site substitutions, which treat the name as a slot reference)
    if scalar_stack:
        body = "".join(f"    uint32_t {n} = rtguest::allocFrame({sz});\n"
                       for n, (_, sz) in sorted(scalar_stack.items())) + body
    # sentinels back to their final G()-shape text: last step, nothing rescans it.
    # Composite text kept early embeds raw tokens the late passes never saw, so
    # resolve each stored entry with the same token rules, then unwrap to a fixed
    # point (a resolved entry may reference further sentinels).
    def resolve_tokens(t: str) -> str:
        t = re.sub(r"\((?:%s|code)\s*\*+\)(?!\s*rtguest::G)" % IDENT, "(uint32_t)", t)
        t = re.sub(r"\bFUN_([0-9a-fA-F]{6,8})\b(?!\s*\()", lambda m: "0x" + m.group(1), t)
        t = re.sub(r"(&?)\b(uRam|cRam|ram0x)([0-9a-fA-F]{6,8})\b", ram_sub, t)
        t = re.sub(r"(&?)(?<!goto )\bLAB_([0-9a-fA-F]{6,8})(?:_\d+)?(?!\w)(?!\s*:)", lab_sub, t)
        t = re.sub(r"(&?)\bs_[\w]*?([0-9a-fA-F]{6,8})(?:_\d+)?(?!\w)(?!\s*:)", s_sub, t)
        t = re.sub(r"(&?)_*PTR_[\w]*?([0-9a-fA-F]{6,8})(?:_\d+)?(?!\w)", ptr_sub, t)
        t = re.sub(r"(?<![\w.])\*{1,2}_?(?:DAT|UNK|EXT)_([0-9a-fA-F]{6,8})(?:_[0-9]+)?(?!\w)",
                   dat_star, t)
        t = re.sub(r"(&?)(?<![\w.])_?(?:DAT|UNK|EXT)_([0-9a-fA-F]{6,8})(?:_[0-9]+)?(?!\w)",
                   lambda m: f"0x{m.group(2)}u" if m.group(1)
                   else global_sub({m.group(0): int(m.group(2), 16)})(m), t)
        if named_data:
            t = re.sub(r"&\b(%s)\b(?!\s*\[)" % nd, lambda m: f"0x{named_data[m.group(1)]:x}u", t)
            t = re.sub(r"\b(%s)\b(?!\s*\()(?!\s*\[)" % nd, global_sub(named_data), t)
            t = re.sub(r"(?<![\w.])(\*{1,2})(%s)\b(?!\s*\[)" % nd,
                       lambda m: ("(*(uint32_t*)rtguest::G((uint32_t){}))"
                                  .format(nd_load(m.group(2), "0"))
                                  if m.group(1) == "*" else
                                  "(*(uint32_t*)rtguest::G((uint32_t)"
                                  "(*(uint32_t*)rtguest::G((uint32_t){}))))".format(nd_load(m.group(2), "0"))), t)
            t = re.sub(r"(?<![\w.])\b(%s)\b\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]"
                       r"((?:\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\])?)" % nd,
                       lambda m: (f"(*(uint32_t*)rtguest::G((uint32_t){nd_load(m.group(1), m.group(2))})"
                                  f" + (uint32_t)({m.group(4)}) * 4))" if m.group(3)
                                  else nd_load(m.group(1), m.group(2))), t)
        # composite keeps can hold subscripted hex bases too (balanced forms only)
        t = re.sub(r"\(0x([0-9a-fA-F]{1,8}u?)\)\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]",
                   lambda m: f"(*(uint32_t*)rtguest::G((uint32_t)0x{m.group(1)}"
                             f" + (uint32_t)({m.group(2)}) * 4))", t)
        t = re.sub(r"(?<![\w)\]])0x([0-9a-fA-F]{1,8})u?\s*\[((?:[^\[\]]|\[[^\[\]]*\])*)\]",
                   lambda m: f"(*(uint32_t*)rtguest::G((uint32_t)0x{m.group(1)}u"
                             f" + (uint32_t)({m.group(2)}) * 4))", t)
        t = re.sub(r"(?<![\w)\]])&(0x[0-9a-fA-F]+u?)", lambda m: m.group(1), t)
        return re.sub(r"\bnull\b", "0u", t)
    if store:
        # early keeps froze raw `*(T*)expr`/call text the late passes never saw
        store[:] = [rewrite_exprs(s, report, addr) for s in store]
        for _ in range(6):
            store[:] = [resolve_tokens(s) for s in store]
            body = re.sub(r"@@(\d+)@@", lambda m: store[int(m.group(1))], body)
            if not re.search(r"@@\d+@@", body):
                break

    m = re.search(r"(\w+)\s*\(", sig.split("\n", 1)[0])
    fname = m.group(1) if m else "sub_" + addr
    report["names"][addr] = fname

    decl = (f"uint32_t {def_alias or sname}("
            + ", ".join(p + " = 0" for p in params) + ", ...);")
    tu = (
        f"// guest:0x{addr} transliterated from Ghidra pseudocode — DO NOT hand-edit\n"
        f'#include "../guest_shim.h"\n#include "guest_decls.inc"\n\nnamespace guest {{\n\n'
        f"{sig}\n{body}"
        f"\nRT_GUEST_FN(0x{addr}, guest::{fname})\n\n}}  // namespace guest\n"
    )
    return addr, tu, decl

def decl_name(d: str) -> str:
    return d[:d.index("(")].split()[-1]

def synth_decl(path: Path) -> str:
    """Skipped fns (VU0 macro instrs, varargs stubs) have no TU but callers still
    reference their names: synthesize a decl from the pseudocode signature line."""
    lines = path.read_text().splitlines()
    i = 0
    while i < len(lines):
        s = lines[i].strip()
        if s.startswith("/*"):
            while i < len(lines) and "*/" not in lines[i]:
                i += 1
        elif s and not s.startswith("//"):
            break
        i += 1
    parts = []
    while i < len(lines) and not lines[i].rstrip().endswith("{"):
        parts.append(lines[i])
        i += 1
    sig = re.sub(r"\bswitchD_([0-9a-fA-F]+)::", r"switchD_\1_", " ".join(parts))
    m = re.search(r"(\w+)\s*\(", sig)
    if not m:
        return ""
    return f"uint32_t {m.group(1)}(uint32_t q0 = 0, ...);"

# Ghidra emits R5900 intrinsics and COP0 register reads as undeclared identifiers;
# stage them as inert host stubs in the decl header (real semantics land with the
# W11 port worker; trap/syscall must never execute on the host — W12 quarantines).
# syscall is NOT staged since W12: the shim declares the real dispatcher
# (guest_shim.h) and guest_kernel.cpp implements the host libkernel.
INTRINSIC = ["trap", "SQRT", "SYNC", "cacheOp", "_pcpyld", "ABS", "_pcpyud",
             "_psubb", "_pand", "_pxor", "_pextuw", "_pextlw", "_pcpyh", "_psubw",
             "EI", "DI", "_cfc2", "_ctc2", "_mips_gp0_value", "int3"]
COP0_VALUE = ["Status", "TagLo"]

def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("files", nargs="*", type=Path)
    ap.add_argument("--out", type=Path)
    ap.add_argument("--decls", action="store_true")
    ap.add_argument("--report", type=Path)
    a = ap.parse_args()

    fn_globals_all, dom = load_globals()
    named_data = load_named_data()
    all_files = sorted(FN.glob("*.c"))

    # pass 1: transliterate every TU once; a fn NAME defined in >1 TU (the game's
    # printf + a newlib copy) keeps the bare name at the lowest address and the
    # others are re-transliterated under an address-suffixed alias (no decl).
    report = new_report()
    res: dict[str, tuple[str, str, str]] = {}
    owner: dict[str, str] = {}
    aliased: set[str] = set()
    for f in all_files:
        g = fn_globals_all.get(f.stem, {})
        res[f.stem] = r = transliterate(f, report, g, dom, named_data)
        if r[2]:
            nm = decl_name(r[2])
            if nm in owner:
                report["dup_def"].append(f"{nm}:{owner[nm]}+{f.stem}")
                aliased.add(f.stem)
                res[f.stem] = transliterate(f, report, g, dom, named_data,
                                            def_alias=f"{nm}__{f.stem}")
            else:
                owner[nm] = f.stem

    if a.decls:
        decls: dict[str, str] = {}
        for f in all_files:
            d = res[f.stem][2] or synth_decl(f)
            if d and f.stem not in aliased:
                decls.setdefault(decl_name(d), d)
        regs = sorted(set(report["ee_reg"]))
        sym_names = set()
        for ln in (ROOT / "build/decomp/symbols.tsv").read_text().splitlines():
            p = ln.split("\t")
            if len(p) > 2 and p[2]:
                sym_names.add(p[2])
        staged = []
        for n in INTRINSIC:
            if n in sym_names or n in regs:
                report["stage_collision"].append(n)
                continue
            staged.append(f"    inline uint32_t {n}(uint32_t p0 = 0, uint32_t p1 = 0, "
                          f"uint32_t p2 = 0, uint32_t p3 = 0, ...) {{ return p0; }}\n")
        for n in COP0_VALUE:
            if n in sym_names or n in regs:
                report["stage_collision"].append(n)
                continue
            staged.append(f"    inline uint32_t {n} = 0;\n")
        out = ROOT / "build/decomp/guest_decls.inc"
        out.write_text("#pragma once\nnamespace guest {\n"
                       + "\n".join(sorted(decls.values())) + "\n"
                       + "".join(f"    inline uint32_t {r}{{}};  // EE MMIO staging (W12)\n"
                                 for r in regs)
                       + "".join(staged)
                       + "}\n")
        print(f"decls: {len(decls)} -> {out}, dup_def {len(aliased)}, ee_regs {len(regs)}, "
              f"staged {len(staged)}")
        if a.report:
            a.report.parent.mkdir(parents=True, exist_ok=True)
            a.report.write_text(json.dumps(report, indent=1))
        return

    files = a.files or all_files
    a.out.mkdir(parents=True, exist_ok=True)
    for f in files:
        addr, tu, _ = res[f.stem]
        if tu:
            (a.out / f"{addr}.cpp").write_text(tu)
    if a.report:
        a.report.parent.mkdir(parents=True, exist_ok=True)
        a.report.write_text(json.dumps(report, indent=1))
    n = sum(1 for f in files if (a.out / f"{f.stem}.cpp").exists())
    print(f"transliterated {n}/{len(files)} -> {a.out}; "
          f"structs {len(report['needs_struct'])}, raw-casts {len(report['raw_ptr_cast'])}, "
          f"unknown-type {len(report['unknown_type'])}, mmio {len(report['mmio_global'])}, "
          f"call-vars {len(report['call_var'])}, dup_def {len(report['dup_def'])}")

if __name__ == "__main__":
    main()
