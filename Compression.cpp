#include "compression.h"

#include <fstream>
#include <chrono>
#include <queue>
#include <deque>
#include <string>

using namespace std;
using namespace std::chrono;

double rleCompressionTime = 0;
double huffmanCompressionTime = 0;

struct HuffmanNode
{
    unsigned char data;
    int freq;

    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(unsigned char d, int f)
    {
        data = d;
        freq = f;

        left = nullptr;
        right = nullptr;
    }
};

struct Compare
{
    bool operator()(HuffmanNode* a, HuffmanNode* b)
    {
        return a->freq > b->freq;
    }
};

string huffmanCodes[256];

void GenerateCodes(HuffmanNode* root, string code)
{
    if (!root)
        return;

    if (!root->left && !root->right)
    {
        huffmanCodes[root->data] = code;
    }

    GenerateCodes(root->left, code + "0");
    GenerateCodes(root->right, code + "1");
}

bool CompressImageRLE(const ImageData& img, const char* filename)
{
    auto start =
        high_resolution_clock::now();

    ofstream file(filename, ios::binary);

    if (!file.is_open())
        return false;

    file.write((char*)&img.width, sizeof(int));
    file.write((char*)&img.height, sizeof(int));

    int total =
        img.width * img.height * 3;

    unsigned char* raw =
        new unsigned char[total];

    for (int i = 0; i < img.width * img.height; i++)
    {
        raw[i * 3 + 0] = img.pixels[i].r;
        raw[i * 3 + 1] = img.pixels[i].g;
        raw[i * 3 + 2] = img.pixels[i].b;
    }

    int i = 0;

    while (i < total)
    {
        unsigned char value = raw[i];
        unsigned char count = 1;

        while (
            i + count < total &&
            raw[i + count] == value &&
            count < 255
            )
        {
            count++;
        }

        file.write((char*)&count, 1);
        file.write((char*)&value, 1);

        i += count;
    }

    delete[] raw;

    file.close();

    auto end =
        high_resolution_clock::now();

    rleCompressionTime =
        duration<double, milli>(end - start).count();

    return true;
}

bool DecompressImageRLE(const char* filename, ImageData& img)
{
    ifstream file(filename, ios::binary);

    if (!file.is_open())
        return false;

    file.read((char*)&img.width, sizeof(int));
    file.read((char*)&img.height, sizeof(int));

    int total =
        img.width * img.height * 3;

    unsigned char* raw =
        new unsigned char[total];

    int index = 0;

    while (index < total && !file.eof())
    {
        unsigned char count;
        unsigned char value;

        file.read((char*)&count, 1);
        file.read((char*)&value, 1);

        for (int i = 0; i < count; i++)
        {
            raw[index++] = value;
        }
    }

    img.pixels =
        new Pixel[img.width * img.height];

    for (int i = 0; i < img.width * img.height; i++)
    {
        img.pixels[i].r = raw[i * 3 + 0];
        img.pixels[i].g = raw[i * 3 + 1];
        img.pixels[i].b = raw[i * 3 + 2];
    }

    delete[] raw;

    file.close();

    return true;
}

bool CompressImageHuffman(const ImageData& img, const char* filename)
{
    auto start =
        high_resolution_clock::now();

    int total =
        img.width * img.height * 3;

    unsigned char* raw =
        new unsigned char[total];

    for (int i = 0; i < img.width * img.height; i++)
    {
        raw[i * 3 + 0] = img.pixels[i].r;
        raw[i * 3 + 1] = img.pixels[i].g;
        raw[i * 3 + 2] = img.pixels[i].b;
    }

    int freq[256] = { 0 };

    for (int i = 0; i < total; i++)
    {
        freq[raw[i]]++;
    }

    priority_queue<
        HuffmanNode*,
        deque<HuffmanNode*>,
        Compare> pq;

    for (int i = 0; i < 256; i++)
    {
        if (freq[i] > 0)
        {
            pq.push(new HuffmanNode(i, freq[i]));
        }
    }

    while (pq.size() > 1)
    {
        HuffmanNode* left = pq.top();
        pq.pop();

        HuffmanNode* right = pq.top();
        pq.pop();

        HuffmanNode* merged =
            new HuffmanNode(
                '\0',
                left->freq + right->freq
            );

        merged->left = left;
        merged->right = right;

        pq.push(merged);
    }

    GenerateCodes(pq.top(), "");

    ofstream file(filename);

    if (!file.is_open())
        return false;

    file << img.width << " "
        << img.height << "\n";

    for (int i = 0; i < total; i++)
    {
        file << huffmanCodes[raw[i]];
    }

    delete[] raw;

    file.close();

    auto end =
        high_resolution_clock::now();

    huffmanCompressionTime =
        duration<double, milli>(end - start).count();

    return true;
}

double GetRLECompressionTime()
{
    return rleCompressionTime;
}

double GetHuffmanCompressionTime()
{
    return huffmanCompressionTime;
}