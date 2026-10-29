#!/usr/bin/env python3
"""Run the Union particle ABI regression in isolated build directories."""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[3]
TEST_DIR = Path(__file__).resolve().parent
INSTRUMENT = TEST_DIR / "Union_particle_regression.instr"
EXAMPLE_SPECS = {
    "Test_logger_2DQ": {
        "path": ROOT / "mcstas-comps" / "examples" / "Tests_union" / "Test_logger_2DQ" / "Test_logger_2DQ.instr",
        "parameters": (),
        "required_outputs": ("PSD.dat", "Union_logger_2DQ.dat", "Union_logger_2DQ_con.dat"),
    },
    "Test_abs_logger_nD": {
        "path": ROOT / "mcstas-comps" / "examples" / "Tests_union" / "Test_abs_logger_nD" / "Test_abs_logger_nD.instr",
        "parameters": (),
        "required_outputs": ("PSD.dat", "Union_abs_logger_nD.dat", "Union_abs_logger_nD_con.dat"),
    },
    "Test_inhomogenous_process": {
        "path": ROOT / "mcstas-comps" / "examples" / "Tests_union" / "Test_inhomogenous_process" / "Test_inhomogenous_process.instr",
        "parameters": ("sample=inho_linear",),
        "required_outputs": ("lin_det.dat",),
    },
    "Mirror_validation": {
        "path": ROOT / "mcstas-comps" / "examples" / "Union_validation" / "Mirror_validation" / "Mirror_validation.instr",
        "parameters": ("angle=0.7",),
        "required_outputs": ("PSD.dat", "PSD_reflected.dat", "PSD_reflected_not_scattered.dat"),
    },
}
NUMERIC_METADATA = re.compile(
    r"^#\s*(?:type|Ncount|position|ratio|statistics|signal|values|xlimits|ylimits|zlimits|xylimits|limits):",
    re.IGNORECASE,
)
DATA_FILE = re.compile(r"\.dat(?:_\d+)?$")


def run(command: list[str], cwd: Path) -> None:
    print("+", shlex.join(command))
    subprocess.run(command, cwd=cwd, check=True, timeout=300)


def default_generator(flavour: str) -> str:
    env_name = "MCSTAS" if flavour == "mcstas" else "MCXTRACE"
    path = os.environ.get(env_name)
    if path:
        candidate = Path(path)
        if candidate.is_file():
            return str(candidate)
    executable = shutil.which(flavour)
    if executable:
        return executable
    raise SystemExit(f"cannot find {flavour}; pass --{flavour} PATH")


def default_resources(flavour: str, generator: str, explicit: str | None) -> Path:
    if explicit:
        return Path(explicit).resolve()
    env_names = ("MCSTAS" if flavour == "mcstas" else "MCXTRACE", "MXTRACE")
    for name in env_names:
        value = os.environ.get(name)
        if value and Path(value).is_dir():
            return Path(value).resolve()
    prefix = Path(generator).resolve().parents[1]
    return (prefix / "share" / flavour / "resources").resolve()


def compiler_command(value: str | None) -> list[str]:
    if value:
        return shlex.split(value)
    return shlex.split(os.environ.get("CC", "cc"))


def ncrystal_compiler_flags(generator: str, generated: Path) -> list[str]:
    if "NCrystal/ncrystal.h" not in generated.read_text(encoding="utf-8"):
        return []
    config = Path(generator).resolve().parent / "ncrystal-config"
    if not config.is_file():
        config = Path(shutil.which("ncrystal-config") or "")
    if config.is_file():
        result = subprocess.run([str(config), "--show", "buildflags"], check=True,
                                capture_output=True, text=True, timeout=60)
        return shlex.split(result.stdout.strip())
    raise SystemExit(f"NCrystal headers are required but ncrystal-config is unavailable for {generator}")


