#define LMU_BLOCK_TYPE_END_MARKER    0x00
#define LMU_BLOCK_TYPE_CHIPSET       0x01
#define LMU_BLOCK_TYPE_WIDTH         0x02
#define LMU_BLOCK_TYPE_HEIGHT        0x03
#define LMU_BLOCK_TYPE_LOWER_LAYER   0x47
#define LMU_BLOCK_TYPE_UPPER_LAYER   0x48
#define LMU_MAP_DATA_DEFAULT_CHIPSET 0x1
#define LMU_MAP_DATA_DEFAULT_WIDTH   0x30
#define LMU_MAP_DATA_DEFAULT_HEIGHT  0x15

typedef unsigned short int LmuMapTile;
typedef struct lmu_map_data {
	int chipset, width, height;
	LmuMapTile *upper, *lower;
} LmuMapData;

long int lmu_read_varint(FILE *in);
int lmu_skip_header(FILE *in);
int lmu_load_map_tiles(LmuMapData *map_data, FILE *in, int is_upper);
int lmu_process_block(LmuMapData *map_data, FILE *in);
void lmu_process_file(LmuMapData *map_data, const char *filename);
void lmu_clean_map_data(LmuMapData *md);
