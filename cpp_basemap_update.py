import re
import os

basemap_replace = {
    "Basemap::darkGrayCanvasVector(this)":      "BasemapStyle::ArcGISDarkGray",
    "Basemap::imagery(this)":                   "BasemapStyle::ArcGISImageryStandard",
    "Basemap::imageryWithLabels(this)":         "BasemapStyle::ArcGISImagery",
    "Basemap::imageryWithLabelsVector(this)":   "BasemapStyle::ArcGISImagery",
    "Basemap::lightGrayCanvas(this)":           "BasemapStyle::ArcGISLightGray",
    "Basemap::lightGrayCanvasVector(this)":     "BasemapStyle::ArcGISLightGray",
    "Basemap::nationalGeographic(this)":        "BasemapStyle::ArcGISTopographic", # what goes here?
    "Basemap::navigationVector(this)":          "BasemapStyle::ArcGISNavigation",
    "Basemap::oceans(this)":                    "BasemapStyle::ArcGISOceans",
    "Basemap::openStreetMap(this)":             "BasemapStyle::OsmStandard",
    "Basemap::streets(this)":                   "BasemapStyle::ArcGISStreets",
    "Basemap::streetsNightVector(this)":        "BasemapStyle::ArcGISStreetsNight",
    "Basemap::streetsVector(this)":             "BasemapStyle::ArcGISStreets",
    "Basemap::streetsWithReliefVector(this)":   "BasemapStyle::ArcGISStreetsRelief",
    "Basemap::terrainWithLabels(this)":         "BasemapStyle::ArcGISTerrainBase",
    "Basemap::terrainWithLabelsVector(this)":   "BasemapStyle::ArcGISTerrain",
    "Basemap::topographic(this)":               "BasemapStyle::ArcGISTopographic",
    "Basemap::topographicVector(this)":         "BasemapStyle::ArcGISTopographic",
}

basemap_regex = [
    r"(m_map = new Map\()"              +   r"(Basemap::[\w]+\(this\))"     + r"(, this\))",
    r"(m_scene = new Scene\()"          +   r"(Basemap::[\w]+\(this\))"     + r"(, this\))",
    r"(Basemap\* basemap = )"           +   r"(Basemap::[\w]+\(this\))",
    r"(Map\* map = new Map\()"          +   r"(Basemap::[\w]+\(this\))"     + r"(, this\))",
    r"(Scene\* scene = new Scene\()"    +   r"(Basemap::[\w]+\(this\))"     + r"(, this\))",
    r"(m_map->setBasemap\()"            +   r"(Basemap::[\w]+\(this\))"     + r"(\))",
    r"(m_map\(new Map\()"               +   r"(Basemap::[\w]+\(this\))"     + r"(, this\))",
    r"(m_scene\(new Scene\()"           +   r"(Basemap::[\w]+\(this\))"     + r"(, this\))",
    r"(selectedBasemap = )"             +   r"(Basemap::[\w]+\(this\))"
]

def update_constructor(type, old_basemap):
    if type == "m_map = new Map(":
        return "m_map = new Map("+basemap_replace[old_basemap]+", this)"

    elif type == "Basemap* basemap = ":
        return "Basemap* basemap = new Basemap("+basemap_replace[old_basemap]+", this)"

    elif type == "Map* map = new Map(":
        return "Map* map = new Map("+basemap_replace[old_basemap]+", this)"
    
    elif type == "Scene* scene = new Scene(":
        return "Scene* scene = new Scene("+basemap_replace[old_basemap]+", this)"
    
    elif type == "m_map->setBasemap(":
        return "m_map->setBasemap(new Basemap("+basemap_replace[old_basemap]+", this))"

    elif type == "m_map(new Map(":
        return "m_map(new Map("+basemap_replace[old_basemap]+", this)"
        
    elif type == "m_scene = new Scene(":
        return "m_scene = new Scene("+basemap_replace[old_basemap]+", this)"

    elif type == "m_scene(new Scene(":
        return "m_scene(new Scene("+basemap_replace[old_basemap]+", this)"

    elif type == "selectedBasemap = ":
        return "selectedBasemap = new Basemap("+basemap_replace[old_basemap]+", this)"

    else:
        raise ValueError

def update_line(line):
    for r in range(len(basemap_regex)):
        match = re.search(basemap_regex[r], line)
        if match:
            return line[:match.start()] + update_constructor(match.group(1), match.group(2)) + line[match.end():]
        else:
            print(f"failed on {line}")

for dirpath, dirnames, filenames in os.walk("."):
    for filename in [f for f in filenames if f.endswith(".qml")]:
        full_file = os.path.join(dirpath, filename)
        new_file = ""
        with open (full_file) as current_file:
            lines = current_file.readlines()
            for i in range(len(lines)):
                if lines[i].find('Basemap::') != -1:
                    a = (update_line(lines[i]))
                    if a:
                        lines[i] = a
            new_file = lines   
        with open (full_file, 'w') as old_file:
            old_file.writelines(new_file)