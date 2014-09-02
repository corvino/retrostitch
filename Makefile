CC=clang++

all: directories bin/stitch bin/chopcrop bin/text

.PHONY: clean
.PHONY: directories

directories:
	mkdir -p bin
	mkdir -p output

clean:
	rm -rf bin
	rm -rf output

bin/stitch : stitch.cpp
	$(CC) `Magick++-config --cppflags --cxxflags --ldflags --libs` stitch.cpp -o bin/stitch

bin/chopcrop: chopcrop.cpp
	$(CC) `Magick++-config --cppflags --cxxflags --ldflags --libs` chopcrop.cpp -o bin/chopcrop

bin/text: text.cpp
	$(CC) `Magick++-config --cppflags --cxxflags --ldflags --libs` text.cpp -o bin/text

