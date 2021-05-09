#pragma once

#include "errors-handler.h"

#include <functional>

namespace cgEngine {

using namespace std;

template<class T>
struct Option {
	T val{};
	bool bExistsVal = false;
	Option() = default;
	Option(const T& val): val{val}, bExistsVal{true} {}
	void clear() { bExistsVal = false; }
	T getVal() const {
		if (!bExistsVal) {
			showError("Option value didn't set");
		}
		return val;
	}
	void setVal(const T& val) { this->val = val; }
};

template<class T, class CompareLess = std::less<T>>
constexpr T clamp(T v, T lo, T hi, CompareLess comp = std::less<T>{})
{
	/*assert(!comp(hi, lo));*/
	return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

}