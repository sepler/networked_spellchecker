// STEPHEN EPLER

#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

typedef struct dictionary {
    int size;
    char** words;
} dictionary;

dictionary create_dict( const char* );
int spellcheck( dictionary, const char* );
void to_lower( char* );

#endif