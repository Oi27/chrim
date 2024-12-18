// chrim.cpp : Defines the entry point for the application.
//

#include "chrim.h"
#include <string.h>
#include <stdio.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define debug 1
#define emptyPixel 0
#define tileSize 0x40
//char is singular bytes & int is at least 16 bits
//char is indeed literally just an 8-bit int & it can be compared directly to numbers.
//needs error handling for when a tile or large tile crosses the end of the file.

//fstream.read seems to advance the seekg pointer
//Also important that this char buffer does not need a terminator because it's not being treated as a string.

char* GetTile(int tileNum, fstream &gfxFile) {
	streampos startPoint = tileNum * tileSize;
	static char tileBuffer[tileSize];
	gfxFile.seekg(startPoint, fstream::beg);
#if debug
	cout << gfxFile.tellg() << endl;
#endif
	gfxFile.read(tileBuffer, (streamsize)tileSize);
#if debug
	cout << gfxFile.tellg() << endl;
#endif
	if ((gfxFile.rdstate() & std::ifstream::failbit) != 0) {
#if debug
		cout << "fail" << endl;
#endif	
	}
	if ((gfxFile.rdstate() & std::ifstream::eofbit) != 0) {
#if debug
		cout << "EoF" << endl;
#endif
		gfxFile.clear(); //clear the error flags for next read
		std::fill_n(tileBuffer, tileSize, 0);
		return tileBuffer;
	}
	if ((gfxFile.rdstate() & std::ifstream::badbit) != 0) {
#if debug
		cout << "bad" << endl;
#endif
	}
	return tileBuffer;
}

//an empty tile is all 00 00
bool TileIsEmpty(char tileBytes[tileSize]) {
	for (int i = 0; i < tileSize; i++)
	{
		if (tileBytes[i] != emptyPixel) {
			return false;
		}
	}
	return true;
}

bool LargeTileEmpty(int tileNum, fstream& gfxFile) {
	return
		TileIsEmpty(GetTile(tileNum + 0x00, gfxFile)) &&
		TileIsEmpty(GetTile(tileNum + 0x01, gfxFile)) &&
		TileIsEmpty(GetTile(tileNum + 0x10, gfxFile)) &&
		TileIsEmpty(GetTile(tileNum + 0x11, gfxFile));
}

//assumes that seekg is still where it was when the file was opened?? That seems bad. This could be avoided with class constructor.
//dividing by tilesize ensures the 8x8 ones will never overflow the file.
int countTiles(fstream& gfxfile) {
	std::streampos fileSize = gfxfile.tellg();
	gfxfile.seekg(0, ios::end);
	fileSize = gfxfile.tellg() - fileSize;
	return fileSize / tileSize;
}

void TrimGfx(fstream &gfxFile, std::filesystem::path gfxPath) {
	//given a filestream:
	//work thru in multiples of $40 bytes
	//when one chunk is all 00 00, chop the file to the beginning of that section
	//it would be smart to check surrounding rows to make sure they're also empty (large tile support check rows #$0040, #$0100, #$0140 bytes ahead )
	int tilesCount = countTiles(gfxFile);
	for (int i = 0; i < tilesCount; i++) {
		bool emptyTile = TileIsEmpty(GetTile(i, gfxFile));
		if (emptyTile) {
			if (LargeTileEmpty(i, gfxFile)) {
				//trim the file to the beginning of tile [i]
				filesystem::resize_file(gfxPath, i * tileSize);
				string formattedCount = std::format("{:x}", i);
				transform(formattedCount.begin(), formattedCount.end(), formattedCount.begin(), ::toupper);
				cout << "Trimmed to $" << formattedCount << " tiles.";
				break;
			}
		}
	}
	gfxFile.close();
}

//NOTE: the first string in argv is whatever was used to call it in console. Eg, the path of the program relative to current dir, or just the name if it was called from $PATH.
//main validates the input before passing to the trim function
int main(int argc, char* argv[])
{
	vector <string> v;
	for (int i = 0; i < argc; i++) {
		v.push_back(argv[i]);
	}
#if debug
	v.push_back("largeCross.gfx");
#endif
	if (v.size() == 1 || v.size() > 2) {
		cout << "Wrong parameter count. Exiting." << endl;
		return 0;
	}
	std::filesystem::path relative = std::filesystem::current_path() / v[1];
	try {
		std::fstream file(relative.string(),ios_base::in | ios_base::binary);
		if (!file.is_open()) {
			cout << relative.string() + " not found." << endl;
			return 0;
		}
		TrimGfx(file, relative);
	}
	catch(const exception& e){
		cout << "Exception " << e.what() << endl;
	}
	return 0;
}
