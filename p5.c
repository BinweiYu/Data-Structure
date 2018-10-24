#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define WORD_LEN 32
#define DEF_LEN 256
#define COMMAND_LEN 32
#define MAX_TREE_DEPTH 200
#define SUCCESS 0
#define FAILURE 1

/* Note your red-black node will need a few more items than
 * your naive node did (e.g., color, parent) */
typedef enum color_type
{
	BLACK,
	RED
} color_t;

typedef struct tree_node{
	struct tree_node* left;
	struct tree_node* right;
	struct tree_node* parent;
	char word[WORD_LEN];
	char definition[DEF_LEN];
	color_t color;
} Tree_node;

/* Functions are defined here
 * */
void insert_case1(Tree_node*);
void insert_case2(Tree_node*);
void insert_case3(Tree_node*);
void insert_case4(Tree_node*);
void insert_case5(Tree_node*);

/* Create node */
Tree_node* create_node(char* word, char* definition)
{
	Tree_node* node = (Tree_node* )malloc(sizeof(Tree_node));
	Tree_node* NIL0 = (Tree_node* )malloc(sizeof(Tree_node));
	Tree_node* NIL1 = (Tree_node* )malloc(sizeof(Tree_node));
	strcpy(node->word, word);
	strcpy(node->definition, definition);
	node->left = NIL0;
	node->right = NIL1;
	node->color = RED;

	NIL0->left = NIL1->left = NULL;
	NIL0->right = NIL1->right = NULL;
	NIL0->parent = NIL1->parent = node;
	NIL0->color = NIL1->color = BLACK;
	return node;
}

/* Left Rotation */
void left_rotate(Tree_node* node)
{
	if(NULL == node || NULL == node->right)
		return;

	/* Record */
	Tree_node* temp_node = node->left;
	char temp_word[WORD_LEN];
	char temp_definition[DEF_LEN];
	color_t temp_color = node->color;
	strcpy(temp_word, node->word);
	strcpy(temp_definition, node->definition);

	/* Swap */
	strcpy(node->word, node->right->word);
	strcpy(node->definition, node->right->definition);
	strcpy(node->right->word, temp_word);
	strcpy(node->right->definition, temp_definition);
	node->color = node->right->color;
	node->right->color = temp_color;

	/* Rotate */
	node->left = node->right;
	node->right = node->left->right;
	node->left->right = node->left->left;
	node->left->left = temp_node;

	/* Modify parent info */
	if(NULL != node->right)
		node->right->parent = node;
	if(NULL != node->left->left)
		node->left->left->parent = node->left;
}

/* Right rotation */
void right_rotate(Tree_node* node)
{
	if(NULL == node || NULL == node->right)
		return;

	/* Record */
	Tree_node* temp_node = node->right;
	char temp_word[WORD_LEN];
	char temp_definition[DEF_LEN];
	color_t temp_color = node->color;
	strcpy(temp_word, node->word);
	strcpy(temp_definition, node->definition);

	/* Swap */
	strcpy(node->word, node->left->word);
	strcpy(node->definition, node->left->definition);
	strcpy(node->left->word, temp_word);
	strcpy(node->left->definition, temp_definition);
	node->color = node->left->color;
	node->left->color = temp_color;

	/* Rotate */
	node->right = node->left;
	node->left = node->right->left;
	node->right->left = node->right->right;
	node->right->right = temp_node;

	/* Modify parent info */
	if(NULL != node->left)
		node->left->parent = node;
	if(NULL != node->right->right)
		node->right->right->parent = node->right;
}

/* Find grandparent*/
Tree_node* find_grandparent(Tree_node* node)
{
	if(node->parent == NULL)
		return NULL;
	else return node->parent->parent;
}

/* Find uncle */
Tree_node* find_uncle(Tree_node* node)
{
	Tree_node* grandparent = find_grandparent(node);
	if(NULL == grandparent)
		return NULL;
	else
	{
		if(grandparent->left == node->parent)
			return grandparent->right;
		else
			return grandparent->left;
	}
}

/* Insert case 1 */
void insert_case1(Tree_node* node)
{
	/* If node is root, paint black */
	if(NULL != node)
	{
		if(NULL == node->parent)
		{
			node->color = BLACK;
		}
		else
			insert_case2(node);
	}
}

/* Insert case 2 */
void insert_case2(Tree_node* node)
{
	/* If parent is black, do nothing */
	if(BLACK == node->parent->color)
		return;
	else
		insert_case3(node);
}

/* Insert case 3 */
void insert_case3(Tree_node* node)
{
	Tree_node* grandparent = find_grandparent(node), * uncle = find_uncle(node);

	/* If both parent and uncle are red, flip color */
	if(RED == node->parent->color && RED == uncle->color)
	{
		node->parent->color = BLACK;
		uncle->color = BLACK;
		grandparent->color = RED;
		insert_case1(grandparent);
	}
	else
		insert_case4(node);
}

/* Insert case 4 */
void insert_case4(Tree_node* node)
{
	Tree_node* grandparent = find_grandparent(node);

	/* If node is right child and parent is grandparent's left child */
	if(node == node->parent->right && node->parent == grandparent->left)
	{
		left_rotate(node->parent);
		//node = node->left;
	}
	/* If node is left child and parent is grandparent's right child */
	else if(node == node->parent->left && node->parent == grandparent->right)
	{
		right_rotate(node->parent);
		//node = node->right;
	}
	insert_case5(node);
}

