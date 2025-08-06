#!/usr/bin/env python3
"""
Run tsp_solver repeatedly with the combinations of parameters described in a
JSON configuration file.

The JSON schema is:

{
  "cmd": "--square-side 10000 --x-square 0 --y-square 0 --seconds 60 --nodes 1000",
  "tests": [
    {
      "algorithm": "grasp",
      "algorithm_flag": "--grasp",
      "parameters_to_test": [
        {"flag": "--p1", "csv_column": "p1", "values": [0.3, 0.5]},
        {"flag": "--p2", "csv_column": "p2", "values": [0.2, 0.4]}
      ]
    }
  ]
}
"""

from __future__ import annotations

import argparse
import shlex
import subprocess
import sys
from enum import Enum
from itertools import product
from pathlib import Path
from typing import Any, List

from pydantic import BaseModel, Field, ValidationError


# --------------------------------------------------------------------------- #
#  Pydantic models                                                            #
# --------------------------------------------------------------------------- #
class AlgorithmName(str, Enum):
    """Supported TSP algorithms."""
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
    # keep cmd as **string** so the JSON is easy to write,
    # but we'll split it once with shlex.split()
    cmd: str
    tests: List[AlgorithmTest] = Field(default_factory=list)


# --------------------------------------------------------------------------- #
#  Utility functions                                                          #
# --------------------------------------------------------------------------- #
def load_config(path: Path) -> TestsConfig:
    """Read *path* and parse/validate it as `TestsConfig`."""
    try:
        return TestsConfig.model_validate_json(path.read_text())
    except ValidationError as e:
        sys.exit(f"Error parsing config file {path!s}:\n{e}")


def build_calls(cfg: TestsConfig) -> list[list[str]]:
    """
    Build the full matrix of solver‐argument lists for *cfg*.

    Each “call” is suitable to pass directly to `subprocess.run(...)`.
    """
    base_tokens: list[str] = shlex.split(cfg.cmd)  # split ONCE here
    calls: list[list[str]] = []

    for test in cfg.tests:
        flags = [p.flag for p in test.parameters_to_test]
        test_values = [p.values for p in test.parameters_to_test]

        for combo in product(*test_values):
            # start with the global flags then the algorithm switch
            command: list[str] = [*base_tokens, test.algorithm_flag]

            # interleave parameter flags with the current value combination
            for flag, value in zip(flags, combo):
                command.extend([flag, str(value)])

            calls.append(command)

    return calls


# --------------------------------------------------------------------------- #
#  Main driver                                                                #
# --------------------------------------------------------------------------- #
def main() -> None:
    parser = argparse.ArgumentParser(
        description=(
            "Run tsp_solver over a parameter grid defined in a JSON "
            "configuration file."
        )
    )
    parser.add_argument(
        "config",
        nargs="?",
        default="config.json",
        help="path to the JSON configuration file (default: %(default)s)",
    )
    args = parser.parse_args()

    cfg_path = Path(args.config).expanduser()
    if not cfg_path.is_file():
        sys.exit(f"Error: cannot find config file {cfg_path!s}")

    cfg = load_config(cfg_path)

    # solver is expected at <project root>/build/tsp_solver/tsp_solver
    script_dir = Path(__file__).resolve().parent
    solver_path = script_dir.parent / "build" / "tsp_solver" / "tsp_solver"
    if not solver_path.is_file():
        sys.exit(f"Error: solver not found at {solver_path!s}")

    for argv_tail in build_calls(cfg):
        argv = [str(solver_path), *argv_tail]
        print("→", " ".join(argv))  # show the exact command we’re running

        ret = subprocess.run(argv, cwd=script_dir.parent)
        if ret.returncode:
            print(f"  …solver returned code {ret.returncode}, aborting.")
            sys.exit(ret.returncode)


if __name__ == "__main__":
    main()
