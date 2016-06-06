#include "imgproc.h"

#define THICKNESS 2

IplImage*
pixbuf2ipl (const GdkPixbuf *image)
{
  IplImage *res_image;
  int width, height;
  int depth, n_channels;
  int stride, res_img_stride;
  guchar *image_data;
  guchar *res_image_data;
  gboolean has_alpha;

  width = gdk_pixbuf_get_width(image);
  height = gdk_pixbuf_get_height(image);
  depth = gdk_pixbuf_get_bits_per_sample(image);
  n_channels = gdk_pixbuf_get_n_channels(image);
  stride = gdk_pixbuf_get_rowstride(image);
  has_alpha = gdk_pixbuf_get_has_alpha(image);

  g_assert(depth == CHANNEL_DEPTH_8);

  image_data = gdk_pixbuf_get_pixels(image);
  res_image = cvCreateImage(cvSize(width, height),
                            depth, n_channels);
  res_image_data = (guchar*)res_image->imageData;
  res_img_stride = res_image->widthStep;

  for(int i = 0; i < height; ++i)
    for(int j = 0; j < width; ++j)
      {
        int index = i * res_img_stride + j * n_channels;
        res_image_data[index] = image_data[index + 2];
        res_image_data[index + 1] = image_data[index + 1];
        res_image_data[index + 2] = image_data[index];
      }

  return res_image;
}

GdkPixbuf*
ipl2pixbuf (const IplImage *image)
{
  uchar *imageData;
  guchar *pixbufData;
  int widthStep, n_channels, res_stride;
  int width, height, depth, res_n_channels;
  int data_order;
  GdkPixbuf *res_image;
  long ipl_depth;
  CvSize roi;

  cvGetRawData(image, &imageData, &widthStep, &roi);
  width = roi.width;
  height = roi.height;
  n_channels = image->nChannels;
  data_order = image->dataOrder;

  g_assert(data_order == IPL_DATA_ORDER_PIXEL);
  g_assert(n_channels == N_CHANNELS_RGB  ||
           n_channels == N_CHANNELS_RGBA ||
           n_channels == N_CHANNELS_GRAY);

  switch(ipl_depth = image->depth)
    {
    case IPL_DEPTH_8U:
      depth = 8;
      break;
    default:
      depth = 0;
      break;
    }
  g_assert(depth == CHANNEL_DEPTH_8);

  res_image = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE,
                             depth, width, height);
  pixbufData = gdk_pixbuf_get_pixels(res_image);
  res_stride = gdk_pixbuf_get_rowstride(res_image);
  res_n_channels = N_CHANNELS_RGB;

  for(int i = 0; i < height; ++i)
    for(int j = 0; j < width; ++j)
      {
        int index = i * widthStep + j * n_channels;
        int res_index = i * res_stride + j * res_n_channels;
        if(n_channels == N_CHANNELS_GRAY)
          pixbufData[res_index] = pixbufData[res_index + 1] =
              pixbufData[res_index + 2] = imageData[index];
        else
          {
            pixbufData[res_index] = imageData[index + 2];
            pixbufData[res_index + 1] = imageData[index + 1];
            pixbufData[res_index + 2] = imageData[index];
          }
      }
  return res_image;
}

#define DRAW_TRIANGLE(image, thickness, width, height)\
{\
  cvLine(image,\
         cvPoint((width - 1) / 2, 0),\
         cvPoint(0, height - 1),\
         cvScalar(0,0,0,0),\
         THICKNESS, 8, 0);\
  cvLine(image,\
         cvPoint((width - 1) / 2, 0),\
         cvPoint(width - 1, height - 1),\
         cvScalar(0,0,0,0),\
         THICKNESS, 8, 0);\
  cvLine(image,\
         cvPoint(0, height - 1),\
         cvPoint(width - 1, height - 1),\
         cvScalar(0,0,0,0),\
         THICKNESS, 8, 0);\
}

