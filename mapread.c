#include <stdio.h>
#include <stdlib.h>

#define LMU_BLOCK_TYPE_END_MARKER    0x00
#define LMU_BLOCK_TYPE_CHIPSET       0x01
#define LMU_BLOCK_TYPE_WIDTH         0x02
#define LMU_BLOCK_TYPE_HEIGHT        0x03
#define LMU_BLOCK_TYPE_LOWER_LAYER   0x47
#define LMU_BLOCK_TYPE_UPPER_LAYER   0x48
#define LMU_MAP_DATA_DEFAULT_CHIPSET 0x1
#define LMU_MAP_DATA_DEFAULT_WIDTH   0x30
#define LMU_MAP_DATA_DEFAULT_HEIGHT  0x15


typedef unsigned short int MapTile;
typedef struct map_data {
	int chipset, width, height;
	MapTile *upper, *lower;
} MapData;

long int read_varint(FILE *in) {
  long int res;
  int byte;
  
  res = 0;
  byte = fgetc(in);
  if (byte & 0x80) {
    while (byte & 0x80 && !feof(in)) {
      res = (res << 7) | (byte & 0x7f);
      byte = fgetc(in);
    };
    res = (res << 7) | (byte & 0x7f);
  }
  else {
    return byte;
  }
  
  return res;
}

int skip_header(FILE *in) {
  int size;
  
  size = read_varint(in);
  /* TODO: check for LcfMapUnit */
  fseek(in, size, SEEK_CUR);
}

int load_map_tiles(MapData *map_data, FILE *in, int is_upper) {
  MapTile **layer;
  
  layer = is_upper ? &map_data->upper : &map_data->lower;
  
  if (*layer == NULL) {
    *layer = calloc(map_data->width * map_data->height, sizeof(MapTile));
    fread(*layer, sizeof(MapTile), map_data->width * map_data->height, in);
  }
}

/* returns the type of the processed block */
int process_block(MapData *map_data, FILE *in) {
  int block_type, block_size, content_int;
  
  block_type = read_varint(in);
  if (block_type == LMU_BLOCK_TYPE_END_MARKER) {
    return 0;
  }

  block_size = read_varint(in);
  if (LMU_BLOCK_TYPE_CHIPSET == block_type) {
    map_data->chipset = read_varint(in);
  }
  else if (LMU_BLOCK_TYPE_WIDTH == block_type) {
    map_data->width = read_varint(in);
  }
  else if (LMU_BLOCK_TYPE_HEIGHT == block_type) {
    map_data->height = read_varint(in);
  }
  else if (LMU_BLOCK_TYPE_UPPER_LAYER == block_type
           || LMU_BLOCK_TYPE_LOWER_LAYER == block_type) {
    load_map_tiles(map_data, in, LMU_BLOCK_TYPE_UPPER_LAYER == block_type);
  }
  else {
    /* Skip unknown block */
    fseek(in, block_size, SEEK_CUR);
  }
  
  return block_type;
}

void process_file(MapData *map_data, const char *filename) {
  FILE *in;
  int block_type;
  
  /* Fill MapData with default values */
  map_data->chipset = LMU_MAP_DATA_DEFAULT_CHIPSET;
  map_data->width = LMU_MAP_DATA_DEFAULT_WIDTH;
  map_data->height = LMU_MAP_DATA_DEFAULT_HEIGHT;
  map_data->upper = NULL;
  map_data->lower = NULL;
  
  in = fopen(filename, "r");
  skip_header(in);
  
  while (!feof(in)) {
    block_type = process_block(map_data, in);
    
    if (LMU_BLOCK_TYPE_END_MARKER == block_type) {
      break;
    }
  }
  
  fclose(in);
}

/* This function frees map layers */
void clean_map_data(MapData *md) {
  free(md->upper);
  md->upper = NULL;
  free(md->lower);
  md->lower = NULL;
}

int main(int argc, char *argv[]) {
  MapData map_data;
  int x, y;
 
  if (2 != argc) {
    printf("Usage: %s mapfile\n", argv[0]);
  }
  else {
    printf("Reading %s\n", argv[1]);
    process_file(&map_data, argv[1]);
   
    /* Example of using the loaded data: draw first 5x5 tiles */
    for (y = 0; y < 5; y++) {
      for (x = 0; x < 5; x++) {
        printf("(%02d,%02d): %05d ",
               x, y,
               map_data.lower[y * map_data.width + x]);
      }
      printf("\n");
    }
    
    /* Cleanup */
    clean_map_data(&map_data);
  }
}
