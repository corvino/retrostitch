#include <iostream>
#include <sstream>
#include <list>

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include <Magick++.h>

using namespace MagickCore;

static const int X_COORDINATE_PIXELS = 20;
static const int Y_COORDINATE_PIXELS = 20;

static const int FRAME_WIDTH = 321;
static const int FRAME_HEIGHT = 161;

struct Grid {
    int minX;
    int maxX;
    int minY;
    int maxY;

    Grid(int minX, int maxX, int minY, int maxY) : minX(minX), maxX(maxX), minY(minY), maxY(maxY) {};

    bool valid() { return INT_MAX != minX && INT_MIN != maxX && INT_MAX != minY && INT_MIN != maxY; };
};

/*
 * Scan directory for images with a "x" separator; find the minimum and
 * maximum x and y values based on files present in directory.
 */
Grid readGrid(char *screensDir) {
    DIR *dp;
    struct dirent *dirp;

    int minX = INT_MAX;
    int minY = INT_MAX;
    int maxX = INT_MIN;
    int maxY = INT_MIN;

    if ((dp = opendir(screensDir)) == NULL) {
        std::cout << "can't open screens directory";
        exit(-1);
    };

    while ((dirp = readdir(dp)) != NULL) {
        int x;
        int y;

        std::string name = dirp->d_name;
        size_t divider = name.find("x");
        size_t extension = name.find(".");

        if (divider != std::string::npos && extension != std::string::npos) {
            x = atoi(name.c_str());
            y = atoi(name.substr(divider + 1, extension - divider).c_str());

            minX = std::min(x, minX);
            minY = std::min(y, minY);
            maxX = std::max(x, maxX);
            maxY = std::max(y, maxY);
        }
    }

    closedir(dp);

    return Grid(minX, maxX, minY, maxY);
}

void drawTemplate(Grid grid, Magick::Image &target, int xPixels, int yPixels) {
    target.strokeColor(Magick::Color("red"));
    target.fillColor(Magick::Color(0, 0, 0, MaxRGB));
    target.strokeWidth(1.0);

    // Draw the lines separating frames.

    for (int i = 0; i <= grid.maxX - grid.minX + 2; i++) {
        int linePixel = i * FRAME_WIDTH + Y_COORDINATE_PIXELS;
        target.draw(Magick::DrawableLine(linePixel, 0, linePixel, yPixels));
    }

    for (int i = 0; i <= grid.maxY - grid.minY + 2; i++) {
        int linePixel = i * FRAME_HEIGHT + X_COORDINATE_PIXELS;
        target.draw(Magick::DrawableLine(0, linePixel, xPixels, linePixel));
    }

    // Draw the coordinate boxes.

    for (int i = grid.minX; i <= grid.maxX; i++) {
        Magick::Image coordinateFrame(Magick::Geometry(320, X_COORDINATE_PIXELS), Magick::Color("gray"));
        std::ostringstream s;
        s << i;

        coordinateFrame.font("-*-helvetica-medium-r-normal-*-*-120-*-*-*-*-iso8859-1");
        coordinateFrame.annotate(s.str(), Magick::CenterGravity);

        target.composite(coordinateFrame, (i - grid.minX) * FRAME_WIDTH + Y_COORDINATE_PIXELS + 1, 0, Magick::OverCompositeOp);
        target.composite(coordinateFrame, (i - grid.minX) * FRAME_WIDTH + Y_COORDINATE_PIXELS + 1, yPixels - X_COORDINATE_PIXELS, Magick::OverCompositeOp);
    }

    for (int i = grid.minY; i <= grid.maxY; i++) {
        Magick::Image coordinateFrame(Magick::Geometry(Y_COORDINATE_PIXELS, 160), Magick::Color("gray"));
        std::ostringstream s;
        s << i;

        std::cout << "y; i = : " << i << " s: " << s << "!!!!\n";
        coordinateFrame.font("-*-helvetica-medium-r-normal-*-*-120-*-*-*-*-iso8859-1");
        coordinateFrame.annotate(s.str(), Magick::CenterGravity);

        target.composite(coordinateFrame, 0, (grid.maxY - i) * FRAME_HEIGHT + X_COORDINATE_PIXELS + 1, Magick::OverCompositeOp);
        target.composite(coordinateFrame, xPixels - Y_COORDINATE_PIXELS, (grid.maxY - i) * FRAME_HEIGHT + X_COORDINATE_PIXELS + 1, Magick::OverCompositeOp);
    }
}

