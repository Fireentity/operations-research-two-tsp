#!/usr/bin/env python3
import argparse
import shlex
import subprocess
import sys
from enum import Enum
from itertools import product
from pathlib import Path
from typing import Any, List

from pydantic import BaseModel, Field, ValidationError


class AlgorithmName(Enum):
    """
    Enumeration of the TSP algorithms available for testing.
    """
    GRASP = "grasp"
    NEAREST_NEIGHBOR = "nearest_neighbor"
    TABU_SEARCH = "tabu_search"
    VARIABLE_NEIGHBORHOOD_SEARCH = "variable_neighborhood_search"


class TestParameter(BaseModel):
    flag: str
    csv_column: str
    values: List[Any] = Field(default_factory=list)


class AlgorithmTest(BaseModel):
    algorithm: AlgorithmName
    algorithm_flag: str
    parameters_to_test: List[TestParameter] = Field(default_factory=list)


class TestsConfig(BaseModel):
    cmd: List[str]
    tests: List[AlgorithmTest] = Field(default_factory=list)


def load_config(path: Path) -> TestsConfig:
    """
    Load the JSON config from path into a TestsConfig via Pydantic.
    """
    try:
        # Pydantic will read & validate nested models and enums automatically
        return TestsConfig.parse_file(str(path))
    except ValidationError as e:
        sys.exit(f"Error parsing config file {path!s}:\n{e}")


def build_calls(cfg: TestsConfig) -> List[List[str]]:
    """
    Given a TestsConfig, build the list of argv calls to run.
    """
    base: List[str] = cfg.cmd
    calls: List[List[str]] = []

    for test in cfg.tests:
        # e.g. "--grasp"
        algorithm_flag = test.algorithm_flag
        # flags: ["--iter", "--alpha", ...]
        flags = [p.flag for p in test.parameters_to_test]
        # values: [[10,100], [0.1,0.5], ...]
        test_values = [p.values for p in test.parameters_to_test]

        for combo in product(*test_values):
            # start with base + algorithm flag
            command = base + [algorithm_flag]
            # interleave flags and their chosen values
            for flag, value in zip(flags, combo):
                command.extend([flag, str(value)])
            calls.append(command)

    return calls


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Esegue tsp_solver su una matrice di parametri definita in un JSON di configurazione."
    )
    parser.add_argument(
        "config",
        nargs="?",
        default="config.json",
        help="percorso del file di configurazione JSON (default: %(default)s)"
    )
    args = parser.parse_args()

    cfg_path = Path(args.config).expanduser()
    if not cfg_path.is_file():
        sys.exit(f"Error: cannot find config file {cfg_path!s}")

    cfg = load_config(cfg_path)

    script_dir = Path(__file__).resolve().parent
    solver_path = script_dir.parent / "build" / "tsp_solver" / "tsp_solver"
    if not solver_path.is_file():
        sys.exit(f"Error: solver not found at {solver_path!s}")

    for argv_tail in build_calls(cfg):
        argv = [str(solver_path), *argv_tail]
        # stampa per verifica
        print("→", " ".join(shlex.quote(a) for a in argv))
        #ret = subprocess.run(argv, cwd=script_dir.parent)
        #if ret.returncode:
        #    print(f"  …solver returned non-zero code {ret.returncode}, aborting.")
        #    sys.exit(ret.returncode)


if __name__ == "__main__":
    main()
