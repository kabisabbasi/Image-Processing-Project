// #include "image.h"

// #include <fstream>
// #include <cstring>

// using namespace std;

// struct QOIPixel
// {
//     unsigned char r, g, b, a;
// };

// static int QOIHash(QOIPixel px)
// {
//     return (px.r * 3 +
//             px.g * 5 +
//             px.b * 7 +
//             px.a * 11) % 64;
// }

// bool SaveQOI(const char* filename, const ImageData& img)
// {
//     ofstream file(filename, ios::binary);

//     if (!file.is_open())
//         return false;

//     // ---------------- HEADER ----------------

//     file.write("qoif", 4);

//     unsigned int width = img.width;
//     unsigned int height = img.height;

//     unsigned char channels = 3;
//     unsigned char colorspace = 0;

//     // big endian
//     for (int i = 3; i >= 0; i--)
//         file.put((width >> (i * 8)) & 0xFF);

//     for (int i = 3; i >= 0; i--)
//         file.put((height >> (i * 8)) & 0xFF);

//     file.put(channels);
//     file.put(colorspace);

//     // ---------------- ENCODING ----------------

//     QOIPixel index[64] = {};

//     QOIPixel prev = {0, 0, 0, 255};

//     int run = 0;

//     int total = img.width * img.height;

//     for (int pxPos = 0; pxPos < total; pxPos++)
//     {
//         QOIPixel px;

//         px.r = img.pixels[pxPos].r;
//         px.g = img.pixels[pxPos].g;
//         px.b = img.pixels[pxPos].b;
//         px.a = 255;

//         if (memcmp(&px, &prev, sizeof(QOIPixel)) == 0)
//         {
//             run++;

//             if (run == 62 || pxPos == total - 1)
//             {
//                 file.put(0xC0 | (run - 1));
//                 run = 0;
//             }

//             continue;
//         }

//         if (run > 0)
//         {
//             file.put(0xC0 | (run - 1));
//             run = 0;
//         }

//         int indexPos = QOIHash(px);

//         if (memcmp(&index[indexPos], &px, sizeof(QOIPixel)) == 0)
//         {
//             file.put(indexPos);
//         }
//         else
//         {
//             index[indexPos] = px;

//             int dr = (int)px.r - prev.r;
//             int dg = (int)px.g - prev.g;
//             int db = (int)px.b - prev.b;

//             if (dr > -3 && dr < 2 &&
//                 dg > -3 && dg < 2 &&
//                 db > -3 && db < 2)
//             {
//                 unsigned char diff =
//                     0x40 |
//                     ((dr + 2) << 4) |
//                     ((dg + 2) << 2) |
//                     (db + 2);

//                 file.put(diff);
//             }
//             else
//             {
//                 file.put(0xFE);

//                 file.put(px.r);
//                 file.put(px.g);
//                 file.put(px.b);
//             }
//         }

//         prev = px;
//     }

//     // end marker
//     unsigned char end[8] = {0,0,0,0,0,0,0,1};

//     file.write((char*)end, 8);

//     file.close();

//     return true;
// }



// bool LoadQOI(const char* filename, ImageData& img)
// {
//     ifstream file(filename, ios::binary);

//     if (!file.is_open())
//         return false;

//     char magic[4];

//     file.read(magic, 4);

//     if (strncmp(magic, "qoif", 4) != 0)
//         return false;

//     unsigned int width = 0;
//     unsigned int height = 0;

//     for (int i = 0; i < 4; i++)
//         width = (width << 8) | file.get();

//     for (int i = 0; i < 4; i++)
//         height = (height << 8) | file.get();

//     unsigned char channels = file.get();
//     unsigned char colorspace = file.get();

//     img.width = width;
//     img.height = height;

//     img.pixels = new Pixel[width * height];

//     QOIPixel index[64] = {};

//     QOIPixel px = {0,0,0,255};

//     int total = width * height;

//     int pxPos = 0;

//     while (pxPos < total)
//     {
//         unsigned char b1 = file.get();

//         if (b1 == 0xFE)
//         {
//             px.r = file.get();
//             px.g = file.get();
//             px.b = file.get();
//         }
//         else if ((b1 & 0xC0) == 0x00)
//         {
//             px = index[b1];
//         }
//         else if ((b1 & 0xC0) == 0x40)
//         {
//             px.r += ((b1 >> 4) & 0x03) - 2;
//             px.g += ((b1 >> 2) & 0x03) - 2;
//             px.b += (b1 & 0x03) - 2;
//         }
//         else if ((b1 & 0xC0) == 0xC0)
//         {
//             int run = (b1 & 0x3F);

//             for (int i = 0; i <= run; i++)
//             {
//                 img.pixels[pxPos].r = px.r;
//                 img.pixels[pxPos].g = px.g;
//                 img.pixels[pxPos].b = px.b;

//                 pxPos++;
//             }

//             continue;
//         }

//         index[QOIHash(px)] = px;

//         img.pixels[pxPos].r = px.r;
//         img.pixels[pxPos].g = px.g;
//         img.pixels[pxPos].b = px.b;

//         pxPos++;
//     }

//     file.close();

//     return true;
// }




#include "image.h"

#include <fstream>
#include <cstring>

using namespace std;

struct QOIPixel
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