/*
 * Crop off border surrounding frame.
 */
void cropFrame(Magick::Image &frame) {
    Magick::Geometry cropGeo(72, 79);
    Magick::Geometry chopGeo(320, 160);

    // We can end up with different size images.  Set the appropriate
    // cropGeo.

    if (464 == frame.columns() && 366 == frame.rows()) {
        cropGeo.width(72);
        cropGeo.height(79);
    } else if (424 == frame.columns() && 326 == frame.rows()) {
        cropGeo.width(52);
        cropGeo.height(64);
    }

    frame.chop(cropGeo);
    frame.crop(chopGeo);
}

void drawFrame(Grid grid, Magick::Image &target, int x, int y, char *directory) {
    std::ostringstream s;
    s << directory << x << "x" << y << ".png";
    std::string frameName(s.str());

    if (access(frameName.c_str(), R_OK) != -1) {
        Magick::Image frameImage(frameName);

        std::cout << x << "," << y << " ("
                  << frameImage.columns() << "," << frameImage.rows() << "): "
                  << "x=" << FRAME_WIDTH * x + Y_COORDINATE_PIXELS + 1
                  << "; y=" << FRAME_HEIGHT * (grid.maxY - y) + X_COORDINATE_PIXELS + 1 << std::endl;

        cropFrame(frameImage);
        target.composite(frameImage, FRAME_WIDTH * (x - grid.minX) + Y_COORDINATE_PIXELS + 1, FRAME_HEIGHT * (grid.maxY - y) + X_COORDINATE_PIXELS + 1);
    } else {
        Magick::Color color("black");

        if (18 == x && -3 == y) {
            color = Magick::Color(0.33 * MaxRGB, 0.52 * MaxRGB, 0.98 * MaxRGB);
        } else if (y < 0) {
            color = Magick::Color(0.27 * MaxRGB, 0.27 * MaxRGB, 0.27 * MaxRGB);
        }

        Magick::Image frameImage(Magick::Geometry(FRAME_WIDTH, FRAME_HEIGHT), color);
        target.composite(frameImage, FRAME_WIDTH * (x - grid.minX) + Y_COORDINATE_PIXELS + 1, FRAME_HEIGHT * (grid.maxY - y) + X_COORDINATE_PIXELS + 1);
    }
}

int main(int argc, char *argv[]) {
    if (3 != argc) {
        std::cout << "usage: stitch source-directory target-file" << std::endl;
        exit(-1);
    }

    Grid grid = readGrid(argv[1]);
    std::cout << "Grid spans x: " << grid.minX << "->" << grid.maxX << " y: " << grid.minY << "->" << grid.maxY << std::endl;

    if (!grid.valid()) {
        std::cout << "File names did not determine a grid!" << std::endl;
        exit(-1);
    }

    int xPixels = (grid.maxX - grid.minX + 1) * FRAME_WIDTH + 2 * Y_COORDINATE_PIXELS + 1;
    int yPixels = (grid.maxY - grid.minY + 1) * FRAME_HEIGHT + 2 * X_COORDINATE_PIXELS + 1;

    std::cout << grid.maxX - grid.minX + 1 << " frame width; xPixels: " << xPixels << std::endl;
    std::cout << grid.maxY - grid.minY + 1 << " frame height; yPixels: " << yPixels << std::endl;

    Magick::Image target(Magick::Geometry(xPixels, yPixels), Magick::Color(MaxRGB, MaxRGB, MaxRGB, 0));

    for (int x = grid.minX; x <= grid.maxX; x++) {
        for (int y = grid.minY; y <= grid.maxY; y++) {
            drawFrame(grid, target, x, y, argv[1]);
        }
    }

    drawTemplate(grid, target, xPixels, yPixels);

    target.write(argv[2]);
    return(0);
}
