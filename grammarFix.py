import re
import os

grammar_fixes = {
    "an `Map":   "a `Map",
    "an `map":   "a `map",
    "an `Scene": "a `Scene",
    "an `scene": "a `scene",
    "an Map":   "a Map",
    "an map":   "a map",
    "an Scene": "a Scene",
    "an scene": "a scene"
    
}

for dirpath, dirnames, filenames in os.walk("."):
    changes = False
    for filename in [f for f in filenames if f.endswith('.md')]:
        full_file = os.path.join(dirpath, filename)
        new_file = ""
        with open (full_file) as current_file:
            lines = current_file.readlines()
            for i in range(len(lines)):
                for key in grammar_fixes.keys():
                    if key in lines[i]:
                        lines[i] = lines[i].replace(key, grammar_fixes[key])
                        changes = True

            new_file = lines
        if changes == True: 
            with open (full_file, 'w') as old_file:
                old_file.writelines(new_file)