static int QOIHash(const QOIPixel& px)
{
    return (
        px.r * 3 +
        px.g * 5 +
        px.b * 7 +
        px.a * 11
    ) % 64;
}

static bool PixelsEqual(const QOIPixel& a, const QOIPixel& b)
{
    return (
        a.r == b.r &&
        a.g == b.g &&
        a.b == b.b &&
        a.a == b.a
    );
}

bool SaveQOI(const char* filename, const ImageData& img)
{
    ofstream file(filename, ios::binary);

    if (!file.is_open())
        return false;

    file.write("qoif", 4);

    unsigned int width = img.width;
    unsigned int height = img.height;

    unsigned char channels = 3;
    unsigned char colorspace = 0;

    // width (big endian)
    for (int i = 3; i >= 0; i--)
    {
        file.put((width >> (i * 8)) & 0xFF);
    }

    // height (big endian)
    for (int i = 3; i >= 0; i--)
    {
        file.put((height >> (i * 8)) & 0xFF);
    }

    file.put(channels);
    file.put(colorspace);

    QOIPixel index[64] = {};

    QOIPixel prev = { 0, 0, 0, 255 };

    int run = 0;

    int total = img.width * img.height;

    for (int pxPos = 0; pxPos < total; pxPos++)
    {
        QOIPixel px;

        px.r = img.pixels[pxPos].r;
        px.g = img.pixels[pxPos].g;
        px.b = img.pixels[pxPos].b;
        px.a = 255;

        if (PixelsEqual(px, prev))
        {
            run++;

            if (run == 62 || pxPos == total - 1)
            {
                file.put(0xC0 | (run - 1));

                run = 0;
            }

            continue;
        }

        if (run > 0)
        {
            file.put(0xC0 | (run - 1));

            run = 0;
        }

        int indexPos = QOIHash(px);

        if (PixelsEqual(index[indexPos], px))
        {
            file.put(indexPos);
        }
        else
        {
            index[indexPos] = px;

            int dr = (int)px.r - (int)prev.r;
            int dg = (int)px.g - (int)prev.g;
            int db = (int)px.b - (int)prev.b;

            if (
                dr >= -2 && dr <= 1 &&
                dg >= -2 && dg <= 1 &&
                db >= -2 && db <= 1
            )
            {
                unsigned char diff =
                    0x40 |
                    ((dr + 2) << 4) |
                    ((dg + 2) << 2) |
                    (db + 2);

                file.put(diff);
            }
            else
            {
                file.put(0xFE);

                file.put(px.r);
                file.put(px.g);
                file.put(px.b);
            }
        }

        prev = px;
    }

    unsigned char end[8] =
    {
        0,0,0,0,0,0,0,1
    };

    file.write((char*)end, 8);

    file.close();

    return true;
}

bool LoadQOI(const char* filename, ImageData& img)
{
    ifstream file(filename, ios::binary);

    if (!file.is_open())
        return false;

    char magic[4];

    file.read(magic, 4);

    if (strncmp(magic, "qoif", 4) != 0)
    {
        file.close();
        return false;
    }

    unsigned int width = 0;
    unsigned int height = 0;

    // width (big endian)
    for (int i = 0; i < 4; i++)
    {
        int byte = file.get();

        if (byte == EOF)
        {
            file.close();
            return false;
        }

        width = (width << 8) | byte;
    }

    // height (big endian)
    for (int i = 0; i < 4; i++)
    {
        int byte = file.get();

        if (byte == EOF)
        {
            file.close();
            return false;
        }

        height = (height << 8) | byte;
    }

    int channels = file.get();
    int colorspace = file.get();

    if (channels == EOF || colorspace == EOF)
    {
        file.close();
        return false;
    }

    img.width = width;
    img.height = height;

    img.pixels = new Pixel[width * height];


    QOIPixel index[64] = {};

    QOIPixel px = { 0, 0, 0, 255 };

    int total = width * height;

    int pxPos = 0;

    while (pxPos < total)       // decoding loop
    {
        int byte = file.get();

        if (byte == EOF)
            break;

        unsigned char b1 = (unsigned char)byte;

        if (b1 == 0xFE)
        {
            px.r = (unsigned char)file.get();
            px.g = (unsigned char)file.get();
            px.b = (unsigned char)file.get();
        }

        else if ((b1 & 0xC0) == 0x00)
        {
            px = index[b1];
        }

        else if ((b1 & 0xC0) == 0x40)
        {
            px.r = (unsigned char)(
                (int)px.r +
                (((b1 >> 4) & 0x03) - 2)
            );

            px.g = (unsigned char)(
                (int)px.g +
                (((b1 >> 2) & 0x03) - 2)
            );

            px.b = (unsigned char)(
                (int)px.b +
                ((b1 & 0x03) - 2)
            );
        }

        else if ((b1 & 0xC0) == 0xC0)
        {
            int run = (b1 & 0x3F);

            for (int i = 0; i <= run && pxPos < total; i++)
            {
                img.pixels[pxPos].r = px.r;
                img.pixels[pxPos].g = px.g;
                img.pixels[pxPos].b = px.b;

                pxPos++;
            }

            continue;
        }

        // update index
        index[QOIHash(px)] = px;

        // store pixel
        img.pixels[pxPos].r = px.r;
        img.pixels[pxPos].g = px.g;
        img.pixels[pxPos].b = px.b;

        pxPos++;
    }

    file.close();

    return true;
}
