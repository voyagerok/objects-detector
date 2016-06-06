#ifndef OBJECTS_GENERATOR_H
#define OBJECTS_GENERATOR_H

#include "imgproc.h"

typedef struct _objects_map
{
  IplImage **objects;
  int n_of_objects;
  IplImage *map;
} ObjectsMap;

void       init_objects_map     (ObjectsMap **map);
void       release_objects_map  (ObjectsMap **map);
void       generate_new_objects (ObjectsMap  *map);
void       generate_new_map     (ObjectsMap  *map);
GdkPixbuf* get_map_as_pixbuf    (ObjectsMap  *map);

#endif // OBJECTS_GENERATOR_H
