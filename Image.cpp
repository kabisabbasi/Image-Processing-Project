#include "image.h"

#include <fstream>
#include <iostream>

using namespace std;

bool LoadPPM(const char* filename, ImageData& img)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Failed to open image\n";
        return false;
    }

    string format;
    int maxValue;

    file >> format;

    if (format != "P3")
    {
        cout << "Only P3 PPM supported\n";
        return false;
    }

    file >> img.width >> img.height;
    file >> maxValue;

    img.pixels = new Pixel[img.width * img.height];

    for (int i = 0; i < img.width * img.height; i++)
    {
        int r, g, b;

        file >> r >> g >> b;

        img.pixels[i].r = (unsigned char)r;
        img.pixels[i].g = (unsigned char)g;
        img.pixels[i].b = (unsigned char)b;
    }

    file.close();

    return true;
}

bool SavePPM(const char* filename, const ImageData& img)
{
    ofstream file(filename);

    if (!file.is_open())
        return false;

    file << "P3\n";
    file << img.width << " " << img.height << "\n";
    file << "255\n";

    for (int i = 0; i < img.width * img.height; i++)
    {
        file << (int)img.pixels[i].r << " ";
        file << (int)img.pixels[i].g << " ";
        file << (int)img.pixels[i].b << "\n";
    }

    file.close();

    return true;
}

void ConvertToGrayscale(ImageData& img)
{
    for (int i = 0; i < img.width * img.height; i++)
    {
        unsigned char gray =
            (unsigned char)(
                0.299f * img.pixels[i].r +
                0.587f * img.pixels[i].g +
                0.114f * img.pixels[i].b
                );

        img.pixels[i].r = gray;
        img.pixels[i].g = gray;
        img.pixels[i].b = gray;
    }
}

void InvertColors(ImageData& img)
{
    for (int i = 0; i < img.width * img.height; i++)
    {
        img.pixels[i].r = 255 - img.pixels[i].r;

        img.pixels[i].g = 255 - img.pixels[i].g;

        img.pixels[i].b = 255 - img.pixels[i].b;
    }
}

void BrightnessFilter(ImageData& img, int value)
{
    for (int i = 0; i < img.width * img.height; i++)
    {
        int r = img.pixels[i].r + value;
        int g = img.pixels[i].g + value;
        int b = img.pixels[i].b + value;

        if (r > 255) r = 255;
        if (g > 255) g = 255;
        if (b > 255) b = 255;

        if (r < 0) r = 0;
        if (g < 0) g = 0;
        if (b < 0) b = 0;

        img.pixels[i].r = r;
        img.pixels[i].g = g;
        img.pixels[i].b = b;
    }
}

void ThresholdFilter(ImageData& img)
{
    for (int i = 0; i < img.width * img.height; i++)
    {
        int gray =
            (
                img.pixels[i].r +
                img.pixels[i].g +
                img.pixels[i].b
                ) / 3;

        if (gray > 128)
            gray = 255;
        else
            gray = 0;

        img.pixels[i].r = gray;
        img.pixels[i].g = gray;
        img.pixels[i].b = gray;
    }
}

Texture2D CreateTextureFromImageData(const ImageData& img)
{
    Color* data =
        new Color[img.width * img.height];

    for (int i = 0; i < img.width * img.height; i++)
    {
        data[i].r = img.pixels[i].r;
        data[i].g = img.pixels[i].g;
        data[i].b = img.pixels[i].b;
        data[i].a = 255;
    }

    Image image;

    image.data = data;
    image.width = img.width;
    image.height = img.height;
    image.mipmaps = 1;
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    Texture2D texture =
        LoadTextureFromImage(image);

    delete[] data;

    return texture;
}

void FreeImageData(ImageData& img)
{
    if (img.pixels != nullptr)
    {
        delete[] img.pixels;
        img.pixels = nullptr;
    }

    img.width = 0;
    img.height = 0;
}

long long GetFileSize(const std::string& filename) // does some pointer arithmetic with eof logic to get the file size
{                                                    // (like ma'am Saima taught us in OOP  :> )
    std::ifstream file(filename, std::ios::binary | std::ios::ate); // open at end
    if (!file.is_open()) return 0;
    return file.tellg();
}

void CopyImage(const ImageData& source, ImageData& destination) // copies pixels from source to destination
{
    FreeImageData(destination);

    destination.width = source.width;
    destination.height = source.height;

    destination.pixels = new Pixel[source.width * source.height];

    for (int i = 0; i < source.width * source.height; i++)
    {
        destination.pixels[i] = source.pixels[i];
    }
}
