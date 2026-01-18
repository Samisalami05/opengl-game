#include "util.h"

float clampf(float v, float min, float max) {
	if (v < min) return min;
	if (v > max) return max;
	return v;
}

double clamp(double v, double min, double max) {
	if (v < min) return min;
	if (v > max) return max;
	return v;
}
