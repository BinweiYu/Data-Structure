#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define WORD_LEN 32
#define DEF_LEN 256
#define COMMAND_LEN 32
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
			printf("A duplicated key is added.\n"); 
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
	/* Empty tree */
	if(node->left == NULL)
	{
		printf("The dictionary is empty.\n");
		return FAILURE;
	}

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
	if(cmp_res == 0)
	{
		printf("%s\n", node->definition);
		return SUCCESS;
	}
	else
	{
		printf("Word \"%s\" doesn't exist.\n", word);
		return FAILURE;
	}
}

int delete_word(Tree_node ** dictionary, Tree_node * parent, char * word)
{
	Tree_node* node = *dictionary, *other_node;
	/* Empty tree */
	if(NULL == node->left)
	{
		printf("The dictionary is empty\n");
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
		printf("Word \"%s\" doesn't exist.\n", word);
		return FAILURE;
	}
}

void print_dictionary(Tree_node * dictionary)
{
	if(dictionary != NULL)
	{
		/* Recursion */
		if(dictionary->right == NULL)
			printf("%s ", dictionary->word);
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
		if(error == FAILURE)
		{
			fclose(fp);
			fp = NULL;
			return FAILURE;
		}
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

	while(1)
	{
		int error;
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

			/* Adds to dictionary */
			error = add_word(&dictionary, word, definition);

			if( error == 0 )
				printf("The word \"%s\" has been added successfully.\n", word);
			else
				printf("Error when adding word \"%s\".\n", word);

		}
		/* Import file */
		if( strncmp(command, "import", COMMAND_LEN) == 0 )
		{
			char fname[DEF_LEN];
			scanf("%s",fname);
			error = import_file(fname, &dictionary );
			if( error == 0 )
				printf("The file \"%s\" has been imported successfully.\n", fname);
			else
				printf("Error when adding file \"%s\".\n", fname);
		}
		/* Delete word */
		if( strncmp(command, "delete", COMMAND_LEN) == 0 )
		{
			char word[WORD_LEN];
			scanf("%s",word);
			error = delete_word(&dictionary, dictionary, word);
			if( error == 0 )
				printf("The word \"%s\" has been deleted successfully.\n", word);
			else
				printf("Error when deleting word \"%s\".\n", word);
		}
		/* Find word */
		if( strncmp(command, "find", COMMAND_LEN) == 0 )
		{
			char word[WORD_LEN];
			scanf("%s",word);
			error = find_word(dictionary, word);
			if( error == 1 )
				printf("Error when finding word \"%s\".\n", word);
		}
		/* Print dictionary */
		if( strncmp(command, "print", COMMAND_LEN) == 0 )
		{
			print_dictionary(dictionary);
			printf("\n");
		}
		/* clear dictionary */
		if( strncmp(command, "clear", COMMAND_LEN) == 0 )
		{
			printf("The dictionary has been deleted successfully.\n");
			clear_dictionary(&dictionary);
		}
		/* quit program */
		if( strncmp(command, "quit", COMMAND_LEN) == 0 )
			break;
	}

	return 0;
}
