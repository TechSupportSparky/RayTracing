#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>

void write_color(std::ostream& out, color pixel_color, int samples_per_pixel) {
	float r = pixel_color.x();
	float g = pixel_color.y();
	float b = pixel_color.z();

	// Divide the color by # of samples and gamme-correct for gamma=2.0
	float scale = 1.0f / samples_per_pixel;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	// Write the color value
	out << static_cast<int>(256 * clamp(r, 0.0f, 0.999f)) << " "
		<< static_cast<int>(256 * clamp(g, 0.0f, 0.999f)) << " "
		<< static_cast<int>(256 * clamp(b, 0.0f, 0.999f)) << "\n";
}

color calculate_color_with_gamma(color pixel_color, int samples_per_pixel) {
	float r = pixel_color.x();
	float g = pixel_color.y();
	float b = pixel_color.z();

	// Divide the color by # of samples and gamme-correct for gamma=2.0
	float scale = 2.0f / samples_per_pixel;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	return color(
		static_cast<int>(256 * clamp(r, 0.0f, 0.999f)),
		static_cast<int>(256 * clamp(g, 0.0f, 0.999f)),
		static_cast<int>(256 * clamp(b, 0.0f, 0.999f))
	);
}

#endif