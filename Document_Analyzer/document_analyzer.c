#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif /*_CRT_SECURE_NO_WARNINGS*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "document_analyzer.h"

#define ALLOCATION_BUFFER (50)
#define WORD_DELIMS_LENGTH (7)
#define SENTENCE_DELIMS_LENGTH (5)
#define PARAGRAPH_DELIMS_LENGTH (2)

char* g_pa_file_reader = NULL;
char* g_pa_words;
char** g_pa_sentences;
char*** g_pa_paragraphs;
const char**** g_pa_document;
long g_file_size = 0L;
size_t g_words_count;
size_t g_sentences_count;
size_t g_paragraphs_count;
size_t g_current_paragraph_index;
size_t g_current_sentence_index;

int load_document(const char* document)
{
    FILE* file;
    int c;
    char* reader = NULL;
    file = fopen(document, "r");
    if (file == NULL) {
        perror("error while opening file");
        return FALSE;
    }
    fseek(file, 0L, SEEK_END);
    g_file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    g_pa_file_reader = (char*)malloc(g_file_size * sizeof(char) + 2);
    memset(g_pa_file_reader, '\0', g_file_size * sizeof(char) + 2);
    if (g_pa_file_reader != NULL) {
        reader = g_pa_file_reader;
    }
    c = fgetc(file);
    while (c != EOF) {
        *reader = c;
        reader++;
        c = fgetc(file);
    }
    if (fclose(file) == EOF) {
        perror("error while closing file");
        return FALSE;
    }
    if (g_file_size != 0) {
        allocate_memory_malloc();
        get_words_list_malloc();
        get_sentences_list_malloc();
        get_paragraphs_list_malloc();
        get_document_list_malloc();
    }
    return TRUE;
}

void dispose(void)
{
    free(g_pa_file_reader);
    g_pa_file_reader = NULL;
    g_file_size = 0;
    free(g_pa_words);
    g_pa_words = NULL;
    free(g_pa_sentences);
    g_pa_sentences = NULL;
    free(g_pa_paragraphs);
    g_pa_paragraphs = NULL;
    free((void*)g_pa_document);
    g_pa_document = NULL;
}

size_t count_words_or_sentences_or_paragraphs(const char* delims)
{
    size_t count = 0;
    char* buffer = (char*)malloc(g_file_size * sizeof(char) + 2);
    char* p = NULL;
    char* token = NULL;
    if (g_file_size == 0) {
        goto finish;
    }
    memset(buffer, '\0', g_file_size + 2);
    memcpy(buffer, g_pa_file_reader, g_file_size + 2);
    if (buffer != NULL) {
        p = buffer;
    }
    token = strtok(p, delims);
    while (token != NULL) {
        token = strtok(NULL, delims);
        count++;
    }
finish:
    free(buffer);
    buffer = NULL;
    return count;
}


size_t get_total_word_count(void)
{
    const char delims[WORD_DELIMS_LENGTH] = { ' ', ',', '.', '!', '?', '\n', '\0' };
    size_t count = 0;
    if (g_pa_file_reader == NULL) {
        return count;
    }
    if (g_file_size == 0) {
        return count;
    }
    count = count_words_or_sentences_or_paragraphs(delims);
    return count;
}

size_t get_total_sentence_count(void)
{
    const char delims[SENTENCE_DELIMS_LENGTH] = { '.', '?', '!', '\n', '\0' };
    size_t count = 0;
    if (g_pa_file_reader == NULL) {
        return count;
    }
    if (g_file_size == 0) {
        return count;
    }
    count = count_words_or_sentences_or_paragraphs(delims);
    return count;
}

size_t get_total_paragraph_count(void)
{
    const char delims[PARAGRAPH_DELIMS_LENGTH] = { '\n', '\0' };
    size_t count = 0;
    if (g_pa_file_reader == NULL) {
        return 0;
    }
    if (g_file_size == 0) {
        return 0;
    }
    count = count_words_or_sentences_or_paragraphs(delims);
    return count;
}

