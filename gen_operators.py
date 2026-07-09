import os, json

outdir = r'C:\Users\me\Desktop\ai-aimassist-source\operator_icons'
with open(os.path.join(outdir, 'operators.json')) as f:
    meta = json.load(f)

lines = ['#pragma once', '', '// Auto-generated operator icons', '']

for op_id in sorted(meta.keys()):
    png_path = os.path.join(outdir, f'{op_id}.png')
    with open(png_path, 'rb') as f:
        data = f.read()
    lines.append(f'inline unsigned char icon_{op_id}[] = {{')
    for i in range(0, len(data), 12):
        chunk = data[i:i+12]
        hex_vals = ', '.join(f'0x{b:02X}' for b in chunk)
        if i + 12 < len(data):
            lines.append(f'\t{hex_vals},')
        else:
            lines.append(f'\t{hex_vals}')
    lines.append('};')
    lines.append(f'inline int icon_{op_id}_size = {len(data)};')
    lines.append('')

lines.append('struct RecoilPattern {')
lines.append('    const char* name;')
lines.append('    const char* weapon;')
lines.append('    float vertical;')
lines.append('    float horizontal;')
lines.append('    float burst_comp;')
lines.append('};')
lines.append('')
lines.append('inline RecoilPattern recoil_data[] = {')

for op_id in sorted(meta.keys()):
    m = meta[op_id]
    lines.append(f'    {{"{m["name"]}", "{m["weapon"]}", {m["v_recoil"]}f, {m["h_recoil"]}f, 0.1f}},')

lines.append('};')
lines.append(f'inline int recoil_data_count = {len(meta)};')

lines.append('')
lines.append('inline const char* operator_ids[] = {')
for op_id in sorted(meta.keys()):
    lines.append(f'    "{op_id}",')
lines.append('};')

header = '\n'.join(lines)
with open(r'C:\Users\me\Desktop\ai-aimassist-source\example_win32_directx11\aimbot\operators.h', 'w') as f:
    f.write(header)

print(f'Generated operators.h ({len(lines)} lines)')