#define DRAW_NINE(cvimage, thickness, width, height)\
{\
  cvLine(cvimage, cvPoint(0, height - 1),\
         cvPoint(width - 1, (height - 1) / 2),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(width - 1, (height - 1) / 2),\
         cvPoint(0, (height - 1) / 2),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(0, (height - 1) / 2),\
         cvPoint(0,0),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(0,0),\
         cvPoint(width - 1, 0),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(width - 1, 0),\
         cvPoint(width - 1, (height - 1) / 2),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  }

#define DRAW_SEVEN(cvimage, thickness, width, height)\
{\
  cvLine(cvimage, cvPoint(0, 0),\
         cvPoint(width - 1, 0),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(width - 1, 0),\
         cvPoint(0, (height - 1) / 2),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(0, (height - 1) / 2),\
         cvPoint(0, height - 1),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  }

#define DRAW_FIVE(cvimage, thickness, width, height)\
{\
  cvLine(cvimage, cvPoint(width - 1, 0),\
         cvPoint(0, 0),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(0, 0),\
         cvPoint(0, (height - 1) / 2),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(0, (height - 1) / 2),\
         cvPoint(width - 1, (height - 1) / 2),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(width - 1, (height - 1) / 2),\
         cvPoint(width - 1, height - 1),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(width - 1, height - 1),\
         cvPoint(0, height - 1),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  }

#define DRAW_THREE(cvimage, thickness, width, height)\
{\
  cvLine(cvimage, cvPoint(0, 0),\
         cvPoint(width - 1, 0),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(width - 1, 0),\
         cvPoint(0, (height - 1) / 2),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(0, (height - 1) / 2),\
         cvPoint(width -1, (height - 1) / 2),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  cvLine(cvimage, cvPoint(width -1, (height - 1) / 2),\
         cvPoint(0, height - 1),\
         cvScalar(0,0,0,0), thickness, 8, 0);\
  }

#define DRAW_CIRCLE(cvimage, thickness, width, height)\
{\
  int radius = (width < height) ? width / 2 : height / 2;\
  cvCircle(cvimage, cvPoint(width / 2, height / 2),  radius, cvScalar(0, 0, 0, 0), 3, 8, 0);\
}

#define DRAW_RECTANGLE(cvimage, thickness, width, height)\
{\
  cvRectangle(cvimage, cvPoint(0, 0),\
              cvPoint(width - 1, height - 1),\
              cvScalar(0, 0, 0, 0), 3, 8, 0);\
}

IplImage*
get_object (int type,
            int width,
            int height)
{
  IplImage *image;

  image = cvCreateImage(cvSize(width, height),
                        CHANNEL_DEPTH_8,
                        N_CHANNELS_GRAY);
  cvSet(image, cvScalar(255,255,255,0), NULL);
  switch (type)
    {
    case 1:
      DRAW_FIVE(image, THICKNESS, width, height);
      break;
    case 2:
      DRAW_CIRCLE(image, THICKNESS, width, height);
      break;
    case 3:
      DRAW_RECTANGLE(image, THICKNESS, width, height);
      break;
    case 4:
      DRAW_FIVE(image, THICKNESS, width, height);
      break;
    default:
      break;
    }
  return image;
}

void
place_image (IplImage *dst,
             IplImage *src,
             CvRect    location)
{
  int dst_width, dst_height;

  dst_width = dst->width;
  dst_height = dst->height;

  cvSetImageROI(dst, location);
  cvCopy(src, dst, NULL);
  cvResetImageROI(dst);
}

void
place_rectangle_with_position(IplImage *dst,
                              CvPoint  *point,
                              int       rec_width,
                              int       rec_height,
                              int       position)
{
  CvPoint up_left;
  CvPoint down_right;
  CvRect roi;
  CvScalar color;
  int x_shift, y_shift;

  switch (position)
    {
    case POS_CENTER:
      x_shift = rec_width / 2;
      y_shift = rec_height / 2;
      break;
    case POS_DOWN_RIGHT:
      x_shift = rec_width;
      y_shift = rec_height;
      break;
    default:
      x_shift = 0;
      y_shift = 0;
      break;
    }


  roi.x = point->x - x_shift;
  roi.y = point->y - y_shift;
  roi.width = rec_width;
  roi.height = rec_height;

  up_left.x = 0;
  up_left.y = 0;
  down_right.x = rec_width - 1;
  down_right.y = rec_height - 1;

  color = cvScalar (0, 0,
                    255, 0);

  cvSetImageROI (dst, roi);
  cvRectangle (dst,
               up_left,
               down_right,
               color,
               3, 8, 0);
  cvResetImageROI(dst);
}
