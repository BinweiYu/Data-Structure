#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<time.h>

#define WORD_LEN 32
#define DEF_LEN 256
#define COMMAND_LEN 32
#define WORD_NUM 90000
#define SUCCESS 0
#define FAILURE 1

typedef struct tree_node{
	struct tree_node * left;
	struct tree_node * right;
	char word[WORD_LEN];      /* key   */
	char definition[DEF_LEN]; /* value */
} Tree_node;

int add_word(Tree_node ** dictionary, char * word, char * definition)
{
	Tree_node* node = *dictionary;
	/* Empty tree */
	if(node == NULL)
	{
		(*dictionary) = (Tree_node* )malloc(sizeof(Tree_node));
		strcpy((*dictionary)->word, word);
		strcpy((*dictionary)->definition, definition);
		(*dictionary)->left = NULL;
		(*dictionary)->right = NULL;
		return SUCCESS;
	}
	/* While not leaf */
	while(node != NULL)
	{
		/* Compare words */
		size_t size = strlen(node->word) > strlen(word)? strlen(node->word) : strlen(word);
		int cmp_res = strncmp(word, node->word, size);
		if(cmp_res == 0)
		{
			/* A duplicated key is added */
			return FAILURE;
		}
		/* Go Left */
		else if(cmp_res < 0)
		{
			/* Leaf */
			if(node->right == NULL)
			{
				/* Create right child */
				Tree_node* new_right_child = (Tree_node* )malloc(sizeof(Tree_node));
				strcpy(new_right_child->word, node->word);
				strcpy(new_right_child->definition, node->definition);
				new_right_child->left = NULL;
				new_right_child->right = NULL;
								
				/* Create left child */
				Tree_node* new_left_child = (Tree_node* )malloc(sizeof(Tree_node));
				strcpy(new_left_child->word, word);
				strcpy(new_left_child->definition, definition);
				new_left_child->left = NULL;
				new_left_child->right = NULL;

				/* Modify parent */
				strcpy(node->definition, "");
				node->right = new_right_child;
				node->left = new_left_child;

				return SUCCESS;
			}
			else
				node = node->left; 
		}
		/* Go right */
		else
		{
			/* Leaf */
			if(node->right == NULL)
			{
				/* Create left child */
				Tree_node* new_left_child = (Tree_node* )malloc(sizeof(Tree_node));
				strcpy(new_left_child->word, node->word);
				strcpy(new_left_child->definition, node->definition);
				new_left_child->left = NULL;
				new_left_child->right = NULL;
				
				/* Create right child */
				Tree_node* new_right_child = (Tree_node* )malloc(sizeof(Tree_node));
				strcpy(new_right_child->word, word);
				strcpy(new_right_child->definition, definition);
				new_right_child->left = NULL;
				new_right_child->right = NULL;

				/* Modify parent */
				strcpy(node->definition, "");
				strcpy(node->word, word);
				node->left = new_left_child;
				node->right = new_right_child;

				return SUCCESS;
			}
			else
				node = node->right;
		}
	}
	return SUCCESS;
}

int find_word(Tree_node * dictionary, char * word)
{
	Tree_node* node = dictionary;
	if(NULL == node)
		return FAILURE;

	/* while not leaf */
	while(NULL != node->right)
	{
		size_t size = strlen(node->word) > strlen(word)? strlen(node->word) : strlen(word);
		int cmp_res = strncmp(word, node->word, size);
		/* Go left */
		if(cmp_res < 0)
		{
			node = node->left;
		}
		/* Go right */
		else
		{
			node = node->right;
		}
	}
	/* Leaf */
	size_t size = strlen(node->word) > strlen(word)? strlen(node->word) : strlen(word);
	int cmp_res = strncmp(word, node->word, size);
	if(0 == cmp_res)
	{
		//printf("%s\n", node->definition);	
		return SUCCESS;
	}
	else
	{
		printf("1 ");
		return FAILURE;
	}
}

