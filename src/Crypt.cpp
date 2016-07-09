#include "Crypt.h"

#include <tclap/CmdLine.h>

int main(int argc, char *argv[]) {
	auto args = parseCMD(argc, argv);

	CImg<PTYPE> secret(args["secret0"].c_str());
	CImg<PTYPE> apparent(args["apparent"].c_str());

	if (args["resize"] == "true") {
		enum class Interpolation { NoneRawMem = -1, NoneBoundaryCondition, NearestNeighbour, MovingAverage, Linear, Grid, Cubic, Lanczos };
		secret.resize(apparent, (int)Interpolation::NearestNeighbour);
	}

	encrypt<PTYPE>(apparent, secret, args);

	if (args["signature"] == "true") {
		sign(apparent, std::stoi(args["secretbitdepth"]) / secret.spectrum());
	}

	apparent.save(args["output"].c_str());

	return 0;
}

//All argument parsing is done here and returned in an argMap<"arg", "value">.
//For multiarguments, the number of the args is passed in argMap["<name>num"] and they can be accessed via argMap[<name>0] ... argMap[<name>n]
std::map<string, string> parseCMD(int argc, char *argv[]) {
	try {
		TCLAP::CmdLine cmd("An Image Steganography tool.", ' ', "0.1");
		std::map<string, string> argMap;

		TCLAP::ValueArg<string> apparent("a", "apparent", "Apparent image to hide secret within.", true, "", "string", cmd);
		TCLAP::ValueArg<string> output("o", "output", "Output image.", false, "Hidden.png", "string", cmd);
		TCLAP::ValueArg<string> bitDepth("b", "bitdepth", "Resulting Color Bit Depth of output image.", false, "8", "integer", cmd);
		TCLAP::ValueArg<string> secretBitDepth("z", "secretbitdepth", "How many bits each secret is going to take up.", false, "8", "integer", cmd);
		TCLAP::MultiArg<string> secret("s", "secret", "Secret image(s) to hide into apparent.", true, "string", cmd);
		TCLAP::SwitchArg resize("r", "resize", "Resize all secrets to apparent?", cmd, false);
		TCLAP::SwitchArg signature("g", "signature", "Embed the rotational key inside the image?", cmd, false);

		cmd.parse(argc, argv);

		for (TCLAP::ArgListIterator it = cmd.getArgList().begin(); it != cmd.getArgList().end(); it++) {
			TCLAP::ValueArg<string>* valArg = dynamic_cast<TCLAP::ValueArg<string>*>(*it);
			TCLAP::SwitchArg* switchArg = dynamic_cast<TCLAP::SwitchArg*>(*it);
			TCLAP::MultiArg<string>* multiArg = dynamic_cast<TCLAP::MultiArg<string>*>(*it);

			if (valArg) {
				argMap[(*it)->getName()] = valArg->getValue();
			}
			else if (switchArg) {
				argMap[(*it)->getName()] = switchArg->getValue() == false ? "false" : "true";
			}
			else if (multiArg) {
				argMap[multiArg->getName() + "num"] = std::to_string(multiArg->getValue().size());

				for (int i = 0; i < multiArg->getValue().size(); i++) {
					argMap[multiArg->getName() + std::to_string(i)] = multiArg->getValue()[i];
				}
			}
		}

		return argMap;
	}
	catch (TCLAP::ArgException &e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
}

