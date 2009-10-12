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

// MerlBRDF.cpp*
#include "pbrt.h"
#include "material.h"
#include "MerlBRDFRead.h"
#include "MerlBxDF.h"
#include "MerlTfs.h"

using namespace std;

#define SIZE_BUF 256

// to prevent redundant loading
// Note: the following two lines was not added inside the class
//
static map<string, double *> dataMap;
map<string, double *>::iterator dataMap_iterator;
static map<string, MerlTfs *> dataMapTfs;
map<string, MerlTfs *>::iterator dataMapTfs_iterator;

// MerlBRDF Class Declarations
class MerlBRDF : public Material {
public:
	// MerlBRDF Public Methods
	MerlBRDF(
			const string &material_type,
			const string &reflection_type,
			const string &sampling_type,
			Reference<Texture<Spectrum> > kd,
			Reference<Texture<float> > sig,
			Reference<Texture<float> > bump);

	~MerlBRDF();
	
	BSDF *GetBSDF(const DifferentialGeometry &dgGeom,
	              const DifferentialGeometry &dgShading) const;

private:
	// MerlBRDF Private Data
	
	Reference<Texture<Spectrum> > Kd;
	Reference<Texture<float> > sigma, bumpMap;

	double* MerlBRDF_table;
	BxDFType reflection_type;
	//string samplng_type;

	MerlTfs* tfs;
	void TestBRDFLoad();
	
};

MerlBRDF::~MerlBRDF()
{
	//delete MerlBRDF_table;
	//delete tfs;
}

// MerlBRDF Method Definitions
MerlBRDF::MerlBRDF(const string &type,
				   const string &refl_type,
				   const string &sampling_type,
				   Reference<Texture<Spectrum> > kd,
				   Reference<Texture<float> > sig,
				   Reference<Texture<float> > bump) 
{
	Kd = kd;
	sigma = sig;
	bumpMap = bump;
	char MerlBRDF_filename[SIZE_BUF];
	char *MerlBRDF_path = getenv("BRDF_PATH");
	sprintf(MerlBRDF_filename,"%s\\%s.binary", MerlBRDF_path, type.c_str());

	//
	// <task> read BRDF file
	//
	dataMap_iterator = dataMap.find(string(MerlBRDF_filename));
	if ( dataMap_iterator == dataMap.end() )
	{
		// no previous load. Load newly.
		if ( !MerlBRDF_load( MerlBRDF_filename, MerlBRDF_table) ) 
		{
			fprintf(stderr, "Error reading %s\n", MerlBRDF_filename);
			exit(1);
		}
		else
		{
			//fprintf(stderr, "BRDF file loaded: %s\n", MerlBRDF_filename);
			dataMap.insert(pair<string, double *>((string)MerlBRDF_filename, MerlBRDF_table));
		}
	}
	else
	{
		//re-use the previous load.
		//fprintf(stderr, "Re-using the BRDF file loaded: %s\n", MerlBRDF_filename);
		MerlBRDF_table = dataMap_iterator->second;
	}

	//
	// reflection type
	//
	{
		if (refl_type == "all")				reflection_type = BSDF_ALL_REFLECTION;
		else if (refl_type == "diffuse")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE);
		else if (refl_type == "glossy")		reflection_type = BxDFType(BSDF_REFLECTION | BSDF_GLOSSY);
		else if (refl_type == "specular")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);
	}

	
	//
	// sampling type
	//
	if (sampling_type == "tfs") {
	
		//fprintf(stderr, ">> MerlBRDF.dll: Sampling type is TFS.\n");

		char MerlTfs_filename[SIZE_BUF];
		sprintf(MerlTfs_filename,"%s\\Sampling\\%s.tfs", MerlBRDF_path, type.c_str());
		fprintf(stderr, ">> MerlBRDF.dll: TFS file name: %s\n", MerlTfs_filename);

		dataMapTfs_iterator = dataMapTfs.find(string(MerlTfs_filename));
		if ( dataMapTfs_iterator == dataMapTfs.end() )
		{
			// no previous load. Load newly.
			tfs = new MerlTfs(type.c_str());
			fprintf(stderr, ">> MerlBRDF.dll: Created a TFS class instance.\n");
			
			if ( !tfs->load() ) 
			{
				fprintf(stderr, "Error reading %s\n", MerlTfs_filename);
				exit(1);
			}
			else
			{
				fprintf(stderr, "Newsly loaded the tabular form sampling (TFS) file loaded: %s\n", MerlTfs_filename);
				dataMapTfs.insert(pair<string, MerlTfs *>((string)MerlTfs_filename, tfs));
			}
		}
		else
		{
			//re-use the previous load.
			fprintf(stderr, "Re-using the tabular form sampling (TFS) file loaded: %s\n", MerlTfs_filename);
			tfs = dataMapTfs_iterator->second;
		}
	} else {
		fprintf(stderr, ">> Sampling type is not TFS: %s\n", sampling_type.c_str());
		tfs = 0;
	}


#if 0
	// This function tests the table load
	TestBRDFLoad();
#endif

}

BSDF *MerlBRDF::GetBSDF(const DifferentialGeometry &dgGeom,
		const DifferentialGeometry &dgShading) const 
{
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	BSDF *bsdf = BSDF_ALLOC(BSDF)(dgs, dgGeom.nn);
	
#if 0
	Spectrum r = Kd->Evaluate(dgs).Clamp();
	bsdf->Add(BSDF_ALLOC(MerlBxDF)(MerlBRDF_table, r));
#else
	bsdf->Add(BSDF_ALLOC(MerlBxDF)(MerlBRDF_table, tfs, reflection_type));
#endif

	return bsdf;
}

void MerlBRDF::TestBRDFLoad()
{
	// Open test file
	FILE* fp = fopen("MerlBRDFTest.txt","w");
#ifdef _DEBUG
	if (!fp)
	{
		fprintf(stderr, "Error writing to \"MerlBRDFTest.txt\"");
		return;
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

					MerlBRDF_lookup(
						MerlBRDF_table, 
						theta_in, phi_in, 
						theta_out, phi_out, 
						red, green, blue);

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

	//
	// Material Name
	//
	string material_type = mp.FindString("type");
	if (material_type.size() == 0 ) material_type = "gold-metallic-paint";

	//
	// Reflection Type
	//
	string reflection_type = mp.FindString("reflection");
	if ( reflection_type != "all" && reflection_type != "diffuse" 
		&& reflection_type != "glossy"
		&& reflection_type != "specular") 
	{
		printf(">> Reflection type has been changed to ""all"" from %s.\n", reflection_type.c_str());
		reflection_type = "all";
	}
	//printf(">> Reflection Type: %s\n", reflection_type.c_str());
	
	//
	// Sampling Method: (2008-11-29) 
	//	- CWH: Cosine-Weighted Hemisphere (Default)
	//	- TFS: Tabular Form Sampling
	//
	string sampling_type = mp.FindString("sampling");
	if ( sampling_type != "cwh" && sampling_type != "tfs") 
	{
		printf(">> Sampling type has been changed to \"cwh\" from \"%s\".\n", sampling_type.c_str());
		sampling_type = "cwh";
	}

	return new MerlBRDF(material_type, reflection_type, sampling_type, Kd, sigma, bump);
}