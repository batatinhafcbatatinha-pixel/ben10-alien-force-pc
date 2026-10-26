"""Run existing PS2Recomp without downloading/building it; cap generated output."""
from pathlib import Path
import hashlib
import json
import os
import subprocess
import time

root = Path(__file__).resolve().parents[1]
tool = Path(os.environ.get('PS2_RECOMP_EXE', str(root.parent / 'codwaw-ff/build/ps2recomp/ps2xRecomp/ps2_recomp.exe')))
out = root / 'project/runner'
stamp = out / '.generation-complete.json'
if not tool.is_file():
    raise SystemExit('Set PS2_RECOMP_EXE to an existing ps2_recomp.exe')
if not (root / 'config/ghidra-functions.csv').is_file():
    raise SystemExit('Run ghidra-export.cmd and scripts/prepare_config.py first')
stamp.unlink(missing_ok=True)
log = root / 'analysis/recompile.log'
with log.open('w') as stream:
    proc = subprocess.Popen([str(tool), 'config/recomp.toml'], cwd=root, stdout=stream, stderr=subprocess.STDOUT)
    try:
        while proc.poll() is None:
            size = sum(p.stat().st_size for p in out.rglob('*') if p.is_file()) if out.exists() else 0
            if size > 700 * 1024**2:
                proc.terminate()
                proc.wait()
                raise SystemExit('Generation stopped: 700 MiB output limit reached; output is incomplete')
            time.sleep(0.5)
    finally:
        if proc.poll() is None:
            proc.terminate()
            proc.wait()
if proc.returncode == 0:
    if not (out / 'register_functions.cpp').is_file():
        raise SystemExit('Generator exited successfully without registration source; incomplete output')
    stamp.write_text(json.dumps({'config_sha256': hashlib.sha256((root / 'config/recomp.toml').read_bytes()).hexdigest(),
                                 'cpp_files': len(list(out.glob('*.cpp')))}, indent=2) + '\n')
raise SystemExit(proc.returncode)
