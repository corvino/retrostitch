#include <Magick++.h>
#include <iostream>
#include <dirent.h>
#include <stdlib.h>
#include <list>

using namespace MagickCore;

class Frame {
    std::string name;
    int x;
    int y;
public:
    Frame(char *s);
    std::string getName();
    int getX();
    int getY();
};

Frame::Frame(char *s) : name(s), x(-1), y(-1) {
    size_t divider = name.find("x");

    if (divider!=std::string::npos) {
        x = atoi(name.c_str());
        y = atoi(name.substr(divider + 1).c_str());
    }

    // std::cout << name << " ; x = " << x << " ; y = " << y << std::endl;
}

std::string Frame::getName() {
    return name;
}

int Frame::getX() {
    return x;
}

int Frame::getY() {
    return y;
}

int main(int argc, char *argv[]) {
    DIR *dp;
    struct dirent *dirp;
    Frame *frame;
    int i;
    int x;
    int y;
    std::list<Frame*> frames;
    std::list<Frame*>::iterator it;
    char charBuff[255];

    Magick::Geometry cropGeo(72, 79);
    Magick::Geometry chopGeo(320, 160);
    int maxX = -1;
    int maxY = -1;


    if (3 != argc) {
        std::cout << "usage: stitch source-directory target-file" << std::endl;
        exit(1);
    }

    if ((dp = opendir(argv[1])) == NULL) {
        std::cout << "can't open screens directory";
        exit(1);
    };

    std::cout << "Stitching '" << argv[1] << "' to '" << argv[2] << "'." << std::endl;

    while ((dirp = readdir(dp)) != NULL) {
        frame = new Frame(dirp->d_name);

        x = frame->getX();
        y = frame->getY();

        if (-1 < x && -1 < y) {
            frames.push_back(frame);

            if (maxX < x) {
                maxX = frame->getX();
            }

            if (maxY < y) {
                maxY = frame->getY();
            }
        } else {
            delete frame;
        }
    }

    closedir(dp);

    //int xPixels = (maxX + 1) * 321 - 2;
    //int yPixels = (maxY + 1) * 161 - 2;

    int xCoordinatePixels = 20;
    int yCoordinatePixels = 20;
    int xPixels = (maxX + 1) * 321 + 2 * yCoordinatePixels + 1;
    int yPixels = (maxY + 1) * 161 + 2 * xCoordinatePixels + 1;

    std::cout << frames.size() << " total frames." << std::endl;
    std::cout << maxX + 1 << " frame width; xPixels: " << xPixels << std::endl;
    std::cout << maxY + 1 << " frame height; yPixels: " << yPixels << std::endl;

    Magick::Image target(Magick::Geometry(xPixels, yPixels), Magick::Color(MaxRGB, MaxRGB, MaxRGB, 0));

    for(it = frames.begin(); it != frames.end(); it++) {
        Magick::Image frameImage(argv[1] + (*it)->getName());
        std::cout << (*it)->getX() << "," << (*it)->getY() << " (" << frameImage.columns() << "," << frameImage.rows() << "): " << "x=" << 321 * (*it)->getX() + yCoordinatePixels + 1 << "; y=" << 161 * (maxY - (*it)->getY()) + xCoordinatePixels + 1 << std::endl;

        // We can end up with different size images.  Set the appropriate cropGeo.

        if (464 == frameImage.columns() && 366 == frameImage.rows()) {
            cropGeo.width(72);
            cropGeo.height(79);
        } else if (424 == frameImage.columns() && 326 == frameImage.rows()) {
            cropGeo.width(52);
            cropGeo.height(64);
        }

        frameImage.chop(cropGeo);
        frameImage.crop(chopGeo);
        target.composite(frameImage, 321 * (*it)->getX() + yCoordinatePixels + 1, 161 * (maxY - (*it)->getY()) + xCoordinatePixels + 1);
    }

    target.strokeColor(Magick::Color("red"));
    target.fillColor(Magick::Color(0, 0, 0, MaxRGB));
    target.strokeWidth(1.0);

    // Draw the lines separating frames.

    for (i = 0; i <= maxX + 1; i++) {
        int linePixel = i * 321 + yCoordinatePixels;
        target.draw(Magick::DrawableLine(linePixel, 0, linePixel, yPixels));
    }

    for (i = 0; i <= maxY + 1; i++) {
        int linePixel = i * 161 + xCoordinatePixels;
        target.draw(Magick::DrawableLine(0, linePixel, xPixels, linePixel));
    }

    // Draw the coordinate boxes.

    for (i = 0; i <= maxX; i++) {
        Magick::Image coordinateFrame(Magick::Geometry(320, xCoordinatePixels), Magick::Color("gray"));

        sprintf(charBuff, "%d", i);

        coordinateFrame.font("-*-helvetica-medium-r-normal-*-*-120-*-*-*-*-iso8859-1");
        coordinateFrame.annotate(charBuff, Magick::CenterGravity);

        target.composite(coordinateFrame, i * 321 + yCoordinatePixels + 1, 0, Magick::OverCompositeOp);
        target.composite(coordinateFrame, i * 321 + yCoordinatePixels + 1, yPixels - xCoordinatePixels, Magick::OverCompositeOp);
    }

    for (i = 0; i <= maxY; i++) {
        Magick::Image coordinateFrame(Magick::Geometry(yCoordinatePixels, 160), Magick::Color("gray"));

        sprintf(charBuff, "%d", maxY - i);

        coordinateFrame.font("-*-helvetica-medium-r-normal-*-*-120-*-*-*-*-iso8859-1");
        coordinateFrame.annotate(charBuff, Magick::CenterGravity);

        target.composite(coordinateFrame, 0, i * 161 + xCoordinatePixels + 1, Magick::OverCompositeOp);
        target.composite(coordinateFrame, xPixels - yCoordinatePixels, i * 161 + xCoordinatePixels + 1, Magick::OverCompositeOp);
    }

    target.write(argv[2]);

    exit(0);
}
