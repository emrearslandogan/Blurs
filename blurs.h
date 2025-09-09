#ifndef BLUR_UTILS_H
#define BLUR_UTILS_H

#include "stb_image.h"
#include "stb_image_write.h"
#include <filesystem>
#include <string>
#include <vector>
#include <libexif/exif-data.h>
#include <iostream>
#include <cmath>

using namespace std;

class blur_utils {
private:
    vector <unsigned char> red_channel;
    vector <unsigned char> green_channel;
    vector <unsigned char> blue_channel;
    string filename;
    int width = 0, height = 0, channels = 3;

    // helper functions for general use
    // functions regarding orientation problem
    void rotate_channel90CW(vector <unsigned char> &channel_data);
    void rotate_channel180(vector <unsigned char> &channel_data);
    void rotate_channel90CCW(vector <unsigned char> &channel_data);
    void rotate90CW();
    void rotate180();
    void rotate90CCW();
    static int get_exif_orientation(const char* img_path);
    void fix_orientation(const char* img_path);

    // functions regarding box-blur algorithm 
    void boxBlurHelper(vector <unsigned char> &channel_data, int kernel_size);     // Non-optimized version
    void boxBlurHelperHorizontal(vector <unsigned char> &channel_data, int kernel_size);   // two-pass version
    void boxBlurHelperVertical(vector <unsigned char> &channel_data, int kernel_size);
    
    vector<vector <double>> generateGaussianKernel(double standard_deviation);
    void applyKernel(vector<unsigned char> &channel_data, vector<vector <double>> &kernel);
    void applyKernelHorizontal(vector<unsigned char> &channel_data, vector<double> &kernel);
    void applyKernelVertical(vector<unsigned char> &channel_data, vector<double> &kernel);

public:
    // File IO
    void load_image(const string& img_path);
    bool save_image_as_jpg();

    // Box-blur functions
    bool boxBlur(int fraction);
    bool boxBlurOptimized(int fraction);

    // Gaussian-blur functions
    bool gaussianBlur(double standart_deviation);
    bool gaussianBlurOptimized(double standart_deviation);

    // pipelines
    void boxBlurPipeline(string img_path, int fraction);
    void gaussianPipeline(string img_path, double standart_deviation);

    // batch pipelines
    void batchBoxBlur(string folder_path, int fraction);
    void batchGaussianBlur(string folder_path, double standard_deviation);
};

#endif //BLUR_UTILS_H