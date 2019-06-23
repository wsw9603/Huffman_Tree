#include <stdio.h>
#include <string.h>
#include "huffman.h"

int fun()
{
	char filename[50];
	struct huffman_node *huffman_tree = NULL;
	struct char_info cinfo[CHARACTER_NUM] = {0};

	printf("please enter filename\n");
	scanf("%s", filename);
//	fgets(filename, sizeof(filename), stdin);
//	gets(buf);
	puts(filename);
	count_char_from_file(filename, cinfo);
	huffman_tree = create_tree(cinfo);
	encode_chars(cinfo);
	encode_file(filename, cinfo);
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
