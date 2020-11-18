#!/usr/bin/env python3

import os
import re
import json
import typing
import argparse

from utilities import check_sentence_case, get_filenames_in_folder
from common_dicts import metadata_categories, readme_json_keys

# Global variables
dirlist = [] # A list of all folders in the file path and .json file
filepath = "" # The path to the folder containing the metadata file. Does not include .json file
samplename = "" # The name of the sample, as defined in the parent directory name
sampletype = "" # The type of sample, ie ArcGISRuntimeSDKQt_CppSamples
categoryname = "" # The category of the sample ie CloudAndPortal or AR
files_in_folder = [] # All files in the folder, including the .json file
metadata = {} # .json file data will be converted into a dictionary here

def check_metadata_file(path):
    global dirlist
    global samplename
    global sampletype
    global categoryname
    global filepath
    global other_files_in_folder
    global metadata
    
    with open(path) as f:
        metadata = json.load(f)
    dirlist = path.split("/")
    filepath = "/".join(dirlist[:-1])
    samplename = dirlist[-2]
    categoryname = dirlist[-3]
    sampletype = dirlist[-4]
    other_files_in_folder = get_filenames_in_folder(filepath)

    meta_errors = []

    for key in metadata.keys():
        if key not in readme_json_keys:
            meta_errors.append(f"Section: '{key}' not found in expected metadata keys")
        else:
            meta_errors += check_sections(key)

    for expected_key in readme_json_keys:
        if expected_key not in metadata.keys():
            meta_errors.append(f"{expected_key} not found in file metadata keys")

    for me in meta_errors:
        print(me)
    return meta_errors

def check_sections(key: str):
    # We do this in case the .json key is not one of the accepted sections
    section_errors = []
    if key == "category":
        section_errors += (check_category(metadata["category"]))
    elif key == "description":
        section_errors += (check_description(metadata["description"]))
    elif key == "ignore":
        return []
    elif key == "images":
        section_errors += (check_images(metadata["images"]))
    elif key == "keywords":
        section_errors += (check_keywords(metadata['keywords']))
    elif key == "redirect_from":
        section_errors += (check_redirect_from(metadata["redirect_from"]))
    elif key == "relevant_apis":
        section_errors += (check_relevant_apis(metadata['relevant_apis']))
    elif key == "snippets":
        section_errors += (check_snippets(metadata["snippets"]))
    elif key == "title":
        section_errors += (check_title(metadata["title"]))
    else:
        section_errors.append(f"{key} found in readme_json_keys file but not in section checks")
    
    return section_errors

def check_category(cat: str):
    if not cat:
        return ["No category"]
    errors = []
    if cat not in metadata_categories:
        errors.append("Category does not match any currently listed metadata categories")
    if "".join(cat.split(" ")).lower() != categoryname.lower():
        errors.append("Category does not match parent category folder")
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
    expected_redirect = "/qt/latest/cpp/sample-code/sample-qt-"+samplename+".htm"
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
        samplename + ".h",
        samplename + ".qml"
    ]
    if sampletype == "ArcGISRuntimeSDKQt_CppSamples":
        expected_snippets.append(samplename + ".cpp")
    for e_snip in expected_snippets:
        if e_snip not in snippets:
            errors.append(f"Expected {e_snip} in snippets")
    return errors

def check_title(title: str):
    if not title:
        return ["No title"]
    errors = []
    if not check_sentence_case:
        errors.append("Title does not follow sentence case")
    if "".join(title.split(" ")).lower() != samplename.lower():
        errors.append("Title does not match sample name")
    if not title[-1].isalnum():
        errors.append("Title does not end in alphanumeric character")
    return errors

print(check_metadata_file("/Users/tan11389/Projects/ty-samples/ArcGISRuntimeSDKQt_CppSamples/CloudAndPortal/AddItemsToPortal/README.metadata.json"))