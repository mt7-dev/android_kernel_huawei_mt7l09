#include <boot/boardid.h>
#include <boot/boot.h>
#include <balongv7r2/types.h>

boardid_parameter boardid_resource;
char*  boardid_string = NULL;

extern void num_to_hex8(unsigned n, char *out);

int boardid_init(void)
{   
    /* just make a stub */
	boardid_resource.chip_id = 0x55;
	boardid_resource.pmu_id = 0x110;
	boardid_resource.board_id = 0x001;

	cprintf("board hw id:0x%x.\n", boardid_resource.board_id);

	return 0;
}

char*  config_boardid_to_string(void)
{
	unsigned int boardid_string_len = 0;
	unsigned int temp_string_len    = 0;

	char temp_string[HEX_STRING_MAXLEN];

	boardid_string =  alloc(BOARDID_STRING_MAXLEN);
	if (NULL == boardid_string) {
		return NULL;
	}

	memset(boardid_string, 0, BOARDID_STRING_MAXLEN);
	memset(temp_string, 0, HEX_STRING_MAXLEN);

	/*identified by "boardid".*/
	memcpy(boardid_string, "boardid=0x", strlen("boardid=0x"));

	/*transfer chip_id to string.*/
	num_to_hex8(boardid_resource.chip_id, temp_string);

	boardid_string_len = strlen(boardid_string);
	temp_string_len = strlen(temp_string);
	memcpy(boardid_string + boardid_string_len, temp_string, temp_string_len);

	/* seperated by ","*/
	boardid_string_len = strlen(boardid_string);
	memcpy(boardid_string + boardid_string_len, ",0x", 3);

	/*transfer pmu_id to string.*/
	num_to_hex8(boardid_resource.pmu_id, temp_string);
	boardid_string_len = strlen(boardid_string);
	temp_string_len = strlen(temp_string);
	memcpy(boardid_string + boardid_string_len, temp_string, temp_string_len);

	/*seperated by ","*/
	boardid_string_len = strlen(boardid_string);
	memcpy(boardid_string + boardid_string_len, ",0x", 3);

	/*transfer board_id to string.*/
	num_to_hex8(boardid_resource.board_id, temp_string);
	boardid_string_len = strlen(boardid_string);
	temp_string_len = strlen(temp_string);
	memcpy(boardid_string + boardid_string_len, temp_string, temp_string_len);

	return boardid_string;
}

/*
 * Function:get_boardid.
 * Discription:get current platform's boardid.
 * Paremeters:
 *		@ void.
 * return value:
 *		@ current platform's boardid.
 */
unsigned int get_boardid(void)
{
	return boardid_resource.board_id;
}

/*
 * Function:get_chipid.
 * Discription:get current platform's chipid.
 * Paremeters:
 *		@ void.
 * return value:
 *		@ current platform's chipid.
 */
unsigned int get_chipid(void)
{
	return boardid_resource.chip_id;
}

/*
 * Function:get_pmuid.
 * Discription:get current platform's pmuid.
 * Paremeters:
 *		@ void.
 * return value:
 *		@ current platform's pmuid.
 */
unsigned int get_pmuid(void)
{
	return boardid_resource.pmu_id;
}


