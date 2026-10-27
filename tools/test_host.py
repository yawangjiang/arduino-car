"""Build and run the control tests using g++; hardware APIs are stubbed."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile
import os


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', default=shutil.which('g++'))
    args = parser.parse_args()
    if not args.compiler:
        parser.error('Install g++ or pass --compiler')
    root = Path(__file__).resolve().parents[1]
    sources = sorted(root.joinpath('tests').glob('*_test.cpp'))
    with tempfile.TemporaryDirectory(prefix='arduino-car-tests-') as build:
        for source in sources:
            executable = Path(build) / (source.stem + ('.exe' if os.name == 'nt' else ''))
            subprocess.run([args.compiler, '-std=c++11', '-Wall', '-Wextra', '-Werror',
                            '-I', str(root / 'tests'), str(source), '-o', str(executable)], check=True)
            subprocess.run([str(executable)], check=True)
    print(f'PASS: all {len(sources)} host test programs')


if __name__ == '__main__':
    main()
