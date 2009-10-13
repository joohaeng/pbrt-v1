#include "MerlBxDF.h"
#include "MerlBRDFRead.h"

 
Spectrum MerlBxDF::f(const Vector &wo, const Vector &wi) const
{
 
	double theta_in, phi_in, theta_out, phi_out;

	const double pi2 = M_PI * 2.0;

	theta_in	= acos(wi[2]);

	phi_in		= atan2(-wi[1], wi[0]); 

	theta_out	= acos(wo[2]);

	phi_out		= atan2(-wo[1], wo[0]);

	double red, green, blue;

	MerlBRDF_lookup(
		MerlBRDF_table, 
		theta_in, phi_in, 
		theta_out, phi_out, 
		red, green, blue
		);

	//if ( red < 0 || green < 0 || blue < 0 ) red = green = blue = 0.0;
	if ( red < 0 ) red = 0.0;
	if ( green < 0 ) green = 0.0;
	if ( blue < 0 ) blue = 0.0;

	float color[3] = {red, green, blue};

	return Spectrum(color);
}