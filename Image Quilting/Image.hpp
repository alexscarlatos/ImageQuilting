//
//  Image.hpp
//  Image Quilting
//
//  Created by Alex Scarlatos on 5/2/16.
//  Copyright © 2016 Alex Scarlatos. All rights reserved.
//

#ifndef Image_hpp
#define Image_hpp

#include <stdio.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class Image {
protected:
    GLubyte *imageData;
    void readPPM(const char *filename, GLubyte **pic);
    void readBMP(const char *filename);
public:
    Image();
    Image(const char *filename);
    ~Image();
    GLsizei width, height;
    void readPixels(int startX, int startY, int width, int height, GLubyte *targetArray);
    void drawFullImage();
};

#endif /* Image_hpp */
