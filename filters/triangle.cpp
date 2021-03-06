
/*
    pbrt source code Copyright(c) 1998-2007 Matt Pharr and Greg Humphreys.

    This file is part of pbrt.

    pbrt is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.  Note that the text contents of
    the book "Physically Based Rendering" are *not* licensed under the
    GNU GPL.

    pbrt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

// triangle.cpp*
#include "sampling.h"
#include "paramset.h"
// Triangle Filter Declarations
class TriangleFilter : public Filter {
public:
	TriangleFilter(float xw, float yw) : Filter(xw, yw) { }
	float Evaluate(float x, float y) const;
};
// Triangle Filter Method Definitions
float TriangleFilter::Evaluate(float x, float y) const {
	return max(0.f, xWidth - fabsf(x)) *
		max(0.f, yWidth - fabsf(y));
}
extern "C" DLLEXPORT Filter *CreateFilter(const ParamSet &ps) {
	// Find common filter parameters
	float xw = ps.FindOneFloat("xwidth", 2.);
	float yw = ps.FindOneFloat("ywidth", 2.);
	return new TriangleFilter(xw, yw);
}
