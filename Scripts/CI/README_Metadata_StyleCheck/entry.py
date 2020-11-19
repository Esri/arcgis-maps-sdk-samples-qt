import os
import argparse
from os import listdir
from os.path import isfile, join
import json

# ***** Global variables *****
directory_list = []
sample_name = ""
sample_type = ""
category_name = ""
file_name = ""
other_files_in_folder = []
metadata = None

def main():
    # Tell the program that we're using global variables
    global directory_list
    global sample_name
    global sample_type
    global category_name
    global file_name
    global other_files_in_folder

    msg = 'Entry point of the docker to run json style check scripts.'
    parser = argparse.ArgumentParser(description=msg)
    parser.add_argument('-s', '--string', help='A JSON array of file paths.')
    args = parser.parse_args()
    cleanstring = ""
    try:
        for letter in args.string:
            if letter not in '[" \']':
                cleanstring+=letter
        file_set = cleanstring.split(",")
    except Exception as e:
        print(f"Unable to split args {args.string}.\nException: {e}")
        exit(1)
    
    print("Files to check: ")
    print(file_set)

    total_errors = 0
    if not file_set:
        print("No files in file set")
        exit(0)
    for file in file_set:
        try:
            errors = []
            print(f"\n**** Checking {file} ****")

            # Set variables for file
            directory_list = file.split("/")
            file_name = directory_list[-1]
            filepath = "/".join(directory_list[:-1])
            sample_name = directory_list[-2]
            category_name = directory_list[-3]
            sample_type = directory_list[-4]
            other_files_in_folder = get_filenames_in_folder(filepath)
            
            # Check skip file conditions
            if skip_file(directory_list):
                continue        

            # Check if metadata file
            if file_name.lower() == 'readme.metadata.json':
                try:
                    errors += check_metadata_file(file)
                except Exception as e:
                    errors.append(f"Error checking {file_name}. Exception: {e}")

            if len(errors) == 0:
                print("No errors found")
            else:
                print(f"Found {len(errors)} errors:")
                for i in range(len(errors)):
                    print(f"{i+1}. {errors[i]}")
            total_errors += len(errors)
        except Exception as e:
            print(f"Critical failure on file: {file}. Exception: {e}")
            total_errors += 1
    
    if total_errors == 0:
        print("\nAll tests passed!")
    print(f"\nTotal errors: {total_errors}")
    # When we exit an pass a non-zero value, GitHub registers the test as failed.
    exit(total_errors)

# PLEASE DELETE BELOW THIS LINE WHEN WE FIGURE OUT THE IMPORT STUFF
# from stylechecks.metadata_style_checker import check_metadata_file
# from utilities.common_dicts import file_categories

def skip_file(directory_list: list)-> bool:
    category_name = directory_list[-3]
    if not os.path.exists("/".join(directory_list)):
        print('File was deleted')
        # The changed file is deleted, no need to style check.
        return True
    if category_name not in file_categories:
        print('File is not in a category folder')
        return True
    
    if file_name.lower() != 'readme.metadata.json':
        print('File is not readme or metadata')
        return True

def check_metadata_file(path):

    global metadata
    with open(path) as f:
        metadata = json.load(f)

    meta_errors = []

    if file_name != "README.metadata.json":
        meta_errors.append(f"{file_name} does not have correct capitalization")

    for key in metadata.keys():
        try:
            meta_errors += (check_sections(key))
        except Exception as e:
            meta_errors.append(f"Errors testing: {key}. Exception: {e}")

    for expected_key in required_metadata_keys:
        if expected_key not in metadata.keys():
            meta_errors.append(f"{expected_key} not found in file metadata keys")

    return meta_errors

def check_sections(key: str):
    # We do this in case the .json key is not one of the accepted sections
    if key == "category":
        return (check_category(metadata["category"]))
    elif key == "description":
        return (check_description(metadata["description"]))
    elif key == "ignore":
        return [] # (check_ignore(metadata["ignore"]))
    elif key == "images":
        return (check_images(metadata["images"]))
    elif key == "keywords":
        return (check_keywords(metadata['keywords']))
    elif key == "redirect_from":
        return (check_redirect_from(metadata["redirect_from"]))
    elif key == "relevant_apis":
        return (check_relevant_apis(metadata['relevant_apis']))
    elif key == "snippets":
        return (check_snippets(metadata["snippets"]))
    elif key == "title":
        return (check_title(metadata["title"]))
    elif key == "dataItems":
        return [] # (check_dataItems(metadata["dataItems"]))
    else:
        # If this triggers, there is something wrong with this file. Ideally this should not trigger.
        return [f"Category: '{key}' is not a recognized readme metadata key."]

