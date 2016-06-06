#include "objects_generator.h"
#include <stdlib.h>
#include <time.h>

#define MAX_OBJECTS 4
#define MAP_WIDTH 800
#define MAP_HEIGHT 600

void
init_objects_map (ObjectsMap **map)
{
  *map = malloc(sizeof(ObjectsMap));
  (*map)->objects = NULL;
  (*map)->map = NULL;
  (*map)->n_of_objects = 0;
}

void
release_objects_map(ObjectsMap **map)
{
  g_assert(*map != NULL);

  if ((*map)->n_of_objects != 0)
    for(int i = 0; i < (*map)->n_of_objects; ++i)
      cvReleaseImage(&(*map)->objects[i]);

  if ((*map)->objects != NULL)
    free((*map)->objects);

  if ((*map)->map != NULL)
    cvReleaseImage(&(*map)->map);

  free(*map);
}

void
generate_new_objects (ObjectsMap *map)
{
  int size, object_type;
  int width, height;
  int max_width, max_height;
  int min_width, min_height;

  if (map->objects != NULL)
    {
      if(map->n_of_objects != 0)
        {
          for (int i = 0; i < map->n_of_objects; ++i)
            cvReleaseImage(&map->objects[i]);
          map->n_of_objects = 0;
        }
      free(map->objects);
      map->objects = NULL;
    }

  srand(time(NULL));
  size = rand() % MAX_OBJECTS + 1;
  map->objects = malloc(sizeof(IplImage*) * size);
  map->n_of_objects = size;

  min_width = MAP_WIDTH / 8;
  min_height = MAP_HEIGHT / 8;
  max_width = MAP_WIDTH / 4;
  max_height = MAP_HEIGHT / 4;

  for (int i = 0; i < map->n_of_objects; ++i)
    {
      width = rand() % max_width + min_width;
      height = rand() % max_height + min_height;
      object_type = rand() % MAX_OBJECTS + 1;
      map->objects[i] = get_object(object_type,
                                   width, height);
    }
}

void
generate_new_map (ObjectsMap *map)
{
  g_assert (map->n_of_objects != 0 && map->objects != NULL);

  struct point
  {
    int x,y;
  } start = {0,0};
  int width, height;

  if (map->map != NULL)
    {
      cvReleaseImage(&map->map);
      map->map = NULL;
    }

  map->map = cvCreateImage(cvSize(MAP_WIDTH,
                                  MAP_HEIGHT),
                           CHANNEL_DEPTH_8,
                           N_CHANNELS_GRAY);
  CLEAR_WHITE(map->map);
  for(int i = 0; i < map->n_of_objects; ++i)
    {
      width = map->objects[i]->width;
      height = map->objects[i]->height;

      do
        {
          start.x = rand() % MAP_WIDTH;
          start.y = rand() % MAP_HEIGHT;
        }
      while (start.x + width > MAP_WIDTH ||
             start.y + height > MAP_HEIGHT);

      place_image(map->map, map->objects[i], cvRect(start.x,
                                                    start.y,
                                                    width,
                                                    height));
    }
}

GdkPixbuf*
get_map_as_pixbuf(ObjectsMap *map)
{
  GdkPixbuf *res = NULL;

  if (map->map != NULL)
    res = ipl2pixbuf(map->map);
  return res;
}

