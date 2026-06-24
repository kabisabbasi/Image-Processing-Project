#ifndef APP_H
#define APP_H


#define _CRT_SECURE_NO_WARNINGS

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"



#include "image.h"
#include "Compression.h"
#include "qoi.h"

#include "NetworkTransfer.h"
#include <thread>

#include <cstring>


using namespace std;

class App {
private:   // variables

    bool editFilename = false;
    bool editReceiverIP = false;

    // vars for network transfer
    char receiverIP[64] = "192.168.1.5";

    bool sending = false;
    bool receiving = false;

    char networkStatus[128] = "Idle";
    // end vars for network transfer

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
    // char filename[256] = "test_solid.ppm";
    
    
private: // button functions
    void loadImage(const string& imagefile) {
        FreeImageData(original);
        
        // if (LoadPPM(imagefile.c_str(), original))
        bool loaded = false;

        string ext =
            imagefile.substr(imagefile.find_last_of('.') + 1);

        if (ext == "ppm")
            loaded = LoadPPM(imagefile.c_str(), original);

        else if (ext == "qoi")
            loaded = LoadQOI(imagefile.c_str(), original);

        if (loaded)
        {
            originalSize = original.width * original.height * 3;
            
            CopyImage(original, processed);     // creating a copy to work non-destructively
            
            if (originalTexture.id != 0)
                UnloadTexture(originalTexture);
            
            if (processedTexture.id != 0)
                UnloadTexture(processedTexture);
            
            originalTexture = CreateTextureFromImageData(original);
            
            processedTexture = CreateTextureFromImageData(processed);
        }
    }
    
private:   // functions
    void imageOperationButtons() {
        if (GuiButton(Rectangle{ 300, 55, 100, 35 }, "Load")) loadImage(string(filename));
        
        if (GuiButton(Rectangle{ 430, 55, 120, 35 }, "Grayscale"))
        {
            ConvertToGrayscale(processed);
            
            if (processedTexture.id != 0)
                UnloadTexture(processedTexture);
            
            processedTexture = CreateTextureFromImageData(processed);
        }
        
        if (GuiButton(Rectangle{ 570, 55, 100, 35 }, "Invert"))
        {
            InvertColors(processed);
            
            if (processedTexture.id != 0)
                UnloadTexture(processedTexture);
            
            processedTexture = CreateTextureFromImageData(processed);
        }
        
        if (GuiButton(Rectangle{ 690, 55, 120, 35 }, "Bright+"))
        {
            BrightnessFilter(processed, 40);
            
            if (processedTexture.id != 0)
                UnloadTexture(processedTexture);
            
            processedTexture = CreateTextureFromImageData(processed);
        }
        
        if (GuiButton(Rectangle{ 830, 55, 120, 35 }, "Threshold"))
        {
            ThresholdFilter(processed);
            
            if (processedTexture.id != 0)
                UnloadTexture(processedTexture);
            
            processedTexture = CreateTextureFromImageData(processed);
        }
        
        if (GuiButton(Rectangle{ 970, 55, 100, 35 }, "Save"))
        {
            SavePPM("output.ppm", processed);
            SaveQOI("output.qoi", processed);
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
                
                bestCompressionTime = GetRLECompressionTime();
            }
            else
            {
                usedRLE = false;
                usedHuffman = true;
                
                bestCompressionTime = GetHuffmanCompressionTime();
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
                    
                    decompressedTexture = CreateTextureFromImageData(decompressed);
                }
            }
            if (usedHuffman)
            {
                if (DecompressImageHuffman("huffman.bin", decompressed))
                {
                    if (decompressedTexture.id != 0)
                        UnloadTexture(decompressedTexture);

                    decompressedTexture = CreateTextureFromImageData(decompressed);
                }
            }
        }
        
    }
    
    // void shareButtons() {
    //     if (GuiButton(Rectangle{ 1360-180, 10, 180, 35 }, "Send Over Network"))
    //     {
    //         ;
    //     }
    // }


    void shareButtons()
    {
        DrawText(
            "Receiver IP:",
            850,
            780-25,
            20,
            BLACK
        );

        // GuiTextBox(
        //     Rectangle{ 1000, 780-30, 160, 35 },
        //     receiverIP,
        //     63,
        //     true
        // );

        if (GuiTextBox(
                    Rectangle{ 1000, 780-30, 160, 35 },
                    receiverIP,
                    63,
                    editReceiverIP
                )) {
                    editReceiverIP = !editReceiverIP;
        }

        if (
            GuiButton(
                Rectangle{ 1180, 790, 180, 35 },
                "Send QOI"
            )
        )
        {
            if (!sending)
            {
                sending = true;

                strcpy(networkStatus, "Sending file...");

                thread senderThread(
                    [this]()
                    {
                        bool success =
                            SendFile("output.qoi", 54000);

                        if (success)
                            strcpy(networkStatus, "File sent!");
                        else
                            strcpy(networkStatus, "Send failed!");

                        sending = false;
                    }
                );

                senderThread.detach();
            }
        }

        if (
            GuiButton(
                Rectangle{ 1180, 780-30, 180, 35 },
                "Receive QOI"
            )
        )
        {
            if (!receiving)
            {
                receiving = true;

                strcpy(networkStatus, "Receiving file...");

                thread receiverThread(
                    [this]()
                    {
                        bool success =
                            ReceiveFile(
                                "received.qoi",
                                receiverIP,
                                54000
                            );

                        if (success)
                        {
                            strcpy(networkStatus, "File received!");

                            loadImage("received.qoi");
                        }
                        else
                        {
                            strcpy(networkStatus, "Receive failed!");
                        }

                        receiving = false;
                    }
                );

                receiverThread.detach();
            }
        }

        DrawText(
            networkStatus,
            1000,
            795,
            20,
            DARKGREEN
        );
    }

    void drawImageFrames() {
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
    }
    
    void rectangleLinesEx() {
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
    }
    
    void comparisonText() {
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
    }
    
    void calculationsText() {
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
                        675,
                        26,
                        DARKGREEN
                        );
            DrawText(
                        TextFormat("RLE Compression Ratio : %.2f%%", 100.0f * rleSize / originalSize),
                        950,
                        640,
                        22,
                        BLACK
                        );
        }
        
        if (usedHuffman)
        {
            DrawText(
                        "Best Compression : Huffman",
                        950,
                        675,
                        26,
                        DARKGREEN
                        );
            DrawText(
                        TextFormat("Huffman Compression Ratio : %.2f%%", 100.0f * huffmanSize / originalSize),
                        470,
                        640,
                        22,
                        BLACK
                        );
        }
        
        DrawText(
                    TextFormat("Best Time : %.3f ms", bestCompressionTime),
                    950,
                    700,
                    22,
                    BLACK
                    );
    }
    
    void prepareCanvas() {
        ClearBackground(Color{ 240, 240, 245, 255 });
        
        GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
        GuiSetStyle(BUTTON, BORDER_WIDTH, 2);
        GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    }
    
    void drawTextures() {
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
    }
    
    void drawSubCanvases()
    {
        DrawRectangle(0, 0, 1400, 100, Color{ 220, 225, 235, 255 });
        
        DrawRectangleRounded(
                    Rectangle{ 20, 620, 1360, 110 },
                    0.04f,
                    6,
                    Color{ 225, 230, 240, 255 }
                    );
    }
public:
    void run() {
        InitWindow(1400, 850, "Advanced Image Processor");
        
        SetTargetFPS(60);
        
        // private stuff was originally here
        
        while (!WindowShouldClose())
        {
            BeginDrawing();
            
            prepareCanvas();
            
            drawSubCanvases();
            
            DrawText("Advanced Image Compression System", 420, 15, 32, DARKBLUE);
            
            // GuiTextBox(
            //             Rectangle{ 20, 55, 260, 35 },
            //             filename,
            //             255,
            //             true
            //             );

            if (GuiTextBox
                    ( Rectangle{ 20, 55, 260, 35 }, filename,
                        255, editFilename )) {
                editFilename = !editFilename;
            }

            drawImageFrames();
            
            shareButtons();

            imageOperationButtons();
            
            rectangleLinesEx();
            
            comparisonText();
            
            drawTextures();
            
            calculationsText();
            
            EndDrawing();
        }
    }
};

#endif // APP_H