def check_category(cat: str):
    if not cat:
        return ["No category"]
    errors = []
    if cat not in metadata_categories:
        errors.append(f"Category does not match any currently listed metadata categories: ('{cat}')")
    if "".join(cat.split(" ")).lower() != category_name.lower():
        errors.append(f"Category does not match parent category folder: ('{cat}' != '{category_name}')")
    return errors

def check_description(description: str):
    if not description:
        return ["No description written"]
    errors = []
    if description[-1] not in ".?!":
        errors.append("Description does not end in proper punctuation")
    return errors

def check_images(images: list):
    if not images:
        return ["No images listed"]
    errors = []
    images_in_folder = other_files_in_folder
    has_screenshotpng = False
    for image in images:
        if not image in images_in_folder:
            errors.append(f"{image} not found in folder")
        if image == "screenshot.png":
            has_screenshotpng = True
    if not has_screenshotpng:
        errors.append("'screenshot.png' not found")
    return errors

def check_keywords(keywords: list):
    if not keywords:
        return ["No keywords found"]
    errors = []
    return errors

def check_redirect_from(redirects: list):
    if redirects == ['']:
        return ["No redirects listed"]
    errors = []
    has_expected_redirect = False
    expected_redirect = "/qt/latest/cpp/sample-code/sample-qt-"+sample_name+".htm"
    for redirect in redirects:
        if redirect.lower() == expected_redirect.lower():
            has_expected_redirect = True
    if not has_expected_redirect:
        errors.append(f"Expected redirect ({expected_redirect}) not found among {redirects}")
    return errors

def check_relevant_apis(apis: list):
    if not apis:
        return ["No APIs listed"]
    errors = []
    return errors

def check_snippets(snippets: list):
    if not snippets:
        return ["No snippets listed"]
    errors = []
    for snippet in snippets:
        if snippet not in other_files_in_folder:
            errors.append(f"{snippet} not found in sample folder")
    expected_snippets = [
        sample_name + ".h",
        sample_name + ".qml"
    ]
    if sample_type == "ArcGISRuntimeSDKQt_CppSamples":
        expected_snippets.append(sample_name + ".cpp")
    for expected_snippet in expected_snippets:
        if expected_snippet not in snippets:
            errors.append(f"Expected {expected_snippet} in snippets")
    return errors

def check_title(title: str):
    if not title:
        return ["No title"]
    errors = []
    if not check_sentence_case:
        errors.append("Title does not follow sentence case")
    if "".join(title.split(" ")).lower() != sample_name.lower():
        errors.append("Title does not match sample name")
    if not title[-1].isalnum():
        errors.append("Title does not end in alphanumeric character")
    return errors

# ***** Large variable sets (from utilities.common_dicts) *****

file_categories = {
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

metadata_categories = {
    'Analysis',
    'AR',
    'Cloud and portal',
    'Display information',
    'Edit data',
    'Features',
    'Geometry',
    'Layers',
    'Local Server',
    'Maps',
    'Routing',
    'Scenes',
    'Search',
    'UtilityNetwork'
}

required_metadata_keys = {
    'category',
    'description',
    'ignore',
    'images',
    'keywords',
    'redirect_from',
    'relevant_apis',
    'snippets',
    'title'
}

# ***** Helper functions from utilities.helper_functions

def check_sentence_case(string: str) -> bool:
    words = string.split(" ")
    for i in range(len(words)):
        for j in range(len(words[i])):
            if not words[i][j].isalpha():
                continue

            if i == 0 and j == 0:
                if not words[i][j].isupper():
                    return False

            if not words[i][j].islower():
                return False

    return True

def get_filenames_in_folder(folderpath):
    return [file for file in listdir(folderpath) if isfile(join(folderpath, file))]

if __name__ == '__main__':
    pass
    main()


# "/Users/tan11389/Projects/ty-samples/ArcGISRuntimeSDKQt_CppSamples/Geometry/FormatCoordinates/README.metadata.json,/Users/tan11389/Projects/ty-samples/ArcGISRuntimeSDKQt_CppSamples/Routing/OfflineRouting/README.metadata.json,"