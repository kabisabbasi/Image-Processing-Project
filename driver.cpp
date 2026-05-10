#define _CRT_SECURE_NO_WARNINGS

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "image.h"
#include "compression.h"

#include <iostream>

using namespace std;

long long GetFileSize(const char* filename)
{
    FILE* f = fopen(filename, "rb");

    if (!f)
        return 0;

    fseek(f, 0, SEEK_END);

    long long size = ftell(f);

    fclose(f);

    return size;
}

void CopyImage(const ImageData& source, ImageData& destination)
{
    FreeImageData(destination);

    destination.width = source.width;
    destination.height = source.height;

    destination.pixels =
        new Pixel[source.width * source.height];

    for (int i = 0; i < source.width * source.height; i++)
    {
        destination.pixels[i] = source.pixels[i];
    }
}

int main()
{
    InitWindow(1400, 760, "Advanced Image Processor");

    SetTargetFPS(60);

    ImageData original;
    ImageData processed;
    ImageData decompressed;

    Texture2D originalTexture = { 0 };
    Texture2D processedTexture = { 0 };
    Texture2D decompressedTexture = { 0 };

    long long originalSize = 0;
    long long rleSize = 0;
    long long huffmanSize = 0;

    double bestCompressionTime = 0;

    bool usedRLE = false;
    bool usedHuffman = false;

    char filename[256] = "test_checkerboard.ppm";

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(Color{ 240, 240, 245, 255 });

        GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
        GuiSetStyle(BUTTON, BORDER_WIDTH, 2);
        GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        DrawRectangle(0, 0, 1400, 100, Color{ 220, 225, 235, 255 });

        DrawText(
            "Advanced Image Compression System",
            420,
            15,
            32,
            DARKBLUE
        );

        GuiTextBox(
            Rectangle{ 20, 55, 260, 35 },
            filename,
            255,
            true
        );

        if (GuiButton(Rectangle{ 300, 55, 100, 35 }, "Load"))
        {
            FreeImageData(original);

            if (LoadPPM(filename, original))
            {
                originalSize =
                    original.width * original.height * 3;

                CopyImage(original, processed);

                if (originalTexture.id != 0)
                    UnloadTexture(originalTexture);

                if (processedTexture.id != 0)
                    UnloadTexture(processedTexture);

                originalTexture =
                    CreateTextureFromImageData(original);

                processedTexture =
                    CreateTextureFromImageData(processed);
            }
        }

        if (GuiButton(Rectangle{ 430, 55, 120, 35 }, "Grayscale"))
        {
            ConvertToGrayscale(processed);

            if (processedTexture.id != 0)
                UnloadTexture(processedTexture);

            processedTexture =
                CreateTextureFromImageData(processed);
        }

        if (GuiButton(Rectangle{ 570, 55, 100, 35 }, "Invert"))
        {
            InvertColors(processed);

            if (processedTexture.id != 0)
                UnloadTexture(processedTexture);

            processedTexture =
                CreateTextureFromImageData(processed);
        }

        if (GuiButton(Rectangle{ 690, 55, 120, 35 }, "Bright+"))
        {
            BrightnessFilter(processed, 40);

            if (processedTexture.id != 0)
                UnloadTexture(processedTexture);

            processedTexture =
                CreateTextureFromImageData(processed);
        }

        if (GuiButton(Rectangle{ 830, 55, 120, 35 }, "Threshold"))
        {
            ThresholdFilter(processed);

            if (processedTexture.id != 0)
                UnloadTexture(processedTexture);

            processedTexture =
                CreateTextureFromImageData(processed);
        }

        if (GuiButton(Rectangle{ 970, 55, 100, 35 }, "Save"))
        {
            SavePPM("output.ppm", processed);
        }

        if (GuiButton(Rectangle{ 1090, 55, 120, 35 }, "Compress"))
        {
            CompressImageRLE(processed, "rle.bin");
            CompressImageHuffman(processed, "huffman.bin");

            rleSize = GetFileSize("rle.bin");
            huffmanSize = GetFileSize("huffman.bin");

            if (rleSize <= huffmanSize)
            {
                usedRLE = true;
                usedHuffman = false;

                bestCompressionTime =
                    GetRLECompressionTime();
            }
            else
            {
                usedRLE = false;
                usedHuffman = true;

                bestCompressionTime =
                    GetHuffmanCompressionTime();
            }
        }

        if (GuiButton(Rectangle{ 1230, 55, 130, 35 }, "Decompress"))
        {
            FreeImageData(decompressed);

            if (usedRLE)
            {
                if (DecompressImageRLE("rle.bin", decompressed))
                {
                    if (decompressedTexture.id != 0)
                        UnloadTexture(decompressedTexture);

                    decompressedTexture =
                        CreateTextureFromImageData(decompressed);
                }
            }
        }

        DrawRectangleRounded(
            Rectangle{ 20, 120, 400, 470 },
            0.03f,
            6,
            WHITE
        );

        DrawRectangleRounded(
            Rectangle{ 500, 120, 400, 470 },
            0.03f,
            6,
            WHITE
        );

        DrawRectangleRounded(
            Rectangle{ 980, 120, 400, 470 },
            0.03f,
            6,
            WHITE
        );

        DrawRectangleLinesEx(
            Rectangle{ 20, 120, 400, 470 },
            2,
            LIGHTGRAY
        );

        DrawRectangleLinesEx(
            Rectangle{ 500, 120, 400, 470 },
            2,
            LIGHTGRAY
        );

        DrawRectangleLinesEx(
            Rectangle{ 980, 120, 400, 470 },
            2,
            LIGHTGRAY
        );

        DrawText(
            "Original Image",
            130,
            130,
            24,
            BLACK
        );

        DrawText(
            "Processed Image",
            600,
            130,
            24,
            BLACK
        );

        DrawText(
            "Reconstructed Image",
            1060,
            130,
            24,
            BLACK
        );

        if (originalTexture.id != 0)
        {
            DrawTextureEx(
                originalTexture,
                Vector2{ 40, 180 },
                0,
                0.52f,
                WHITE
            );
        }

        if (processedTexture.id != 0)
        {
            DrawTextureEx(
                processedTexture,
                Vector2{ 520, 180 },
                0,
                0.52f,
                WHITE
            );
        }

        if (decompressedTexture.id != 0)
        {
            DrawTextureEx(
                decompressedTexture,
                Vector2{ 1000, 180 },
                0,
                0.52f,
                WHITE
            );
        }

        DrawRectangleRounded(
            Rectangle{ 20, 620, 1360, 110 },
            0.04f,
            6,
            Color{ 225, 230, 240, 255 }
        );

        DrawText(
            TextFormat("Original Size : %lld bytes", originalSize),
            40,
            640,
            22,
            BLACK
        );

        DrawText(
            TextFormat("RLE Size : %lld bytes", rleSize),
            40,
            675,
            22,
            DARKBLUE
        );

        DrawText(
            TextFormat("Huffman Size : %lld bytes", huffmanSize),
            40,
            705,
            22,
            MAROON
        );

        DrawText(
            TextFormat("RLE Time : %.3f ms", GetRLECompressionTime()),
            470,
            675,
            22,
            DARKBLUE
        );

        DrawText(
            TextFormat("Huffman Time : %.3f ms", GetHuffmanCompressionTime()),
            470,
            705,
            22,
            MAROON
        );

        if (usedRLE)
        {
            DrawText(
                "Best Compression : RLE",
                950,
                655,
                26,
                DARKGREEN
            );
        }

        if (usedHuffman)
        {
            DrawText(
                "Best Compression : Huffman",
                950,
                655,
                26,
                DARKGREEN
            );
        }

        DrawText(
            TextFormat("Best Time : %.3f ms", bestCompressionTime),
            950,
            700,
            22,
            BLACK
        );

        EndDrawing();


    }
    return 0;
}
