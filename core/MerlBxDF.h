#ifndef PBRT_MERLBXDF_H
#define PBRT_MERLBXDF_H
// MerlBxDF.h*
#include "pbrt.h"
#include "reflection.h"

//
// Merl BxDF
//

#define MERL_BXDF_REF_TYPE BSDF_ALL_REFLECTION
//#define MERL_BXDF_REF_TYPE BSDF_REFLECTION | BSDF_SPECULAR
//#define MERL_BXDF_REF_TYPE BSDF_REFLECTION | BSDF_DIFFUSE

class COREDLL MerlBxDF : public BxDF {
public:
	// MerlBxDFDebug Public Methods
	~MerlBxDF() {}
	
	MerlBxDF(double* table)
		: BxDF(BxDFType(MERL_BXDF_REF_TYPE)),
		MerlBRDF_table(table)
	{

	} 

	MerlBxDF(double* table, BxDFType type)
		: BxDF(type), MerlBRDF_table(table)
	{

	} 

	MerlBxDF(double* table, const Spectrum &reflectance)
		: BxDF(BxDFType(MERL_BXDF_REF_TYPE)),
		MerlBRDF_table(table),
		R(reflectance),
		RoverPI(reflectance * INV_PI)
	{

	}

	Spectrum f(const Vector &wo, const Vector &wi) const;
	double* MerlBRDF_table;

private:
	// Lambertian Private Data
	Spectrum R, RoverPI;


};
#endif // PBRT_MERLBXDF_H