int delete_word(Tree_node ** dictionary, Tree_node * parent, char * word)
{
	Tree_node* node = *dictionary, *other_node;
	/* Empty tree */
	if(NULL == node->left)
	{
		return FAILURE;
	}
	
	/* While not leaf */
	while(NULL != node->right)
	{
		size_t size = strlen(node->word) > strlen(word)? strlen(node->word) : strlen(word);
		int cmp_res = strncmp(word, node->word, size);
		/* Go left */
		if(cmp_res < 0)
		{
			parent = node;
			other_node = node->right;
			node = node->left;
		}
		/* Go right */
		else
		{
			parent = node;
			other_node = node->left;
			node = node->right;
		}
	}
	/* Leaf */
	size_t size = strlen(node->word) > strlen(word)? strlen(node->word) : strlen(word);
	int cmp_res = strncmp(word, node->word, size);
	if(cmp_res == 0)
	{
		strcpy(parent->word, other_node->word);
		if(NULL == other_node->right)
		{
			strcpy(parent->definition, other_node->definition);
		}
		parent->left = other_node->left;
		parent->right = other_node->right;

		/* Free memory of deleted node */
		free(node);

		return SUCCESS;
	}
	else
	{
		return FAILURE;
	}
}

void print_dictionary(Tree_node * dictionary)
{
	if(dictionary != NULL)
	{
		/* Recursion */
		if(dictionary->right == NULL)
		{	
			printf("%s ", dictionary->word);
		}
		print_dictionary(dictionary->left);
		print_dictionary(dictionary->right);
	}
}

void clear_dictionary(Tree_node ** dictionary)
{
	Tree_node* node = *dictionary;
	if(NULL == node)
		return;
	if(NULL != node->left)
		clear_dictionary(&node->left);
	if(NULL != node->right)
		clear_dictionary(&node->right);
	/* Leaf */
	free(node);
	*dictionary = NULL;
}

int import_file(char * fname, Tree_node ** dictionary )
{
	FILE* fp = fopen(fname, "r");
	if(NULL == fp)
	{
		printf("Failed to open file: %s\n", strerror(errno));
		return FAILURE;
	}
	char word[WORD_LEN];
	char definition[DEF_LEN];
	char* pos;
	int error;
	while(EOF != fscanf(fp, "%s", word))
	{
		fgets(definition, DEF_LEN, fp);
		if((pos = strchr(definition, '\n')) != NULL)
			*pos = '\0';
		error = add_word(dictionary, word, definition);
		/*
		 * Ignore error when duplicated word appears
		 *
		 * if(error == FAILURE)
		 * {
		 *	fclose(fp);
		 *	fp = NULL;
		 *	return FAILURE;
		 * }
		 */
		memset(word, 0x00, sizeof(char) * WORD_LEN);
		memset(definition, 0x00, sizeof(char) * DEF_LEN);
	}

	fclose(fp);
	fp = NULL;
	return SUCCESS;
}

int main(void)
{
	Tree_node * dictionary = NULL;

	/* Generate sample array */
	char samples[WORD_NUM][WORD_LEN];
	FILE* fp = fopen("alphabetized_dictionary.txt", "r");
	if(NULL == fp)
	{
		printf("Failed to open file: %s\n", strerror(errno));
		return FAILURE;
	}
	char definition[DEF_LEN];
	size_t index = 0;
	while(EOF != fscanf(fp, "%s", samples[index]))
	{
		fgets(definition, DEF_LEN, fp);
		memset(definition, 0x00, sizeof(char) * DEF_LEN);
		index++;
	}
	fclose(fp);
	fp = NULL;

	int indicies[100000];
	srand((unsigned)time(NULL));
	for(int i = 0; i < 100000; i++)
	{
		indicies[i] = rand() % index;
	}

	/* Time of importing file */
	clock_t start = clock();
	import_file("alphabetized_dictionary.txt", &dictionary);
	clock_t end = clock();
	printf("Time cost of 'import' in naive BST: \t%f(s)\n", (end - start) * 1.0 / CLOCKS_PER_SEC);
	
	/* Time of finding definition */
	start = clock();
	for(int i = 0; i < 100000; i++)
	{
		find_word(dictionary, samples[indicies[i]]);
	}
	end = clock();
	printf("Time cost of 'find' in naive BST: \t%f(us)\n", (end - start) * 1000000.0 / CLOCKS_PER_SEC / 100000);

	/* Free memory */
	clear_dictionary(&dictionary);
	return 0;
}