def _read_detector(path: Path) -> tuple[list[str], list[str], float, float]:
    """Read numeric detector metadata, payload, positive signal, and event count."""
    if not path.is_file():
        raise AssertionError(f"missing detector output: {path}")
    metadata: list[str] = []
    payload: list[str] = []
    for line in path.read_text(encoding="utf-8").splitlines():
        if line.startswith("#"):
            if NUMERIC_METADATA.match(line):
                metadata.append(line.strip())
        elif line.strip():
            payload.append(line.strip())
    if not payload:
        raise AssertionError(f"detector output has no numeric payload: {path}")
    positive_signal = 0.0
    event_count = 0.0
    for line in metadata:
        if not re.match(r"#\s*values:", line, re.IGNORECASE):
            continue
        values = line.split(":", 1)[1].split()
        try:
            if values:
                positive_signal = max(positive_signal, float(values[0]))
            if len(values) >= 3:
                event_count = max(event_count, float(values[2]))
        except ValueError:
            continue
    if not event_count:
        numeric_payload: list[float] = []
        for line in payload:
            for token in line.split():
                try:
                    numeric_payload.append(float(token))
                except ValueError:
                    pass
        positive_signal = max(positive_signal, max((abs(value) for value in numeric_payload), default=0.0))
        if positive_signal:
            event_count = 1.0
    return metadata, payload, positive_signal, event_count


def detector_signature(path: Path) -> tuple[list[str], list[str]]:
    """Return numeric metadata and payload, excluding paths, dates, and parameters."""
    metadata, payload, _positive_signal, _event_count = _read_detector(path)
    return metadata, payload


def detector_files(run_dir: Path) -> list[Path]:
    return sorted(path for path in run_dir.rglob("*") if path.is_file() and DATA_FILE.search(path.name))


def detector_bundle(run_dir: Path) -> dict[str, tuple[list[str], list[str]]]:
    paths = detector_files(run_dir)
    if not paths:
        raise AssertionError(f"no detector outputs found in {run_dir}")
    return {path.relative_to(run_dir).as_posix(): detector_signature(path) for path in paths}


def verify_detector_activity(run_dir: Path, required_outputs: tuple[str, ...] = ()) -> None:
    paths = {path.name: path for path in detector_files(run_dir)}
    for name in required_outputs:
        if name not in paths:
            raise AssertionError(f"missing required detector output {name} in {run_dir}")
        _metadata, _payload, positive_signal, event_count = _read_detector(paths[name])
        if positive_signal <= 0 or event_count <= 0:
            raise AssertionError(f"required detector has no events or positive signal: {paths[name]}")
    active = [_read_detector(path)[2:] for path in paths.values()]
    if not any(positive_signal > 0 and event_count > 0 for positive_signal, event_count in active):
        raise AssertionError(f"detectors have no positive event data in {run_dir}")


def require_file(run_dir: Path, name: str) -> str:
    path = run_dir / name
    if not path.is_file():
        raise AssertionError(f"missing regression output: {path}")
    return path.read_text(encoding="utf-8")


