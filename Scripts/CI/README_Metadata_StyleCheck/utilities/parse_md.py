readme_contents = None

def check_README(file_path):
    global readme_contents
    with open(file_path, 'r') as readme:
        readme_contents = readme.read()
    #print(readme_contents)
    #for i in range(len(readme_contents)):
    #    print(f"line {i}: {readme_contents[i]}")
    md = {
        'title': "",
        'description': "",
        'Use case': [],
        'How to use the sample': [],
        'How it works': [],
        'Relevant API': [],
        'About the_data': [],
        'Additional information': [],
        'Tags': []
    }
    i = 0
    rm_c = []
    
    for i in range(len(readme_contents)):
        if readme_contents[i] != "\n":
            rm_c.append(readme_contents[i].strip())
    i = 0
    while i < len(rm_c):
        if readme_contents[i][:2] == "# ":
            md['title'] = rm_c[i][2:].strip()
            i+=1
            md['description'] = rm_c[i]
            while rm_c[i][:3] != "## ":
                i+=1
        if rm_c[i][:3] == "## ":
            j = i
            i += 1
            while i < len(rm_c) and rm_c[i][:3] != "## ":
                md[rm_c[j][3:]].append(rm_c[i])
                i += 1
        
            
    h2 = {}
    h3 = {}
    h4 = {}

r = "/Users/tan11389/Projects/ty-samples/ArcGISRuntimeSDKQt_CppSamples/CloudAndPortal/AddItemsToPortal/README.md"
check_README(r)