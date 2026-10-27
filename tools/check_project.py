"""Check local documentation, image provenance, sketch names, and report datasets."""
from pathlib import Path
import csv
import hashlib
import json
import re


def require(condition, message):
    if not condition:
        raise SystemExit(message)


def rows(path):
    with path.open(encoding='utf-8', newline='') as handle:
        return list(csv.DictReader(handle))


def main():
    root = Path(__file__).resolve().parents[1]
    links = 0
    for path in root.rglob('*.md'):
        text = path.read_text(encoding='utf-8')
        require(text.count('```') % 2 == 0, f'Unclosed code fence: {path.relative_to(root)}')
        require(not re.search(r'(?<![A-Za-z])[A-Za-z]:[\\/]', text), 'Machine-specific path in documentation')
        for target in re.findall(r'!?\[[^\]]*\]\(([^)]+)\)', text):
            if target.startswith(('https://', 'http://', '#')):
                continue
            name, _, anchor = target.partition('#')
            destination = (path.parent / name).resolve()
            require(destination.is_relative_to(root), f'Link outside project: {target}')
            require(destination.is_file(), f'Broken link: {path.relative_to(root)} -> {target}')
            if anchor:
                headings = re.findall(r'^#+\s+(.+?)\s*$', destination.read_text(encoding='utf-8'), re.M)
                ids = [re.sub(r'[^\w\-\s]', '', heading.lower()).replace(' ', '-') for heading in headings]
                require(anchor in ids, f'Broken anchor: {target}')
            links += 1

    manifest = json.loads((root / 'media/manifest.json').read_text(encoding='utf-8'))
    seen = set()
    for entry in manifest['images']:
        path = (root / 'media' / entry['path']).resolve()
        require(path.is_relative_to(root / 'media'), 'Image path outside media')
        require(entry['path'] not in seen, 'Duplicate image provenance entry')
        seen.add(entry['path'])
        require(hashlib.sha256(path.read_bytes()).hexdigest() == entry['sha256'], f'Image changed: {entry["path"]}')
    actual = {path.relative_to(root / 'media').as_posix() for path in (root / 'media').rglob('*')
              if path.suffix.lower() in ('.png', '.jpg')}
    require(actual == seen and len(seen) == 16, 'Image provenance inventory differs')

    sketches = [*root.joinpath('firmware').rglob('*.ino'), *root.joinpath('experiments').rglob('*.ino')]
    require(len(sketches) == 13, 'Expected 13 classified sketches')
    for path in sketches:
        require(path.stem == path.parent.name, f'Sketch/folder name mismatch: {path.name}')

    ldr = rows(root / 'data/ldr/measurements.csv')
    require(len(ldr) == 240, 'Expected 240 LDR samples')
    keys = {(r['sensor'], r['fixed_resistor_ohm'], r['connection_label'], r['illumination_label'], r['trial']) for r in ldr}
    require(len(keys) == 240, 'Duplicate LDR sample')
    require(all(0 <= int(r['adc']) <= 1023 for r in ldr), 'Invalid LDR ADC value')
    ir = rows(root / 'data/infrared/height_response.csv')
    require(len(ir) == 10, 'Expected 10 height groups')
    for row in ir:
        mean = sum(float(row[f'trial_{i}_adc']) for i in range(1, 4)) / 3
        require(abs(mean - float(row['reported_mean_adc'])) < 0.051, 'IR mean does not match trials')
    motor = rows(root / 'data/motor/speed_trials.csv')
    require(len(motor) == 8, 'Expected 8 speed groups')
    require(all(not r['confirmed_distance_m'] for r in motor), 'Unconfirmed distance must remain empty')
    print(f'PASS: {links} local links, 16 image hashes, 13 sketch names, 240 LDR samples, 10 IR groups, 8 motor groups')


if __name__ == '__main__':
    main()
