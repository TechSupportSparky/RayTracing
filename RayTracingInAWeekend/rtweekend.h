#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <cstdlib>

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385f;

// Utility Functions
inline float degrees_to_radians(float degrees) {
	return degrees * pi / 180.0f;
}

/*
inline float random_float() {
	// Returns a random real in [0, 1).
	static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	static std::mt19937 generator;
	return distribution(generator);
}

inline float random_float(float min, float max) {
	// Returns a random real in [min, max).
	static std::uniform_real_distribution<float> distribution(min, max);
	static std::mt19937 generator;
	return distribution(generator);
}

inline int random_int(int min, int max) {
	static std::uniform_int_distribution<int> distribution(min, max);
	static std::mt19937 generator;
	return distribution(generator);
}
*/



inline float random_float() {
	// Returns a random real in [0,1).
	return rand() / (RAND_MAX + 1.0f);
}

inline float random_float(float min, float max) {
	// Returns a random real in [min,max).
	return min + (max - min) * random_float();
}

inline int random_int(int min, int max) {
	// Returns a random integer in [min,max].
	return static_cast<int>(random_float(min, max+1));
}


inline float clamp(float x, float min, float max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}


// Common headers
#include "ray.h"
#include "vec3.h"

#endif