all: bin/chopcrop bin/stitch bin/text

bin/chopcrop: chopcrop.cpp bin
	g++ `Magick++-config --cppflags --cxxflags --ldflags --libs` chopcrop.cpp -o bin/chopcrop

bin/stitch : stitch.cpp bin
	g++ `Magick++-config --cppflags --cxxflags --ldflags --libs` stitch.cpp -o bin/stitch

bin/text: text.cpp bin
	g++ `Magick++-config --cppflags --cxxflags --ldflags --libs` text.cpp -o bin/text

bin:
	mkdir bin
