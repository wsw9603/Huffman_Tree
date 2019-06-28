#include <stdio.h>
#include <string.h>
#include "huffman.h"

int fun()
{
	int length;
	char filename[50];
	struct huffman_node *huffman_tree = NULL;
	struct char_info cinfo[CHARACTER_NUM] = {0};

	printf("please enter filename\n");
	scanf("%s", filename);

	count_char_from_file(filename, cinfo);
	length = create_tree(cinfo, &huffman_tree);
	print_tree(huffman_tree, length);

	encode_chars(cinfo, huffman_tree);
	print_info(cinfo);

	encode_file(filename, cinfo);
	decode_file(strcat(filename, ".encode"), huffman_tree);

	destroy_tree(huffman_tree);

	return 0;
}
int main()
{
	fun();

	return 0;
}
