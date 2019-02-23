#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hash.h"
#include "errno.h"

typedef struct hash_table_t {
    int size;
    int used_size;
    int word_cnt;
    unsigned int (*hash)(char* , int);
    vector* bins;
} hash_table;

unsigned int naive_hash     (char* , int);
unsigned int bernstein_hash (char* , int);
unsigned int FNV_hash       (char* , int);
unsigned int universal_hash (char* , int);
void hash_table_insert      (hash_table* , char* , char* );
void hash_table_rehash      (hash_table* , int);
list_node* hash_table_find  (hash_table* , char* );

static int a[WORD_LEN];

int cmp_aux(const void* a, const void* b) {
    //printf("%s, %s\n", ((list_node* )a)->word, ((list_node* )b)->word);
    list_node* l1 = *(list_node** )a;
    list_node* l2 = *(list_node** )b;
    if(l1 == NULL && l2 == NULL)
        return 0;
    else if(l1 == NULL)
        return -1;
    else if(l2 == NULL)
        return 1;
    else
        return strcmp((*(list_node** )a)->word, (*(list_node** )b)->word);
}

void generate_a_aux(int nbins) {
    srand(time(NULL));
    for(int i = 0; i < WORD_LEN; i++)
        a[i] = rand() % nbins;
}

int get_prime_aux(int x) {
    while(++x) {
        int i;
        for(i = 2; i <= x; i++) {
            if(x % i == 0)
                break;
        }
        if(i == x)
            return x;
    }
    return -1;
}

/*===---------------------- CREATE ----------------------===*/
hash_table* hash_table_create(int size, unsigned int (*hash)(char* , int)) {
    if(size > 1) {
        hash_table* HT = (hash_table* )malloc(sizeof(hash_table));
        HT->bins = vector_create(size);
        HT->size = size;
        HT->used_size = 0;
        HT->word_cnt = 0;
        HT->hash = hash;
        return HT;
    }
    else {
        printf("error: initial size must be bigger than 1\n");
        return NULL;
    }
}

/*===---------------------- INSERT ----------------------===*/
void hash_table_insert(hash_table* HT, char* word, char* definition) {
    /* Get position of word */
    unsigned int pos = HT->hash(word, HT->size);
    if(HT->bins->data[pos].size == 0)
        ++(HT->used_size);
    
    list_node* res = hash_table_find(HT, word);
    if(res) {
        /* Clear */
        int word_len = strlen(word);
        memcpy(res->word, word, sizeof(char) * word_len);
        int def_len = strlen(definition);
        memcpy(res->definition, definition, sizeof(char) * def_len);
    }
    else {
        list_append(&HT->bins->data[pos], word, definition);
        ++HT->word_cnt;
        
        /* Universal hash */
        if(HT->hash == universal_hash) {
            // TODO
            int list_size = HT->bins->data[pos].size;
            // printf("@ HASH: list-size: %d\n", list_size);
            /* 1) more than 25% of entries in a single bin
             * 2) more than 10 entries
             */
            if((HT->word_cnt * 1.0 / HT->size >= 0.75) ||
               (HT->word_cnt > 10 && HT->word_cnt * 0.25 < list_size)) {
                int new_nbins = get_prime_aux(HT->size * 2);
                generate_a_aux(new_nbins);
                //printf("@ HASH: rehash to: %d\n", new_nbins);
                hash_table_rehash(HT, new_nbins);
            }
        }
        else {
            /* Calculate occupacy */
            if(HT->word_cnt * 1.0 / HT->size >= 0.75)
                hash_table_rehash(HT, HT->size * 2);
        }
    }
}

/*===---------------------- REHASH ----------------------===*/
void hash_table_rehash(hash_table* HT, int size) {
    vector* new_v = vector_create(size);
    vector* old_v = HT->bins;
    HT->bins = new_v;
    HT->size = size;
    HT->used_size = 0;
    HT->word_cnt = 0;
    
    /* Project */
    for(int i = 0; i < old_v->size; i++) {
        list_node* node = old_v->data[i].head;
        while(NULL != node) {
            hash_table_insert(HT, node->word, node->definition);
            node = node->next;
        }
    }
    
    /* Free old hash table */
    for(int i = 0; i < old_v->size; i++) {
        list_destroy(&old_v->data[i]);
    }
    free(old_v->data);
    free(old_v);
}

