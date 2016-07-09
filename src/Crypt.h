#include "CImg.h"
#include "Info.h"

#include <stdexcept>
#include <bitset>
#include <map>
#include <sstream>


using namespace cimg_library;
using std::string;

std::map<string, string> parseCMD(int argc, char *argv[]);

template<typename T>
void encrypt(CImg<T>& apparent, CImg<T>& secret, std::map<string, string>& const args) {
	if (!apparent.containsXYZC(secret.width() - 1, secret.height() - 1, secret.depth() - 1, secret.spectrum() - 1)) { //Check that secret fits inside apparent.
		throw std::invalid_argument("Secret is out of bounds of apparent.");
	}

	int bitDepth = std::stoi(args["bitdepth"]) / apparent.spectrum();
	int secretBitDepth = std::stoi(args["secretbitdepth"]) / secret.spectrum();

	apparent.normalize(0, bitMax(bitDepth));
	secret.normalize(0, bitMax(secretBitDepth));

	T secretMask = ~bitMax(secretBitDepth);

	cimg_forXYZC(apparent, x, y, z, v) {
		apparent.atXYZC(x, y, z, v) = (apparent.atXYZC(x, y, z, v) & secretMask) | secret.atXYZC(x, y, z, v); //Set apparent's *secretBitDepth* least significant bits to secret's value. 
	}
}

//Sets the R value of the first *keySpace* pixels of apparent to the binary value of the rotational key.
template<typename T>
void sign(CImg<T>& img, int const key) {
	if (key > bitMax(keySpace)) {
		std::stringstream ss;
		ss << "Key is too large to fit into " << keySpace << " bits.";

		throw std::invalid_argument(ss.str());
	}

	std::bitset<keySpace> bitKey(key);
	for (int i = 0; i < keySpace; i++) {
		img.atXYZC(i, 0, 0, 0) = bitKey[i];
	}
}