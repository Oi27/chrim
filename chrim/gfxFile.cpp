#include "gfxFile.h"
using namespace std;
//this class opens the given file as-needed for methods.
GfxFile::GfxFile(std::filesystem::path Path, int bpp = 4) {
	PathAbs = Path;
	BPP = bpp;
	TileSize = (bpp / 8) * 0x40;
	TileBuffer = (char*)malloc(sizeof(char) * (TileSize + 1));
}

std::filesystem::path GfxFile::GetPath() {
	return PathAbs;
}

void GfxFile::Trim() {
	//given a filestream:
	//work thru in multiples of $40 bytes
	//when one chunk is all 00 00, chop the file to the beginning of that section
	//it would be smart to check surrounding rows to make sure they're also empty (large tile support check rows #$0040, #$0100, #$0140 bytes ahead )
	std::fstream gfxFile(PathAbs.string(), ios_base::in | ios_base::binary);
	if (!gfxFile.is_open()) {
		cout << PathAbs.string() + " not found." << endl;
		return;
	}
	int tilesCount = countTiles(gfxFile);
	for (int i = 0; i < tilesCount; i++) {
		bool emptyTile = TileIsEmpty(GetTile(i, gfxFile));
		if (emptyTile) {
			if (LargeTileEmpty(i, gfxFile)) {
				//trim the file to the beginning of tile [i]
				filesystem::resize_file(PathAbs, i * TileSize);
				string formattedCount = std::format("{:x}", i);
				transform(formattedCount.begin(), formattedCount.end(), formattedCount.begin(), ::toupper);
				cout << "Trimmed to $" << formattedCount << " tiles.";
				break;
			}
		}
	}
	gfxFile.close();
}

void GfxFile::Dispose() {
	free(TileBuffer);
}

//char is singular bytes & int is at least 16 bits
//char is indeed literally just an 8-bit int & it can be compared directly to numbers.
//needs error handling for when a tile or large tile crosses the end of the file.
//const int is better than #define because it enforces type checking

//fstream.read seems to advance the seekg pointer
//Also important that this char buffer does not need a terminator because it's not being treated as a string.

char* GfxFile::GetTile(int tileNum, fstream& gfxFile) {
	streampos startPoint = tileNum * TileSize;
	gfxFile.seekg(startPoint, fstream::beg);
	gfxFile.read(TileBuffer, (streamsize)TileSize);
	if ((gfxFile.rdstate() & std::ifstream::failbit) != 0) {
	}
	if ((gfxFile.rdstate() & std::ifstream::eofbit) != 0) {
		gfxFile.clear(); //clear the error flags for next read
		std::fill_n(TileBuffer, TileSize, 0);
		return TileBuffer;
	}
	if ((gfxFile.rdstate() & std::ifstream::badbit) != 0) {
	}
	return TileBuffer;
}

//an empty tile is all 00 00
bool GfxFile::TileIsEmpty(char tileBytes[]) {
	for (int i = 0; i < TileSize; i++)
	{
		if (tileBytes[i] != emptyPixel) {
			return false;
		}
	}
	return true;
}

bool GfxFile::LargeTileEmpty(int tileNum, fstream& gfxFile) {
	return
		TileIsEmpty(GetTile(tileNum + 0x00, gfxFile)) &&
		TileIsEmpty(GetTile(tileNum + 0x01, gfxFile)) &&
		TileIsEmpty(GetTile(tileNum + 0x10, gfxFile)) &&
		TileIsEmpty(GetTile(tileNum + 0x11, gfxFile));
}

//assumes that seekg is still where it was when the file was opened?? That seems bad. This could be avoided with class constructor.
//dividing by tilesize ensures the 8x8 ones will never overflow the file.
int GfxFile::countTiles(fstream& gfxfile) {
	std::streampos fileSize = gfxfile.tellg();
	gfxfile.seekg(0, ios::end);
	fileSize = gfxfile.tellg() - fileSize;
	return fileSize / TileSize;
}