/*===---------------------- FIND ----------------------===*/
list_node* hash_table_find(hash_table* HT, char* word) {
    /* Get position of word */
    unsigned int pos = HT->hash(word, HT->size);
    
    return list_find(&HT->bins->data[pos], word);
}

/*===---------------------- DELETE ----------------------===*/
int hash_table_delete(hash_table* HT, char* word) {
    /* Get position of word */
    unsigned int pos = HT->hash(word, HT->size);
    if(HT->bins->data[pos].size == 1)
        --HT->used_size;
    --HT->word_cnt;
    return list_remove_word(&HT->bins->data[pos], word);
}

/*===---------------------- PRINT CATALOG ----------------------===*/
void hash_table_print_catalog(hash_table* HT) {
    for(int i = 0; i < HT->size; i++) {
        list_node* node = HT->bins->data[i].head;
        printf("[%d] ", i);
        while(node) {
            printf("-> %s ", node->word);
            node = node->next;
        }
        printf("\n");
    }
}

/*===---------------------- PRINT ----------------------===*/
void hash_table_print(hash_table* HT) {
    list_node** container = (list_node** )malloc(sizeof(list_node* ) * HT->word_cnt);
    int idx = 0;
    for(int i = 0; i < HT->size; i++) {
        list_node* node = HT->bins->data[i].head;
        while(node) {
            container[idx] = node;
            ++idx;
            node = node->next;
        }
    }
    
    qsort(container, HT->word_cnt, sizeof(list_node* ), cmp_aux);
    for(int i = 0; i < HT->word_cnt; i++) {
        if(container[i])
            printf("%s %s\n", container[i]->word, container[i]->definition);
    }
}

/*===---------------------- DESTROY ----------------------===*/
void hash_table_destroy(hash_table* HT) {
    for(int i = 0; i < HT->size; i++) {
        list_destroy(&HT->bins->data[i]);
        HT->bins->data[i].size = 0;
        HT->bins->data[i].head = NULL;
        HT->bins->data[i].tail = NULL;
    }
    HT->used_size = 0;
    HT->word_cnt = 0;
}

