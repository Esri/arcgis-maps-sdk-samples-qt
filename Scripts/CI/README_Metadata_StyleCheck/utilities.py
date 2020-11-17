from os import truncate


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

def is_subsequence(subsequence, full_sequence) -> bool:
    if (len(subsequence) == 0) or (len(subsequence) > len(full_sequence)):
        return False

    idx = 0
    for item in subsequence:
        try:
            idx = full_sequence[idx:].index(item)
        except ValueError:
            return False

    return True
