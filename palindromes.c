#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <omp.h>
#define MAXSIZE 10000
#define MAXWORKERS 8

int count;

char *buf1, *buf2, *tmp, *palindromic;

FILE *file, *pFile, *file2, *write;

long eoFile, soFile;

static void delChar(char *str, char trash) {

    char *src, *dst;

    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != trash) dst++;
    }
    *dst = '\0';
}

static void *reverse(char *word){

    char *p1, *p2;

    if (! word || ! *word) return word;

    for (p1 = word, p2 = word + strlen(word) - 2; p2 > p1; ++p1, --p2){
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return word;
}

static int ls(char *wrd, char *argv, long l, long h, char *temp){

    fgets(buf1, 100, pFile);

    if(strcasecmp(wrd, buf1) == 0){
        count++;
        return 1;
    }
    if(l >= h) return -1;

    l = ftell(pFile);

    return ls(wrd, argv, l, h, temp);

}

int bs(char *word, char *argv, long low , long high, char *tmpword){

    if(high == low) return 0;

    if((high - low)<= 1000){
        fseek(pFile, low, SEEK_SET);
        return ls(word, argv, low, high, tmpword);
    }

    long mid = (low + high)/2;

    if(pFile== NULL) printf("fucked up\n");

    fseek(pFile, mid ,SEEK_SET);

    while(fgetc(pFile)!= '\n') fseek(pFile, -2, SEEK_CUR);

    mid = ftell(pFile);

    fgets(buf1, 100, pFile);

    for(size_t i = 0; word[i] != '\0'; ++i) {

        if(word[i] > 32 && word[i] < 48){

            delChar(tmpword, word[i]);

            if(strcasecmp(buf1, tmpword) == 0){

                count++;

                strcpy(tmpword,word);

                return 1;
            }
            if(strcasecmp(buf1, tmpword) > 0){

                high = mid;

                strcpy(tmpword,word);

                return bs(word, argv, low, high, tmpword);
            }else{
                low = ftell(pFile);

                strcpy(tmpword,word);

                return bs(word, argv, low, high, tmpword);
            }
        }
    }
    if(strcasecmp(word, buf1) == 0){

        count++;

        return 1;
    }
    if(strcasecmp(word, buf1) > 0){

        low = ftell(pFile);

        return bs(word, argv, low, high, tmpword);
    }else{
        high = mid;

        return bs(word, argv, low, high, tmpword);
    }
}
int helpMalloc(char *wrd, char *argv, long l , long h, char *temp){

    buf1 = malloc(100);

    rewind(pFile);

    if((h - l) <= 1000) return ls(wrd, argv, l, h, temp);

    return bs(wrd, argv, l, h, temp);
}

int main(int argc, char *argv[]) {

    file = fopen(argv[1], "r");

    soFile = ftell(file);

    fseek(file, 0, SEEK_END);

    eoFile = ftell(file);

    file2 = fopen(argv[2], "r");

    write = fopen(argv[3], "w");

    buf2 = malloc(100);

    tmp = malloc(100);

    palindromic = malloc(100);

    int i;

    pFile = fopen(argv[1], "r");

    int numWorkers = (argc > 4)? atoi(argv[4]) : MAXWORKERS;

    omp_set_num_threads(numWorkers);

#pragma omp parallel for private(pFile)
    for(i = 0; i< 25143;i++){

        fgets(buf2, 100, file2);

        int result;

        strcpy(tmp, buf2);

        strcpy(palindromic, buf2);

        reverse(buf2);

        result = helpMalloc(buf2, argv[1], soFile, eoFile, tmp);

        if(result == 1){
#pragma omp critical
            {
                fprintf(write, "%s", palindromic);
                fflush(write);
            }
        }
    }
    fclose(write);
    printf("%d\n", count);
}
