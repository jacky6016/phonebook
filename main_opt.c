#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>

#include IMPL
#define TABLE_SIZE 42737
//72073
//93563
//65213
//42737
#define DICT_FILE "./dictionary/words.txt"

entry *hashTable[TABLE_SIZE];
pthread_mutex_t locks[TABLE_SIZE];
int hash_stats[TABLE_SIZE] = {0};

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main(int argc, char *argv[])
{
    FILE **fps;
    struct timespec start, end;
    double cpu_time1, cpu_time2;

    int threadnum = 10;
    /*
    	printf("Specify number of worker threads: ");
    	scanf("%d", &threadnum);
    */
    /* check file opening */
    int k;
    fps = (FILE **)malloc(threadnum * sizeof(FILE *));
    for(k = 0; k < threadnum; k++) {
        fps[k] = fopen(DICT_FILE, "r");
        if (fps[k] == NULL) {
            printf("cannot open the file\n");
            return -1;
        }
    }

    /* build the entry */
    entry *pHead, *e;
    pHead = (entry *) malloc(sizeof(entry));
    printf("size of entry : %lu bytes\n", sizeof(entry));
    e = pHead;
    e->pNext = NULL;

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif


    pthread_t *threads = (pthread_t *)malloc(threadnum * sizeof(pthread_t));
    thread_data **param = (thread_data **)malloc(threadnum * sizeof(thread_data *));

    for(int k=0; k < TABLE_SIZE; k++) {
        if (pthread_mutex_init(&locks[k], NULL) != 0) {
            printf("\n mutex init failed\n");
            return 1;
        }
    }

    clock_gettime(CLOCK_REALTIME, &start);

    int err;
    for(k = 0; k < threadnum; k++) {
        param[k] = pack_param(fps[k], k, threadnum);
        err = pthread_create(&threads[k], NULL, (void *) &thread_work, (void *) param[k]);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
    }

    for(k = 0; k < threadnum; k++) {
        pthread_join(threads[k], NULL);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);

    /* close file as soon as possible */
    for(k = 0; k < threadnum; k++) {
        fclose(fps[k]);
    }

    for(int k=0; k < TABLE_SIZE; k++) {
        // printf("%d\n", hash_stats[k]);
        pthread_mutex_destroy(&locks[k]);
    }
    e = pHead;

    /* the givn last name to find */
    char input[MAX_LAST_NAME_SIZE] = "zyxel";
    e = pHead;

    assert(findName(input, e) &&
           "Did you implement findName() in " IMPL "?");
    assert(0 == strcmp(findName(input, e)->lastName, "zyxel"));

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
    /* compute the execution time */
    clock_gettime(CLOCK_REALTIME, &start);
    findName(input, e);
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);

    FILE *output;
#if defined(OPT)
    output = fopen("opt.txt", "a");
#else
    output = fopen("orig.txt", "a");
#endif
    fprintf(output, "append() findName() %lf %lf\n", cpu_time1, cpu_time2);
    fclose(output);

    printf("execution time of append() : %lf sec\n", cpu_time1);
    printf("execution time of findName() : %lf sec\n", cpu_time2);

    if (pHead->pNext) free(pHead->pNext);
    free(pHead);

    return 0;
}
