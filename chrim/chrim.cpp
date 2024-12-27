// chrim.cpp : Defines the entry point for the application.
//
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

#include "gfxFile.h"

#define debug 0

//NOTE: the first string in argv is whatever was used to call it in console. Eg, the path of the program relative to current dir, or just the name if it was called from $PATH.
//main validates the input before passing to the trim function
int main(int argc, char* argv[])
{
	std::vector<std::string> v;
#if !debug
	for (int i = 0; i < argc; i++) {
		v.push_back(argv[i]);
	}
#endif 
#if debug
	v.push_back("."); //this would be a path to the program
	v.push_back("largeCross.gfx");
	//v.push_back("2");
#endif
	int a = v.size();
	if (v.size() == 1 || v.size() > 3) {
		std::cout << "Wrong parameter count. Exiting." << std::endl;
		return 0;
	}
	int bppInput;
	if (v.size() == 2) {
		bppInput = 4;
	}
	else {
		try {
			bppInput = std::stoi(v[2]);
		}
		catch (const std::invalid_argument& ia) {
			std::cout << "Invalid BPP provided. Exiting." << "\n";
		}
		
	}
	std::filesystem::path relative = std::filesystem::current_path() / v[1];
	try {
		std::fstream file(relative.string(), std::ios_base::in | std::ios_base::binary);
		if (!file.is_open()) {
			std::cout << relative.string() + " not found." << std::endl;
			return 0;
		}
		file.close();
		//TrimGfx(file, relative);
		GfxFile gfx = GfxFile(relative,bppInput);
		gfx.Trim();
		gfx.GetPath();
	}
	catch(const std::exception& e){
		std::cout << "Exception " << e.what() << std::endl;
	}
	return 0;
}
