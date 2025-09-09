#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "blurs.h"

#define PI 3.14159

void blur_utils::rotate_channel90CW(vector <unsigned char> &channel_data){
    vector<unsigned char> rotated(width * height);
    for (int r = 0; r < height; r++)
        for (int c = 0; c < width; c++)
            rotated[c * height + (height - 1 - r)] = channel_data[r * width + c];

    channel_data.swap(rotated);
}

void blur_utils::rotate_channel180(vector <unsigned char> &channel_data) {
    vector<unsigned char> rotated(width * height);
    for (int r = 0; r < height; r++)
        for (int c = 0; c < width; c++)
            rotated[(height - 1 - r) * width + (width - 1 - c)] = channel_data[r * width + c];

    channel_data.swap(rotated);
}

void blur_utils::rotate_channel90CCW(vector <unsigned char> &channel_data) {
    vector<unsigned char> rotated(width * height);
    for (int r = 0; r < height; r++)
        for (int c = 0; c < width; c++)
            rotated[(width - 1 - c) * height + r] = channel_data[r * width + c];

    channel_data.swap(rotated);
}

void blur_utils::rotate90CW() {
    rotate_channel90CW(red_channel);
    rotate_channel90CW(green_channel);
    rotate_channel90CW(blue_channel);
    swap(width, height);
}

void blur_utils::rotate180() {
    rotate_channel180(red_channel);
    rotate_channel180(green_channel);
    rotate_channel180(blue_channel);
}

void blur_utils::rotate90CCW() {
    rotate_channel90CCW(red_channel);
    rotate_channel90CCW(green_channel);
    rotate_channel90CCW(blue_channel);
    swap(width, height);
}

