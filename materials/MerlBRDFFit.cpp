/*
 * MERL BRDF shader for PBRT.
 *
 * Source code Copyright(c) 2007 Joo-Haeng Lee (joohaeng@etri.re.kr)
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 *
 * I do not provide MERL BRDF data files.
 * For the details including license of MERL BRDF, contact brdf@merl.com.
 * 
 */

// MerlBRDFFit.cpp*
#include "pbrt.h"
#include "material.h"
#include "BRDF_Vol_Pub.h"
#include "MerlBxDFFit.h"

using namespace std;

#define SIZE_BUF 256

// to prevent redundant loading
// Note: the following two lines was not be added inside the class
//
static map<string, VsNurbVolume	*> dataMap;
map<string, VsNurbVolume *>::iterator dataMap_iterator;

// MerlBRDFFit Class Declarations
class MerlBRDFFit : public Material {
public:
	// MerlBRDFFit Public Methods
	MerlBRDFFit(const string &type,
		const string &refl_type,
		Reference<Texture<Spectrum> > kd,
		Reference<Texture<float> > sig,
		Reference<Texture<float> > bump);

	BSDF *GetBSDF(const DifferentialGeometry &dgGeom,
	              const DifferentialGeometry &dgShading) const;



private:
	// MerlBRDFFit Private Data
	
	Reference<Texture<Spectrum> > Kd;
	Reference<Texture<float> > sigma, bumpMap;

	VsNurbVolume	*MerlBRDF_volume;
	BxDFType reflection_type;

	void TestBRDFLoad();
};

// MerlBRDFFit Method Definitions
MerlBRDFFit::MerlBRDFFit(const string &type,
						 const string &refl_type,
						 Reference<Texture<Spectrum> > kd,
						 Reference<Texture<float> > sig,
						 Reference<Texture<float> > bump) 
{
	Kd = kd;
	sigma = sig;
	bumpMap = bump;
	char MerlBRDF_filename[256];
	char *MerlBRDF_path = getenv("BRDF_SEARCHPATH");
	sprintf(MerlBRDF_filename,"%s\\%s.volume", MerlBRDF_path, type.data());

	MerlBRDF_volume=V_NULL;

	//
	// <task> read BRDF file
	//
	dataMap_iterator = dataMap.find(string(MerlBRDF_filename));
	if ( dataMap_iterator == dataMap.end() )
	{
		// no previous load. Hence, load it newly.
		if (LoadNurbsVolume_BIN(MerlBRDF_filename, &MerlBRDF_volume)) 
		{
			fprintf(stderr, "Error reading %s\n", MerlBRDF_filename);
			exit(1);
		}
		else
		{
			//fprintf(stderr, "BRDF file loaded: %s\n", MerlBRDF_filename);
			dataMap.insert(pair<string, VsNurbVolume	*>((string)MerlBRDF_filename, MerlBRDF_volume));
		}
	}
	else
	{
		//re-use the previous load.
		//fprintf(stderr, "Re-using the BRDF file loaded: %s\n", MerlBRDF_filename);
		MerlBRDF_volume = dataMap_iterator->second;
	}


	if (refl_type == "all")				reflection_type = BSDF_ALL_REFLECTION;
	else if (refl_type == "diffuse")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE);
	else if (refl_type == "glossy")		reflection_type = BxDFType(BSDF_REFLECTION | BSDF_GLOSSY);
	else if (refl_type == "specular")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);


#if 0 
	// This function tests the table load
	TestBRDFLoad();
#endif


}

BSDF *MerlBRDFFit::GetBSDF(const DifferentialGeometry &dgGeom,
		const DifferentialGeometry &dgShading) const 
{
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	BSDF *bsdf = BSDF_ALLOC(BSDF)(dgs, dgGeom.nn);
	
	bsdf->Add(BSDF_ALLOC(MerlBxDFFit)(MerlBRDF_volume, reflection_type));

	return bsdf;
}

void MerlBRDFFit::TestBRDFLoad()
{
	// Open test file
	FILE* fp = fopen("MerlBRDFFitTest.txt","w");
#ifdef _DEBUG
	if (!fp)
	{
		fprintf(stderr, "Error writing to \"Merl BRDF Fit Test.txt\"");
		exit(1);
	}
#endif

	// print out a 16x64x16x64 table table of BRDF values
	const int n = 16; // samples for theta
	const int m = 16; // samples for phi 
	double red,green,blue;
	double length;
	double theta_in, phi_in, theta_out, phi_out;
	for (int i = 0 ; i < n ; i++) 
	{
		theta_in = i * 0.5 * M_PI / n;
		for (int j = 1 ; j < 2 ; j++) // This loop is meaningless in isotropic BRDF
		{
			phi_in = j * 2.0 * M_PI / 4;
			for (int k = 0 ; k < n ; k++) 
			{
				theta_out = k * 0.5 * M_PI / n;
				for (int l = 0 ; l < m ; l++) 
				{
					phi_out = - M_PI + l * 2.0 * M_PI / m; // The interval has been changed from [0, 2Pi] to [-Pi, Pi]

					GetBrdfValFmVol(
						MerlBRDF_volume, 
						theta_in, phi_in, 
						theta_out, phi_out, 
						&red, &green, &blue
						);
					
					//fprintf(fp, "%3d %3d %3d %3d: %f %f %f\n", i, j, k, l, (float)red, (float)green, (float)blue);
					length = sqrt (red*red + green*green + blue*blue);
					fprintf(fp, "%f %f %f %f\n", theta_in, theta_out, phi_out, length);
				}
			}
	    }
	}

	fclose(fp);	
}


extern "C" DLLEXPORT Material * CreateMaterial(const Transform &xform,
		const TextureParams &mp) {

	Reference<Texture<Spectrum> > Kd = mp.GetSpectrumTexture("Kd", Spectrum(1.f));
	Reference<Texture<float> > sigma = mp.GetFloatTexture("sigma", 0.f);
	Reference<Texture<float> > bump = mp.GetFloatTexture("bumpmap", 0.f);
	
	string material_type = mp.FindString("type");
	if (material_type.size() == 0 ) material_type = "gold-metallic-paint";

	string reflection_type = mp.FindString("reflection");
	if ( reflection_type != "all" && reflection_type != "diffuse" && reflection_type != "glossy") 
	{
		printf(">> Reflection type has been changed to ""all"" from %s.\n", reflection_type.c_str());
		reflection_type = "all";
	}
	//printf(">> Reflection Type: %s\n", reflection_type.c_str());

	//
	// Sampling Method: (2008-11-29) 
	//	- CWH: Cosine-Weighted Hemisphere (Default)
	//	- TFP: Tabular Form Probability
	//
	string sampling_type = mp.FindString("sampling");
	if ( sampling_type != "cwh" && sampling_type != "tfp") 
	{
		printf(">> Sampling type has been changed to \"cwh\" from \"%s\".\n", sampling_type.c_str());
		sampling_type = "cwh";
	}

	return new MerlBRDFFit(material_type, reflection_type, Kd, sigma, bump);
}
