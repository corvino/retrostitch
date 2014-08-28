#!/usr/bin/python

import os
import sys

def main():
    if 4 != len(sys.argv):
        print "usage: renamfiles directory x-addition y-addition"
        exit(1)
    elif not (sys.argv[2].isdigit() or ("-" == sys.argv[2][0] and sys.argv[2][1:].isdigit())):
        print "x-coordinate adjustment must be an integer"
    elif not (sys.argv[3].isdigit() or ("-" == sys.argv[3][0] and sys.argv[3][1:].isdigit())):
        print "y-coordinate adjustmanet must be an integer"
    else:
        src = sys.argv[1]
        xCoordAdjustment = int(sys.argv[2])
        yCoordAdjustment = int(sys.argv[3])

        for file in os.listdir(src):
            xStart = file.find("x")
            extStart = file.find(".")

            if -1 != xStart and -1 != extStart:
                xCoordStr = file[0 : xStart]
                yCoordStr = file[xStart + 1 : extStart]
                extension = file[extStart:]

                if xCoordStr.isdigit() and yCoordStr.isdigit():
                    xCoord = int(xCoordStr)
                    yCoord = int(yCoordStr)
                    adjustedXCoord = xCoord + xCoordAdjustment
                    adjustedYCoord = yCoord + yCoordAdjustment
                    os.rename(src + file, src + str(adjustedXCoord) + "x" + str(adjustedYCoord) + extension)
                    #print xCoord, "x", yCoord, " => ", adjustedXCoord, "x", adjustedYCoord
                    #print src + str(adjustedXCoord) + "x" + str(adjustedYCoord) + extension

if __name__ == "__main__":
    main()
