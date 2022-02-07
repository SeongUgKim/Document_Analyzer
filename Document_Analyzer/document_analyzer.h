#ifndef DOCUMENT_ANALYZER_H
#define DOCUMENT_ANALYZER_H
#define TRUE (1)
#define FALSE (0)

extern char* g_pa_file_reader;
extern char* g_pa_words;
extern char** g_pa_sentences;
extern char*** g_pa_paragraphs;
extern const char**** g_pa_document;
extern long g_file_size;
extern size_t g_words_count;
extern size_t g_sentences_count;
extern size_t g_paragraphs_count;
extern size_t g_current_paragraph_index;
extern size_t g_current_sentence_index;

void allocate_memory_malloc(void);

int load_document(const char* document);

void dispose(void);

size_t count_words_or_sentences_or_paragraphs(const char* delims);

size_t get_total_word_count(void);

size_t get_total_sentence_count(void);

size_t get_total_paragraph_count(void);

void get_words_list_malloc(void);

void get_sentences_list_malloc(void);

void get_paragraphs_list_malloc(void);

void get_document_list_malloc(void);

const char*** get_paragraph(const size_t paragraph_index);

size_t get_paragraph_word_count(const char*** paragraph);

size_t get_paragraph_sentence_count(const char*** paragraph);

const char** get_sentence(const size_t paragraph_index, const size_t sentence_index);

size_t get_sentence_word_count(const char** sentence);

int print_as_tree(const char* filename);

#endif /* DOCUMENT_ANALYZER_H */
