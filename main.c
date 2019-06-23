#include <stdio.h>
#include <string.h>
#include "huffman.h"

int fun()
{
	char buf[256];
	struct huffman_node *huffman_tree = NULL;
	struct char_info cinfo[CHARACTER_NUM] = {0};

	printf("please enter a string, end with enter key\n");
	fgets(buf, sizeof(buf), stdin);
//	gets(buf);
	count_char(buf, cinfo);
	huffman_tree = create_tree(cinfo);
	encode_chars(cinfo);
	print_info(cinfo);

	puts("\nprint tree:\n");
	travel_preorder(huffman_tree, print_node);
	destroy_tree(huffman_tree);

	return 0;
}
int main()
{
	fun();

	return 0;
}