int blur_utils::get_exif_orientation(const char* img_path) {
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

void blur_utils::fix_orientation(const char* img_path) {
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

// box blur helpers
void blur_utils::boxBlurHelper(vector<unsigned char> &channel_data, int kernel_size){
    int margin = (kernel_size-1) / 2;
    vector <unsigned char> blurred_data(channel_data.size());   // to avoid using already changed pixels

    for(int row_point = 0; row_point < height; row_point++)     // for handling the edges, we start from 0,0            
        for(int column_point = 0; column_point < width; column_point++){
            int sum = 0;
            int count = 0;  // since we don't get the whole kernel at the edges, we need to count the actual pixels

            for (int i = -margin; i < (margin+1); i++)
                for (int j = -margin; j < (margin+1); j++)
                    if (row_point+i >= 0 && row_point+i < height && column_point+j >= 0 && column_point+j < width){
                            sum += channel_data[(row_point+i) * width + (column_point+j)];
                            count++;
                        }
            unsigned char avg = sum / count;
            // calculated the avg and now place the value to all the pixels
            blurred_data[row_point* width + column_point] = avg;
        }
    
    swap(channel_data, blurred_data);
}

void blur_utils::boxBlurHelperHorizontal(vector<unsigned char> &channel_data, int kernel_size){
    int margin = (kernel_size-1) / 2;
    vector <unsigned char> blurred_data(channel_data.size());

    for(int row_point = 0; row_point < height; row_point++) 
        for(int column_point = 0; column_point < width; column_point++){
            int sum = 0;
            int count = 0;

            for (int j = -margin; j < (margin+1); j++)
                if (column_point+j >= 0 && column_point+j < width){
                            sum += channel_data[row_point * width + (column_point+j)];
                            count++;
                        }

            unsigned char avg = sum / count;
            blurred_data[row_point* width + column_point] = avg;
        }
    
    swap(channel_data, blurred_data);
}

void blur_utils::boxBlurHelperVertical(vector<unsigned char> &channel_data, int kernel_size){
    int margin = (kernel_size-1) / 2;
    vector <unsigned char> blurred_data(channel_data.size());

    for(int row_point = 0; row_point < height; row_point++)            
        for(int column_point = 0; column_point < width; column_point++){
            int sum = 0;
            int count = 0;

            for (int i = -margin; i < (margin+1); i++)
                if (row_point+i >= 0 && row_point+i < height){
                            sum += channel_data[(row_point+i) * width + column_point];
                            count++;
                        }

            unsigned char avg = sum / count;
            blurred_data[row_point* width + column_point] = avg;
        }
    swap(channel_data, blurred_data);
}

// gaussian blur helpers
void blur_utils::gaussianBlurHelper(vector<unsigned char> &channel_data, double standard_deviation){
    // TODO
}

void blur_utils::gaussianBlurHelperHorizontal(vector<unsigned char> &channel_data, double standard_deviation){
    // TODO
}

void blur_utils::gaussianBlurHelperVertical(vector<unsigned char> &channel_data, double standard_deviation){
    // TODO
}

void blur_utils::load_image(const string& img_path){
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
    if (img_path.size() > 9)    // avoid out of range
        filename = img_path.substr(5, img_path.size() - 5 - 4);
    else
        filename = img_path;    // fallback

    stbi_image_free(data);
}

bool blur_utils::save_image_as_jpg() {
    string return_name = "out/" + filename + ".jpg";
    size_t channel_size = width * height;
    vector <unsigned char> image_data(channel_size*channels);

    for (int i = 0; i < channel_size; i++) {
        image_data[i*channels] = red_channel[i];
        image_data[i*channels + 1] = green_channel[i];
        image_data[i*channels + 2] = blue_channel[i];
    }
    return stbi_write_jpg(return_name.c_str(), width, height, channels, image_data.data(), 90) != 0;
}

bool blur_utils::boxBlur(int fraction){
    if (fraction % 2 == 0) 
        cout << fraction++ <<" kernel size must be odd, increasing it by 1." << endl;
    
    if (fraction < 3) {
        cout << "Kernel size must be at least 3, setting it to 3." << endl;
        fraction = 3;
    }
    if (fraction > 101) {
        cout << "Kernel size too large, setting it to 101." << endl;
        fraction = 101;
    }

    cout << "Blurring red channel..." << endl;
    boxBlurHelper(red_channel, fraction);
    cout << "Blurring green channel..." << endl;
    boxBlurHelper(green_channel, fraction);
    cout << "Blurring blue channel..." << endl;
    boxBlurHelper(blue_channel, fraction);
    return save_image_as_jpg();
}

bool blur_utils::boxBlurOptimized(int fraction){
    if (fraction % 2 == 0) 
        cout << fraction++ <<" kernel size must be odd, increasing it by 1." << endl;
    
    if (fraction < 3) {
        cout << "Kernel size must be at least 3, setting it to 3." << endl;
        fraction = 3;
    }
    if (fraction > 101) {
        cout << "Kernel size too large, setting it to 101." << endl;
        fraction = 101;
    }

    // horizontal pass
    cout << "Red channel horizontal" << endl;
    boxBlurHelperHorizontal(red_channel, fraction);
    cout << "Blue channel horizontal" << endl;
    boxBlurHelperHorizontal(green_channel, fraction);
    cout << "Green channel horizontal" << endl;
    boxBlurHelperHorizontal(blue_channel, fraction);

    // vertical pass
    cout << "Red channel vertical" << endl;
    boxBlurHelperVertical(red_channel, fraction);
    cout << "Blue channel vertical" << endl;
    boxBlurHelperVertical(green_channel, fraction);
    cout << "Green channel vertical" << endl;
    boxBlurHelperVertical(blue_channel, fraction);

    return save_image_as_jpg();
}

bool blur_utils::gaussianBlur(double standart_deviation){
    if (standart_deviation <= 0) {
        cout << "Standard deviation must be positive, setting it to 1." << endl;
        standart_deviation = 1.0;
    }
    cout << "Blurring red channel..." << endl;
    gaussianBlurHelper(red_channel, standart_deviation);
    cout << "Blurring green channel..." << endl;
    gaussianBlurHelper(green_channel, standart_deviation);
    cout << "Blurring blue channel..." << endl;
    gaussianBlurHelper(blue_channel, standart_deviation);

    return save_image_as_jpg();
}

bool blur_utils::gaussianBlurOptimized(double standart_deviation){
    if (standart_deviation <= 0) {
        cout << "Standard deviation must be positive, setting it to 1." << endl;
        standart_deviation = 1.0;
    }

    // horizontal pass
    cout << "Red channel horizontal" << endl;
    boxBlurHelperHorizontal(red_channel, standart_deviation);
    cout << "Blue channel horizontal" << endl;
    boxBlurHelperHorizontal(green_channel, standart_deviation);
    cout << "Green channel horizontal" << endl;
    boxBlurHelperHorizontal(blue_channel, standart_deviation);

    // vertical pass
    cout << "Red channel vertical" << endl;
    boxBlurHelperVertical(red_channel, standart_deviation);
    cout << "Blue channel vertical" << endl;
    boxBlurHelperVertical(green_channel, standart_deviation);
    cout << "Green channel vertical" << endl;
    boxBlurHelperVertical(blue_channel, standart_deviation);

    return save_image_as_jpg();
}
