#ifndef OBJECTS_GENERATOR_H
#define OBJECTS_GENERATOR_H

#include "imgproc.h"

typedef struct _objects_map
{
  IplImage **objects;
  int n_of_objects;
  IplImage *map;
} objects_map;

void
init_objects_map(objects_map **map);
void
release_objects_map(objects_map **map);
void
generate_new_objects(objects_map *map);
void
generate_new_map(objects_map *map);
GdkPixbuf*
get_map_as_pixbuf(objects_map *map);

#endif // OBJECTS_GENERATOR_H
