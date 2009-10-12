
/*
 * MERL-CookTorrance Material Copyright(c) 2007 Joo-Haeng Lee
 *
 * All Rights Reserved.
 * NO WARRANTY, express or implied, for this software.
 *
 */

// MerlCookTorrance.cpp*
#include "pbrt.h"
#include "material.h"

#include "MerlNganFit.h"

float diffuse[3];
float specular[3];
float F0;
float m;

// MerlCookTorrance Class Declarations
class MerlCookTorrance : public Material {
public:
	MerlCookTorrance(
		const string &name,
		const string &refl_type,
		Reference<Texture<float> > bump);

	BSDF *GetBSDF(const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading) const;

private:
	Reference<Texture<float> > bumpMap;
	BxDFType reflection_type;
	string material_name;
	StrFMap p;

};
// MerlCookTorrance Method Definitions

MerlCookTorrance::MerlCookTorrance(
		const string &name,
		const string &refl_type,
		Reference<Texture<float> > bump) : bumpMap(bump) 
{ 
	material_name = name;

	if (refl_type == "all")				reflection_type = BSDF_ALL_REFLECTION;
	else if (refl_type == "diffuse")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE);
	else if (refl_type == "glossy")		reflection_type = BxDFType(BSDF_REFLECTION | BSDF_GLOSSY);
	else if (refl_type == "specular")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);

	buildMap_CookTorrance();

	p = param_CookTorrance(material_name);

	diffuse[0] = p["d_r"]; 
	diffuse[1] = p["d_g"]; 
	diffuse[2] = p["d_b"]; 
	
	specular[0] = p["s_r"]; 
	specular[1] = p["s_g"]; 
	specular[2] = p["s_b"]; 

	F0 = p["F0"];

	m = p["m"];

}

BSDF *MerlCookTorrance::GetBSDF(const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading) const {
	// Declare MerlCookTorrance coefficients

#if 0 // gold-metallic-paint
	static float diffuse[3] =	{ 0.0657, 0.0312,0.00247 }; // given value
	static float specular[3] =	{ 0.231, 0.135, 0.0387  };
	static float F0 = 0.663;	// F(0)
	static float m	= 0.241;	// rms
#endif

	static Spectrum ks[1]	= { Spectrum(specular) };

	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;
	BSDF *bsdf = BSDF_ALLOC(BSDF)(dgs, dgGeom.nn);

	MicrofacetDistribution **D = (MicrofacetDistribution **)BSDF::Alloc(1 * sizeof(MicrofacetDistribution *));
	D[0] = BSDF_ALLOC(Beckmann)(m);


	Fresnel **F = (Fresnel **)BSDF::Alloc(1 * sizeof(Fresnel *));
	F[0] = BSDF_ALLOC(FresnelSchlick)(F0);


	bsdf->Add(BSDF_ALLOC(CookTorrance)(Spectrum(diffuse), 1, ks, D, F, reflection_type));

	return bsdf;
}

extern "C" DLLEXPORT Material * CreateMaterial(const Transform &xform,
		const TextureParams &mp) {
	Reference<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap", 0.f);

	string material_name = mp.FindString("type");
	if (material_name.size() == 0 ) material_name = "gold-metallic-paint";

	string reflection_type = mp.FindString("reflection");
	if ( reflection_type != "all" && reflection_type != "diffuse" && reflection_type != "glossy") 
	{
		printf(">> Reflection type has been changed to ""all"" from %s.\n", reflection_type.c_str());
		reflection_type = "all";
	}

	return new MerlCookTorrance(material_name, reflection_type, bumpMap);
}