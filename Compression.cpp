// #include "compression.h"

// #include <fstream>
// #include <chrono>
// #include <queue>
// #include <string>

// using namespace std;
// using namespace std::chrono;

// double rleCompressionTime = 0;
// double huffmanCompressionTime = 0;

// struct HuffmanNode
// {
//     unsigned char data;
//     int freq;

//     HuffmanNode* left;
//     HuffmanNode* right;

//     HuffmanNode(unsigned char d, int f)
//     {
//         data = d;
//         freq = f;

//         left = nullptr;
//         right = nullptr;
//     }
// };

// struct Compare
// {
//     bool operator()(HuffmanNode* a, HuffmanNode* b)
//     {
//         return a->freq > b->freq;
//     }
// };

// string huffmanCodes[256];

// void GenerateCodes(HuffmanNode* root, string code)
// {
//     if (!root)
//         return;

//     if (!root->left && !root->right)
//     {
//         huffmanCodes[root->data] = code;
//     }

//     GenerateCodes(root->left, code + "0");
//     GenerateCodes(root->right, code + "1");
// }

// bool CompressImageRLE(const ImageData& img, const char* filename)
// {
//     auto start =
//         high_resolution_clock::now();

//     ofstream file(filename, ios::binary);

//     if (!file.is_open())
//         return false;

//     file.write((char*)&img.width, sizeof(int));
//     file.write((char*)&img.height, sizeof(int));

//     int total = img.width * img.height * 3;

//     unsigned char* raw = new unsigned char[total];

//     for (int i = 0; i < img.width * img.height; i++)
//     {
//         raw[i * 3 + 0] = img.pixels[i].r;
//         raw[i * 3 + 1] = img.pixels[i].g;
//         raw[i * 3 + 2] = img.pixels[i].b;
//     }

//     int i = 0;

//     while (i < total)
//     {
//         unsigned char value = raw[i];
//         unsigned char count = 1;

//         while ( i + count < total && count < 255 )
//         {
//             if (raw[i + count] != value) break;
//             count++;
//         }

//         file.write((char*)&count, 1);
//         file.write((char*)&value, 1);

//         i += count;
//     }

//     delete[] raw;

//     file.close();

//     auto end = high_resolution_clock::now();

//     rleCompressionTime = duration<double, milli>(end - start).count();

//     return true;
// }

// bool DecompressImageRLE(const char* filename, ImageData& img)
// {
//     ifstream file(filename, ios::binary);

//     if (!file.is_open())
//         return false;

//     file.read((char*)&img.width, sizeof(int));
//     file.read((char*)&img.height, sizeof(int));

//     int total = img.width * img.height * 3;

//     unsigned char* raw = new unsigned char[total];

//     int index = 0;

//     while (index < total && !file.eof())
//     {
//         unsigned char count;
//         unsigned char value;

//         file.read((char*)&count, 1);
//         file.read((char*)&value, 1);

//         for (int i = 0; i < count; i++)
//         {
//             raw[index++] = value;
//         }
//     }

//     img.pixels = new Pixel[img.width * img.height];

//     for (int i = 0; i < img.width * img.height; i++)
//     {
//         img.pixels[i].r = raw[i * 3 + 0];
//         img.pixels[i].g = raw[i * 3 + 1];
//         img.pixels[i].b = raw[i * 3 + 2];
//     }

//     delete[] raw;

//     file.close();

//     return true;
// }

// bool CompressImageHuffman(const ImageData& img, const char* filename)
// {
//     auto start = high_resolution_clock::now();

//     int total = img.width * img.height * 3;

//     unsigned char* raw = new unsigned char[total];

//     for (int i = 0; i < img.width * img.height; i++)
//     {
//         raw[i * 3 + 0] = img.pixels[i].r;
//         raw[i * 3 + 1] = img.pixels[i].g;
//         raw[i * 3 + 2] = img.pixels[i].b;
//     }

//     int freq[256] = { 0 };

//     for (int i = 0; i < total; i++)
//     {
//         freq[raw[i]]++;
//     }

//     priority_queue<
//         HuffmanNode*,
//         vector<HuffmanNode*>,
//         Compare> pq;

//     for (int i = 0; i < 256; i++)
//     {
//         if (freq[i] > 0)
//         {
//             pq.push(new HuffmanNode(i, freq[i]));
//         }
//     }

//     while (pq.size() > 1)
//     {
//         HuffmanNode* left = pq.top();
//         pq.pop();

//         HuffmanNode* right = pq.top();
//         pq.pop();

//         HuffmanNode* merged =
//             new HuffmanNode(
//                 '\0',
//                 left->freq + right->freq
//             );

//         merged->left = left;
//         merged->right = right;

//         pq.push(merged);
//     }

//     GenerateCodes(pq.top(), "");

//     ofstream file(filename, ios::binary);

//     if (!file.is_open())
//         return false;

//     file << img.width << " "
//         << img.height << "\n";

//     for (int i = 0; i < total; i++)
//     {
//         file << huffmanCodes[raw[i]];
//     }

//     delete[] raw;

//     file.close();

//     auto end =
//         high_resolution_clock::now();

//     huffmanCompressionTime =
//         duration<double, milli>(end - start).count();

//     return true;
// }

