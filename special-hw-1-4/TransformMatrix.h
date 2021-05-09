#pragma once

#include "math-functions.h"

#include <array>

using namespace std;

struct TranslationMatrix : array<float, 3 * 3> {
	template<class T>
	static void fillMatrix(TranslationMatrix& matrix, T ar[3 * 3]) {
		for (size_t i = 0; i < sizeof(T[3 * 3]) / sizeof(ar[0]); ++i)
			matrix[i] = ar[i];
	}
	static TranslationMatrix getMoveMatrix(Coords coords) {
		float ar[] = {
		1, 0, coords.x,
		0, 1, coords.y,
		0, 0, 1
		};
		return TranslationMatrix{ ar };
	}
	static TranslationMatrix getPositionMatrix(Coords coords) {
		float ar[] = {
		1, 0, 0,
		0, 1, 0,
		coords.x, coords.y, 1
		};
		return TranslationMatrix{ ar };
	}
	static TranslationMatrix getUnitMatrix() {
		static float ar[] = {
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		};
		return TranslationMatrix{ ar };
	}
	TranslationMatrix() {
		*this = getUnitMatrix();
		//for (size_t i = 0; i < sizeof(ar) / sizeof(ar[0]); ++i)
		//	(*this)[i] = ar[i];
	}
	TranslationMatrix(float ar[3 * 3]) {
		fillMatrix(*this, ar);
	}
	TranslationMatrix(int ar[3 * 3]) {
		fillMatrix(*this, ar);
	}
	//TranslationMatrix(const array<T, 3*3>& ar) { //array<T, 3 * 3>
	////TranslationMatrix(initializer_list<T> init) {
	//TranslationMatrix(int ar[3 * 3]) {
	//	//for (int i = 0; i < 9; ++i) {
	//	//	(*this) = static_cast<float>(init[i]);
	//	//}
	//	fillMatrix(*this, ar);
	//}

	TranslationMatrix(float deltaX, float deltaY) {
		*this = getMoveMatrix({ deltaX, deltaY });
	}

	TranslationMatrix(Coords coords) {
		*this = getMoveMatrix(coords);
	}

	/*
		[a c e
		 b d f
		 g h 1]
	*/
	float a() const { return (*this)[0]; }
	float c() const { return (*this)[1]; }
	float e() const { return (*this)[2]; }
	void e(float x) { (*this)[2] = x; }
	float b() const { return (*this)[3]; }
	float d() const { return (*this)[4]; }
	float f() const { return (*this)[5]; }
	void f(float y) { (*this)[5] = y; }
	float g() const { return (*this)[6]; }
	float h() const { return (*this)[7]; }
	Coords getMovePosition() const {
		return { e(), f() };
	}
	void getMovePosition(float& x, float& y) const {
		x = e();
		y = f();
	}
	void setMovePosition(Coords coords) {
		e(coords.x);
		f(coords.y);
	}
	Coords getPosition() const {
		return { g(), h() };
	}
	void getPosition(float& x, float& y) const {
		x = g();
		y = h();
	}
	Coords apply(Coords pt) const {
		float globX = g() ? g() : pt.x;
		float globY = h() ? h() : pt.y;
		float x = a() * globX + c() * globY + e();
		float y = b() * globX + d() * globY + f();
		return { x, y };
	}
	//void apply(Coords& pt) const {
	//	float x, y;
	//	getPosition(x, y);
	//	pt.x += x;
	//	pt.y += y;
	//}
};