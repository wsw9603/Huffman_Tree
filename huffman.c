#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "huffman.h"

#define open_and_check(file_dec, fname, mode)	\
	FILE *file_dec = fopen(fname, mode);	\
do {						\
	if (file_dec == NULL) {			\
		printf("failed to open file \'%s\'\n", fname);\
		return;				\
	}					\
}while (0)

//分配一个根节点，成功返回节点的地址，失败返回NULL
static struct huffman_node *alloc_root(int weight, unsigned char ch)
{
	struct huffman_node *temp = (struct huffman_node *)malloc(sizeof(*temp));
	if (temp) {
		temp->character = ch;
		temp->weight = weight;
		temp->parent = NULL;
		temp->left = NULL;
		temp->right = NULL;
	}
	return temp;
}

//将两棵子树归并成一棵树，成功返回新树的地址，失败返回NULL
static struct huffman_node *combine_tree(struct huffman_node *tree1,
			struct huffman_node *tree2)
{
	struct huffman_node *temp = (struct huffman_node *)malloc(sizeof(*temp));
	if (temp) {
		temp->weight = tree1->weight + tree2->weight;
		temp->left = tree1;
		temp->right = tree2;
		temp->parent = NULL;
		tree1->parent = temp;
		tree2->parent = temp;
	}
	return temp;
}

void count_char(char *str, struct char_info cinfo[])
{
	while(*str) {
		if (is_char_valid(*str))
			cinfo[char_to_num(*str)].weight++;
		str++;
	}
}

#define buf_size 256
void count_char_from_file(char *fname, struct char_info cinfo[])
{
	unsigned char buf[buf_size] = {0};
	open_and_check(file, fname, "r");

	while (!feof(file)) {
		int act_len = fread(buf, sizeof(unsigned char),
				    buf_size, file);
		int i;
		for (i = 0; i < act_len; i++)
			cinfo[buf[i]].weight++;
	}

	fclose(file);
}

void print_node(struct huffman_node *tree)
{
	printf("weight: %d, self: %p, parent: %p, left: %p, right: %p\n\
		character: %c",
		tree->weight, &tree->weight, tree->parent, tree->left, tree->right, tree->character);
}

//先序遍历
void travel_preorder(struct huffman_node *tree, opt_t operation)
{
	if (!tree)
		return;
	operation(tree);
	travel_preorder(tree->left, operation);
	travel_preorder(tree->right, operation);
}

//中序遍历
void travel_inorder(struct huffman_node *tree, opt_t operation)
{
	if (!tree)
		return;
	travel_inorder(tree->left, operation);
	operation(tree);
	travel_inorder(tree->right, operation);
}

//后序遍历
void travel_postorder(struct huffman_node *tree, opt_t operation)
{
	if (!tree)
		return;
	travel_postorder(tree->left, operation);
	travel_postorder(tree->right, operation);
	operation(tree);
}

//打印每个字符的频率，编码等信息
void print_info(struct char_info cinfo[])
{
	int i;
	printf("print code:\n");
	for (i = 0; i < CHARACTER_NUM; i++) {
		if (cinfo[i].weight == 0)
			continue;
		printf("\t%c:  weight: %d, code: %s\n", num_to_char(i),
				cinfo[i].weight, cinfo[i].code);
	}
}

static void print_trees(struct huffman_node *trees[], int length)
{
	int i;
	printf("weight of %d trees: ", length);
	for (i = 0; i < length; i++) {
		printf("%-4d", trees[i]->weight);
	}
	putchar('\n');
}

//为所有频率非零的字符分配节点，返回分配的非零字符数目
static int init_all_node(struct huffman_node *trees[], struct char_info cinfo[])
{
	int i, length = 0;

	for (i = 0; i < CHARACTER_NUM; i++) {
		if (cinfo[i].weight) {
			trees[length] = alloc_root(cinfo[i].weight,
					num_to_char(i));
			if (!trees[length])
				return length;
			cinfo[i].node = trees[length];
			length++;
		}
	}

	return length;
}

//为所有树节点排序，根据节点权值降序排列
static void sort_trees(struct huffman_node *trees[], int length)
{
	int i, j, temp;
	struct huffman_node *swap;
	for (i = 0; i < length; i++) {
		for (temp = i, j = i+1; j < length; j++)
			if (trees[temp]->weight < trees[j]->weight)
				temp = j;
		if (temp != i) {
			swap = trees[i];
			trees[i] = trees[temp];
			trees[temp] = swap;
		}
	}
}

struct huffman_node *create_tree(struct char_info cinfo[])
{
	int i;
	struct huffman_node *temp = NULL;
	struct huffman_node *trees[CHARACTER_NUM] = {0};
	int length = init_all_node(trees, cinfo);
	sort_trees(trees, length);

	while (length > 1) {
		temp = combine_tree(trees[length-1], trees[length-2]);
		if (!temp) {
			//错误处理，删除节点
			return NULL;
		}

		for (i = length - 2; i > 0 &&
				trees[i-1]->weight < temp->weight; i--) {
			trees[i] = trees[i-1];
		}
		trees[i] = temp;
		length--;
	}

	return trees[0];
}

void encode_chars(struct char_info cinfo[])
{
	int i;
	char code[MAX_TREE_DEPTH] = {0};
	char *cursor = NULL;
	struct huffman_node *temp = NULL;

	for (i = 0; i < CHARACTER_NUM; i++) {
		if (cinfo[i].weight == 0)
			continue;
		for (temp = cinfo[i].node, cursor = &code[MAX_TREE_DEPTH-2];
				temp->parent; temp = temp->parent, cursor--)
			*cursor = (temp->parent)->left == temp ? '0' : '1';
		strcpy(cinfo[i].code, &cursor[1]);
	}
}

void encode_file(char *fname, struct char_info cinfo[])
{
	char outfname[50];
	strcpy(outfname, fname);
	open_and_check(fin, fname, "r");
	open_and_check(fout, strcat(outfname, ".encode"), "w");

	//暂时以字符为单位进行编码处理
	while (!feof(fin)) {
		unsigned char temp = fgetc(fin);
		fputs(cinfo[char_to_num(temp)].code, fout);
	}

	fclose(fin);
	fclose(fout);
}

void decode_file(char *fname, struct huffman_node *tree)
{
	char outfname[50];
	char *src = fname;
	char *dst = outfname;
	while(*src && *src != '.')
		*dst++ = *src++;
	*dst = '\0';

	open_and_check(fin, fname, "r");
	open_and_check(fout, strcat(outfname, ".decode"), "w");

	struct huffman_node *cursor = tree;
	while (!feof(fin)) {
		if (!cursor->left) {
			fputc(cursor->character, fout);
			cursor = tree;
		}
		char temp = fgetc(fin);
		if (temp == '1')
			cursor = cursor->right;
		else
			cursor = cursor->left;
	}

	fclose(fin);
	fclose(fout);
}

void destroy_tree(struct huffman_node *tree)
{
	travel_postorder(tree, (opt_t)free);
}
