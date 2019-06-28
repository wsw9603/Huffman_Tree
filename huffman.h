#ifndef __HUFFMAN_H
#define __HUFFMAN_H

/*
 * 将整棵树存放在一个树节点的数组里，n个叶子节点的二叉树一共有2n-1个
 * 节点，据此可以在程序开始时就一次性申请好内存，而且方便将整棵树存放
 * 到文件中，再次读取时仍然有意义。
 */
struct huffman_node {
	int weight;
	unsigned char character;
	int left;
	int right;
};

/*
 * 定义最深哈夫曼树的深度，也就是编码的长度，在字符出现频率相差
 * 不太大的时候成立，特殊情形暂未考虑
 */
#define MAX_TREE_DEPTH 30

//描述每个字符的频率，在哈夫曼树中的位置和编码
struct char_info {
	int weight;
	unsigned char character;
	char code[MAX_TREE_DEPTH];
};

/*
 * 支持编码的字符为0到256的ASCII字符，应该按字节编码
 * 每个字节按照unsigned char处理
 */
#define CHARACTER_NUM 256
#define is_char_valid(ch) ({unsigned int __ch = ch;\
			    ch >= 0 && ch <= 255;})
#define num_to_char(num) ((unsigned char)(num))
#define char_to_num(ch) ((int)(unsigned char)(ch))

//统计文件中每个字符出现的频率
void count_char_from_file(char *fname, struct char_info cinfo[]);

//打印每个字符的频率，编码等信息
void print_info(struct char_info cinfo[]);
void print_tree(struct huffman_node *tree, int length);

//创建一棵Huffman树，成功返回树的所以叶子节点数目，
//并动态分配所有节点的存储空间，失败返回0
int create_tree(struct char_info cinfo[], struct huffman_node *tree[]);
void encode_chars(struct char_info cinfo[], struct huffman_node tree[]);
void destroy_tree(struct huffman_node *tree);

typedef void (*opt_t)(struct huffman_node *);
//遍历
//void travel_preorder(struct huffman_node *tree, opt_t operation);
//void travel_inorder(struct huffman_node *tree, opt_t operation);
//void travel_postorder(struct huffman_node *tree, opt_t operation);
//void print_node(struct huffman_node *tree);
//重新编码文件
void encode_file(char *fname, struct char_info cinfo[]);
void decode_file(char *fname, struct huffman_node *tree);

#endif
