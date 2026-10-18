"""Read-only W3-05 ELF proof; no extracted ROM is written. Requires capstone."""
import pathlib
import struct
import capstone

rom = pathlib.Path.home() / 'Library/Application Support/RoadTripRecomp/disc/SLUS_203.98'
if not rom.is_file():
    print('NO_ELF SKIP')
    raise SystemExit(0)
b = rom.read_bytes()
print(f'ROM bytes={len(b)}')
md = capstone.Cs(capstone.CS_ARCH_MIPS,
                 capstone.CS_MODE_MIPS64 | capstone.CS_MODE_BIG_ENDIAN)
for address in (0x23B140, 0x23B144, 0x20A710, 0x20A724, 0x20A72C,
                0x20A740, 0x20A748, 0x20A750, 0x20A780, 0x20A784,
                0x20A78C, 0x20A798, 0x20A7A0, 0x20A7B4,
                0x20A7E0, 0x20A7E4, 0x20A7F0, 0x20A7F8,
                0x21CE48, 0x21CE94, 0x21CEA4, 0x21CF00, 0x21CF20):
    raw = b[address - 0x1FF000:address - 0x1FF000 + 4][::-1]
    ins = next(md.disasm(raw, address))
    print(f'{address:08X} {ins.mnemonic} {ins.op_str}')
for i in range(8):
    address = struct.unpack_from('<I', b, 0x29AD60 - 0x1FF000 + 4*i)[0]
    # Print typed records, not a byte dump. The test extracts the live bytes.
    offset = 0
    records = []
    while offset < 8:
        op = b[address - 0x1FF000 + offset]
        width = 2 if op == 0x82 else 1
        records.append(f'{address+offset:08X}:op={op:02X}/w={width}')
        offset += width
        if op == 0xFF:
            break
    print(f'pattern={i} ' + ' '.join(records))
# GP is carried explicitly in ELF .reginfo. Do not use .data+0x7ff0:
# this ELF has a separate .sdata section and that shortcut is false here.
gp = struct.unpack_from('<I', b, 0x29A830 - 0x1FF000 + 20)[0]
print(f'ELF .reginfo gp=0x{gp:08X}')
