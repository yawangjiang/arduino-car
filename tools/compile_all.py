"""Compile every standalone sketch for UNO; keep build output outside the repository."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cli', default=shutil.which('arduino-cli'))
    parser.add_argument('--config', help='Optional Arduino CLI configuration file')
    args = parser.parse_args()
    if not args.cli:
        parser.error('Install arduino-cli or pass --cli')
    root = Path(__file__).resolve().parents[1]
    sketches = sorted([*root.joinpath('firmware').rglob('*.ino'),
                       *root.joinpath('experiments').rglob('*.ino')])
    for sketch in sketches:
        if sketch.stem != sketch.parent.name:
            raise SystemExit(f'Sketch/folder name mismatch: {sketch.relative_to(root)}')
    failed = []
    with tempfile.TemporaryDirectory(prefix='arduino-car-build-') as build_root:
        for index, sketch in enumerate(sketches):
            command = [args.cli]
            if args.config:
                command.extend(['--config-file', args.config])
            command.extend(['compile', '--fqbn', 'arduino:avr:uno', '--warnings', 'all',
                            '--build-path', str(Path(build_root) / str(index)), str(sketch.parent)])
            result = subprocess.run(command, capture_output=True, text=True, encoding='utf-8', errors='replace')
            relative = sketch.relative_to(root).as_posix()
            if result.returncode:
                failed.append(relative)
                print(f'FAIL: {relative}\n{result.stdout}\n{result.stderr}', flush=True)
            else:
                summary = ' | '.join(line for line in result.stdout.splitlines()
                                     if 'Sketch uses' in line or 'Global variables use' in line)
                print(f'PASS: {relative}\n{summary}', flush=True)
                # Surface warnings even when the build succeeds.
                if 'warning:' in result.stderr:
                    print(result.stderr, flush=True)
    print(f'{len(sketches) - len(failed)}/{len(sketches)} UNO sketches compiled', flush=True)
    return 1 if failed else 0


if __name__ == '__main__':
    raise SystemExit(main())
