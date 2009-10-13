#ifndef PBRT_PBRTBXDF_H
#define PBRT_PBRTBXDF_H
// PbrtBxDF.h*
#include "pbrt.h"
#include "reflection.h"

//
// Pbrt BxDF
//

#define PBRT_BXDF_REF_TYPE BSDF_ALL_REFLECTION
//#define PBRT_BXDF_REF_TYPE BSDF_REFLECTION | BSDF_SPECULAR
//#define PBRT_BXDF_REF_TYPE BSDF_REFLECTION | BSDF_DIFFUSE

class COREDLL PbrtBxDF : public BxDF {
public:
	// PbrtBxDFDebug Public Methods
	~PbrtBxDF() {}
	
	PbrtBxDF(double* table)
		: BxDF(BxDFType(PBRT_BXDF_REF_TYPE)),
		PbrtBRDF_table(table)
	{

	} 

	PbrtBxDF(double* table, BxDFType type)
		: BxDF(type), PbrtBRDF_table(table)
	{

	} 

	PbrtBxDF(double* table, const Spectrum &reflectance)
		: BxDF(BxDFType(PBRT_BXDF_REF_TYPE)),
		PbrtBRDF_table(table),
		R(reflectance),
		RoverPI(reflectance * INV_PI)
	{

	}

	Spectrum f(const Vector &wo, const Vector &wi) const;



private:
	double* PbrtBRDF_table;

	// Lambertian Private Data
	Spectrum R, RoverPI;


};
class COREDLL PbrtBxDFPdf : public BxDF {
public:
	// PbrtBxDFPdfDebug Public Methods
	~PbrtBxDFPdf() {}
	
	PbrtBxDFPdf(double* brdf, double* pdf, BxDFType type)
		: BxDF(type), PbrtBRDF_table(brdf), PbrtPDF_table(pdf)
	{

	} 

	Spectrum f(const Vector &wo, const Vector &wi) const;
	float Pdf(const Vector &wo, const Vector &wi) const;

private:

	double* PbrtBRDF_table;
	double* PbrtPDF_table;

	// Lambertian Private Data
	Spectrum R, RoverPI;


};
#endif // PBRT_PBRTBXDF_H