import os
import re
import argparse

EXCLUDE_PATTERN = re.compile(r"(.*(?:__pycache__|cmake_build_debug|build|target|\.idea|\.venv|\.git|\.svn|\.vscode).*)")

def print_tree(directory, exclude_pattern=None, prefix="", output_file=None, file=None, only_ext=None):
    if not os.path.isdir(directory):
        msg = f"Error: '{directory}' is not a valid directory."
        (file.write(msg + "\n") if file else print(msg))
        return

    items = [item for item in os.listdir(directory) if not exclude_pattern.match(item)]

    for i, item in enumerate(items):
        path = os.path.join(directory, item)
        if output_file and os.path.abspath(path) == os.path.abspath(output_file):
            continue

        if only_ext and os.path.isfile(path) and not path.endswith(only_ext):
            continue

        line = f"{prefix}{'└──' if i == len(items) - 1 else '├──'} {item}"
        (file.write(line + "\n") if file else print(line))

        if os.path.isdir(path):
            print_tree(path, exclude_pattern, prefix + ("│   " if i != len(items) - 1 else "    "),
                       output_file, file, only_ext)

def is_binary_file(filepath):
    with open(filepath, 'rb') as f:
        return b'\0' in f.read(1024)

def print_file_content(filepath, output_file=None, file=None, only_ext=None):
    if only_ext and not filepath.endswith(only_ext):
        return
    if output_file and os.path.abspath(filepath) == os.path.abspath(output_file):
        return
    if is_binary_file(filepath):
        msg = f"Skipping binary file: {filepath}"
        (file.write(msg + "\n") if file else print(msg))
        return

    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
            header = f"--- Path: {filepath} ---\n"
            if file:
                file.write(header + content + "\n")
            else:
                print(header + content)
    except Exception as e:
        msg = f"Error reading file {filepath}: {e}"
        (file.write(msg + "\n") if file else print(msg))

def main():
    parser = argparse.ArgumentParser(description="Print directory structure and file contents.")
    parser.add_argument("path")
    parser.add_argument("output_file", nargs="?", default=None)
    parser.add_argument("--only", dest="only_ext", default=None)
    args = parser.parse_args()

    path = os.path.abspath(args.path)
    output_file = os.path.abspath(args.output_file) if args.output_file else None
    only_ext = args.only_ext if args.only_ext and args.only_ext.startswith('.') else args.only_ext

    if not os.path.isdir(path):
        print(f"Error: '{path}' is not a valid directory.")
        return

    if output_file:
        with open(output_file, 'w', encoding='utf-8') as file:
            file.write("Directory structure:\n")
            print_tree(path, EXCLUDE_PATTERN, output_file=output_file, file=file, only_ext=only_ext)
            file.write("\n")
            for root, _, files in os.walk(path):
                for file_name in files:
                    file_path = os.path.join(root, file_name)
                    if re.match(EXCLUDE_PATTERN, file_path) or os.path.abspath(file_path) == output_file:
                        continue
                    print_file_content(file_path, output_file=output_file, file=file, only_ext=only_ext)
    else:
        print("Directory structure:")
        print_tree(path, EXCLUDE_PATTERN, only_ext=only_ext)
        for root, _, files in os.walk(path):
            for file_name in files:
                file_path = os.path.join(root, file_name)
                if re.match(EXCLUDE_PATTERN, file_path):
                    continue
                print_file_content(file_path, only_ext=only_ext)

if __name__ == "__main__":
    main()
