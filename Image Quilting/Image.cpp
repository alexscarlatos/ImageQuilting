//
//  Image.cpp
//  Image Quilting
//
//  Created by Alex Scarlatos on 5/2/16.
//  Copyright © 2016 Alex Scarlatos. All rights reserved.
//

#include "Image.hpp"
#include <iostream>
#include <string>

// Create standard image object from filepath
Image::Image(const char *filename) {
    std::string fname = (std::string)filename;
    std::string extension = fname.substr(fname.find_last_of("."));
    if (extension.compare(".ppm") == 0)
        readPPM(filename, &imageData);
    else if (extension.compare(".bmp") == 0)
        readBMP(filename);
    else {
        std::cout << fname << " is of an unsupported file type.\n";
        exit(-1);
    }
}

Image::~Image() {
    delete [] imageData;
}

// Read a ppm file and put the contents in pic and set width and height
void Image::readPPM(const char *filename, GLubyte **pic) {
    FILE *file;
    char line[80];
    int size, rowSize;
    file = fopen(filename, "rb");
    if (file == NULL) {
        std::cout << filename << " cannot be read.\n";
        exit(-1);
    }
    
    // line with file type
    fgets(line, 80, file);
    
    // line with width and height
    fgets(line, 80, file);
    sscanf(line, "%d %d", &width, &height);
    std::cout << "Reading " << filename << "...\nwidth:" << width << " height:" << height << "\n";
    size = width * height * 3; // 3 bytes per pixel
    rowSize = width * 3;
    *pic = new GLubyte[size];
    
    fgets(line, 80, file);
    
    // Read image from bottom up
    GLubyte *ptr;
    ptr = *pic + (height-1) * rowSize;
    for (int i = height; i > 0; i--) {
        fread(ptr, 1, rowSize, file);
        ptr -= rowSize;
    }
    fclose(file);
}

// Read a bmp file and put the contents in pic and set width and height
void Image::readBMP(const char *filename) {
    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        std::cout << filename << " cannot be read.\n";
        exit(-1);
    }
    
    // read the 54-byte header
    GLubyte info[54];
    fread(info, sizeof(GLubyte), 54, f);
    
    // extract image height and width from header
    width = *(int*)&info[18];
    height = *(int*)&info[22];
    std::cout << "Reading " << filename << "...\nwidth:" << width << " height:" << height << "\n";
    
    int size = 3 * width * height;
    imageData = new GLubyte[size]; // allocate 3 bytes per pixel

    // read in the bitmap image data
    fread(imageData, size, 1, f);
    
    // make sure bitmap image data was read
    if (imageData == NULL) {
        std::cout << filename << " cannot be read.\n";
        fclose(f);
        exit(-1);
    }

    // swap the r and b values to get RGB (bitmap is BGR)
    for (int imageIdx = 0; imageIdx < size; imageIdx += 3) {
        GLubyte tempRGB = imageData[imageIdx];
        imageData[imageIdx] = imageData[imageIdx + 2];
        imageData[imageIdx + 2] = tempRGB;
    }
    
    //close file and return bitmap iamge data
    fclose(f);
}

// Draw this image on the screen
void Image::drawFullImage() {
    glDrawBuffer(GL_FRONT);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
}

// Copy the data of this image at the given coordinates and size into targetArray
void Image::readPixels(int startX, int startY, int readWidth, int readHeight, GLubyte *targetArray) {
    /*
    // Cancel read if coordinates go out of bounds
    if (startY + readHeight > height || startX + readWidth > width) {
        std::cout << "Invalid coordinates for read pixels.\n";
        return;
    }
    */
    for (int y = 0; y < readHeight; y++) {
        for (int x = 0; x < readWidth; x++) {
            // TODO: excessive calculation...
            int targetIndex = (y * readWidth + x) * 3;
            int sourceIndex = ((startY + y) * width + startX + x) * 3;
            // TODO: this can probably be done quicker, without a check every iteration
            if (x + startX > width || y + startY >= height) {
                targetArray[targetIndex] = 0;
                targetArray[targetIndex + 1] = 0;
                targetArray[targetIndex + 2] = 0;
            } else {
                targetArray[targetIndex] = imageData[sourceIndex];
                targetArray[targetIndex + 1] = imageData[sourceIndex + 1];
                targetArray[targetIndex + 2] = imageData[sourceIndex + 2];
            }
        }
    }
}
