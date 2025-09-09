#include "blurs.h"

int main(){
    blur_utils blur_engine;

    blur_engine.load_image("test/DSC05508.JPG");
    
    blur_engine.gaussianBlur(7.0);

    blur_engine.save_image_as_jpg();
    return 0;
}