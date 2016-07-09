#include "Decrypt.h"
#include <tclap/CmdLine.h>

int main(int argc, char *argv[]) {
	auto args = parseCMD(argc, argv);
	CImg<PTYPE> apparent(args["apparent"].c_str());

	int key = 0;
	if (args["signature"] == "true") {
		key = getKey(apparent);
	}
	else {
		key = std::stoi(args["key"]);
	}

	bool soleImage;
	istringstream(args["soleimage"]) >> std::boolalpha >> soleImage;

	rotateDecrypt(apparent, key);
	apparent.save(args["output"].c_str());

	return 0;
}

//All argument parsing is done here and returned in an argMap<"arg", "value">.
std::map<string, string> parseCMD(int argc, char *argv[]) {
	try {
		TCLAP::CmdLine cmd("An Image Steganography tool.", ' ', "0.1");
		std::map<string, string> argMap;

		TCLAP::ValueArg<string> output("o", "output", "Output image.", false, "unhidden.png", "string", cmd);
		TCLAP::ValueArg<string> key("k", "key", "Rotational key.", false, "8", "integer", cmd);
		TCLAP::ValueArg<string> apparent("a", "apparent", "Input apparent encrypted image.", false, "8", "integer", cmd);
		TCLAP::SwitchArg signature("s", "signature", "Embed the rotational key inside the image?", cmd, false);

		cmd.parse(argc, argv);

		for (TCLAP::ArgListIterator it = cmd.getArgList().begin(); it != cmd.getArgList().end(); it++) {
			TCLAP::ValueArg<string>* valArg = dynamic_cast<TCLAP::ValueArg<string>*>(*it);
			TCLAP::SwitchArg* switchArg = dynamic_cast<TCLAP::SwitchArg*>(*it);

			if (valArg) {
				argMap[(*it)->getName()] = valArg->getValue();
			}
			else if (switchArg) {
				argMap[(*it)->getName()] = switchArg->getValue() == false ? "false" : "true";
			}
		}

		return argMap;
	}
	catch (TCLAP::ArgException &e) {
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
}