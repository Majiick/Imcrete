#include "CImg.h"
#include "Info.h"

#include <map>
#include <bitset>
#include <vector>

using namespace cimg_library;
using std::string;

std::map<string, string> parseCMD(int argc, char *argv[]);

template<typename T>
int getKey(CImg<T>& img) {
	std::bitset<keySpace> bitKey;
	for (int i = 0; i < keySpace; i++) {
		bitKey[i] = img.atXYZC(i, 0, 0, 0);
	}

	return static_cast<int>(bitKey.to_ulong());
}

template<typename T>
void rotateDecrypt(CImg<T>& img, int const rotateKey) {
	std::vector<int> bitDepths = {8, 24, 48};

	T secretMask = bitMax(rotateKey);

	cimg_forXYZC(img, x, y, z, v) {
		img.atXYZC(x, y, z, v) &= secretMask;
	}

	img.normalize(0, bitMax(*std::upper_bound(bitDepths.begin(), bitDepths.end(), rotateKey)));
}