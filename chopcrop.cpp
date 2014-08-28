#include <Magick++.h>

int main() {
    int cropX;
    int cropY;

    Magick::Image frame("test-images/0x1.png");

    // We see two different image sizes, based on whether the windows
    // was active or not when the screenshot was taken.  Easy enough
    // to accomodate both.
    //
    // This seems to follow a predictable formula on the horizonal:
    // cropX = (frame.columns() - 320) / 2;
    // However, the vertical does not follow the same pattern.

    if (464 == frame.columns() && 366 == frame.rows()) {
        cropX = 72;
        cropY = 79;
    } else if (424 == frame.columns() && 326 == frame.rows()) {
        cropX = 52;
        cropY = 64;
    } else {
        cropX = cropY = -1;
    }

    Magick::Geometry cropGeo(cropX, cropY);
    Magick::Geometry chopGeo(320, 160);

    frame.chop(cropGeo);
    frame.crop(chopGeo);
    frame.write("output/chopcrop.png");

    return 0;
}
