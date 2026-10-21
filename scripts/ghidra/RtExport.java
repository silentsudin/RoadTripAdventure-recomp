// Road Trip: export per-function pseudocode and a call graph (run by scripts/ghidra/decompile.py).
//
// Arguments: <out dir> [only: comma-separated hex addresses, re-export just these]
//   <out dir>/fn/<addr>.c     pseudocode, one file per function (addr = 8 hex digits, lower case)
//   <out dir>/callgraph.json  {"0x...": {"name", "end", "callers": [...], "callees": [...]}}
//@category RoadTrip

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;

import ghidra.app.decompiler.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.pcode.*;
import ghidra.program.model.symbol.Symbol;

public class RtExport extends GhidraScript {

    private static String hex(Address a) {
        return String.format("%08x", a.getOffset());
    }

    private static String js(String s) {
        StringBuilder b = new StringBuilder("\"");
        for (char c : s.toCharArray()) {
            if (c == '"' || c == '\\') b.append('\\').append(c);
            else if (c < 0x20) b.append(String.format("\\u%04x", (int) c));
            else b.append(c);
        }
        return b.append('"').toString();
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        Path out = Paths.get(args[0]);
        Path fnDir = out.resolve("fn");
        Files.createDirectories(fnDir);
        Set<Long> only = null;
        if (args.length > 1 && !args[1].isEmpty()) {
            only = new HashSet<>();
            for (String s : args[1].split(",")) only.add(Long.decode(s.trim()));
        }

        DecompInterface dec = new DecompInterface();
        DecompileOptions opts = new DecompileOptions();
        dec.setOptions(opts);
        dec.toggleCCode(true);
        dec.toggleSyntaxTree(true);
        dec.setSimplificationStyle("decompile");
        if (!dec.openProgram(currentProgram)) throw new RuntimeException("decompiler: " + dec.getLastMessage());

        FunctionManager fm = currentProgram.getFunctionManager();
        Path globalsPath = out.resolve("globals.tsv");
        BufferedWriter globals = new BufferedWriter(Files.newBufferedWriter(globalsPath, StandardCharsets.UTF_8));
        // fnaddr  varname  register  size  -- callee-saved storage the decompiler
        // attributes to a variable (s0..s7/fp, f20..f31). The gen layer binds these
        // names to the shadow register file so machine sd/swc1 spill stores carry the
        // caller's values (see scripts/native/emit_prologue_saves.py).
        Path regsPath = out.resolve("regstore.tsv");
        BufferedWriter regw = new BufferedWriter(Files.newBufferedWriter(regsPath, StandardCharsets.UTF_8));
        StringBuilder graph = new StringBuilder("{\n");
        int n = 0, failed = 0;
        boolean first = true;
        for (Function f : fm.getFunctions(true)) {
            monitor.checkCancelled();
            if (f.isExternal() || f.isThunk() && f.getBody().isEmpty()) continue;
            Address ep = f.getEntryPoint();

            if (only == null || only.contains(ep.getOffset())) {
                DecompileResults r = dec.decompileFunction(f, 120, monitor);
                String code;
                if (r != null && r.decompileCompleted() && r.getDecompiledFunction() != null) {
                    code = r.getDecompiledFunction().getC();
                    // What type the decompiler used for each global in THIS function:
                    // fnaddr  symaddr  name  type  size. The transliterator types each
                    // bare DAT_/named global site by this, not by a global guess.
                    try {
                        java.util.Iterator<HighSymbol> gs = r.getHighFunction().getGlobalSymbolMap().getSymbols();
                        while (gs.hasNext()) {
                            HighSymbol hs = gs.next();
                            Symbol s = hs.getSymbol();
                            if (s == null || hs.getDataType() == null) continue;
                            globals.write(hex(ep) + "\t" + hex(s.getAddress()) + "\t"
                                    + hs.getName() + "\t" + hs.getDataType().getName() + "\t" + hs.getSize());
                            globals.newLine();
                        }
                    } catch (Exception ge) {
                        println("RtExport: globals skipped at 0x" + hex(ep) + ": " + ge);
                    }
                    try {
                        HashSet<String> seen = new HashSet<>();
                        java.util.Iterator<HighSymbol> ls = r.getHighFunction().getLocalSymbolMap().getSymbols();
                        while (ls.hasNext()) {
                            HighSymbol hs = ls.next();
                            VariableStorage st = hs.getStorage();
                            if (st == null || !st.isRegisterStorage() || st.getRegister() == null) continue;
                            String reg = st.getRegister().getName();
                            reg = reg.toLowerCase();
                            if (!(reg.matches("s[0-7]") || reg.equals("fp")
                                    || reg.matches("f(2[0-9]|3[01])"))) continue;
                            if (!seen.add(hs.getName() + "\t" + reg)) continue;
                            regw.write(hex(ep) + "\t" + hs.getName() + "\t" + reg + "\t" + hs.getSize());
                            regw.newLine();
                        }
                    } catch (Exception re) {
                        println("RtExport: regstore skipped at 0x" + hex(ep) + ": " + re);
                    }
                } else {
                    code = "/* decompilation failed: " + (r == null ? "?" : r.getErrorMessage()) + " */\n";
                    failed++;
                }
                String head = "// " + f.getName() + " @ 0x" + hex(ep) + " .. 0x" + hex(f.getBody().getMaxAddress()) + "\n";
                Files.write(fnDir.resolve(hex(ep) + ".c"), (head + code).getBytes(StandardCharsets.UTF_8));
                n++;
            }

            if (!first) graph.append(",\n");
            first = false;
            graph.append("  ").append(js("0x" + hex(ep))).append(": {\"name\": ").append(js(f.getName()));
            graph.append(", \"end\": ").append(js("0x" + hex(f.getBody().getMaxAddress())));
            graph.append(", \"callers\": [");
            boolean c1 = true;
            TreeSet<String> callers = new TreeSet<>();
            for (Function c : f.getCallingFunctions(monitor)) callers.add("0x" + hex(c.getEntryPoint()));
            for (String c : callers) { if (!c1) graph.append(", "); c1 = false; graph.append(js(c)); }
            graph.append("], \"callees\": [");
            c1 = true;
            TreeSet<String> callees = new TreeSet<>();
            for (Function c : f.getCalledFunctions(monitor)) callees.add("0x" + hex(c.getEntryPoint()));
            for (String c : callees) { if (!c1) graph.append(", "); c1 = false; graph.append(js(c)); }
            graph.append("]}");
        }
        graph.append("\n}\n");
        Files.write(out.resolve("callgraph.json"), graph.toString().getBytes(StandardCharsets.UTF_8));
        globals.close();
        regw.close();
        dec.dispose();
        println("RtExport: " + n + " functions written (" + failed + " failed) to " + out);
    }
}
