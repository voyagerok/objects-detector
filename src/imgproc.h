#ifndef IMGPROC_H
#define IMGPROC_H

#include <gtk/gtk.h>
#include <opencv2/core/core_c.h>

#define CHANNEL_DEPTH_8 8
#define N_CHANNELS_RGB 3
#define N_CHANNELS_RGBA 4
#define N_CHANNELS_GRAY 1

#define POS_UP_LEFT 10
#define POS_CENTER 20
#define POS_DOWN_RIGHT 30

#define CLEAR_WHITE(image) (cvSet((image), cvScalar(255,255,255, 0), NULL))

#define TRIANGLE 1
#define NINE 2
#define FIVE 3
#define SEVEN 4
#define THREE 5

IplImage  *pixbuf2ipl                    (const GdkPixbuf *image);
GdkPixbuf *ipl2pixbuf                    (const IplImage *image);
IplImage  *get_object                    (int             type,
                                          int             width,
                                          int             height);
void       place_image                   (IplImage       *dst,
                                          IplImage       *src,
                                          CvRect          location);
void       place_rectangle_with_position (IplImage       *dst,
                                          CvPoint        *center,
                                          int             rec_width,
                                          int             rec_height,
                                          int             position);

#endif // IMGPROC_H