// double GetRLECompressionTime()
// {
//     return rleCompressionTime;
// }

// double GetHuffmanCompressionTime()
// {
//     return huffmanCompressionTime;
// }




#include "compression.h"

#include <fstream>
#include <chrono>
#include <queue>
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

void DeleteTree(HuffmanNode* root)
{
    if (!root)
        return;

    DeleteTree(root->left);
    DeleteTree(root->right);

    delete root;
}

void GenerateCodes(HuffmanNode* root, string code)
{
    if (!root)
        return;
    if (!root->left && !root->right)
    {
        if (code.empty())
            code = "0";

        huffmanCodes[root->data] = code;
        return;
    }

    GenerateCodes(root->left, code + "0");
    GenerateCodes(root->right, code + "1");
}

bool CompressImageRLE(const ImageData& img, const char* filename)
{
    auto start = high_resolution_clock::now();

    ofstream file(filename, ios::binary);

    if (!file.is_open())
        return false;

    file.write((char*)&img.width, sizeof(int));
    file.write((char*)&img.height, sizeof(int));

    int total = img.width * img.height * 3;

    unsigned char* raw = new unsigned char[total];

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

        while (i + count < total &&
               count < 255 &&
               raw[i + count] == value)
        {
            count++;
        }

        file.write((char*)&count, 1);
        file.write((char*)&value, 1);

        i += count;
    }

    delete[] raw;

    file.close();

    auto end = high_resolution_clock::now();

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

    int total = img.width * img.height * 3;

    unsigned char* raw = new unsigned char[total];

    int index = 0;

    while (index < total)
    {
        unsigned char count;
        unsigned char value;

        if (!file.read((char*)&count, 1))
            break;

        if (!file.read((char*)&value, 1))
            break;

        for (int i = 0; i < count && index < total; i++)
        {
            raw[index++] = value;
        }
    }

    img.pixels = new Pixel[img.width * img.height];

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
    auto start = high_resolution_clock::now();

    for (int i = 0; i < 256; i++)
    {
        huffmanCodes[i].clear();
    }

    int total = img.width * img.height * 3;

    unsigned char* raw = new unsigned char[total];

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
        vector<HuffmanNode*>,
        Compare> pq;

    for (int i = 0; i < 256; i++)
    {
        if (freq[i] > 0)
        {
            pq.push(new HuffmanNode((unsigned char)i, freq[i]));
        }
    }

    if (pq.empty())
    {
        delete[] raw;
        return false;
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

    HuffmanNode* root = pq.top();

    GenerateCodes(root, "");

    ofstream file(filename, ios::binary);

    if (!file.is_open())
    {
        delete[] raw;
        DeleteTree(root);
        return false;
    }

    file.write((char*)&img.width, sizeof(int));
    file.write((char*)&img.height, sizeof(int));

    file.write((char*)freq, sizeof(freq));


    unsigned char buffer = 0;
    int bitCount = 0;

    for (int i = 0; i < total; i++)
    {
        string& code = huffmanCodes[raw[i]];

        for (char bit : code)
        {
            buffer <<= 1;

            if (bit == '1')
                buffer |= 1;

            bitCount++;

            if (bitCount == 8)
            {
                file.write((char*)&buffer, 1);

                buffer = 0;
                bitCount = 0;
            }
        }
    }


    if (bitCount > 0)
    {
        buffer <<= (8 - bitCount);
        file.write((char*)&buffer, 1);
    }


    delete[] raw;

    DeleteTree(root);

    file.close();

    auto end = high_resolution_clock::now();

    huffmanCompressionTime =
        duration<double, milli>(end - start).count();

    return true;
}


bool DecompressImageHuffman(const char* filename, ImageData& img)
{
    ifstream file(filename, ios::binary);

    if (!file.is_open())
        return false;


    file.read((char*)&img.width, sizeof(int));
    file.read((char*)&img.height, sizeof(int));

    int total = img.width * img.height * 3;

    int freq[256];

    file.read((char*)freq, sizeof(freq));

    priority_queue<
        HuffmanNode*,
        vector<HuffmanNode*>,
        Compare> pq;

    for (int i = 0; i < 256; i++)
    {
        if (freq[i] > 0)
        {
            pq.push(new HuffmanNode((unsigned char)i, freq[i]));
        }
    }

    if (pq.empty())
        return false;

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

    HuffmanNode* root = pq.top();

    unsigned char* raw = new unsigned char[total];

    int index = 0;

    HuffmanNode* current = root;

    unsigned char byte;

    while (file.read((char*)&byte, 1) && index < total)
    {
        for (int bit = 7; bit >= 0; bit--)
        {
            bool bitValue = (byte >> bit) & 1;

            if (bitValue)
                current = current->right;
            else
                current = current->left;

            if (!current->left && !current->right)
            {
                raw[index++] = current->data;

                current = root;

                if (index >= total)
                    break;
            }
        }
    }


    img.pixels = new Pixel[img.width * img.height];

    for (int i = 0; i < img.width * img.height; i++)
    {
        img.pixels[i].r = raw[i * 3 + 0];
        img.pixels[i].g = raw[i * 3 + 1];
        img.pixels[i].b = raw[i * 3 + 2];
    }


    delete[] raw;

    DeleteTree(root);

    file.close();

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

