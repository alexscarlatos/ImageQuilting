//
//  SourceImage.hpp
//  Image Quilting
//
//  Created by Alex Scarlatos on 3/31/16.
//  Copyright © 2016 Alex Scarlatos. All rights reserved.
//

#ifndef SourceImage_hpp
#define SourceImage_hpp

#include <stdio.h>
#include "Image.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

enum BlockMatch {
    Right,
    Top,
    Both,
    None
};

class SourceImage {
private:
    Image *image;
    GLint numCols, numRows;
    GLint blockChoosingRandomness;
    GLint posX(GLint col);
    GLint posY(GLint row);
public:
    SourceImage();
    SourceImage(const char *filename, GLint blockSize, GLint borderSize, GLint randomness);
    ~SourceImage();
    GLsizei blockSize, borderSize;
    // returns a completely random block index
    GLint getRandomBlock();
    GLint findMinimumErrorBlock(int sourceBlock1, int sourceBlock2, BlockMatch type, int *borderPathLeft, int *borderPathBottom, Image *targetImage, int drawX, int drawY);
    void getMinimumErrorPath(GLubyte *targetBorder, GLubyte *sourceBorder, GLint *path, BlockMatch type);
    void getMinimumErrorPathWithTargetImage(GLubyte *targetBorder, GLubyte *sourceBorder, GLubyte *targetImageBorder, GLint *path, BlockMatch type);
    int pixelError(int r, int c, int **errorMatrix, int **pathErrorMatrix, int **pathMatrix);
    int min2(int a, int b);
    int min3(int a, int b, int c);
    int pixelLuminance(int r, int g, int b);
    // draws the block at the given index at x,y on the screen
    void drawBlock(GLint index, GLint drawX, GLint drawY, int *borderPathLeft, int *borderPathBottom);
    void drawFullImage();
    GLint getWidth() { return image->width; }
    GLint getHeight() { return image->height; }
};

#endif /* SourceImage_hpp */
