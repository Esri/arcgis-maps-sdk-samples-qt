def check_readme(path):
    global data
    text = []
    with open(path, 'r') as f:
        data = f.readlines()
        # Remove blank lines
        for line in data:
            if line != "\n":
                text.append(line.strip())
    if text[0][:2] != "# ":
        print("Error, expected title to be first line")
        exit(1)
    title = text[0][2:]
    description = text[1]
    if text[2] != "![](screenshot.png)":
        print("Error, expected screenshot.png")
    screenshot = text[2]
    print(1)


check_readme("/Users/tan11389/Projects/ty-samples/ArcGISRuntimeSDKQt_CppSamples/Routing/OfflineRouting/README.md")