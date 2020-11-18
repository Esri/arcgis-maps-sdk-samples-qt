import os
import json
import argparse
import subprocess as sp
from metadata_style_checker import check_metadata_file
from common_dicts import file_categories

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
            print('FATAL ERROR: Invalid input file paths string, abort.')
            exit(1)    
    else:
        print('FATAL ERROR: Invalid arguments, abort.')
        exit(1)

    errors = []

    # A set of dirname strings to avoid duplicate checks on the same sample.

    for file in file_set:
        print(f"**** Checking {file} ****")
        directory_list = file.split("/")
        filename = directory_list[-1]
        
        # Check skip file conditions
        if skip_file(directory_list):
            continue        

        # Check if metadata file
        if filename.lower() == 'readme.metadata.json':
            errors += check_metadata_file

        # Run the markdownlint linter on README file. Thanks to Ting Chen (tchen@esri.com) for this one.
        if filename.lower() == 'readme.md':
            print('filename is README.md type')

            # Run the linter on markdown file.
            r = run_mdl(file)
            if r > 0:
                errors.append(f"markdown linter results: "+ r)

            # Run the style checker


    if len(errors) == 0:
        print("No errors found")
        return 0
    else:
        print(f"Found {len(errors)}:")
        for i in range(len(errors)):
            print(f"{i+1}. {errors[i]}")
        return len(errors)
            
def run_mdl(readme_path: str): # Run markdown linter
    print("**** mdl ****")
    return sp.call(f'mdl --style /style.rb "{readme_path}', shell=True)

def skip_file(directory_list: list)-> bool:
    filename = directory_list[-1]
    # sample name = directory_list[-2]
    category_name = directory_list[-3]

    if not os.path.exists(directory_list.join("/")):
        print('File was deleted')
        # The changed file is deleted, no need to style check.
        return True
    
    if category_name not in file_categories:
        print('File is not in a category folder')
        return True
    
    if filename.lower() != 'readme.md' and filename.lower() != 'readme.metadata.json':
        print('File is not readme or metadata')
        return True

if __name__ == '__main__':
    main()