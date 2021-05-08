#pragma once

#include "Objects.h"

#include <cstdint>
#include <functional>

using namespace std;

class Timer : public Object {
  public:
	int ms;
	unsigned accum = 0;
	bool bRepeat;
	bool bStop = false;
	function<void()> callback;
	Timer(int ms, function<void()> callback, bool bRepeat = false): ms{ms}, bRepeat{bRepeat}, callback{callback} {
		fnTickHandler = [this](uint64_t deltaTime) {
			if (bStop) return;
			accum += static_cast<unsigned>(deltaTime);
			if (accum > this->ms) {
				accum = 0;
				this->callback();
				if (!this->bRepeat) {
					bStop = true;
					this->remove();
				}
			}
		};
	}
};