def verify_callback_logs(run_dir: Path, safe_mode: int) -> None:
    ordinary_corrupt = require_file(run_dir, "ordinary_corrupt.log")
    ordinary_observer = require_file(run_dir, "ordinary_observer.log")
    abs_corrupt = require_file(run_dir, "abs_corrupt.log")
    abs_observer = require_file(run_dir, "abs_observer.log")
    sequence = require_file(run_dir, "union_particle_sequence.log")

    if "before " not in ordinary_corrupt or "after " not in ordinary_corrupt:
        raise AssertionError("ordinary logger did not record before and after particles")
    if "absorbed_weight=" not in abs_corrupt or "event " not in abs_corrupt:
        raise AssertionError("absorption logger did not record event weight and particle")
    if safe_mode == 0 and ordinary_corrupt == ordinary_observer:
        raise AssertionError("unsafe logger callback did not reach the subsequent observer")
    if safe_mode == 1 and ordinary_corrupt != ordinary_observer:
        raise AssertionError("safe logger callback changed the subsequent observer input")
    if safe_mode == 0 and abs_corrupt == abs_observer:
        raise AssertionError("unsafe absorption callback did not reach the subsequent observer")
    if safe_mode == 1 and abs_corrupt != abs_observer:
        raise AssertionError("safe absorption callback changed the subsequent observer input")

    ordinary_records = [line for line in ordinary_corrupt.splitlines() if line.startswith(("before ", "after "))]
    if len(ordinary_records) < 2 or len(ordinary_records) % 2:
        raise AssertionError("ordinary logger did not record complete before/after pairs")
    for before, after in zip(ordinary_records[::2], ordinary_records[1::2]):
        before_match = re.search(r"^before x=(\S+) y=(\S+) z=(\S+).* t=(\S+).* marker=(\S+)", before)
        after_match = re.search(r"^after x=(\S+) y=(\S+) z=(\S+).* t=(\S+).* marker=(\S+)", after)
        if not before_match or not after_match:
            raise AssertionError("ordinary logger dump is missing position, time, or marker")
        if before_match.group(5) != after_match.group(5) or before_match.group(5) != "0x1.52p+5":
            raise AssertionError("ordinary logger did not preserve the particle marker")
        if before_match.groups()[:4] != after_match.groups()[:4]:
            raise AssertionError("ordinary logger changed particle position or time")

    absorption_records = []
    pending_weight = None
    for line in abs_corrupt.splitlines():
        if line.startswith("absorbed_weight="):
            try:
                pending_weight = float.fromhex(line.split("=", 1)[1])
            except ValueError as error:
                raise AssertionError("absorption logger wrote an invalid absorbed weight") from error
        elif line.startswith("event "):
            event_match = re.search(r"^event x=(\S+) y=(\S+) z=(\S+).* t=(\S+) p=(\S+).* marker=(\S+)", line)
            if not event_match or pending_weight is None:
                raise AssertionError("absorption logger did not pair event and absorbed weight")
            try:
                full_weight = float.fromhex(event_match.group(5))
            except ValueError as error:
                raise AssertionError("absorption logger wrote an invalid full weight") from error
            if full_weight < pending_weight or pending_weight <= 0 or full_weight <= 0:
                raise AssertionError("absorption logger did not preserve full weight and positive absorbed weight")
            if event_match.group(6) != "0x1.52p+5":
                raise AssertionError("absorption logger did not preserve the particle marker")
            absorption_records.append((full_weight, pending_weight))
            pending_weight = None
    if pending_weight is not None or not absorption_records:
        raise AssertionError("absorption callbacks did not receive positive absorbed weight")
    if "PROCESS_SCATTER" not in sequence:
        raise AssertionError("test process never scattered")
    if "ABS_OBSERVER" not in sequence:
        raise AssertionError("absorption callback never ran")
    first_scatter = sequence.index("PROCESS_SCATTER")
    if "ABS_" not in sequence[:first_scatter]:
        raise AssertionError("absorption was not logged before the first scattering callback")
    if sequence.count("PROCESS_SCATTER") == 0:
        raise AssertionError("rotated test process produced no scattering events")


def build_and_run(generator: str, resources: Path, compiler: list[str], work_dir: Path,
                  safe_mode: int, ncount: int, seed: int, flavour: str, bad_process: int = 0) -> Path:
    run_dir = work_dir / f"{flavour}-safe-{safe_mode}-bad-{bad_process}"
    run_dir.mkdir()
    source = run_dir / "Union_particle_regression.c"
    executable = run_dir / "Union_particle_regression"
    component_root = ROOT / f"{flavour}-comps"
    search_dirs = [
        TEST_DIR,
        component_root / "share",
        component_root / "union",
        component_root / "monitors",
        component_root / "optics",
        component_root,
        resources,
    ]
    command = [generator, "-o", str(source)]
    for directory in search_dirs:
        command.extend(["-I", str(directory)])
    command.append(str(INSTRUMENT))
    run(command, run_dir)
    run(compiler + ["-std=c99", "-O0", "-g", "-o", str(executable), str(source), "-lm"], run_dir)
    run([str(executable), "--ncount", str(ncount), "--seed", str(seed),
         f"safe_mode={safe_mode}", f"bad_process={bad_process}"], run_dir)
    verify_callback_logs(run_dir, safe_mode)
    if not require_file(run_dir, "final_state.log").strip():
        raise AssertionError("no particles reached the final-state observer")
    verify_detector_activity(run_dir, ("detector.dat",))
    return run_dir