int main(int argc, char* argv[], char* env[]) {
    /* Filter initial input */
    if(argc != 2) {
        printf("error: please select a hash function\n");
        printf("\t\t\t1: naive hash\n");
        printf("\t\t\t2: bernstein hash\n");
        printf("\t\t\t3: FVN hash\n");
        printf("\t\t\t4: stochastic universal hash\n");
        return EXIT_FAILURE;
    }
    
    int hash_mode = atoi(argv[1]);
    hash_table* HT = NULL;
    switch(hash_mode) {
        case 1: HT = hash_table_create(16, naive_hash); break;
        case 2: HT = hash_table_create(16, bernstein_hash); break;
        case 3: HT = hash_table_create(16, FNV_hash); break;
        case 4:
            HT = hash_table_create(get_prime_aux(WORD_LEN), universal_hash); break;
        default:
            printf("error: invalid hash function\n");
            return EXIT_FAILURE;
    }
    
    while(1)
    {
        char command[COMMAND_LEN];
        
        /* Get the User's Command */
        printf(">$ ");
        scanf("%s", command);
        
        /* Add word */
        if( strncmp(command, "add", COMMAND_LEN) == 0 )
        {
            char word[WORD_LEN];
            char definition[DEF_LEN];
            char * pos;
            scanf("%s ",word);
            /* Gets the rest of the input */
            fgets(definition, DEF_LEN, stdin);
            if ((pos=strchr(definition, '\n')) != NULL)
                *pos = '\0';
            
            /* Adds to table */
            hash_table_insert(HT, word, definition);
            
            printf("prompt: successfully add word '%s'\n", word);
        }
        /* Import file */
        else if( strncmp(command, "import", COMMAND_LEN) == 0 )
        {
            char fname[DEF_LEN];
            scanf("%s",fname);
            
            FILE* fp = fopen(fname, "r");
            if(NULL == fp)
                printf("error: failed to open file '%s'\n", strerror(errno));
            else {
                char word[WORD_LEN];
                char definition[DEF_LEN];
                char* pos;
                while(EOF != fscanf(fp, "%s", word))
                {
                    fgets(definition, DEF_LEN, fp);
                    if((pos = strchr(definition, '\n')) != NULL)
                        *pos = '\0';
                    hash_table_insert(HT, word, definition);
                    memset(word, 0x00, sizeof(char) * WORD_LEN);
                    memset(definition, 0x00, sizeof(char) * DEF_LEN);
                }
                
                fclose(fp);
                fp = NULL;
                printf("prompt: successfully import file '%s'\n", fname);
            }
        }
        /* Delete word */
        else if( strncmp(command, "delete", COMMAND_LEN) == 0 )
        {
            char word[WORD_LEN];
            scanf("%s",word);
            int res = hash_table_delete(HT, word);
            if(!res)
                printf("warning: word '%s' doesn't exist\n", word);
            else
                printf("propmt: successfully delete word '%s'\n", word);
        }
        /* Find word */
        else if( strncmp(command, "find", COMMAND_LEN) == 0 )
        {
            char word[WORD_LEN];
            scanf("%s", word);
            list_node* res = hash_table_find(HT, word);
            if( !res )
                printf("prompt: cannot find word '%s'\n", word);
            else {
                printf("\n+------------------------------------\n");
                printf("%s: %s\n", res->word, res->definition);
                printf("+------------------------------------\n\n");
            }
        }
        /* Print catalog */
        else if( strncmp(command, "print-catalog", COMMAND_LEN) == 0 )
        {
            hash_table_print_catalog(HT);
        }
        /* Print dictionary */
        else if( strncmp(command, "print", COMMAND_LEN) == 0 )
        {
            hash_table_print(HT);
        }
        /* Print stats */
        else if( strncmp(command, "stats", COMMAND_LEN) == 0 )
        {
            printf("Bins: %d  ", HT->size);
            printf("occupancy: %lf  ", HT->word_cnt * 1.0 / HT->size);
            printf("used bin fraction: %lf  ", HT->used_size * 1.0 / HT->size);
            int max_len = 0;
            for(int i = 0; i < HT->size; i++) {
                int len = HT->bins->data[i].size;
                max_len = len > max_len? len : max_len;
            }
            printf("max entries in a bin: %d\n", max_len);
        }
        /* clear dictionary */
        else if( strncmp(command, "clear", COMMAND_LEN) == 0 )
        {
            hash_table_destroy(HT);
            printf("prompt: successfully clear\n");
        }
        /* quit program */
        else if( strncmp(command, "quit", COMMAND_LEN) == 0 ) {
            if(HT) {
                hash_table_destroy(HT);
                free(HT->bins);
                free(HT);
            }
            break;
        }
    }
    
    return EXIT_SUCCESS;
}




/* Naive hash */
unsigned int naive_hash(char * word, int nbins) {
    unsigned int h = 0;
    int c;
    while((c = *(word++)))
        h += c;
    return h % nbins;
}

/* Berstein hash */
unsigned int bernstein_hash(char * word, int nbins)
{
    unsigned int h = 5381;
    int c;
    while((c = *(word++)))
        h = 33 * h + c;
    return h % nbins;
}

/* FNV hash */
unsigned int FNV_hash(char * word, int nbins)
{
    unsigned long h = 14695981039346656037lu;
    char c;
    while((c = *(word++)))
    {
        h = h * 1099511628211lu;
        h = h ^ c; }
    return h % nbins;
}

/* Universal hash */
unsigned int universal_hash(char* word, int nbins) {
    unsigned int h = 0;
    char c;
    int idx = 0;
    while((c = *(word++))) {
        h += a[idx] * (c % nbins);
        ++idx;
    }
    return h % nbins;
}
