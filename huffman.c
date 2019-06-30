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
/*
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
*/
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

void print_tree(struct huffman_node *tree, int length)
{
	int i;
	printf("tree info:\n");
	for (i = 0; i < length; i++) {
		printf("no: %-4d, character: %c, weight: %-4d, left: %-4d, right: %-4d\n",
			i+1, tree[i].character, tree[i].weight,
			tree[i].left, tree[i].right);
	}
	putchar('\n');
}

//返回权值非零的字符数目
static int count_valid(struct char_info cinfo[])
{
	int i, count = 0;
	for (i = 0; i < CHARACTER_NUM; i++)
		if (cinfo[i].weight > 0)
			count++;
	return count;
}

//初始化所有叶子节点
static void init_all_nodes(struct char_info cinfo[],
			  struct huffman_node nodes[],
			  int length)
{
	int i, j;
	for (i = 0, j = 0; i < CHARACTER_NUM; i++) {
		if (cinfo[i].weight == 0)
			continue;
		nodes[j].weight = cinfo[i].weight;
		nodes[j].character = i;
		nodes[j].left = -1;
		nodes[j].right = -1;
		j++;
	}
}

//按照权值降序排列
static void sort(struct huffman_node nodes[], int length)
{
	int i, j, temp;
	for (i = 0; i < length-1; i++) {
		temp = i;
		for (j = i + 1; j < length; j++)
			if (nodes[j].weight > nodes[temp].weight)
				temp = j;

		if (temp != i) {
			struct huffman_node swap;
			swap = nodes[temp];
			nodes[temp] = nodes[i];
			nodes[i] = swap;
		}
	}
}

/*
 * 入参为Huffman树的叶子节点数目和叶子节点所在的数组，并且叶子节点已经
 * 按照权值降序排列存放在数组的后半部分。
 * 函数将根据入参生成一棵完整的Huffman树，树的根节点即为数组首元素。
 */
static void combine_nodes(struct huffman_node tree[], int leaves)
{
	int low = leaves - 1;
	int high = 2 * leaves - 2;
	while (low > 0) {
		int j;
		int temp = tree[high].weight + tree[high-1].weight;
		for (j = low; tree[j].weight > temp; j++)
			tree[j-1] = tree[j];
		tree[j-1].weight = temp;
		tree[j-1].left = high - 1;
		tree[j-1].right = high;
		tree[j-1].character = '%';	//标志非叶子节点
		low--;
		high -= 2;
	}
}

int create_tree(struct char_info cinfo[], struct huffman_node *tree[])
{
	const int leaves = count_valid(cinfo);

	struct huffman_node *base = (struct huffman_node *)malloc(
				    sizeof(*base) * (2*leaves - 1));
	if (!base) {
		printf("alloc huffman node table failed\n");
		return 0;
	}

	init_all_nodes(cinfo, &base[leaves-1], leaves);
	sort(&base[leaves-1], leaves);
	combine_nodes(base, leaves);

	*tree = base;
	return 2 * leaves - 1;
}

void encode_chars_recursive(struct char_info cinfo[],
			    struct huffman_node tree[],
			    int cursor, char base[], int n)
{
	struct huffman_node *node = &tree[cursor];
	if (node->left == -1) {
		strncpy(cinfo[node->character].code, base, n);
		cinfo[node->character].code[n] = '\0';
		return;
	}
	base[n] = '0';
	encode_chars_recursive(cinfo, tree, node->left, base, n+1);
	base[n] = '1';
	encode_chars_recursive(cinfo, tree, node->right, base, n+1);
}

void encode_chars(struct char_info cinfo[], struct huffman_node tree[])
{
	char code[MAX_TREE_DEPTH] = {0};

	encode_chars_recursive(cinfo, tree, 0, code, 0);
}

void encode_file(char *fname, struct char_info cinfo[],
		 struct huffman_node tree[], int length)
{
	char outfname[50];
	strcpy(outfname, fname);
	open_and_check(fin, fname, "r");
	open_and_check(fout, strcat(outfname, ".encode"), "w");

	//将整棵树写进文件头
	fwrite(&length, sizeof(length), 1, fout);
	fwrite(tree, sizeof(struct huffman_node), length, fout);

	//暂时以字符为单位进行编码处理
	while (!feof(fin)) {
		unsigned char temp = fgetc(fin);
		fputs(cinfo[char_to_num(temp)].code, fout);
	}

	fclose(fin);
	fclose(fout);
}

void decode_file(char *fname)
{
	char outfname[50];
	char *src = fname;
	char *dst = outfname;
	while(*src && *src != '.')
		*dst++ = *src++;
	*dst = '\0';
	//TODO:对文件名进行更合理的处理
	strcat(outfname, ".decode");

	//打开要解码的文件，读出头部字节
	open_and_check(fin, fname, "r");
	int length;
	fread(&length, sizeof(length), 1, fin);
	if (length <= 0) {
		printf("invalid header length\n");
		goto errout;
	}

	struct huffman_node *tree = (struct huffman_node *)malloc(
				    sizeof(*tree) * length);
	if (!tree) {
		printf("alloc space for huffman tree failed\n");
		goto errout;
	}
	fread(tree, sizeof(struct huffman_node), length, fin);

	//TODO:这一步失败，不能释放前面的资源，有问题
	open_and_check(fout, outfname, "w");

	int cursor = 0;
	while (!feof(fin)) {
		if (tree[cursor].left == -1) {
			fputc(tree[cursor].character, fout);
			cursor = 0;
		}
		char temp = fgetc(fin);
		if (temp == '1')
			cursor = tree[cursor].right;
		else
			cursor = tree[cursor].left;
	}

	fclose(fout);
errout:
	fclose(fin);
}

void destroy_tree(struct huffman_node *tree)
{
	free(tree);
}
