﻿// chrim.cpp : Defines the entry point for the application.
//
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

#include "gfxFile.h"

#define debug 1
//void TrimGfx(fstream &gfxFile, std::filesystem::path gfxPath) {
//	//given a filestream:
//	//work thru in multiples of $40 bytes
//	//when one chunk is all 00 00, chop the file to the beginning of that section
//	//it would be smart to check surrounding rows to make sure they're also empty (large tile support check rows #$0040, #$0100, #$0140 bytes ahead )
//	int tilesCount = countTiles(gfxFile);
//	for (int i = 0; i < tilesCount; i++) {
//		bool emptyTile = TileIsEmpty(GetTile(i, gfxFile));
//		if (emptyTile) {
//			if (LargeTileEmpty(i, gfxFile)) {
//				//trim the file to the beginning of tile [i]
//				filesystem::resize_file(gfxPath, i * tileSize);
//				string formattedCount = std::format("{:x}", i);
//				transform(formattedCount.begin(), formattedCount.end(), formattedCount.begin(), ::toupper);
//				cout << "Trimmed to $" << formattedCount << " tiles.";
//				break;
//			}
//		}
//	}
//}

//NOTE: the first string in argv is whatever was used to call it in console. Eg, the path of the program relative to current dir, or just the name if it was called from $PATH.
//main validates the input before passing to the trim function
int main(int argc, char* argv[])
{
	std::vector<std::string> v;
	for (int i = 0; i < argc; i++) {
		v.push_back(argv[i]);
	}
#if debug
	v.push_back("largeCross.gfx");
#endif
	if (v.size() == 1 || v.size() > 2) {
		std::cout << "Wrong parameter count. Exiting." << std::endl;
		return 0;
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
		GfxFile gfx = GfxFile(relative);
		gfx.Trim();
		gfx.GetPath();
	}
	catch(const std::exception& e){
		std::cout << "Exception " << e.what() << std::endl;
	}
	return 0;
}
