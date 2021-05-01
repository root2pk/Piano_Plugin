/**
  ==============================================================================

	Hann.h
	Author:  Ruthu

	Class to create instances of a Hann object
	Set duration of the Hann window in samples, and peak amplitude using setDur() and setFamp()

	Use fullHann() or halfHann() to get a full hann window or half hann window returned as a float vector
  ==============================================================================
*/

#pragma once
#include<math.h>
#include<vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Hann {
public:

	/* Returns a full Hann window with duration dur(in samples) and peak amplitude famp*/
	std::vector<float> fullHann() {

		std::vector<float> f;
		for (int i = 0; i < dur; i++) {
			f.push_back(famp * 0.5 * (1 - cos(2 * M_PI * float(i) / float(dur))));
		}
		return f;
	}

	/* Returns a half Hann window with duration dur(in samples) and peak amplitude famp*/
	std::vector<float> halfHann() {

		std::vector<float> f;
		for (int i = 0; i < dur; i++) {
			f.push_back(famp * 0.5 * (1 - cos(M_PI * float(i) / float(dur))));
		}

		return f;
	}
	/* Set peak amplitude of force signal */
	void setFamp(float f) {
		famp = f;
	}
	float getFamp() {
		return famp;
	}

	/* Set duration of force signal in samples*/
	void setDur(int d) {
		dur = d;
	}
	int getDur() {
		return dur;
	}

private:
	float famp;                         //  Peak amplitude of input (N)
	int dur;                            // Duration of input in samples

};