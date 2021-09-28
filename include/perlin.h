#ifndef PERLIN_H
#define PERLIN_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>

float perlin(double x, double y, double freq, int depth);

#endif 