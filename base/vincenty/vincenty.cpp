#include "vincenty.h"
#include <math.h>
#include "qnumeric.h"

// a length of semi-major axis of the ellipsoid (radius at equator); (6378137.0 metres in WGS-84) 
// ƒ flattening of the ellipsoid; (1/298.257223563 in WGS-84) 
// b = (1 - ƒ) a length of semi-minor axis of the ellipsoid (radius at the poles); (6356752.314245 meters in WGS-84) 
// Φ1, Φ2 latitude of the points; 
// U1 = arctan[(1 − ƒ) tan Φ1],
// U2 = arctan[(1 − ƒ) tan Φ2] reduced latitude (latitude on the auxiliary sphere) 
// L = L2 - L1 difference in longitude of two points; 
// λ1, λ2 longitude of the points on the auxiliary sphere; 
// α1, α2 forward azimuths at the points; 
// α azimuth at the equator; 
// s ellipsoidal distance between the two points; 
// σ arc length between points on the auxiliary sphere 

#define a 6378137.0
#define f (1/298.257223563)
#define b 6356752.314245
#define M_PI       3.14159265358979323846

bool vincenty::calc_dist(double x1, double y1, double x2, double y2, double &dist, double &a1, double &a2)
{
	x1 = x1 / 180 * M_PI;
	x2 = x2 / 180 * M_PI;
	y1 = y1 / 180 * M_PI;
	y2 = y2 / 180 * M_PI;
	//L = difference in longitude
	double L = x2 - x1;
	double tanU1 = (1-f) * tan(y1), cosU1 = 1 / sqrt((1 + tanU1*tanU1)), sinU1 = tanU1 * cosU1;
	double tanU2 = (1-f) * tan(y2), cosU2 = 1 / sqrt((1 + tanU2*tanU2)), sinU2 = tanU2 * cosU2;

	double λ = L, λʹ;
	int iterationLimit = 100;
	double sinλ, cosλ;
	double sinSqσ;
	double sinσ;
	double cosσ;
	double σ;
	double sinα;
	double cosSqα;
	double cos2σM;
	double C;
	do {
		 sinλ = sin(λ), cosλ = cos(λ);
		 sinSqσ = (cosU2*sinλ) * (cosU2*sinλ) + (cosU1*sinU2-sinU1*cosU2*cosλ) * (cosU1*sinU2-sinU1*cosU2*cosλ);
		 sinσ = sqrt(sinSqσ);
		if (sinσ==0) return 0;  // co-incident points
		 cosσ = sinU1*sinU2 + cosU1*cosU2*cosλ;
		 σ = atan2(sinσ, cosσ);
		 sinα = cosU1 * cosU2 * sinλ / sinσ;
		 cosSqα = 1 - sinα*sinα;
		 cos2σM = cosσ - 2*sinU1*sinU2/cosSqα;
		if (qIsNaN(cos2σM)) cos2σM = 0;  // equatorial line: cosSqα=0 (§6)
		 C = f/16*cosSqα*(4+f*(4-3*cosSqα));
		λʹ = λ;
		λ = L + (1-C) * f * sinα * (σ + C*sinσ*(cos2σM+C*cosσ*(-1+2*cos2σM*cos2σM)));
	} while (abs(λ-λʹ) > 1e-12 && --iterationLimit>0);
	if (iterationLimit==0) return false; //throw new Error('Formula failed to converge');

 	double uSq = cosSqα * (a*a - b*b) / (b*b);
 	double A = 1 + uSq/16384*(4096+uSq*(-768+uSq*(320-175*uSq)));
 	double B = uSq/1024 * (256+uSq*(-128+uSq*(74-47*uSq)));
 	double Δσ = B*sinσ*(cos2σM+B/4*(cosσ*(-1+2*cos2σM*cos2σM)-
 		B/6*cos2σM*(-3+4*sinσ*sinσ)*(-3+4*cos2σM*cos2σM)));
 
 	dist = b*A*(σ-Δσ);
 
 	a1 = atan2(cosU2*sinλ,  cosU1*sinU2-sinU1*cosU2*cosλ);
 	a2 = atan2(cosU1*sinλ, -sinU1*cosU2+cosU1*sinU2*cosλ);
	a1 = a1 / M_PI * 180;
	a2 = a2 / M_PI * 180;
	return true;
}

void vincenty::calc_coor(double λ1, double φ1, double s, double angle, double &λ2, double &φ2, double &revAz)
{
	λ1 = λ1 / 180 * M_PI;
	φ1 = φ1 / 180 * M_PI;
	angle = angle /180 * M_PI;
	double sinα1 = sin(angle);
	double cosα1 = cos(angle);

	double tanU1 = (1-f) * tan(φ1), cosU1 = 1 / sqrt((1 + tanU1*tanU1)), sinU1 = tanU1 * cosU1;
	double σ1 = atan2(tanU1, cosα1);
	double sinα = cosU1 * sinα1;
	double cosSqα = 1 - sinα*sinα;
	double uSq = cosSqα * (a*a - b*b) / (b*b);
	double A = 1 + uSq/16384*(4096+uSq*(-768+uSq*(320-175*uSq)));
	double B = uSq/1024 * (256+uSq*(-128+uSq*(74-47*uSq)));

	double cos2σM;
	double sinσ;
	double cosσ;
	double Δσ;
	double σ = s / (b*A), σʹ;
	do {
		 cos2σM = cos(2*σ1 + σ);
		 sinσ = sin(σ);
		 cosσ = cos(σ);
		 Δσ = B*sinσ*(cos2σM+B/4*(cosσ*(-1+2*cos2σM*cos2σM)-
			B/6*cos2σM*(-3+4*sinσ*sinσ)*(-3+4*cos2σM*cos2σM)));
		σʹ = σ;
		σ = s / (b*A) + Δσ;
	} while (abs(σ-σʹ) > 1e-12);

	double tmp = sinU1*sinσ - cosU1*cosσ*cosα1;
	 φ2 = atan2(sinU1*cosσ + cosU1*sinσ*cosα1, (1-f)*sqrt(sinα*sinα + tmp*tmp));
	double λ = atan2(sinσ*sinα1, cosU1*cosσ - sinU1*sinσ*cosα1);
	double C = f/16*cosSqα*(4+f*(4-3*cosSqα));
	double L = λ - (1-C) * f * sinα *
		(σ + C*sinσ*(cos2σM+C*cosσ*(-1+2*cos2σM*cos2σM)));
	 //λ2 = (λ1+L+3*M_PI)%(2*M_PI) - M_PI;  // normalise to -180...+180
	λ2 = λ1+L;
	 revAz = atan2(sinα, -tmp);
	 
	 φ2 = φ2 / M_PI * 180;
	 λ2 = λ2 / M_PI * 180;
	 revAz = revAz / M_PI * 180;
}
