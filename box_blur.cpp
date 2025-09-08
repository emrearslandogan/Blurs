#include "basics.h"

#include <filesystem>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

int main() {
    file_work file_work;

    file_work.load_image("test/DSC03056.JPG");
    
    file_work.boxBlur(175);

    file_work.save_image_as_png();
    return 0;

}