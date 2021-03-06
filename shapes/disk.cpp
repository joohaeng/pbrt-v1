
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

// disk.cpp*
#include "shape.h"
// Disk Declarations
class Disk : public Shape {
public:
	// Disk Public Methods
	Disk(const Transform &o2w, bool ro, float height,
	     float radius, float innerRadius, float phiMax);
	BBox ObjectBound() const;
	bool Intersect(const Ray &ray, float *tHit,
	               DifferentialGeometry *dg) const;
	bool IntersectP(const Ray &ray) const;
	float Area() const;
	Point Sample(float u1, float u2, Normal *Ns) const {
		Point p;
		ConcentricSampleDisk(u1, u2, &p.x, &p.y);
		p.x *= radius;
		p.y *= radius;
		p.z = height;
		*Ns = Normalize(ObjectToWorld(Normal(0,0,1)));
		if (reverseOrientation) *Ns *= -1.f;
		return ObjectToWorld(p);
	}
private:
	// Disk Private Data
	float height, radius, innerRadius, phiMax;
};
// Disk Method Definitions
Disk::Disk(const Transform &o2w, bool ro, float ht,
           float r, float ri, float tmax)
	: Shape(o2w, ro) {
	height = ht;
	radius = r;
	innerRadius = ri;
	phiMax = Radians(Clamp(tmax, 0.0f, 360.0f));
}
BBox Disk::ObjectBound() const {
	return BBox(Point(-radius, -radius, height),
				Point(radius, radius, height));
}
bool Disk::Intersect(const Ray &r, float *tHit,
		DifferentialGeometry *dg) const {
	// Transform _Ray_ to object space
	Ray ray;
	WorldToObject(r, &ray);
	// Compute plane intersection for disk
	if (fabsf(ray.d.z) < 1e-7) return false;
	float thit = (height - ray.o.z) / ray.d.z;
	if (thit < ray.mint || thit > ray.maxt)
		return false;
	// See if hit point is inside disk radii and \phimax
	Point phit = ray(thit);
	float dist2 = phit.x * phit.x + phit.y * phit.y;
	if (dist2 > radius * radius ||
	    dist2 < innerRadius*innerRadius)
		return false;
	// Test disk $\phi$ value against \phimax
	float phi = atan2f(phit.y, phit.x);
	if (phi < 0) phi += 2. * M_PI;
	if (phi > phiMax)
		return false;
	// Find parametric representation of disk hit
	float u = phi / phiMax;
	float v = 1.f - ((sqrtf(dist2)-innerRadius) /
	                 (radius-innerRadius));
	Vector dpdu(-phiMax * phit.y, phiMax * phit.x, 0.);
	dpdu *= phiMax * INV_TWOPI;
	Vector dpdv(-phit.x / (1-v), -phit.y / (1-v), 0.);
	dpdv *= (radius - innerRadius) / radius;
	Vector dndu(0,0,0), dndv(0,0,0);
	// Initialize _DifferentialGeometry_ from parametric information
	*dg = DifferentialGeometry(ObjectToWorld(phit),
	                           ObjectToWorld(dpdu),
							   ObjectToWorld(dpdv),
	                           ObjectToWorld(dndu),
							   ObjectToWorld(dndv),
	                           u, v, this);
	// Update _tHit_ for quadric intersection
	*tHit = thit;
	return true;
}
bool Disk::IntersectP(const Ray &r) const {
	// Transform _Ray_ to object space
	Ray ray;
	WorldToObject(r, &ray);
	// Compute plane intersection for disk
	if (fabsf(ray.d.z) < 1e-7) return false;
	float thit = (height - ray.o.z) / ray.d.z;
	if (thit < ray.mint || thit > ray.maxt)
		return false;
	// See if hit point is inside disk radii and \phimax
	Point phit = ray(thit);
	float dist2 = phit.x * phit.x + phit.y * phit.y;
	if (dist2 > radius * radius ||
	    dist2 < innerRadius*innerRadius)
		return false;
	// Test disk $\phi$ value against \phimax
	float phi = atan2f(phit.y, phit.x);
	if (phi < 0) phi += 2. * M_PI;
	if (phi > phiMax)
		return false;
	return true;
}
float Disk::Area() const {
	return phiMax * 0.5f *
            (radius * radius -innerRadius * innerRadius );
}
extern "C" DLLEXPORT Shape *CreateShape(const Transform &o2w,
		bool reverseOrientation, const ParamSet &params) {
	float height = params.FindOneFloat( "height", 0. );
	float radius = params.FindOneFloat( "radius", 1 );
	float inner_radius = params.FindOneFloat( "innerradius", 0 );
	float phimax = params.FindOneFloat( "phimax", 360 );
	return new Disk(o2w, reverseOrientation, height, radius, inner_radius, phimax);
}
