#pragma once

#include "image.h"

bool CompressImageRLE(const ImageData& img, const char* filename);

bool DecompressImageRLE(const char* filename, ImageData& img);

bool CompressImageHuffman(const ImageData& img, const char* filename);

double GetRLECompressionTime();

double GetHuffmanCompressionTime();

bool DecompressImageHuffman(const char* filename, ImageData& img);
