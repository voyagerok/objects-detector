#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <gtk/gtk.h>
#include "objects_generator.h"

GdkPixbuf* find_objects     (ObjectsMap *map);
GdkPixbuf* find_objects_dft (ObjectsMap *map);

#endif // CONVOLUTION_H
