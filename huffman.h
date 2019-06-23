#ifndef __HUFFMAN_H
#define __HUFFMAN_H

//叶子节点左右子树为空，根节点parent为空
//三个全空则既是叶子节点也是根节点
struct huffman_node {
	int weight;
	struct huffman_node *parent;
	struct huffman_node *left;
	struct huffman_node *right;
};

/*
 * 定义最深哈夫曼树的深度，在字符出现频率相差不太大的时候成立，特殊情形暂未考虑
 */
#define MAX_TREE_DEPTH 20

//描述每个字符的频率，在哈夫曼树中的位置和编码
struct char_info {
	char character;
	int weight;
	struct huffman_node *node;
	char code[MAX_TREE_DEPTH];
};

//支持编码的字符是26个小写字母，abc...z
#define CHARACTER_NUM 26
#define is_char_valid(ch) ((ch) <= 'z' && (ch) >= 'a')
#define num_to_char(num) (char)((num) + 'a')
#define char_to_num(ch) ((ch) - 'a')

//给定字符串，统计字符串中每个字符的频率信息
//只统计小写英文字母，其他字符忽略，要求入参合法且 cinfo已经初始化完成
void count_char(char *str, struct char_info cinfo[]);
//打印每个字符的频率，编码等信息
void print_info(struct char_info cinfo[]);

//创建一棵Huffman树，成功返回根节点，失败返回NULL
struct huffman_node *create_tree(struct char_info cinfo[]);
void encode_chars(struct char_info cinfo[]);
void destroy_tree(struct huffman_node *tree);

typedef void (*opt_t)(struct huffman_node *);
//遍历
void travel_preorder(struct huffman_node *tree, opt_t operation);
void travel_inorder(struct huffman_node *tree, opt_t operation);
void travel_postorder(struct huffman_node *tree, opt_t operation);
void print_node(struct huffman_node *tree);

#endif