import re
import os

basemap_replace = {
    'BasemapDarkGrayCanvasVector':      'BasemapStyleArcGISDarkGray',
    'BasemapImagery':                   'BasemapStyleArcGISImagery',
    'BasemapImageryWithLabels':         'BasemapStyleArcGISImageryStandard',
    'BasemapImageryWithLabelsVector':   'BasemapStyleArcGISImageryStandard',
    'BasemapLightGrayCanvas':           'BasemapStyleArcGISLightGray',
    'BasemapLightGrayCanvasVector':     'BasemapStyleArcGISLightGray',
    'BasemapNationalGeographic':        'BasemapStyleArcGISTopographic',
    'BasemapNavigationVector':          'BasemapStyleArcGISNavigation',
    'BasemapOceans':                    'BasemapStyleArcGISOceans',
    'BasemapOpenStreetMap':             'BasemapStyleArcGISOsmStandard',
    'BasemapStreets':                   'BasemapStyleArcGISStreets',
    'BasemapStreetsNightVector':        'BasemapStyleArcGISStreetsNight',
    'BasemapStreetsVector':             'BasemapStyleArcGISStreets',
    'BasemapStreetsWithReliefVector':   'BasemapStyleArcGISStreetsRelief',
    'BasemapTerrainWithLabels':         'BasemapStyleArcGISTerrain', 
    'BasemapTerrainWithLabelsVector':   'BasemapStyleArcGISTerrain',
    'BasemapTopographic':               'BasemapStyleArcGISTopographic',
    'BasemapTopographicVector':         'BasemapStyleArcGISTopographic'
}

'''
Old style:
Map/Scene {
    BasemapName {}
}
---
New style:
Map/Scene {
    Basemap {
        initStyle: Enums.BasemapStyleName
    }
}
'''

def update_constructor(old_basemap, count_lines):
    try:
        ret = [
            (count_lines * " ") + "Basemap {\n",
            f"{(count_lines+4) * ' '}initStyle: Enums.{basemap_replace[old_basemap]}\n",
            (count_lines * " ") + "}\n"
        ]
        return ret
    except Exception as e:
        print(f'error: {e}')

basemap_regex = r"([a-zA-Z]+)(\s*\{\s*\})" # This means [BasemapName] [0 or more space] ['{' character] [0 or more space] ['}' character]

def update_line(line):
    match = re.search(basemap_regex, line)
    if match:
        return update_constructor(match.group(1), len(line[:match.start()]))
    else:
        print(f"failed on {line}")
        return None
'''
for dirpath, dirnames, filenames in os.walk("./ArcGISRuntimeSDKQt_QMLSamples"):
    for filename in [f for f in filenames if f.endswith(".qml")]:
        changed = False
        full_file = os.path.join(dirpath, filename)
        new_file = []
        with open (full_file) as current_file:
            lines = current_file.readlines()
            for i in range(len(lines)):
                replaced = False
                for key in basemap_replace.keys():
                    if key in lines[i]:
                        updated_constructor = (update_line(lines[i]))
                        if updated_constructor:
                            new_file += updated_constructor
                            replaced = True
                            changed = True
                            break
                if not replaced:
                    new_file.append(lines[i])

        if changed:
            with open (full_file, 'w') as old_file:
                old_file.writelines(new_file)

'''
all_changes = []
for dirpath, dirnames, filenames in os.walk("./ArcGISRuntimeSDKQt_QMLSamples"):
    for filename in [f for f in filenames]:
        full_file = os.path.join(dirpath, filename)
        new_file = ""
        try:
            with open (full_file) as current_file:
                lines = current_file.readlines()
                for i in range(len(lines)):
                    for key in basemap_replace.keys():
                        if key in lines[i]:
                            to_add = f"{full_file}:{i+1} -- {lines[i].strip()}\n"
                            if not all_changes or to_add != all_changes[-1]:
                                all_changes.append(to_add)
        except:
            pass
    with open ('output.txt', 'w') as o:
        o.writelines(all_changes)
