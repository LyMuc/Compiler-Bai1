#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN 50
#define MAX_LINES 10000
#define MAX_STOPWORDS 100

typedef struct {
    char word[MAX_WORD_LEN];
    int count;
    int *lines; 
    int lineCount;
    int lineCapacity; 
} WordItem;

char stopWords[MAX_STOPWORDS][MAX_WORD_LEN];
int stopWordCount = 0;

WordItem *indexTable = NULL; 
int indexSize = 0;
int indexCapacity = 0; 

void loadStopWords(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Loi: Khong the mo file %s. Hay tao file nay truoc.\n", filename);
        exit(1);
    }
    char buffer[MAX_WORD_LEN];
    while (fscanf(f, "%s", buffer) != EOF) {
        for(int i = 0; buffer[i]; i++) buffer[i] = tolower(buffer[i]);
        strcpy(stopWords[stopWordCount++], buffer);
    }
    fclose(f);
}

/**
 * @brief Kiem tra tu co nghia theo dinh nghia
 * @param rawWord: Tu can kiem tra
 * @param isStartOfSentence: Co phai dau cau khong
 * @return 1 neu tu hop le, 0 neu can loai bo
 */
int kiemTraTuCoNghia(char *rawWord, int isStartOfSentence) {
    if (isupper(rawWord[0]) && !isStartOfSentence) {
        return 0; 
    }

    char lowerWord[MAX_WORD_LEN];
    int len = strlen(rawWord);
    for(int i = 0; i < len; i++) {
        lowerWord[i] = tolower(rawWord[i]);
    }
    lowerWord[len] = '\0';

    for (int i = 0; i < stopWordCount; i++) {
        if (strcmp(lowerWord, stopWords[i]) == 0) {
            return 0; 
        }
    }

    strcpy(rawWord, lowerWord);
    return 1;
}

/**
 * @brief Chen tu vao bang chi muc
 * @param word: Tu can chen
 * @param line: Dong xuat hien
 */
void chenTuVaoDanhSach(char *word, int line) {
    int pos = -1;
    for (int i = 0; i < indexSize; i++) {
        if (strcmp(indexTable[i].word, word) == 0) {
            pos = i;
            break;
        }
    }

    if (pos != -1) {
        indexTable[pos].count++;
        if (indexTable[pos].lineCount >= indexTable[pos].lineCapacity) {
            indexTable[pos].lineCapacity = (indexTable[pos].lineCapacity == 0) ? 10 : indexTable[pos].lineCapacity * 2;
            indexTable[pos].lines = realloc(indexTable[pos].lines, indexTable[pos].lineCapacity * sizeof(int));
            if (!indexTable[pos].lines) {
                printf("Loi: Khong the cap phat bo nho cho lines.\n");
                exit(1);
            }
        }
        indexTable[pos].lines[indexTable[pos].lineCount++] = line;
    } 
    else {
        if (indexSize >= indexCapacity) {
            indexCapacity = (indexCapacity == 0) ? 1000 : indexCapacity * 2;
            indexTable = realloc(indexTable, indexCapacity * sizeof(WordItem));
            if (!indexTable) {
                printf("Loi: Khong the cap phat bo nho cho indexTable.\n");
                exit(1);
            }
        }

        int i = indexSize - 1;
        while (i >= 0 && strcmp(indexTable[i].word, word) > 0) {
            indexTable[i + 1] = indexTable[i];
            i--;
        }
        
        strcpy(indexTable[i + 1].word, word);
        indexTable[i + 1].count = 1;
        indexTable[i + 1].lineCount = 1;
        indexTable[i + 1].lineCapacity = 10; 
        indexTable[i + 1].lines = malloc(indexTable[i + 1].lineCapacity * sizeof(int));
        if (!indexTable[i + 1].lines) {
            printf("Loi: Khong the cap phat bo nho cho lines moi.\n");
            exit(1);
        }
        indexTable[i + 1].lines[0] = line;
        indexSize++;
    }
}


void inKetQua() {
    printf("\n--- KET QUA BANG CHI MUC ---\n");
    for (int i = 0; i < indexSize; i++) {
        printf("%-15s %d", indexTable[i].word, indexTable[i].count);
        
        for (int j = 0; j < indexTable[i].lineCount; j++) {
            printf(",%d", indexTable[i].lines[j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    loadStopWords("stopw.txt");
    if(argc < 2) {
        printf("Loi: Vui long cung cap ten file van ban.\n");
        return 1;
    }
    
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        printf("Loi: Khong tim thay file %s\n", argv[1]);
        return 1;
    }

    char currentWord[MAX_WORD_LEN];
    int currentLine = 1;
    int isStartOfSentence = 1;

    char ch;
    int len = 0;
    int c;
    
    while ((c = fgetc(f)) != EOF) {
        ch = (char)c;
        
        if (ch == '\n') currentLine++; 

        if (isalpha(ch)) {
            currentWord[len++] = ch;
        } else {
            if (len > 0) {
                currentWord[len] = '\0';
                
                if (kiemTraTuCoNghia(currentWord, isStartOfSentence)) {
                    chenTuVaoDanhSach(currentWord, currentLine);
                }
                
                isStartOfSentence = 0; 
                len = 0;
            }

            if (ch == '.' || ch == '?' || ch == '!') {
                isStartOfSentence = 1;
            }
        }
    }
    
    if (len > 0) {
        currentWord[len] = '\0';
        if (kiemTraTuCoNghia(currentWord, isStartOfSentence)) {
            chenTuVaoDanhSach(currentWord, currentLine);
        }
    }

    fclose(f);
    inKetQua();

    return 0;
}