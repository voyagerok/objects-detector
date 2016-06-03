#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <gtk/gtk.h>
#include "objects_generator.h"

GdkPixbuf*
find_objects(objects_map *map);
GdkPixbuf*
find_objects_dft (objects_map *map);

#endif // CONVOLUTION_H
