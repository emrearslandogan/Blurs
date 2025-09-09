#include "blurs.h"

int main(){
    blur_utils blur_engine;

    blur_engine.load_image("test/DSC03056.JPG");
    
    blur_engine.boxBlurOptimized(50);

    blur_engine.save_image_as_jpg();
    return 0;
}