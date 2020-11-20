api_set = set()
tag_set = set()

def parse_readme(path):
    global data
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
    header = text.pop(0)
    # This splits the header by the first instance of \n
    # We do this in case description is multiple lines
    header = [header[:header.find("\n")]] + [header[header.find("\n")+1:]]
    title = header[0][2:]
    description, screenshot = header[1].split("\n![]") # This doesn't work if screenshot is formatted improperly.
    all_titles = [title]
    all_errors = []

    for section in text:
        # If all goes well, this the only items remaining in the text are the h2's
        
        section_body = section.split("\n")
        section_title = section_body.pop(0)
        if section_title not in all_titles:
            all_titles.append(section_title)
            all_errors += (check_readme_sections(section_title, section_body))
        else:
            print("Error, duplicate section title: " + section_title)
    print(f"{len(all_errors)} total errors")
    for i in range(len(all_errors)):
        print(f"{i + 1}. {all_errors[i]}")
    exit(len(all_errors))


def check_readme_sections(s_title, s_body):
    if s_title in ["Use case","How to use the sample","About the data","Additional information"]:
        return check_sentence_section(s_title, s_body)

    elif s_title == "How it works":
        return check_how_it_works(s_body)

    elif s_title == "Relevant API":
        return check_relevant_api(s_body)

    elif s_title == "Tags":
        return check_tags(s_body)

    elif s_title == "Offline data":
        return []

    else:
        print(f"No test written for section title: {s_title}")
        return []

def check_how_it_works(body):
    if not body:
        return ["No text found in 'How it works'"]
    errors = []
    for i in range(len(body)):
        try:
            line_num = body[i][:body[i].find(". ")]
            if line_num != str(i):
                errors.append(f"Expected ordered list item number {line_num} to be {i}. Please check ordering.")
        except Exception as e:
            errors.append("Improper formatting. Expected ordered list like: '1. ...'\nError:"+e)
    return errors

def check_relevant_api(body):
    global api_set

    if not body:
        return ["No text found in 'Relevant API'"]
    errors = []
    for i in range(len(body)):
        if body[i][:2] != "* ":
            errors.append(f"Expected unordered list item as '* ' on line {i}")
        else:
            api = body[i][3:]
            if api in api_set:
                errors.append(f"{api} is duplicate on line {i}")
            else:
                api_set.add(api)
                if api in tag_set:
                    errors.append(f"{api} should not be in tags")
    return errors

def check_sentence_section(title, body):
    print("checking "+title+" section")
    if not body:
        return [f"No text found in {title} section body."]
    errors = []
    for line in body:
        if not check_sentence_case(line):
            errors.append(f"'{line}' does not follow sentence case pattern. If due to proper noun, please add it to the list.")    
    return errors

def check_tags(body):
    global tag_set

    if not body:
        return ["No tags found in 'Tag' section"]
    errors = []
    tags = body[0].split(", ")
    for tag in tags:
        if tag in tag_set:
            errors.append(f"Tag '{tag}' has duplicates")
        else:
            tag_set.add(tag)
            if tag in api_set:
                errors.append(f"Tag '{tag}' should not be an API.")
    return errors


def check_sentence_case(string: str) -> bool:
    words = string.split(" ")[1:-1]
    for i in range(len(words)):
        if words[i] in proper_nouns:
            continue
        for j in range(len(words[i])):
            if not words[i][j].isalpha():
                if words[i][j] == "`":
                    break
                continue
            if i == 0 and j == 0:
                if not words[i][j].isupper():
                    print(words[i])
                    return False

            if not words[i][j].islower():
                print(words[i])
                return False

    return True
    
proper_nouns = {
    "San",
    "Diego",
    '"Fastest"',
    '"Shortest"',
    "`Route`",
    "`RouteTask`"
}

parse_readme("/Users/tan11389/Projects/ty-samples/ArcGISRuntimeSDKQt_CppSamples/Routing/OfflineRouting/README.md")