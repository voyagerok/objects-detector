#include "convolution.h"
#include "imgproc.h"
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>


GdkPixbuf*
find_objects (objects_map *map)
{
  int obj_count;
  IplImage *res_ipl;
  IplImage *f_result;
  GdkPixbuf *res_pbuf;
  int obj_width, obj_height;
  int map_width, map_height;
  double min, max;
  CvPoint min_pnt, max_pnt;

  g_assert(map->map != NULL && map->objects != NULL &&
      map->n_of_objects != 0);

  map_width = map->map->width;
  map_height = map->map->height;
  res_ipl = cvCreateImage(cvGetSize(map->map),
                      map->map->depth,
                      N_CHANNELS_RGB);
  cvCvtColor(map->map, res_ipl, CV_GRAY2BGR);

  obj_count = map->n_of_objects;
  for (int i = 0; i < obj_count; ++i)
    {
      obj_width = map->objects[i]->width;
      obj_height = map->objects[i]->height;

      f_result = cvCreateImage(cvSize(map_width - obj_width + 1,
                                      map_height - obj_height + 1),
                               IPL_DEPTH_32F,
                               N_CHANNELS_GRAY);
      cvMatchTemplate(map->map, map->objects[i], f_result, CV_TM_CCOEFF);
      cvMinMaxLoc(f_result, &min, &max, &min_pnt, &max_pnt, NULL);
      place_rectangle_with_position(res_ipl, &max_pnt, obj_width, obj_height, POS_UP_LEFT);

      cvReleaseImage(&f_result);
    }

  res_pbuf = ipl2pixbuf(res_ipl);
  cvReleaseImage(&res_ipl);

  return res_pbuf;
}

static IplImage*
get_convolution (const IplImage *image,
                 const IplImage *filter)
{
  CvSize dft_size;
  IplImage *reversed_image, *reversed_filter;
  IplImage *dft_image, *dft_filter, *dft_res;
  IplImage *res;

  dft_size.height = cvGetOptimalDFTSize(image->height + filter->height - 1);
  dft_size.width = cvGetOptimalDFTSize(image->width + filter->width - 1);

  res = cvCreateImage(cvSize(image->width,
                             image->height),
                                         IPL_DEPTH_32F,
                                         N_CHANNELS_GRAY);
  reversed_image = cvCreateImage(cvGetSize(image),
                              IPL_DEPTH_8U,
                              N_CHANNELS_GRAY);
  reversed_filter = cvCreateImage(cvGetSize(filter),
                              IPL_DEPTH_8U,
                              N_CHANNELS_GRAY);

  cvNot(image, reversed_image);
  cvNot(filter, reversed_filter);

  dft_image = cvCreateImage(dft_size,
                                  IPL_DEPTH_32F,
                                  N_CHANNELS_GRAY);
  cvSet(dft_image, cvScalar(0, 0, 0, 0), NULL);
  dft_filter = cvCreateImage(dft_size,
                                  IPL_DEPTH_32F,
                                  N_CHANNELS_GRAY);
  cvSet(dft_filter, cvScalar(0, 0, 0, 0), NULL);

  cvSetImageROI(dft_image, cvRect(0, 0,
                                        reversed_image->width,
                                        reversed_image->height));
  cvSetImageROI(dft_filter, cvRect(0, 0,
                                        reversed_filter->width,
                                        reversed_filter->height));
  double scaling_factor = 1.0/255;
  cvConvertScale(reversed_image, dft_image, scaling_factor, 0);
  cvConvertScale(reversed_filter, dft_filter, scaling_factor, 0);
  cvResetImageROI(dft_image);
  cvResetImageROI(dft_filter);


  cvDFT(dft_image, dft_image, CV_DXT_FORWARD, image->height);
  cvDFT(dft_filter, dft_filter, CV_DXT_FORWARD, filter->height);

  dft_res = cvCreateImage(dft_size,
                          IPL_DEPTH_32F,
                          N_CHANNELS_GRAY);

  cvMulSpectrums(dft_image, dft_filter, dft_res, 0);

  cvDFT(dft_res, dft_res, CV_DXT_INVERSE, res->height);
  cvSetImageROI(dft_res, cvRect(0, 0, res->width, res->height));
  cvCopy(dft_res, res, NULL);
  cvResetImageROI(dft_res);

  cvReleaseImage(&reversed_filter);
  cvReleaseImage(&reversed_image);
  cvReleaseImage(&dft_image);
  cvReleaseImage(&dft_filter);
  cvReleaseImage(&dft_res);

  return res;
}

static GQueue*
get_n_max_coords (const IplImage *image, int n)
{
  GQueue *coords;
  float max = -1.0;
  CvSize size;
  CvPoint *new_elem, *old_elem;
  int stride, n_channels, depth;
  uchar *pixels;
  float *row;

  n_channels = image->nChannels;
  depth = image->depth;
  g_assert (n_channels == N_CHANNELS_GRAY &&
            depth == IPL_DEPTH_32F);

  cvGetRawData(image, &pixels, &stride, &size);
  coords = g_queue_new();

  for (int i = 0; i < size.height; ++i)
    {
      row = (float *)(pixels + i * stride);
      for (int j = 0; j < size.width; ++j)
        {
          if (row[j] > max)
            {
              max = row[j];
              new_elem = (CvPoint*)malloc(sizeof(CvPoint));
              new_elem->x = j;
              new_elem->y = i;
              g_queue_push_head(coords, new_elem);
              if (g_queue_get_length(coords) > n)
                {
                  old_elem = (CvPoint*)g_queue_pop_tail(coords);
                  free(old_elem);
                }
            }
        }
    }
  return coords;
}

static void
get_lower_right_coords (gpointer data, gpointer user_data)
{
  CvPoint *current_coords, *best_coords;

  current_coords = (CvPoint*)data;
  best_coords = (CvPoint*)user_data;

  if (current_coords->x >= best_coords->x &&
      current_coords->y >= best_coords->y)
    *best_coords = *current_coords;
}

GdkPixbuf*
find_objects_dft (objects_map *map)
{
  int obj_count;
  IplImage *res_ipl;
  IplImage *f_result;
  GdkPixbuf *res_pbuf;
  int obj_width, obj_height;
  int map_width, map_height;
  double min, max;
  CvPoint min_pnt, max_pnt;

  g_assert(map->map != NULL && map->objects != NULL &&
      map->n_of_objects != 0);

  map_width = map->map->width;
  map_height = map->map->height;
  res_ipl = cvCreateImage(cvGetSize(map->map),
                      map->map->depth,
                      N_CHANNELS_RGB);
  cvCvtColor(map->map, res_ipl, CV_GRAY2BGR);

  obj_count = map->n_of_objects;
  for (int i = 0; i < obj_count; ++i)
    {
      obj_width = map->objects[i]->width;
      obj_height = map->objects[i]->height;

      f_result = get_convolution(map->map, map->objects[i]);
      cvMinMaxLoc(f_result, &min, &max, &min_pnt, &max_pnt, NULL);
      place_rectangle_with_position(res_ipl, &max_pnt, obj_width, obj_height, POS_DOWN_RIGHT);

      cvReleaseImage(&f_result);
    }

  res_pbuf = ipl2pixbuf(res_ipl);
  cvReleaseImage(&res_ipl);

  return res_pbuf;
}