void allocate_memory_malloc(void)
{
    g_words_count = get_total_word_count();
    g_sentences_count = get_total_sentence_count();
    g_paragraphs_count = get_total_paragraph_count();

    g_pa_words = (char*)malloc(g_file_size * sizeof(char) + 2);
    memset(g_pa_words, '\0', g_file_size * sizeof(char) + 2);
    g_pa_sentences = (char**)malloc(g_words_count * sizeof(char*));
    memset(g_pa_sentences, '\0', g_words_count * sizeof(char*));
    g_pa_paragraphs = (char***)malloc(g_sentences_count * sizeof(char**));
    memset(g_pa_paragraphs, '\0', g_sentences_count * sizeof(char**));
    g_pa_document = (const char****)malloc(g_paragraphs_count * sizeof(char***));
    memset((void****)g_pa_document, '\0', g_paragraphs_count * sizeof(char***));
}

void get_words_list_malloc(void)
{
    char* buffer = (char*)malloc(g_file_size * sizeof(char) + ALLOCATION_BUFFER);
    char* token = NULL;
    char* p = NULL;
    size_t i;
    size_t j;
    const char delims[WORD_DELIMS_LENGTH] = { ' ', ',', '\n', '\0' };
    memset(buffer, '\0', g_file_size * sizeof(char) + ALLOCATION_BUFFER);
    memcpy(buffer, g_pa_file_reader, g_file_size + ALLOCATION_BUFFER);
    p = buffer;
    token = strtok(p, delims);
    while (token != NULL) {
        token = strtok(NULL, delims);
    }
    for (i = 0; i < (size_t)g_file_size; ++i) {
        if (buffer[i] == ' ' || buffer[i] == '.' || buffer[i] == ',' || buffer[i] == '?' || buffer[i] == '!' || buffer[i] == '\n') {
            for (j = i; j < (size_t)g_file_size; ++j) {
                buffer[j] = buffer[j + 1];
            }
        }
    }
    memcpy(g_pa_words, buffer, g_file_size);
    free(buffer);
    buffer = NULL;
}

void get_sentences_list_malloc(void)
{
    size_t i;
    char** buffer = (char**)malloc(g_words_count * sizeof(const char*) + ALLOCATION_BUFFER);
    char* p = g_pa_words;
    memset(buffer, '\0', g_words_count * sizeof(const char*) + ALLOCATION_BUFFER);
    for (i = 0; i < g_words_count; ++i) {
        buffer[i] = p;
        if (i == g_words_count - 1) {
            goto done;
        }
        while (*p != '\0') {
            p++;
        }
        p++;
    }
done:
    memcpy(g_pa_sentences, buffer, g_words_count * sizeof(char*));
    free(buffer);
    buffer = NULL;
}

void get_paragraphs_list_malloc(void)
{
    char*** buffer = (char***)malloc(g_sentences_count * sizeof(const char**) + ALLOCATION_BUFFER);
    char** p = g_pa_sentences;
    char* q = g_pa_file_reader;
    size_t count = 1;
    size_t i;
    memset(buffer, '\0', g_sentences_count * sizeof(const char**) + ALLOCATION_BUFFER);
    for (i = 0; i < g_sentences_count; ++i) {
        buffer[i] = p;
        if (i == g_sentences_count - 1) {
            goto finish;
        }
        while (*q != '!' && *q != '.' && *q != '?') {
            if (*q == '\n') {
                count = 1;
            }
            if (*q == ' ') {
                count++;
            }
            q++;
        }
        q++;
        p += count;
        count = 0;
    }
finish:
    memcpy(g_pa_paragraphs, buffer, g_sentences_count * sizeof(const char**));
    free(buffer);
    buffer = NULL;
}

void get_document_list_malloc(void)
{
    char**** buffer = (char****)malloc(g_paragraphs_count * sizeof(const char***) + ALLOCATION_BUFFER);
    char*** p = g_pa_paragraphs;
    char* q = g_pa_file_reader;
    size_t count = 0;
    size_t i;
    memset(buffer, '\0', g_paragraphs_count * sizeof(const char***) + ALLOCATION_BUFFER);
    for (i = 0; i < g_paragraphs_count; ++i) {
        buffer[i] = p;
        if (i == g_paragraphs_count - 1) {
            goto out;
        }
        while (*q != '\n' && *q != '\0') {
            if (*q == '.' || *q == '?' || *q == '!') {
                count++;
            }
            q++;
        }
        while (*q == '\n') {
            q++;
        }
        p += count;
        count = 0;
    }
out:
    memcpy(g_pa_document, buffer, g_paragraphs_count * sizeof(const char***));
    free(buffer);
    buffer = NULL;
}

