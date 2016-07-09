#pragma once
#include <stdint.h>
#include <math.h>

const int keySpace = 6;
typedef uint16_t PTYPE;

unsigned int bitMax(int bits) {
	return (unsigned int)pow(2, bits) - 1;
}