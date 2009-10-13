#ifndef PBRT_MERLBXDFFIT_H
#define PBRT_MERLBXDFFIT_H
// MerlBxDF.h*
#include "pbrt.h"
#include "reflection.h"
#include "BRDF_Vol_Pub.h"

//
// Fitted Merl BxDF
//

class COREDLL MerlBxDFFit : public BxDF {
public:
	// MerlBxDFDebug Public Methods
	~MerlBxDFFit() {}
	//MerlBxDF(const double* table)
	//	: BxDF(BxDFType(BSDF_ALL_REFLECTION)),
	//	  MerlBRDF_table(table) {
	//}

	MerlBxDFFit(VsNurbVolume* volume)
		: BxDF(BxDFType(BSDF_ALL_REFLECTION)),
		  MerlBRDF_volume(volume)
	{
	}

	MerlBxDFFit(VsNurbVolume* table, BxDFType type)
		: BxDF(type), MerlBRDF_volume(table)
	{

	} 

	Spectrum f(const Vector &wo, const Vector &wi) const;
private:
	VsNurbVolume	*MerlBRDF_volume;

};
#endif // PBRT_MERLBXDFFIT_H