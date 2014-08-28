all: chopcrop stitch text

chopcrop: chopcrop.cpp
	g++ `/opt/local/bin/Magick++-config --cppflags --cxxflags --ldflags --libs` chopcrop.cpp -o chopcrop

stitch : stitch.cpp
	g++ `/opt/local/bin/Magick++-config --cppflags --cxxflags --ldflags --libs` stitch.cpp -o stitch

text: text.cpp
	g++ `/opt/local/bin/Magick++-config --cppflags --cxxflags --ldflags --libs` text.cpp -o text
