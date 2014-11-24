#include <stdio.h>
#include <stdlib.h>
#include "mapread.h"

int main(int argc, char *argv[]) {
  LmuMapData map_data;
  int x, y;
 
  if (2 != argc) {
    printf("Usage: %s mapfile\n", argv[0]);
  }
  else {
    printf("Reading %s\n", argv[1]);
    lmu_process_file(&map_data, argv[1]);
   
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
    lmu_clean_map_data(&map_data);
  }
}
