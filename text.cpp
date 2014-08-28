#include <Magick++.h>
#include <iostream>

using namespace Magick;

int main() {
    Image my_image( Geometry(320,220), Color("white"));

    // set the text rendering font
    // (the color is determined by the "current" image setting)
    my_image.font("-*-helvetica-medium-r-normal-*-*-120-*-*-*-*-iso8859-1");

    // draw text with different gravity position
    my_image.annotate("NorthWest gravity annotation", NorthWestGravity);
    my_image.annotate("SouthEast gravity annotation", SouthEastGravity);
    my_image.annotate("Center gravity annotation", CenterGravity);

    my_image.write("output/text.png");

    exit(0);
}
