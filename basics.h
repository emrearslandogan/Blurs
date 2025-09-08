//
// Created by emre on 9/8/25.
/// Some code about the rotation logic, loading and writing images are copied from ImgToAscii project

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#ifndef BLURS_BASICS_H
#define BLURS_BASICS_H

#include "stb_image.h"
#include "stb_image_write.h"
#include <filesystem>
#include <string>
#include <vector>
#include <libexif/exif-data.h>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

class file_work {
private:
    vector <unsigned char> red_channel;
    vector <unsigned char> green_channel;
    vector <unsigned char> blue_channel;
    string filename;
    int width = 0, height = 0, channels = 3;
    friend class Blurs;

    // Functions regarding orientation problem
    void rotate_channel90CW(vector <unsigned char> &channel_data){
        vector<unsigned char> rotated(width * height);
        for (int r = 0; r < height; r++)
            for (int c = 0; c < width; c++)
                rotated[c * height + (height - 1 - r)] = channel_data[r * width + c];

        channel_data.swap(rotated);
    }

    void rotate_channel180(vector <unsigned char> &channel_data) {
        vector<unsigned char> rotated(width * height);
        for (int r = 0; r < height; r++)
            for (int c = 0; c < width; c++)
                rotated[(height - 1 - r) * width + (width - 1 - c)] = channel_data[r * width + c];

        channel_data.swap(rotated);
    }

    void rotate_channel90CCW(vector <unsigned char> &channel_data) {
        vector<unsigned char> rotated(width * height);
        for (int r = 0; r < height; r++)
            for (int c = 0; c < width; c++)
                rotated[(width - 1 - c) * height + r] = channel_data[r * width + c];

        channel_data.swap(rotated);
    }

    void rotate90CW() {
        rotate_channel90CW(red_channel);
        rotate_channel90CW(green_channel);
        rotate_channel90CW(blue_channel);
		swap(width, height);
    }

    void rotate180() {
        rotate_channel180(red_channel);
        rotate_channel180(green_channel);
        rotate_channel180(blue_channel);
    }

    void rotate90CCW() {
        rotate_channel90CCW(red_channel);
        rotate_channel90CCW(green_channel);
        rotate_channel90CCW(blue_channel);
		swap(width, height);
    }

    static int get_exif_orientation(const char* img_path) {
        ExifData* exifData = exif_data_new_from_file(img_path);
        if (!exifData)
            return 0; // No EXIF data found

        int orientation = 0;
        ExifByteOrder byteOrder = exif_data_get_byte_order(exifData);
        ExifEntry* entry = exif_data_get_entry(exifData, EXIF_TAG_ORIENTATION);

        if (entry)  orientation = exif_get_short(entry->data, byteOrder);

        exif_data_free(exifData);
        return orientation;
    }

    void fix_orientation(const char* img_path) {
        switch(get_exif_orientation(img_path)){
            case 1: // normal
                break;
            case 3: // rotated 180
                rotate180();
                break;
            case 6:
                rotate90CW();
                break;
            case 8:
                rotate90CCW();
                break;
            default:
                break;
        }
    }

    void boxBlurHelper(vector<unsigned char> &channel_data, int kernel_size){
        int margin = (kernel_size-1) / 2;
        vector <unsigned char> original = channel_data; // to avoid using already changed pixels

        for(int row = margin; row + margin < height; row++){
            if (row % 50 == 0)
                cout << "Processing row " << row << " of " << height << endl;
            else
                continue;
            for(int column = margin; column + margin < width; column++){
                float sum = 0;

                for (int i = -margin; i < (margin+1); i++)
                    for (int j = -margin; j < (margin+1); j++)
                        sum += original[(row+i) * width + (column+j)];

                float avg = sum / (kernel_size * kernel_size);
                // calculated the avg and now place the value to all the pixels
                channel_data[row* width + column] = avg;
            }
        }
    }

public:
// MAIN FUNCTIONS
    void load_image(const string& img_path){
        unsigned char* data = stbi_load(img_path.c_str(), &width, &height, &channels, channels);

        if (!data)  throw runtime_error("Failed to load the image given: " + string(stbi_failure_reason()));

        size_t channel_size = width * height;
        // will need 3 loops to assign every channel a distinct vector;
        red_channel.resize(channel_size);
        green_channel.resize(channel_size);
        blue_channel.resize(channel_size);

        for (int i = 0; i < channel_size; i++)
            red_channel[i] = data[i*channels];
        for (int i = 0; i < channel_size; i++)
            green_channel[i] = data[i*channels + 1];
        for (int i = 0; i < channel_size; i++)
            blue_channel[i] = data[i*channels + 2];

        int orientation = get_exif_orientation(img_path.c_str());
        if (orientation == 3 || orientation == 6 || orientation == 8)
            fix_orientation(img_path.c_str());

        // assign filename except first 5 and last 4 chars, for test/ and .jpg
        if (img_path.size() > 9)// avoid out of range
            filename = img_path.substr(5, img_path.size() - 5 - 4);
        else
            filename = img_path; // fallback

        stbi_image_free(data);
    }

    bool save_image_as_png() {
        string return_name = "out/" + filename + ".png";
		size_t channel_size = width * height;
        vector <unsigned char> image_data(channel_size*channels);

        for (int i = 0; i < channel_size; i++) {
            image_data[i*channels] = red_channel[i];
            image_data[i*channels + 1] = green_channel[i];
            image_data[i*channels + 2] = blue_channel[i];
        }
        return stbi_write_jpg(return_name.c_str(), width, height, channels, image_data.data(), 90) != 0;
    }

    bool boxBlur(int fraction){
        cout << "Blurring red channel..." << endl;
        boxBlurHelper(red_channel, fraction);
        cout << "Blurring green channel..." << endl;
        boxBlurHelper(green_channel, fraction);
        cout << "Blurring blue channel..." << endl;
        boxBlurHelper(blue_channel, fraction);
        return save_image_as_png();
    }
};

#endif //BLURS_BASICS_H