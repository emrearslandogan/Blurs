#include "basics.h"

#include <filesystem>
#include <iostream>
#include <vector>

using namespace std;

// TODO folder wise scanning just like the ImgToAscii

int main() {
    file_work file_work;
    int width, height;

    file_work.load_image("test/DSC03056.JPG", width, height);
    file_work.save_image_as_png();
    return 0;

}