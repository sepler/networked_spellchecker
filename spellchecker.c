#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

const char* DEFUALT_DIRECTORY = "words.txt";
const int WORD_LENGTH = 32;

typedef struct dictionary {
    int size;
    char** words;
} dictionary;

void read_file(char*);
int spellcheck(dictionary, const char*);
void to_lower(char*);

int main(int argc, char* argv[]) {
    FILE* fp;
    if (argc == 2) {
        if ((fp = fopen(argv[1], "r")) == NULL) {
            printf("could not read supplied file\n");
            return 1;
        }
    } else {
        if ((fp = fopen(DEFUALT_DIRECTORY, "r")) == NULL) {
            printf("could not read supplied file\n");
            return 1;
        }
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

    dictionary dict;
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
    /*for (i = 0; i < dict.size; i++) {
        printf("%s\n", dict.words[i]);
    }*/

    const char* test[] = { "who", "oranges", "youth's", "a", "études", "notindiciontary" };
    for (i = 0; i < 6; i++) {
        printf("%s\t%d\n", test[i], spellcheck(dict, test[i]));
    }

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

void to_lower(char* str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        str[i] = tolower(str[i]);
    }
}