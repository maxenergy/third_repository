#ifndef UTILS_H
#define UTILS_H

#include "hi_comm_video.h"
#include <string>

std::string videoFormat2String(VIDEO_FORMAT_E videoFormat);
std::string pixelFormat2String(PIXEL_FORMAT_E pixelFormat);
void yuv420spToRgb(void *src, void *dst, int );

#endif // UTILS_H