const char*** get_paragraph(const size_t paragraph_index)
{
    if (g_pa_file_reader == NULL) {
        return NULL;
    }
    if (g_file_size == 0) {
        return NULL;
    }
    if (paragraph_index > g_paragraphs_count - 1) {
        return NULL;
    }
    g_current_paragraph_index = paragraph_index;
    return g_pa_document[paragraph_index];
}

size_t get_paragraph_word_count(const char*** paragraph)
{
    size_t memory_length;
    const char** p;
    const char** q;
    if (g_current_paragraph_index == g_paragraphs_count - 1) {
        p = *paragraph;
        q = *(g_pa_document[0]);
        memory_length = p - q;
        return g_words_count - memory_length;
    }
    p = *paragraph;
    q = *(g_pa_document[g_current_paragraph_index + 1]);
    memory_length = q - p;
    return memory_length;
}

size_t get_paragraph_sentence_count(const char*** paragraph)
{
    size_t memory_length;
    const char*** p;
    const char*** q;
    if (g_current_paragraph_index == g_paragraphs_count - 1) {
        p = paragraph;
        q = g_pa_document[0];
        memory_length = p - q;
        return g_sentences_count - memory_length;
    }
    p = paragraph;
    q = g_pa_document[g_current_paragraph_index + 1];
    memory_length = q - p;
    return memory_length;
}

const char** get_sentence(const size_t paragraph_index, const size_t sentence_index)
{
    const char*** paragraph;
    size_t paragraph_sentence_count;
    if (g_pa_file_reader == NULL) {
        return NULL;
    }
    if (g_file_size == 0) {
        return NULL;
    }
    if (g_file_size == 0) {
        return NULL;
    }
    if (paragraph_index > g_paragraphs_count - 1) {
        return NULL;
    }
    paragraph = get_paragraph(paragraph_index);
    paragraph_sentence_count = get_paragraph_sentence_count(paragraph);
    if (sentence_index > paragraph_sentence_count - 1) {
        return NULL;
    }
    g_current_paragraph_index = paragraph_index;
    g_current_sentence_index = sentence_index;
    return g_pa_document[paragraph_index][sentence_index];
}

size_t get_sentence_word_count(const char** sentence)
{
    const char*** paragraph = g_pa_document[g_current_paragraph_index];
    size_t paragraph_sentence_count = get_paragraph_sentence_count(paragraph);
    size_t memory_length;
    const char** p;
    const char** q;

    if (g_current_paragraph_index == g_paragraphs_count - 1 && g_current_sentence_index == paragraph_sentence_count - 1) {
        p = sentence;
        q = g_pa_document[0][0];
        memory_length = p - q;
        return g_words_count - memory_length;
    }
    if (g_current_sentence_index == paragraph_sentence_count - 1) {
        p = sentence;
        q = g_pa_document[g_current_paragraph_index + 1][0];
        memory_length = q - p;
        return memory_length;
    }
    p = sentence;
    q = g_pa_document[g_current_paragraph_index][g_current_sentence_index + 1];
    memory_length = q - p;
    return memory_length;
}

int print_as_tree(const char* filename)
{
    FILE* file;
    size_t paragraph_sentence_count;
    size_t sentence_word_count;
    size_t i;
    size_t j;
    size_t k;
    if (g_pa_file_reader == NULL) {
        fprintf(stderr, "No input file");
        return FALSE;
    }
    if (g_file_size == 0) {
        fprintf(stderr, "Input file is blank file");
        return FALSE;
    }
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("error while opeining file");
        return FALSE;
    }
    for (i = 0; i < g_paragraphs_count; ++i) {
        if (i != 0) {
            fprintf(file, "\n");
        }
        fprintf(file, "Paragraph %d:\n", (int)i);
        g_current_paragraph_index = i;
        paragraph_sentence_count = get_paragraph_sentence_count(g_pa_document[i]);
        for (j = 0; j < paragraph_sentence_count; ++j) {
            fprintf(file, "    Sentence %d:\n", (int)j);
            g_current_sentence_index = j;
            sentence_word_count = get_sentence_word_count(g_pa_document[i][j]);
            for (k = 0; k < sentence_word_count; ++k) {
                if (i == g_paragraphs_count - 1 && j == paragraph_sentence_count - 1 && k == sentence_word_count - 1) {
                    fprintf(file, "        %s", g_pa_document[i][j][k]);
                } else {
                    fprintf(file, "        %s\n", g_pa_document[i][j][k]); 
                }
            }
        }
    }
    if (fclose(file) == EOF) {
        perror("error while closing file");
        return FALSE;
    }
    return TRUE;
}
