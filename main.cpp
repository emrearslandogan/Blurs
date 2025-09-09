#include "blurs.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]){
    if (argc < 4)
        cout << "Not enough parameters. State path, gaussian or mean, then the standard deviation for gaussian and kernel size for mean blur...\n";
    else if (argc > 4)
        cout << "Too much parameters, work correctly\n";
    
    blur_utils blur_engine;

    string img_path(argv[1]);
    string blur_type(argv[2]);
    string blur_value(argv[3]);
    
    if (fs::is_directory(img_path)){
        if (blur_type == "gaussian")
            blur_engine.batchGaussianBlur(img_path, stod(blur_value));

        else if ((blur_type == "mean"))
            blur_engine.batchBoxBlur(img_path, stoi(blur_value));

        else{
            cout << "please provide 'gaussian' or 'mean' as parameter...\n";
            return 0;
        }
    }

    else if (fs::is_regular_file(img_path)){
        if (blur_type == "gaussian")
            blur_engine.gaussianPipeline(img_path, stod(blur_value));

        else if ((blur_type == "mean"))
            blur_engine.boxBlurPipeline(img_path, stoi(blur_value));

        else{
            cout << "please provide 'gaussian' or 'mean' as parameter...\n";
            return 0;
        }
    }
    return 0;
}