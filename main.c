#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

#define bool int
#define false 0
#define true  1

#define MAX_FILE_NAME_LEN	50

void print_menu()
{
	printf("*********************************************************\n\
		0. exit\n\
		1. encode file\n\
		2. decode file\n\
	please select a menu\n");
}

//检查输入文件名是否合法，格式化
bool check_and_format(char *fname)
{
	if (!fname || !*fname)
		return false;
	//删除最后一个回车字符
	int length = strlen(fname);
	if (length < MAX_FILE_NAME_LEN-1)
		fname[length-1] = '\0';
	return true;
}

void encode()
{
	int length;
	struct huffman_node *huffman_tree = NULL;
	char fname[MAX_FILE_NAME_LEN] = {0};
	struct char_info cinfo[CHARACTER_NUM] = {0};

	printf("please enter file name\n");
	while(getchar() != '\n');

	fgets(fname, sizeof(fname), stdin);
	if (!check_and_format(fname)) {
		printf("invalid filename\n");
		return;
	}

	count_char_from_file(fname, cinfo);
	length = create_tree(cinfo, &huffman_tree);
	if (length == 0) {
		printf("create tree failed\n");
		return;
	}
	encode_chars(cinfo, huffman_tree);
	encode_file(fname, cinfo, huffman_tree, length);

	free(huffman_tree);
}

void decode()
{

}

void fun_with_menu()
{
	while (1) {
		print_menu();
		int menu;

		scanf("%d", &menu);
		switch (menu) {
			case 0: return;
			case 1: encode();
				break;
			case 2: decode();
				break;
			default:
				printf("unsupported menu, try again\n");
		}
	}
}

int main()
{
	fun_with_menu();

	return 0;
}
