#pragma once

#include "raylib.h"

struct Pixel
{
    unsigned char r, g, b;
};

struct ImageData
{
    int width;
    int height;
    Pixel* pixels;

    ImageData()
    {
        width = 0;
        height = 0;
        pixels = nullptr;
    }
};

bool LoadPPM(const char* filename, ImageData& img);

bool SavePPM(const char* filename, const ImageData& img);

void ConvertToGrayscale(ImageData& img);

void InvertColors(ImageData& img);

void BrightnessFilter(ImageData& img, int value);

void ThresholdFilter(ImageData& img);

Texture2D CreateTextureFromImageData(const ImageData& img);

void FreeImageData(ImageData& img);