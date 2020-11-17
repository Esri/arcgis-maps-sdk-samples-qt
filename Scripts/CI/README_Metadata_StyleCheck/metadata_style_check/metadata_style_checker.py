#!/usr/bin/env python3

import os
import re
import json
import typing
import argparse

from ..utilities import check_sentence_case, get_filenames_in_folder
from ..common_dicts import metadata_categories, readme_json_keys

# Global variables
dirlist = []
samplename = None
sampletype = None
categoryname = None
dirpath = None
other_files_in_folder = None

def check_metadata_file(file):

    metadata = json.loads(file)
    dirlist = file.split("/")
    samplename = dirlist[-2]
    categoryname = dirlist[-3]
    sampletype = dirlist[-4]
    other_files_in_folder = get_filenames_in_folder(file)

    meta_errors = []

    for key in metadata.keys():
        if key not in readme_json_keys:
            meta_errors.append(f"{key} not found in expected metadata keys")
    for expected_key in readme_json_keys:
        if expected_key not in metadata.keys():
            meta_errors.append(f"{expected_key} not found in file metadata keys")

    meta_errors.append(check_category(metadata["category"]))
    meta_errors.append(check_description(metadata["description"]))
    meta_errors.append(check_images(metadata["images"]))
    meta_errors.append(check_keywords(metadata['keywords']))
    meta_errors.append(check_redirect_from(metadata["redirect_from"]))
    meta_errors.append(check_relevant_apis(metadata['relevant_apis']))
    meta_errors.append(check_snippets(metadata["snippets"]))
    meta_errors.append(check_title(metadata["title"]))

    return meta_errors


def check_category(cat: str):
    if not cat:
        return "No category"
    errors = []
    if cat not in metadata_categories:
        errors.append("Category does not match any currently listed metadata categories")
    if cat.split(" ").join("").lower() != categoryname.lower():
        errors.append("Category does not match parent category folder")
    return errors

def check_description(description: str):
    if not description:
        return "No description written"
    errors = []
    if description[-1] not in ".?!":
        errors.append("Description does not end in proper punctuation")
    return errors

def check_images(images: list):
    if not images:
        return "No images listed"
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
    errors = []
    if len(keywords) < 1:
        errors.append("No keywords found")
    return errors

def check_redirect_from(redirects: list):
    if not redirects:
        return "No redirects listed"
    errors = []
    has_expected_redirect = False
    expected_redirect = "/qt/latest/cpp/sample-code/sample-qt-"+samplename+".htm"
    for redirect in redirects:
        if redirect == expected_redirect:
            has_expected_redirect = True
    if not has_expected_redirect:
        errors.append(f"Expected redirect ({expected_redirect}) not found")
    return errors

def check_relevant_apis(apis: list):
    if not apis:
        return "No APIs listed"
    errors = []
    return errors

def check_snippets(snippets: list):
    if not snippets:
        return "No snippets listed"
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
        return "No title"
    errors = []
    if not check_sentence_case:
        errors.append("Title does not follow sentence case")
    if title.split(" ").join("").lower() != samplename.lower():
        errors.append("Title does not match sample name")
    if not title[-1].isalnum():
        errors.append("Title does not end in alphanumeric character")
    return errors