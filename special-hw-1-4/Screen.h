#pragma once

#include <cstdint>
#include <vector>
#include <memory>

using namespace std;

struct IScreen {
	virtual void set(int16_t x, int16_t y, bool bSet) = 0;
	virtual void set(float fx, float fy, bool bSet) = 0;
	virtual bool get(uint16_t x, uint16_t y) = 0;
	virtual void draw() = 0;
	virtual uint16_t getWidth() = 0;
	virtual uint16_t getHeight() = 0;

	virtual void clear() = 0;
	virtual void createBuffersCollection() = 0;
	virtual shared_ptr<IScreen> addGetBuffer() = 0;
	virtual vector<shared_ptr<IScreen>>& getBuffersCollection() = 0;
	virtual void clearBuffersCollection() = 0;
	virtual void combineBuffers() = 0;
};

struct Display : vector<bool> {

};

class Screen : public IScreen {
	Display displayMatrix;
	shared_ptr<vector<shared_ptr<IScreen>>> buffers;
public:
	uint16_t width;
	uint16_t height;
	Screen(uint16_t width, uint16_t height);
	void set(int16_t x, int16_t y, bool bSet) override;
	void set(float fx, float fy, bool bSet) override;
	bool get(uint16_t x, uint16_t y) override;
	void draw() override;
	uint16_t getWidth() override {return width;}
	uint16_t getHeight() override {return height;}
	void clear() override;
	void createBuffersCollection() override;
	shared_ptr<IScreen> addGetBuffer() override;
	void clearBuffersCollection() override;
	vector<shared_ptr<IScreen>>& getBuffersCollection() override;
	void combineBuffers() override;
};