def _mask_comments_and_strings(source: str) -> str:
    masked = list(source)
    state = "normal"
    index = 0
    while index < len(source):
        char = source[index]
        next_char = source[index + 1] if index + 1 < len(source) else ""
        if state == "normal":
            if char == "/" and next_char == "/":
                masked[index] = masked[index + 1] = " "
                state = "line_comment"
                index += 2
                continue
            if char == "/" and next_char == "*":
                masked[index] = masked[index + 1] = " "
                state = "block_comment"
                index += 2
                continue
            if char == '"':
                masked[index] = " "
                state = "string"
            elif char == "'":
                masked[index] = " "
                state = "char"
        elif state == "line_comment":
            if char == "\n":
                state = "normal"
            else:
                masked[index] = " "
        elif state == "block_comment":
            if char == "*" and next_char == "/":
                masked[index] = masked[index + 1] = " "
                state = "normal"
                index += 2
                continue
            if char != "\n":
                masked[index] = " "
        else:
            if char == "\\":
                masked[index] = " "
                if index + 1 < len(source):
                    masked[index + 1] = " "
                    index += 2
                    continue
            elif (state == "string" and char == '"') or (state == "char" and char == "'"):
                masked[index] = " "
                state = "normal"
            elif char != "\n":
                masked[index] = " "
        index += 1
    return "".join(masked)


def instrument_with_safe_mode(source: str, safe_mode: int) -> str:
    masked = _mask_comments_and_strings(source)
    calls: list[tuple[int, int]] = []
    for match in re.finditer(r"\bUnion_master\s*\(", masked):
        opening = masked.find("(", match.start(), match.end())
        depth = 1
        index = opening + 1
        while index < len(masked) and depth:
            if masked[index] == "(":
                depth += 1
            elif masked[index] == ")":
                depth -= 1
            index += 1
        if depth:
            raise AssertionError("unterminated Union_master invocation")
        calls.append((opening, index - 1))
    if not calls:
        raise AssertionError("example contains no CPU Union_master invocation")

    instrumented = source
    for opening, closing in reversed(calls):
        arguments = instrumented[opening + 1:closing]
        safe_parameter = re.search(r"\bsafe_mode\s*=\s*[^,)]*", arguments)
        if safe_parameter:
            start = opening + 1 + safe_parameter.start()
            end = opening + 1 + safe_parameter.end()
            instrumented = instrumented[:start] + f"safe_mode={safe_mode}" + instrumented[end:]
        else:
            separator = ", " if arguments.strip() else ""
            instrumented = instrumented[:opening + 1] + f"safe_mode={safe_mode}{separator}" + instrumented[opening + 1:]
    return instrumented


def stage_input_assets(source: Path, run_dir: Path, component_root: Path, resources: Path) -> None:
    names = set(re.findall(r"[A-Za-z0-9_./-]+\.(?:laz|ncmat|off|xray|xml|txt)", source.read_text(encoding="utf-8"), re.IGNORECASE))
    for name in names:
        basename = Path(name).name
        candidates = [source.parent / name, component_root / name, component_root / "data" / basename,
                      resources / name, resources / "data" / basename]
        asset = next((candidate for candidate in candidates if candidate.is_file()), None)
        if asset is None:
            for root in (component_root, resources):
                matches = list(root.rglob(basename))
                if matches:
                    asset = matches[0]
                    break
        if asset is None:
            continue
        destination = run_dir / basename
        if not destination.exists():
            shutil.copy2(asset, destination)


def build_and_run_example(generator: str, resources: Path, compiler: list[str], work_dir: Path,
                          safe_mode: int, ncount: int, seed: int, example_name: str) -> Path:
    spec = EXAMPLE_SPECS[example_name]
    original = spec["path"]
    if not original.is_file():
        raise AssertionError(f"missing example instrument: {original}")
    run_dir = work_dir / "examples" / f"{example_name}-safe-{safe_mode}"
    run_dir.mkdir(parents=True)
    source = run_dir / original.name
    source.write_text(instrument_with_safe_mode(original.read_text(encoding="utf-8"), safe_mode), encoding="utf-8")
    component_root = ROOT / "mcstas-comps"
    stage_input_assets(original, run_dir, component_root, resources)
    search_dirs = [
        TEST_DIR,
        component_root / "share",
        component_root / "union",
        component_root / "monitors",
        component_root / "optics",
        component_root,
        original.parent,
        run_dir,
        resources,
        resources / "data",
    ]
    generated = run_dir / f"{original.stem}.c"
    executable = run_dir / original.stem
    command = [generator, "-o", str(generated)]
    for directory in search_dirs:
        command.extend(["-I", str(directory)])
    command.append(str(source))
    run(command, run_dir)
    compile_command = compiler + ncrystal_compiler_flags(generator, generated)
    compile_command += ["-std=c99", "-O2", "-g", "-o", str(executable), str(generated), "-lm"]
    run(compile_command, run_dir)
    run([str(executable), "--ncount", str(ncount), "--seed", str(seed), *spec["parameters"]], run_dir)
    verify_detector_activity(run_dir, spec["required_outputs"])
    return run_dir


