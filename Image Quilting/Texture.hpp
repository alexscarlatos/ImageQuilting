//
//  Texture.hpp
//  Image Quilting
//
//  Created by Alex Scarlatos on 3/31/16.
//  Copyright © 2016 Alex Scarlatos. All rights reserved.
//

#ifndef Texture_hpp
#define Texture_hpp

#include <stdio.h>
#include <vector>
#include "SourceImage.hpp"
#include "Image.hpp"

struct block {
    int sourceImageIndex;
    int x;
    int y;
    int size;
    int *borderPathLeft;
    int *borderPathBottom;
};

class Texture {
private:
    // A list of which blocks to draw to the texture, reference index from sourceImage
    std::vector<block*> blocks;
    SourceImage *sourceImage;
    Image *targetImage;
    int cols, rows;
    int width, height;
public:
    Texture();
    Texture(SourceImage *sImage, int w, int h);
    Texture(SourceImage *sImage, Image *tImage);
    ~Texture();
    void generateTexture();
    void drawTexture();
    int getWidth();
    int getHeight();
};

#endif /* Texture_hpp */
