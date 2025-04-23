import os
import re
import argparse

EXCLUDE_PATTERN = re.compile(r"(.*(?:__pycache__|cmake_build_debug|build|target|\.idea|\.venv|\.git|\.svn|\.vscode).*)")


def print_tree(directory, exclude_pattern=None, prefix="", output_file=None, file=None):
    if not os.path.isdir(directory):
        if file:
            file.write(f"Error: '{directory}' is not a valid directory.\n")
        else:
            print(f"Error: '{directory}' is not a valid directory.")
        return

    items = os.listdir(directory)
    items = [item for item in items if not exclude_pattern.match(item)]

    for i, item in enumerate(items):
        path = os.path.join(directory, item)
        if output_file and os.path.abspath(str(path)) == os.path.abspath(output_file):
            continue
        if i == len(items) - 1:
            if file:
                file.write(f"{prefix}└── {item}\n")
            else:
                print(f"{prefix}└── {item}")
        else:
            if file:
                file.write(f"{prefix}├── {item}\n")
            else:
                print(f"{prefix}├── {item}")
        if os.path.isdir(path):
            print_tree(path, exclude_pattern, prefix + ("│   " if i != len(items) - 1 else "    "), output_file, file)


def is_binary_file(filepath):
    with open(filepath, 'rb') as file:
        chunk = file.read(1024)
        if b'\0' in chunk:
            return True
    return False


def print_file_content(filepath, output_file=None, file=None):
    if output_file and os.path.abspath(filepath) == os.path.abspath(output_file):
        return

    if is_binary_file(filepath):
        if file:
            file.write(f"Skipping binary file: {filepath}\n")
        else:
            print(f"Skipping binary file: {filepath}")
        return

    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            if file:
                file.write(f"--- Path: {filepath} ---\n")
                file.write(f.read())
                file.write("\n")
            else:
                print(f"--- Path: {filepath} ---")
                print(f.read())
    except Exception as e:
        if file:
            file.write(f"Error reading file {filepath}: {e}\n")
        else:
            print(f"Error reading file {filepath}: {e}")


def main():
    parser = argparse.ArgumentParser(description="Print directory structure and file contents.")
    parser.add_argument("output_file", help="The path to the output file that will be ignored in the process.",
                        nargs='?', default=None)
    args = parser.parse_args()

    output_file = args.output_file

    if output_file:
        # Open the output file for writing with utf-8 encoding
        with open(output_file, 'w', encoding='utf-8') as file:
            file.write("Directory structure:\n")
            print_tree(os.getcwd(), EXCLUDE_PATTERN, output_file=output_file, file=file)
            file.write("\n")

            for root, dirs, files in os.walk(os.getcwd()):
                for file_name in files:
                    file_path = os.path.join(root, file_name)
                    if os.path.abspath(str(file_path)) == os.path.abspath(output_file):
                        continue
                    if re.match(EXCLUDE_PATTERN, str(file_path)):
                        continue
                    print_file_content(file_path, output_file=output_file, file=file)
    else:
        print("Directory structure:")
        print_tree(os.getcwd(), EXCLUDE_PATTERN)

        for root, dirs, files in os.walk(os.getcwd()):
            for file_name in files:
                file_path = os.path.join(root, file_name)
                if re.match(EXCLUDE_PATTERN, str(file_path)):
                    continue
                print_file_content(file_path)


if __name__ == "__main__":
    main()
