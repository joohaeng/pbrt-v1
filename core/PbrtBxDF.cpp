#include "PbrtBxDF.h"
#include "PbrtBRDFRead.h"

 
Spectrum PbrtBxDF::f(const Vector &wo, const Vector &wi) const
{
 
	double theta_in, phi_in, theta_out, phi_out;

	const double pi2 = M_PI * 2.0;

	theta_in	= acos(wi[2]);

	phi_in		= atan2(-wi[1], wi[0]); 

	theta_out	= acos(wo[2]);

	phi_out		= atan2(-wo[1], wo[0]);

	double red, green, blue;

	PbrtBRDF_lookup(
		PbrtBRDF_table, 
		theta_in, phi_in, 
		theta_out, phi_out, 
		red, green, blue
		);

	//if ( red < 0 || green < 0 || blue < 0 ) red = green = blue = 0.0;
#if 1
	if ( red < 0 ) red = 0.0;
	if ( green < 0 ) green = 0.0;
	if ( blue < 0 ) blue = 0.0;
#endif

	float color[3] = {red, green, blue};

	return Spectrum(color);
}

Spectrum PbrtBxDFPdf::f(const Vector &wo, const Vector &wi) const
{
 
	double theta_in, phi_in, theta_out, phi_out;

	const double pi2 = M_PI * 2.0;

	theta_in	= acos(wi[2]);

	phi_in		= atan2(-wi[1], wi[0]); 

	theta_out	= acos(wo[2]);

	phi_out		= atan2(-wo[1], wo[0]);

	double red, green, blue;

	PbrtBRDF_lookup(
		PbrtBRDF_table, 
		theta_in, phi_in, 
		theta_out, phi_out, 
		red, green, blue
		);

	//if ( red < 0 || green < 0 || blue < 0 ) red = green = blue = 0.0;
#if 1
	if ( red < 0 ) red = 0.0;
	if ( green < 0 ) green = 0.0;
	if ( blue < 0 ) blue = 0.0;
#endif

	float color[3] = {red, green, blue};

	return Spectrum(color);
}

float PbrtBxDFPdf::Pdf(const Vector &wo, const Vector &wi) const
{
 
	if (!SameHemisphere(wo, wi)) return 0.f;

	double theta_in, phi_in, theta_out, phi_out;

	const double pi2 = M_PI * 2.0;

	theta_in	= acos(wi[2]);

	phi_in		= atan2(-wi[1], wi[0]); 

	theta_out	= acos(wo[2]);

	phi_out		= atan2(-wo[1], wo[0]);

	double pdf;

	PbrtPDF_lookup(
		PbrtBRDF_table, 
		theta_in, phi_in, 
		theta_out, phi_out, 
		pdf
		);

#if 0
#ifdef _DEBUG
	if ( isnan(pdf) ) Error("PbrtBxDFPdf::Pdf() returned NaN.");
#endif
#endif

	return pdf;
}