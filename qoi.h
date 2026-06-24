#ifndef QOI_H
#define QOI_H

#include "image.h"

bool SaveQOI(const char* filename, const ImageData& img);
bool LoadQOI(const char* filename, ImageData& img);


#endif // QOI_H
