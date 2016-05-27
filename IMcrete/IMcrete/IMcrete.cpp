#include "stdafx.h"
#include "CImg.h"
#include <tclap/CmdLine.h>

using namespace cimg_library;

template<typename T>
void encrypt(CImg<T> apparent, CImg<T> secret) {
	int i = 0;
	/*
	cimg_forX(secret, x) {
	cimg_forY(secret, y) {
	cimg_forZ(secret, z) {
	unsigned char r;
	r = apparent.atXYZ(x, y, z) & 0b11110000; //Clear last 4 bits of apparent.jjksasa
	r = r | (secret.atXYZ(x, y, z) >> 4); //Set last 4 bits of apparent with the msb 4 bits of secret.


	if (x > 498) {
	std::bitset<8> b((secret.atXYZ(x, y, z)));
	std::cout << x << " " << y << " " << z << " " << b << std::endl;
	}


	secret.atXYZ(x, y, z) = r;

	i++;
	}
	}
	}
	*/

	cimg_forXYZC(apparent, x, y, z, v) {
		unsigned char r;
		r = apparent.atXYZC(x, y, z, v) & 0b11110000; //Clear last 4 bits of apparent.
		r = r | (secret.atXYZC(x, y, z, v) >> 4); //Set last 4 bits of apparent with the msb 4 bits of secret.


		if (x > 498) {
			std::bitset<8> b((secret.atXYZC(x, y, z, v)));
			//std::cout << "x: " << x << " y: " << y << " z: " << z << " v: "  << v << " -- " << b << std::endl;
		}


		secret.atXYZC(x, y, z, v) = r;

		i++;
	}
	std::cout << std::endl << "pixels = " << i;

	/*
	i = 0;
	cimg_for(apparent, ptr, T) {
	//a &= 0xF0| (b>>4);
	unsigned char r;
	//std::bitset<8> x(*ptr);
	//std::cout << x << std::endl;
	r = *ptr & 0b11110000; //Clear last 4 bits of apparent.

	try {
	//std::cout << i << std::endl;
	r = r | (secret[i] >> 4); //Set last 4 bits of apparent with the msb 4 bits of secret.
	}
	catch (int e) {
	//std::cout << e;
	}
	secret[i] = r;
	i++;
	}*/
	secret.save("Hidden.png");
}

template<typename T>
void decrypt(CImg<T> img) {
	/*
	int i = 0;
	cimg_for(img, ptr, T) {
	*ptr = _rotr8(*ptr, 4);
	}*/

	img.ror(4);

	img.save("Unhidden.png");
}

template<typename T>
int bit_depth(CImg<T> img) {
	printf(" size: %d,\n width: %d,\n height: %d,\n depth: %d,\n spectrum: %d,\n", img.size(), img.width(), img.height(), img.depth(), img.spectrum());
	return img.size() / (img.size() / img.spectrum()) * sizeof(T) * 8;
}

enum class Match { Secret, Apparent };
enum class Interpolation { NoneRawMem = -1, NoneBoundaryCondition, NearestNeighbour, MovingAverage, Linear, Grid, Cubic, Lanczos };

int main(int argc, char *argv[]) {
	/////////////////////////////
	try {
		TCLAP::CmdLine cmd("Command description message", ' ', "0.1");
		TCLAP::ValueArg<std::string> secretArg("s", "secret", "Secret image to encode.", true, "homer", "string", cmd); //Flag, Name, Desc, Req, T val?, const std::string& typeDesc

		cmd.parse(argc, argv);
	} catch (TCLAP::ArgException &e) {
		std::cout << "Exception thrown on argument parsing.";
	}

	/////////////////////////////
	Match match = Match::Secret;

	CImg<uint8_t> secret("C:\\Users\\Ecoste\\Source\\Repos\\Imcrete\\cat.jpg");
	CImg<uint8_t> apparent("C:\\Users\\Ecoste\\Source\\Repos\\Imcrete\\nebula.jpg");


	if (!secret.is_sameXYZC(apparent)) {
		std::cout << "Image width, height, depth or spectrum doesn't match. Resizing." << std::endl;

		switch (match) {
		case Match::Secret:
			std::cout << "Resizing apparent to secret." << std::endl;

			apparent.resize(secret, (int)Interpolation::NearestNeighbour);
			break;

		case Match::Apparent:
			std::cout << "Resizing secret to apparent." << std::endl;
			secret.resize(apparent, (int)Interpolation::NearestNeighbour);
			break;
		}
	}

	int i = 0;
	cimg_forXYZC(apparent, x, y, z, v) {
		//std::bitset<8> b((apparent.atXYZC(x, y, z, v)));
		//std::cout << "x: " << x << " y: " << y << " z: " << z << " v: " << v << " -- " << b << std::endl;
		i++;
	}

	std::cout << bit_depth<unsigned char>(apparent) << "pixels: " << i;

	//std::cout << secret.size() << " " << apparent.size();

	encrypt<uint8_t>(secret, apparent);

	CImg<uint8_t> hidden("Hidden.png");
	decrypt<uint8_t>(hidden);

	//CImg<double> test("Hidden.png");
	//test[200] = std::numeric_limits<double>::max();
	//test.normalize(0, 2 ^ 32);
	//test *= 100;
	//test.save("resavedTestHidden.png");

	//std::cout << bit_depth<unsigned char>(apparent);

	std::cout << "Finished";
	//apparent.display();
	//apparent.save("1_bitresaved.png");
	getchar();

	return 0;
}

