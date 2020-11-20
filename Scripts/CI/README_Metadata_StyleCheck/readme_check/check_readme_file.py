api_list = []
tag_list = []

def check_readme(path):
    print("---\nChecking "+path)
    global data
    global api_list
    global tag_list

    api_list = []
    tag_list = []

    text = ""
    with open(path, 'r') as f:
        data = f.readlines()
        # Remove blank lines
        for line in data:
            if line != "\n":
                text += line

    # Split out sections by header 2
    text = text.split("\n## ")
    

    # Parse header
    try:
        header = text.pop(0)
        # This splits the header by the first instance of \n
        # We do this in case description is multiple lines
        header = [header[:header.find("\n")]] + [header[header.find("\n")+1:]]
        title = header[0][2:]
        description, screenshot = header[1].split("\n![]") # This doesn't work if screenshot is formatted improperly.
    except Exception as e:
        r = f"Title, description, or screenshot.png formatted incorrectly. README check cannot proceed.\nFatal error: {e}"
        print(r)
        return [r]
    all_section_titles = []
    all_errors = []

    # If all goes well, this the only items remaining in the text are the h2's

    for section in text:
        section_body = section.split("\n")
        section_title = section_body.pop(0)
        if section_title not in all_section_titles:
            all_section_titles.append(section_title)
            all_errors += (check_readme_sections(section_title, section_body))
        else:
            all_errors.append("Error, duplicate section title: " + section_title)

    if not is_subsequence(all_section_titles, possible_readme_headers):
        all_errors.append("Section headers are not in the correct order.")
        print(f"Section are not in the correct order. The correct order is:\n{possible_readme_headers}")
    
    missing_headers = []
    for essential_header in essential_readme_headers:
        if essential_header not in all_section_titles:
            missing_headers.append(essential_header)
    if missing_headers:
        all_errors.append(f"Missing essential headers: {missing_headers}. (This may be due to improper capitalization).")

    if len(all_errors) > 0:
        print(f"{len(all_errors)} total errors")
        for i in range(len(all_errors)):
            print(f"{i + 1}. {all_errors[i]}")
    else:
        print("No errors found!")
    return all_errors
    


def check_readme_sections(s_title, s_body):

    if s_title.lower() == "relevant api":
        return check_relevant_api(s_body)

    elif s_title.lower() == "tags":
        return check_tags(s_body)

    elif s_title.lower() == "offline data":
        return []

    elif s_title in possible_readme_headers:
        return check_if_section_contents(s_title, s_body)
    else:
        return [f"{s_title} not found in potential README headers."]

def check_relevant_api(body):
    global api_list

    if not body:
        return ["No text found in 'Relevant API'"]
    errors = []
    for i in range(len(body)):
        if body[i][:2] != "* ":
            errors.append(f"Expected unordered list item as '* ' on line {i}")
        else:
            api = body[i][3:]
            if api in api_list:
                errors.append(f"{api} is duplicate on line {i}")
            else:
                api_list.append(api)
                if api in tag_list:
                    errors.append(f"{api} should not be in tags")
    return errors

def check_tags(body):
    global tag_list

    if not body:
        return ["No tags found in 'Tag' section"]
    errors = []
    tags = body[0].split(", ")
    for tag in tags:
        if tag in tag_list:
            errors.append(f"Tag '{tag}' has duplicates.")
        else:
            tag_list.append(tag)
            if tag in api_list:
                errors.append(f"Tag '{tag}' should not be an API.")
    if tag_list != sorted(tag_list):
        errors.append("Expected tag list to be sorted alphabetically")
    
    return errors

def check_if_section_contents(title, body):
    if not body:
        return [f"No text found in {title} section body."]
    return []

def check_sentence_case(string: str) -> bool:

    ignore_list = {
        "ArcGIS"
    }

    alnum_string = filter_string_for_alpha_numeric(string)
    words = alnum_string.split(" ")
    i = 0
    while i < len(words):
        if words[i] in ignore_list or words[i][0] == "`":
            continue
        j = 0
        starting_letter = 0
        while j < len(words[i]):
            if not words[i][j].isalpha():
                starting_letter += 1
            elif i == 0 and j == starting_letter:
                if not words[i][j].isupper():
                    return False
            elif not words[i][j].islower():
                return False
            j += 1
        i += 1
    return True

def filter_string_for_alpha_numeric(string):
    clean_string = ""
    for letter in string:
        if letter.isalnum() or letter in " `":
             clean_string += letter
    return clean_string

essential_readme_headers = [
    'Use case',
    'How to use the sample',
    'How it works',
    'Relevant API',
    'Tags'
]

possible_readme_headers = [
    'Use case',
    'How to use the sample',
    'How it works',
    'Relevant API',
    'Offline data',
    'About the data',
    'Additional information',
    'Tags'
]


def is_subsequence(arr1, arr2) -> bool:
    if len(arr1) > len(arr2) or not arr1:
        return False
    i, j = 0, 0
    while i < len(arr1):
        if arr1[i].lower() == arr2[j].lower():
            i += 1
        j += 1
    if i == len(arr1):
        return True
    return False


check_paths = [
    "/Users/tan11389/Projects/ty-samples/ArcGISRuntimeSDKQt_CppSamples/Scenes/AddIntegratedMeshLayer/README.md",
    "/Users/tan11389/Projects/ty-samples/ArcGISRuntimeSDKQt_CppSamples/Scenes/Symbols/README.md",
    "/Users/tan11389/Projects/ty-samples/ArcGISRuntimeSDKQt_CppSamples/Scenes/RealisticLightingAndShadows/README.md",
    "/Users/tan11389/Projects/ty-samples/ArcGISRuntimeSDKQt_CppSamples/Search/OfflineGeocode/README.md",
    "/Users/tan11389/Projects/ty-samples/ArcGISRuntimeSDKQt_CppSamples/LocalServer/LocalServerMapImageLayer/README.md"
]

for path in check_paths:
    check_readme(path)
