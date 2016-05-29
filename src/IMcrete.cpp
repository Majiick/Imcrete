#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <tchar.h>
#include <intrin.h>
#include <bitset>
#include "CImg.h"
#include <tclap/CmdLine.h>
#include <map>
#include <vector>
#include <memory>

using namespace cimg_library;
using std::string;

template<typename T>
void encrypt(CImg<T> apparent, CImg<T> secret) {
	if (!apparent.containsXYZC(secret.width()-1, secret.height()-1, secret.depth()-1, secret.spectrum()-1)) { //Check that secret is smaller than apparent.
		throw std::invalid_argument("Secret is out of bounds of apparent.");
	}

	cimg_forXYZC(apparent, x, y, z, v) {
		//Clear last 4 bits of apparent and or them with the first 4 bits of secret.
		apparent.atXYZC(x, y, z, v) = (apparent.atXYZC(x, y, z, v) & 0xF0) | (secret.atXYZC(x, y, z, v) >> 4);
	}

	apparent.save("Hidden.png");
}

template<typename T>
void rotateDecrypt(CImg<T> img, int rotateKey) {
	img.ror(rotateKey);

	img.save("Unhidden.png");
}


enum class MatchQuality { Secret, Apparent };
enum class Interpolation { NoneRawMem = -1, NoneBoundaryCondition, NearestNeighbour, MovingAverage, Linear, Grid, Cubic, Lanczos };

//All argument parsing/accepting/whatever is done here and returned in an argMap<"arg", "value">.
//Maybe would be easier done using an unsafe cast on cmd.getArgList() or something.
std::map<string, string> parseCMD(int argc, char *argv[]) {
	try {
		TCLAP::CmdLine cmd("An Image Steganography tool.", ' ', "0.1");

		std::vector<std::unique_ptr<TCLAP::ValueArg<string>> > args{};
		args.push_back(std::make_unique<TCLAP::ValueArg<string>>("s", "secret", "Secret image to hide into apparent.", true, "", "string", cmd));
		args.push_back(std::make_unique<TCLAP::ValueArg<string>>("a", "apparent", "Apparent image to hide secret within.", true, "", "string", cmd));
		args.push_back(std::make_unique<TCLAP::ValueArg<string>>("o", "output", "Output image.", false, "Hidden.png", "string", cmd));
		args.push_back(std::make_unique<TCLAP::ValueArg<string>>("m", "matchdimensions", "Which input image should all other input images match the dimension of? No resizing if not specified.", false, "", "string", cmd));

		TCLAP::SwitchArg reverseSwitch("r", "resize", "Print name backwards", cmd, false);

		std::map<string, string> argMap;

		//2nd way
		for (TCLAP::ArgListIterator it = cmd.getArgList().begin(); it != cmd.getArgList().end(); it++) {
			std::cout << (*it)->getName() << std::endl;

			TCLAP::ValueArg<string>* valArg = dynamic_cast<TCLAP::ValueArg<string>*>(*it);
			TCLAP::SwitchArg* switchArg = dynamic_cast<TCLAP::SwitchArg*>(*it);
			
			if (valArg) {
				argMap[(*it)->getName()] = valArg->getValue();
			}
			else if (switchArg) {
				argMap[(*it)->getName()] = switchArg->getValue();
			}
		}

		cmd.parse(argc, argv);

		//First Way
		//std::for_each(args.begin(), args.end(), [&argMap](std::unique_ptr<TCLAP::ValueArg<string>>& arg) {argMap[arg->getName()] = arg->getValue(); });

		std::cout << argMap["secret"] << argMap["apparent"] << argMap["output"];
		getchar();

		return argMap;
	}
	catch (TCLAP::ArgException &e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
		throw; //Does this need a rethrow?
	}
}

int main(int argc, char *argv[]) {
	typedef uint16_t TTYPE;

	auto args = parseCMD(argc, argv);


	MatchQuality match = MatchQuality::Secret;

	CImg<TTYPE> secret("C:\\Users\\Ecoste\\Source\\Repos\\Imcrete\\cat.jpg");
	CImg<TTYPE> apparent("C:\\Users\\Ecoste\\Source\\Repos\\Imcrete\\nebula.jpg");


	if (!secret.is_sameXYZC(apparent)) {
		std::cout << "Image width, height, depth or spectrum doesn't match. Resizing." << std::endl;

		switch (match) {
		case MatchQuality::Secret:
			std::cout << "Resizing apparent to secret." << std::endl;

			apparent.resize(secret, (int)Interpolation::NearestNeighbour);
			break;

		case MatchQuality::Apparent:
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

	//std::cout << secret.size() << " " << apparent.size();

	encrypt<TTYPE>(secret, apparent);

	CImg<TTYPE> hidden("Hidden.png");
	rotateDecrypt<TTYPE>(hidden, 4);

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

