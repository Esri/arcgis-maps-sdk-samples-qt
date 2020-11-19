from os import listdir
from os.path import isfile, join

def check_sentence_case(string: str) -> bool:
    words = string.split(" ")
    for i in range(len(words)):
        for j in range(len(words[i])):
            if not words[i][j].isalpha():
                continue

            if i == 0 and j == 0:
                if not words[i][j].isupper():
                    return False

            if not words[i][j].islower():
                return False

    return True

def get_filenames_in_folder(folderpath):
    return [file for file in listdir(folderpath) if isfile(join(folderpath, file))]