def compare_runs(run_zero: Path, run_one: Path) -> None:
    state_zero_path = run_zero / "final_state.log"
    state_one_path = run_one / "final_state.log"
    if state_zero_path.exists() or state_one_path.exists():
        state_zero = require_file(run_zero, "final_state.log")
        state_one = require_file(run_one, "final_state.log")
        if state_zero != state_one:
            raise AssertionError("safe_mode changed the exact final particle state dump")
    detector_zero = detector_bundle(run_zero)
    detector_one = detector_bundle(run_one)
    if detector_zero != detector_one:
        raise AssertionError("safe_mode changed detector metadata or numeric payload")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--mcstas", help="McStas generator executable")
    group.add_argument("--mcxtrace", help="McXtrace generator executable")
    parser.add_argument("--resources", help="component resources directory")
    parser.add_argument("--workdir", help="temporary parent directory (never the repository)")
    parser.add_argument("--compiler", help="C compiler command, optionally including flags")
    parser.add_argument("--ncount", type=int, default=64)
    parser.add_argument("--example-ncount", type=int, default=5_000_000)
    parser.add_argument("--examples", nargs="*", metavar="NAME",
                        help="McStas examples to run; omit names for all defaults")
    parser.add_argument("--no-examples", action="store_true", help="skip the McStas example pairs")
    parser.add_argument("--seed", type=int, default=1729)
    args = parser.parse_args()

    flavour = "mcxtrace" if args.mcxtrace else "mcstas"
    generator = args.mcxtrace or args.mcstas or default_generator(flavour)
    generator = shutil.which(generator) or generator
    resources = default_resources(flavour, generator, args.resources)
    if not resources.is_dir():
        raise SystemExit(f"resources directory does not exist: {resources}")
    compiler = compiler_command(args.compiler)
    work_parent = Path(args.workdir).resolve() if args.workdir else None
    if work_parent and not work_parent.is_dir():
        raise SystemExit(f"workdir does not exist: {work_parent}")

    with tempfile.TemporaryDirectory(prefix="union-particle-", dir=work_parent) as temp:
        work_dir = Path(temp)
        run_zero = build_and_run(generator, resources, compiler, work_dir, 0, args.ncount, args.seed, flavour)
        run_one = build_and_run(generator, resources, compiler, work_dir, 1, args.ncount, args.seed, flavour)
        compare_runs(run_zero, run_one)
        run_bad = build_and_run(generator, resources, compiler, work_dir, 1, args.ncount, args.seed, flavour, bad_process=1)
        compare_runs(run_one, run_bad)
        print(f"custom union particle regression passed for {flavour}: 3 runs, exact safe_mode equality")
        print("safe mode also rejected forbidden process writes without changing outgoing state or RNG")
        if flavour == "mcstas" and not args.no_examples:
            requested = [item for value in (args.examples or []) for item in value.split(",") if item]
            example_names = requested or list(EXAMPLE_SPECS)
            unknown = sorted(set(example_names) - set(EXAMPLE_SPECS))
            if unknown:
                raise SystemExit(f"unknown McStas example(s): {', '.join(unknown)}")
            for example_name in example_names:
                example_zero = build_and_run_example(generator, resources, compiler, work_dir, 0,
                                                      args.example_ncount, args.seed, example_name)
                example_one = build_and_run_example(generator, resources, compiler, work_dir, 1,
                                                     args.example_ncount, args.seed, example_name)
                compare_runs(example_zero, example_one)
                print(f"example {example_name} passed: safe_mode=0 == safe_mode=1")
            print(f"McStas example regression passed: {len(example_names)} exact safe-mode pairs")
        elif args.examples and flavour != "mcstas":
            raise SystemExit("--examples is only supported with --mcstas")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
