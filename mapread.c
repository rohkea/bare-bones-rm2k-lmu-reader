#include <stdio.h>
#include <stdlib.h>
#include "mapread.h"

long int lmu_read_varint(FILE *in) {
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

int lmu_skip_header(FILE *in) {
  int size;
  
  size = lmu_read_varint(in);
  /* TODO: check for LcfMapUnit */
  fseek(in, size, SEEK_CUR);
}

int lmu_load_map_tiles(LmuMapData *map_data, FILE *in, int is_upper) {
  LmuMapTile **layer;
  
  layer = is_upper ? &map_data->upper : &map_data->lower;
  
  if (*layer == NULL) {
    *layer = calloc(map_data->width * map_data->height, sizeof(LmuMapTile));
    fread(*layer, sizeof(LmuMapTile), map_data->width * map_data->height, in);
  }
}

/* returns the type of the processed block */
int lmu_process_block(LmuMapData *map_data, FILE *in) {
  int block_type, block_size, content_int;
  
  block_type = lmu_read_varint(in);
  if (block_type == LMU_BLOCK_TYPE_END_MARKER) {
    return 0;
  }

  block_size = lmu_read_varint(in);
  if (LMU_BLOCK_TYPE_CHIPSET == block_type) {
    map_data->chipset = lmu_read_varint(in);
  }
  else if (LMU_BLOCK_TYPE_WIDTH == block_type) {
    map_data->width = lmu_read_varint(in);
  }
  else if (LMU_BLOCK_TYPE_HEIGHT == block_type) {
    map_data->height = lmu_read_varint(in);
  }
  else if (LMU_BLOCK_TYPE_UPPER_LAYER == block_type
           || LMU_BLOCK_TYPE_LOWER_LAYER == block_type) {
    lmu_load_map_tiles(map_data, in, LMU_BLOCK_TYPE_UPPER_LAYER == block_type);
  }
  else {
    /* Skip unknown block */
    fseek(in, block_size, SEEK_CUR);
  }
  
  return block_type;
}

void lmu_process_file(LmuMapData *map_data, const char *filename) {
  FILE *in;
  int block_type;
  
  /* Fill MapData with default values */
  map_data->chipset = LMU_MAP_DATA_DEFAULT_CHIPSET;
  map_data->width = LMU_MAP_DATA_DEFAULT_WIDTH;
  map_data->height = LMU_MAP_DATA_DEFAULT_HEIGHT;
  map_data->upper = NULL;
  map_data->lower = NULL;
  
  in = fopen(filename, "r");
  lmu_skip_header(in);
  
  while (!feof(in)) {
    block_type = lmu_process_block(map_data, in);
    
    if (LMU_BLOCK_TYPE_END_MARKER == block_type) {
      break;
    }
  }
  
  fclose(in);
}

/* This function frees map layers */
void lmu_clean_map_data(LmuMapData *md) {
  free(md->upper);
  md->upper = NULL;
  free(md->lower);
  md->lower = NULL;
}

