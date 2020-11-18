from os import read


readme_contents = None

def check_README(file_path):
    global readme_contents
    with open(file_path, 'r') as readme:
        readme_contents = readme.readlines()
        print(readme_contents)
    for i in range(len(readme_contents)):
        print(f"line {i}: {readme_contents[i]}")

r = "/Users/tan11389/Projects/ty-samples/ArcGISRuntimeSDKQt_CppSamples/CloudAndPortal/AddItemsToPortal/README.md"
check_README(r)