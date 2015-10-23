#ifndef __BOARDID_H__
#define __BOARDID_H__

typedef struct board_id_parameter{
	unsigned int chip_id;
	unsigned int pmu_id;
	unsigned int board_id;
}boardid_parameter;

#define HEX_STRING_MAXLEN		    (10)
#define BOARDID_STRING_MAXLEN		(64)

int boardid_init(void);
unsigned int get_boardid(void);
unsigned int get_chipid(void);
unsigned int get_pmuid(void);
char *config_boardid_to_string(void);
#endif
