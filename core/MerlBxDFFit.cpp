#include "MerlBxDFFit.h"

Spectrum MerlBxDFFit::f(const Vector &wo, const Vector &wi) const
{

	double theta_in, phi_in, theta_out, phi_out;
	bool error = 1;

	const double pi2 = M_PI * 2.0;

	theta_in	= acos(wi[2]);

	phi_in		= atan2(-wi[1], wi[0]);

	theta_out	= acos(wo[2]);

	phi_out		= atan2(-wo[1], wo[0]);
	//phi_out		= atan2(-wo[1], wo[0]) - phi_in;

	double red, green, blue;

	error = GetBrdfValFmVol(
		MerlBRDF_volume, 
		theta_in,  phi_in, 
		theta_out, phi_out, 
		&red, &green, &blue
		);

	if ( red < 0 || green < 0 || blue < 0 ) red = green = blue = 0.0;

	//red =	(red	< 0. ? 0. : red);
	//green = (green	< 0. ? 0. : green);
	//blue =	(blue	< 0. ? 0. : blue);

	float color[3] = {red, green, blue};

	return Spectrum(color);
}
#if 0
#define M_PI_2 1.57079632679489661923
Spectrum MerlBxDFFit::f(const Vector &wo, const Vector &wi) const
{

	double theta_in, phi_in, theta_out, phi_out;

	const double pi2 = M_PI * 2.0;

	theta_in	= acos(wi[2]);
#ifdef _DEBUG
	assert( 0. <= theta_in && theta_in <= M_PI_2 );
#endif
	
	phi_in		= atan2(-wi[1], wi[0]);
	phi_in += M_PI;
#if 0
	if ( phi_in < 0 )
	{
		//phi_in = pi2 + phi_in;
		phi_in *= -1.0;
	}
#endif
	
	theta_out	= acos(wo[2]);
#ifdef _DEBUG
	assert( 0. <= theta_out && theta_out <= M_PI_2 );
#endif

	phi_out		= atan2(-wo[1], wo[0]);
	phi_out += M_PI;
#if 0
	if ( phi_out < 0 ) 
	{
#ifdef _DEBUG
#if 0
	printf ("Phi_o (before): %f\n", phi_out );
#endif
#endif
		//phi_out = pi2 + phi_out ;
		phi_out *= -1.0;
#ifdef _DEBUG
#if 0
	printf ("Phi_o (after ): %f\n", phi_out );
#endif
#endif
	}
#endif

#ifdef _DEBUG
	assert( 0. <= phi_out);
	assert( phi_out <= (2.0 * M_PI) );
#endif
	double red, green, blue;

	GetBrdfValFmVol(
		MerlBRDF_volume, 
		theta_in,  phi_in, 
		theta_out, phi_out, 
		&red, &green, &blue
		);

	red =	(red	< 0. ? 0. : red);
	green = (green	< 0. ? 0. : green);
	blue =	(blue	< 0. ? 0. : blue);

	float color[3] = {red, green, blue};

	return Spectrum(color);
}
#endif