/* Insert case 5 */
void insert_case5(Tree_node* node)
{
	Tree_node* grandparent = find_grandparent(node);
	/* If node and parent are on the same side */
	node->parent->color = BLACK;
	grandparent->color = RED;
	if(node == node->parent->left)
		right_rotate(grandparent);
	else
		left_rotate(grandparent);
}

/* This is the same interface as was in Problem 4, but you will
 * need to implement a number of helper functions to achieve the
 * ability to self balance your dictionary whenever nodes are added.
 * The interfaces for these functions (e.g., rotations,
 * grandparent/sibling helper functions, node swaps, etc) are up
 * for you to design and implement. */
int add_word(Tree_node ** dictionary, char * word, char * definition, int ignore_error)
{
	Tree_node* node = *dictionary;

	/* Empty tree*/
	if(NULL == node)
	{
		*dictionary = create_node(word, definition);
		(*dictionary)->parent = NULL;
		insert_case1(*dictionary);
		return SUCCESS;
	}
	else
	{
		/* While not NIL */
		int cmp_res;
		while(NULL != node->left)
		{
			size_t size = strlen(node->word) > strlen(word)? strlen(node->word) : strlen(word);
			cmp_res = strncmp(word, node->word, size);
			/* Match */
			if(0 == cmp_res)
			{
				if(ignore_error)
				{
					printf("A duplicated word \"%s\" has been added...\n", word);
					return FAILURE;
				}
				else
				{
					printf("A duplicated word has been added.\n");
					return FAILURE;
				}
			}
			/* Go left */
			else if(cmp_res < 0)
			{
				node = node->left;
			}
			/* Go right */
			else
			{
				node = node->right;
			}
		}
		/* Add node */
		Tree_node* new_node = create_node(word, definition);
		node = node->parent;
		if(cmp_res < 0)
		{
			free(node->left);
			node->left = new_node;
			new_node->parent = node;
		}
		else
		{
			free(node->right);
			node->right = new_node;
			new_node->parent = node;
		}

		insert_case1(new_node);
		return SUCCESS;
	}
}

/* This should be identical to your find_word() function from Problem 4 */
int find_word(Tree_node * dictionary, char * word)
{
	Tree_node* node = dictionary;
	/* While not null node or NIL */
	while(NULL != node && NULL != node->left)
	{
		size_t size = strlen(node->word) > strlen(word) ? strlen(node->word) : strlen(word);
		int cmp_res = strncmp(word, node->word, size);
		/* Match successfully */
		if(0 == cmp_res)
		{
			printf("%s\n", node->definition);
			return SUCCESS;
		}
		else if(cmp_res < 0)
		{
			node = node->left;
		}
		else
		{
			node = node->right;
		}
	}
	printf("Word doesn't exist\n");
	return FAILURE;
}

/* This should be identical to your print_dictionary() function from Problem 4 */
void print_dictionary(Tree_node * dictionary)
{
	/* Never print null node or NIL */
	if(NULL == dictionary || NULL == dictionary->left)
		return;

	/* In-order traversal(recursion) */
	print_dictionary(dictionary->left);
	printf("%s ", dictionary->word);
	print_dictionary(dictionary->right);
}

/* This should be identical to your clear_dictionary() function from Problem 4 */
void clear_dictionary(Tree_node ** dictionary)
{
	//printf("2: %p\n", *dictionary);
	if(NULL != *dictionary)
	{
	//	printf("left\n");
		clear_dictionary(&((*dictionary)->left));
	//	printf("right\n");
		clear_dictionary(&((*dictionary)->right));
		free(*dictionary);
		*dictionary = NULL;
	}
	//printf("3\n");
}

/* This should be identical to your import_file() function from Problem 4 */
int import_file(char * fname, Tree_node ** dictionary )
{
	int word_cnt = 0;
	FILE* fp = fopen(fname, "r");
	if(NULL == fp)
	{
		printf("Failed to open file: %s\n", strerror(errno));
		return FAILURE;
	}
	else
	{
		/* Claim ignore-error when import file*/
		printf("Option 'ignore-error' is opened when import file...\n");

		char word[WORD_LEN];
		char definition[DEF_LEN];
		char* pos;
		int error;
		while(EOF != fscanf(fp, "%s", word))
		{
			fgets(definition, DEF_LEN, fp);
			if(NULL != (pos = strchr(definition, '\n')))
				*pos = '\0';
			error = add_word(dictionary, word, definition, 1);
			if(SUCCESS == error)
				word_cnt++;
			memset(word, 0x00, sizeof(char) * WORD_LEN);
			memset(definition, 0x00, sizeof(char) * DEF_LEN);
		}
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

		/* Get the Users Command */
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
			error = add_word(&dictionary, word, definition, 0);

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
			clear_dictionary(&dictionary);
			printf("The dictioanry has been cleared successfully.\n");
		}
		/* quit program */
		if( strncmp(command, "quit", COMMAND_LEN) == 0 )
			break;
	}

	return 0;
}

