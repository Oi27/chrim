#pragma once
#include <filesystem>
#include <fstream>

class GfxFile {
private:
	std::filesystem::path PathAbs;
	int BPP;
	int TileSize;
	char* TileBuffer;

public:
	const char emptyPixel = 0;
	//const int tileSize = 0x20; //0.5byte/px * 64px in 8x8... may want optional cmd line params to change the bpp of the gfx.
	GfxFile(std::filesystem::path Path, int bpp = 4);
	~GfxFile();

	std::filesystem::path GetPath();

	void Trim();

	int countTiles(std::fstream& gfxfile);
	bool TileIsEmpty(char tileBytes[]);
	bool LargeTileEmpty(int tileNum, std::fstream& gfxFile);
	char* GetTile(int tileNum, std::fstream& gfxFile);
};
