import os
import json
import argparse
import subprocess as sp
from README_style_checker import main as rmsc

# Category folder names in current sample viewer
# Checks only run when a file path is within one of these category folders
categories = {
    'Analysis',
    'AR',
    'CloudAndPortal',
    'DisplayInformation',
    'EditData',
    'Features',
    'Geometry',
    'Layers',
    'LocalServer',
    'Maps',
    'Routing',
    'Scenes',
    'Search',
    'UtilityNetwork'
}

def run_mdl(readme_path: str): # Run markdown linter
    print("**** mdl ****")
    return sp.call(f'mdl --style /style.rb "{readme_path}', shell=True)

def run_style_check(dirname: str):
    print("**** README_style_checker ****")
    # code1 = rmsc(dirname)
    code1 = sp.call(f'python3 /README_style_checker.py -s "{dirname}"', shell=True)

    print("**** title_differ ****")
    code2 = sp.call(f'python3 /title_differ.py -s "{dirname}"', shell=True)

    print("**** description_differ ****")
    code3 = sp.call(f'python3 /description_differ.py -s "{dirname}"', shell=True)

    print("**** metadata_style_checker ****")
    code4 = sp.call(f'python3 /metadata_style_checker.py -s "{dirname}"', shell=True)

    return code1 + code2 + code3 + code4

def skip_file(directory_list: list)-> bool:
    filename = directory_list[-1]
    # sample name = directory_list[-2]
    category_name = directory_list[-3]

    if not os.path.exists(directory_list.join("/")):
        print('File was deleted')
        # The changed file is deleted, no need to style check.
        return True
    
    if category_name not in categories:
        print('File is not in a category folder')
        return True
    
    if filename.lower() != 'readme.md' and filename.lower() != 'readme.metadata.json':
        print('File is not readme or metadata')
        return True
 
def main():
    msg = 'Entry point of the docker to run mdl and style check scripts.'
    parser = argparse.ArgumentParser(description=msg)
    parser.add_argument('-s', '--string', help='A JSON array of file paths.')
    args = parser.parse_args()
    file_set = set()

    print("** Starting checks **")
    if args.string:
        for file in json.loads(args.string):
            file_set.add(file)
        if not file_set:
            print('Invalid input file paths string, abort.')
            exit(1)    
    else:
        print('Invalid arguments, abort.')
        exit(1)

    return_code = 0

    # A set of dirname strings to avoid duplicate checks on the same sample.

    for f in file_set:
        print(f"**** Checking {f} ****")
        directory_list = f.split("/")
        
        # Check skip file conditions
        if skip_file:
            continue
        
        # Get filename and folder name of the changed sample.
        filename = directory_list[-1]

        # Check if the capitalization of doc filenames are correct.
        if filename.lower() == 'readme.md' and filename != 'README.md':
            print('checking capitalization')
            print(f'Error: {filename} filename has wrong capitalization')
            return_code += 1

        if filename.lower() == 'readme.metadata.json' and filename != 'README.metadata.json':
            print('checking capitalization')
            print(f'Error: {filename} filename has wrong capitalization')
            return_code += 1

        # Run the markdownlint linter on README file.
        if filename.lower() == 'readme.md':
            print('filename is README.md type')
            # Run the linter on markdown file.
            return_code += run_mdl(f)

        # Run the other Python checks on the whole sample folder.
        return_code += run_style_check(f)

    if return_code != 0:
        # Non-zero code occurred during the process.
        exit(return_code)
    else:
        exit(0)


if __name__ == '__main__':
    main()
