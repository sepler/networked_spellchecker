#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

const int WORD_LENGTH = 32;

typedef struct dictionary {
    int size;
    char** words;
} dictionary;

void to_lower(char* str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        str[i] = tolower(str[i]);
    }
}

dictionary create_dict(const char* path) {
    FILE* fp;
    dictionary dict;
    dict.size = 0;
    if ((fp = fopen(path, "r")) == NULL) {
        printf("could not read supplied file\n");
        return dict;
    }
    // Get # of dict entries
    char ch;
    int lines = 0;
    do {
        ch = fgetc(fp);
        if (ch == '\n')
            lines++;
    } while (ch != EOF);
    rewind(fp);

    dict.size = lines;
    dict.words = malloc(lines * sizeof(char*));
    int i;
    for (i = 0; i < dict.size; i++) {
        dict.words[i] = malloc(WORD_LENGTH * sizeof(char*));
        fgets(dict.words[i], WORD_LENGTH, fp);
        int len = strlen(dict.words[i]);
        if (dict.words[i][len-1] == '\n') {
            dict.words[i][len-1] = 0;
        }
        to_lower(dict.words[i]);
    }
    return dict;
}

int spellcheck(dictionary dict, const char* word) {
    int i;
    for (i = 0; i < dict.size; i++) {
        if (strcmp(dict.words[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

