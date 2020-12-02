import re
import os

api_key_set_text = [
'\n',
'  // Use of Esri location services, including basemaps and geocoding,\n',
'  // requires authentication using either an ArcGIS identity or an API Key.\n',
'  // 1. ArcGIS identity: An ArcGIS named user account that is a member of an\n',
'  //    organization in ArcGIS Online or ArcGIS Enterprise.\n',
'  // 2. API key: A permanent key that gives your application access to Esri\n',
'  //    location services. Visit your ArcGIS Developers Dashboard create a new\n',
'  //    API keys or access an existing API key.\n',
'  const QString apiKey = "";\n',
'  if (apiKey.isEmpty())\n',
'  {\n',
'      qWarning() << "Use of Esri location services, including basemaps, requires"\n',
'                    "you to authenticate with an ArcGIS identity or set the API Key property.";\n',
'  }\n',
'  else\n',
'  {\n',
'      Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setApiKey(apiKey);\n',
'  }\n'
]

include_runtime_environment = '#include "ArcGISRuntimeEnvironment.h"\n'

text_to_preceed = "#define STRINGIZE(x) #x"

# app.setApplicationName("Buffer - C++");
text_to_follow = 'app.setApplicationName'

all_changes = []
for dirpath, dirnames, filenames in os.walk("./ArcGISRuntimeSDKQt_CppSamples"):
    for filename in [f for f in filenames if f == "main.cpp"]:
        full_file = os.path.join(dirpath, filename)
        new_file = []
        include_set = False
        api_key_set = False
        try:
            with open (full_file) as current_file:
                lines = current_file.readlines()
                for i in range(len(lines)):
                    if text_to_preceed in lines[i]:
                        if new_file[-1] == "\n":
                            new_file[-1] = include_runtime_environment
                        else:
                            new_file.append(include_runtime_environment)
                        new_file.append("\n")
                        new_file.append(lines[i])
                        include_set = True
                        continue

                    if text_to_follow in lines[i]:
                        new_file.append(lines[i])
                        new_file += api_key_set_text
                        api_key_set = True

                    new_file.append(lines[i])
        except Exception as e:
            print(e)
        
        if api_key_set and include_set:
            with open (full_file, 'w') as o:
                o.writelines(new_file)
