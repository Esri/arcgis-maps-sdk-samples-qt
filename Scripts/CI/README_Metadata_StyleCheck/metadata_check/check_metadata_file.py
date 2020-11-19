import os
import json
from ..utilities.helper_functions import get_filenames_in_folder, check_sentence_case
from ..utilities.common_dicts import readme_json_keys, file_categories, metadata_categories

def check_metadata_file(path):
    global directory_list
    global sample_name
    global sample_type
    global category_name
    global file_name
    global other_files_in_folder
    global metadata
    
    with open(path) as f:
        metadata = json.load(f)

    directory_list = path.split("/")
    file_name = directory_list[-1]
    filepath = "/".join(directory_list[:-1])
    sample_name = directory_list[-2]
    category_name = directory_list[-3]
    sample_type = directory_list[-4]
    other_files_in_folder = get_filenames_in_folder(filepath)

    meta_errors = []

    if file_name != "README.metadata.json":
        meta_errors.append(f"{file_name} does not have correct capitalization")

    for key in metadata.keys():
        if key not in readme_json_keys:
            meta_errors.append(f"Section: '{key}' not found in expected metadata keys")
        else:
            try:
                meta_errors += (check_sections(key))
            except Exception as e:
                meta_errors.append(f"Errors testing: {key}. Exception: {e}")

    for expected_key in readme_json_keys:
        if expected_key not in metadata.keys():
            meta_errors.append(f"{expected_key} not found in file metadata keys")

    return meta_errors

def skip_file(directory_list: list)-> bool:
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

def check_sections(key: str):
    # We do this in case the .json key is not one of the accepted sections
    if key == "category":
        return (check_category(metadata["category"]))
    elif key == "description":
        return (check_description(metadata["description"]))
    elif key == "ignore":
        return []
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
    else:
        # If this triggers, there is something wrong with this file. Ideally this should not trigger.
        return f"{key} found in readme_json_keys file but not in section checks. Please check this code (metadata_style_checker.py) to ensure all json keys are checked."

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
    if not redirects:
        return ["No redirects listed"]
    errors = []
    has_expected_redirect = False
    expected_redirect = "/qt/latest/cpp/sample-code/sample-qt-"+sample_name+".htm"
    for redirect in redirects:
        if redirect.lower() == expected_redirect.lower():
            has_expected_redirect = True
    if not has_expected_redirect:
        errors.append(f"Expected redirect ({expected_redirect}) not found")
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
