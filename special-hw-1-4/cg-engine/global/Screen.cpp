#include "Screen.h"
#include "Game.h"
#include "../graphics/Shapes.h"

#include <vector>
#include <iostream>

#include <windows.h>

using namespace std;

namespace cgEngine {

Screen::Screen(uint16_t width, uint16_t height) : width{ width }, height{height} {
	displayMatrix.resize(static_cast<size_t>(width) * height);
	createBuffersCollection();
}

void Screen::set(int16_t x, int16_t y, bool bSet) {
	auto pos = y * width + x;
	if (abs(pos) >= width * height) return;
	displayMatrix[pos] = bSet;
}

void Screen::set(float fx, float fy, bool bSet) {
	auto x = static_cast<int16_t>(fx);
	auto y = static_cast<int16_t>(fy);
	displayMatrix[static_cast<size_t>(y) * width + x] = bSet;
}

bool Screen::get(uint16_t x, uint16_t y) {
	return displayMatrix[static_cast<size_t>(x) * width + y];
}
void Screen::draw() {
	if (width == 0 || displayMatrix.empty())
		return;

	auto hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hStdOut, COORD{0, 0});

	const char none = 'x';
	const char topbot = '\xDB';
	const char bot = '\xDC';
	const char top = '\xDF';

	for (int16_t i = 0,
		height = static_cast<int16_t>(displayMatrix.size()) / width;
		i < height;
		i += 2) {
		string twoLine(width, '\0');
		for (int16_t j = 0; j < width; ++j) {
			bool bMark = displayMatrix[static_cast<size_t>(i) * width + j];
			twoLine[j] = bMark ? top : none;
		}
		if (i + 1 < height) {
			for (int16_t j = 0; j < width; ++j)
			{
				int16_t pos = (i + 1) * width + j;
				bool bMark = displayMatrix[pos];
				if (bMark) {
					switch (twoLine[j]) {
					case top:
						twoLine[j] = topbot;
						break;
					case none:
						twoLine[j] = bot;
						break;
					}
				}
			}
		}
		cout << twoLine << endl;
	}
}

void Screen::clear()
{
	clearBuffersCollection();
	for(size_t i = 0; i < static_cast<size_t>(width) * height; ++i)
		displayMatrix[i] = false;
}

void Screen::createBuffersCollection() {
	buffers = make_shared<vector<shared_ptr<IScreen>>>();
	static vector<IScreen> buffers;
}

shared_ptr<IScreen> Screen::addGetBuffer() {
	auto ptr = static_pointer_cast<IScreen>(make_shared<Screen>(width, height));
	buffers->push_back(ptr);
	return ptr;
}

void Screen::clearBuffersCollection() {
	buffers->clear();
}

vector<shared_ptr<IScreen>>& Screen::getBuffersCollection()
{
	return *this->buffers;
}

void Screen::combineBuffers()
{
	auto& buffers = getBuffersCollection();
	//if (buffers.empty()) return;
	for (auto& buffer : buffers) {
		for(int16_t i = 0; i < width; ++i)
			for(int16_t j = 0; j < height; ++j)
				if (buffer->get(j, i))
					set(i, j, true);
	}
	clearBuffersCollection();
}

}