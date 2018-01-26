// ReedAndSheppDll.cpp : définit les fonctions exportées pour l'application DLL.
//

#include "stdafx.h"
#include <stdio.h>
#include <math.h>

/*

The functions c_c_c through cs2_cb compute the lengths of the RS curves, for
a certain increment of configuration (x,y,phi) computed in the function
reed-shepp.

The function reed-shepp computes the increment between two configurations and
scans all RS curves to determine the shortest one. It is called through the
function min_length_rs to avoid a too small increment (which could yield
numerical errors).

The function constRS computes the discretized path (in pathx, pathy,
and patht) of the RS curve number NUM, parameters t, u and v, starting
at (x1,y1,t1).  It calls fct_curve that computes the path for a right
circular arc, left circular atc, and straight line. The discretization
done in fct-curve is rather ugly, and you could rewrite this function.



What is the significance of the parameters to reed_shepp() ?

From the brief description of the RS curves given in HO 9 you can
gather that RS curves consist of 3 to 5 segments (straight segments
and/or circular arcs). However, at most 3 have unpredetermined
lengths.  The parameters tr, ur, vr are intended to contain these
lengths.  For example, the first call in the function reed_shepp() is
to c_c_c(). The pointers to tn, un, and vn are passed to c_c_c(),
which computes the length of the 3 circular arcs of this RS curve. At
the end, reed_shepp puts in tr, ur, and vr the parameters
corresponding to the shortest RS curves found.  The unit of length for
a straight segment is whatever you want.  For a circular arc it is in
radians.

The parameter numero is just the number identifying a RS curve.  The
value given at the end of reed-shepp is the number (1 to 48)
identifying the shortes RS curve found between the two given
configurations.


*/

#define EPS1 1.0e-12
#define EPS2 1.0e-12
#define EPS3 1.0e-12
#define EPS4 1.0e-12
#define INFINITY 10000

#define MPI 3.1415926536
#define MPIMUL2 6.2831853072
#define MPIDIV2 1.5707963268


/*

RADCURV is the radius of the circular arcs in the RS curves (the
turning radius of the robot). It is in whatever units you want.

RADCURVMUL2 is defined as 2 * RADCURV
RADCURVMUL4 is defined as 4 * RADCURV
SQRADCURV   is defined as RADCURV * RADCURV
SQRADCURVMUL2 is defined as 4 * RADCURV * RADCURV

*/

double RADCURV = 1.0;
double RADCURVMUL2 = 2.0;
double RADCURVMUL4 = 4.0;
double SQRADCURV = 1.0;
double SQRADCURVMUL2 = 4.0;

extern "C" __declspec(dllexport) void change_radcurv(double radcurv)
{
	RADCURV = radcurv;
	RADCURVMUL2 = 2 * RADCURV;
	RADCURVMUL4 = 4 * RADCURV;
	SQRADCURV = RADCURV * RADCURV;
	SQRADCURVMUL2 = 4 * RADCURV * RADCURV;
}

/***********************************************************/
double mod2pi(double angle)
{
	while (angle < 0.0) angle = angle + MPIMUL2;
	while (angle >= MPIMUL2) angle = angle - MPIMUL2;
	return angle;
}


/***********************************************************/
double my_atan2(double y, double x)
{
	double a;
	if ((x == 0.0) && (y == 0.0)) return 0.0;
	if (x == 0.0)
		if (y > 0) return MPIDIV2;
		else return -MPIDIV2;
	a = atan(y / x);
	if (a > 0.0)
		if (x > 0) return a;
		else return (a + MPI);
	else
		if (x > 0) return (a + MPIMUL2);
		else return (a + MPI);
}


/***********************************************************/
double c_c_c(double x, double y, double phi, double rs, double rc, double* t, double *u, double* v)
{
	double a, b, u1, theta, alpha, length_rs;

	a = x - rs;
	b = y + rc;
	if ((fabs(a)<EPS3) && (fabs(b)<EPS3)) return(INFINITY);
	u1 = sqrt(a*a + b * b);
	if (u1>RADCURVMUL4) return(INFINITY);
	theta = my_atan2(b, a);
	alpha = acos(u1 / RADCURVMUL4);
	*t = mod2pi(MPIDIV2 + alpha + theta);
	*u = mod2pi(MPI - 2 * alpha);
	*v = mod2pi(phi - *t - *u);

	length_rs = RADCURV * (*t + *u + *v);
	return(length_rs);
}


/***********************************************************/
double c_cc(double x, double y, double phi, double rs, double rc, double* t, double *u, double* v)
{
	double a, b, u1, theta, alpha, length_rs;

	a = x - rs;
	b = y + rc;
	if ((fabs(a)<EPS3) && (fabs(b)<EPS3)) return(INFINITY);
	u1 = sqrt(a*a + b * b);
	if (u1>RADCURVMUL4) return(INFINITY);
	theta = my_atan2(b, a);
	alpha = acos(u1 / RADCURVMUL4);
	*t = mod2pi(MPIDIV2 + alpha + theta);
	*u = mod2pi(MPI - 2 * alpha);
	*v = mod2pi(*t + *u - phi);

	length_rs = RADCURV * (*t + *u + *v);
	return(length_rs);
}


/***********************************************************/
double csca(double x, double y, double phi, double rs, double rc, double* t, double *u, double* v)
{
	double a, b, length_rs;

	a = x - rs;
	b = y + rc;
	*t = mod2pi(my_atan2(b, a));
	*u = sqrt(a*a + b * b);
	*v = mod2pi(phi - *t);

	length_rs = RADCURV * (*t + *v) + *u;
	return(length_rs);
}


/***********************************************************/
double cscb(double x, double y, double phi, double rs, double rc, double* t, double *u, double* v)
{
	double a, b, u1, theta, alpha, length_rs;

	a = x + rs;
	b = y - rc;
	u1 = sqrt(a*a + b * b);
	if (u1 < RADCURVMUL2) return(INFINITY);
	theta = my_atan2(b, a);
	*u = sqrt(u1*u1 - SQRADCURVMUL2);
	alpha = my_atan2(RADCURVMUL2, *u);
	*t = mod2pi(theta + alpha);
	*v = mod2pi(*t - phi);

	length_rs = RADCURV * (*t + *v) + *u;
	return(length_rs);
}


/***********************************************************/
double ccu_cuc(double x, double y, double phi, double rs, double rc, double* t, double *u, double* v)
{
	double a, b, u1, theta, alpha, length_rs;

	a = x + rs;
	b = y - rc;
	if ((fabs(a)<EPS3) && (fabs(b)<EPS3)) return(INFINITY);
	u1 = sqrt(a*a + b * b);
	if (u1 > RADCURVMUL4) return(INFINITY);
	theta = my_atan2(b, a);
	if (u1>RADCURVMUL2)
	{
		alpha = acos((u1 / 2 - RADCURV) / RADCURVMUL2);
		*t = mod2pi(MPIDIV2 + theta - alpha);
		*u = mod2pi(MPI - alpha);
		*v = mod2pi(phi - *t + 2 * (*u));
	}
	else
	{
		alpha = acos((u1 / 2 + RADCURV) / (RADCURVMUL2));
		*t = mod2pi(MPIDIV2 + theta + alpha);
		*u = mod2pi(alpha);
		*v = mod2pi(phi - *t + 2 * (*u));
	}

	length_rs = RADCURV * (2 * (*u) + *t + *v);
	return(length_rs);
}


/***********************************************************/
double c_cucu_c(double x, double y, double phi, double rs, double rc, double* t, double *u, double* v)
{
	double a, b, u1, theta, alpha, length_rs, va1, va2;

	a = x + rs;
	b = y - rc;
	if ((fabs(a)<EPS3) && (fabs(b)<EPS3)) return(INFINITY);
	u1 = sqrt(a*a + b * b);
	if (u1 > 6 * RADCURV) return(INFINITY);
	theta = my_atan2(b, a);
	va1 = (5 * SQRADCURV - u1 * u1 / 4) / SQRADCURVMUL2;
	if ((va1 < 0.0) || (va1 > 1.0)) return(INFINITY);
	*u = acos(va1);
	va2 = sin(*u);
	alpha = asin(RADCURVMUL2*va2 / u1);
	*t = mod2pi(MPIDIV2 + theta + alpha);
	*v = mod2pi(*t - phi);

	length_rs = RADCURV * (2 * (*u) + *t + *v);
	return(length_rs);
}


/***********************************************************/
double c_c2sca(double x, double y, double phi, double rs, double rc, double* t, double *u, double* v)
{
	double a, b, u1, theta, alpha, length_rs;

	a = x - rs;
	b = y + rc;
	u1 = sqrt(a*a + b * b);
	if (u1 < RADCURVMUL2) return(INFINITY);
	theta = my_atan2(b, a);
	*u = sqrt(u1*u1 - SQRADCURVMUL2) - RADCURVMUL2;
	if (*u < 0.0) return(INFINITY);
	alpha = my_atan2(RADCURVMUL2, (*u + RADCURVMUL2));
	*t = mod2pi(MPIDIV2 + theta + alpha);
	*v = mod2pi(*t + MPIDIV2 - phi);

	length_rs = RADCURV * (*t + MPIDIV2 + *v) + *u;
	return(length_rs);
}


/***********************************************************/
double c_c2scb(double x, double y, double phi, double rs, double rc, double* t, double *u, double* v)
{
	double a, b, u1, theta, length_rs;

	a = x + rs;
	b = y - rc;
	u1 = sqrt(a*a + b * b);
	if (u1 < RADCURVMUL2) return(INFINITY);
	theta = my_atan2(b, a);
	*t = mod2pi(MPIDIV2 + theta);
	*u = u1 - RADCURVMUL2;
	*v = mod2pi(phi - *t - MPIDIV2);

	length_rs = RADCURV * (*t + MPIDIV2 + *v) + *u;
	return(length_rs);
}


/***********************************************************/
double c_c2sc2_c(double x, double y, double phi, double rs, double rc, double* t, double *u, double* v)
{
	double a, b, u1, theta, alpha, length_rs;

	a = x + rs;
	b = y - rc;
	u1 = sqrt(a*a + b * b);
	if (u1 < RADCURVMUL4) return(INFINITY);
	theta = my_atan2(b, a);
	*u = sqrt(u1*u1 - SQRADCURVMUL2) - RADCURVMUL4;
	if (*u < 0.0) return(INFINITY);
	alpha = my_atan2(RADCURVMUL2, (*u + RADCURVMUL4));
	*t = mod2pi(MPIDIV2 + theta + alpha);
	*v = mod2pi(*t - phi);

	length_rs = RADCURV * (*t + MPI + *v) + *u;
	return(length_rs);
}


/***********************************************************/
double cc_c(double x, double y, double phi, double rs, double rc, double* t, double *u, double* v)
{
	double a, b, u1, theta, alpha, length_rs, va;

	a = x - rs;
	b = y + rc;
	if ((fabs(a)<EPS3) && (fabs(b)<EPS3)) return(INFINITY);
	u1 = sqrt(a*a + b * b);
	if (u1>RADCURVMUL4) return(INFINITY);
	theta = my_atan2(b, a);
	*u = acos((8 * SQRADCURV - u1 * u1) / (8 * SQRADCURV));
	va = sin(*u);
	if (fabs(va)<0.001) va = 0.0;
	if ((fabs(va)<0.001) && (fabs(u1)<0.001)) return(INFINITY);
	alpha = asin(RADCURVMUL2*va / u1);
	*t = mod2pi(MPIDIV2 - alpha + theta);
	*v = mod2pi(*t - *u - phi);

	length_rs = RADCURV * (*t + *u + *v);
	return(length_rs);
}


/***********************************************************/
double csc2_ca(double x, double y, double phi, double rs, double rc, double* t, double *u, double* v)
{
	double a, b, u1, theta, alpha, length_rs;

	a = x - rs;
	b = y + rc;
	u1 = sqrt(a*a + b * b);
	if (u1 < RADCURVMUL2) return(INFINITY);
	theta = my_atan2(b, a);
	*u = sqrt(u1*u1 - SQRADCURVMUL2) - RADCURVMUL2;
	if (*u < 0.0) return(INFINITY);
	alpha = my_atan2((*u + RADCURVMUL2), RADCURVMUL2);
	*t = mod2pi(MPIDIV2 + theta - alpha);
	*v = mod2pi(*t - MPIDIV2 - phi);

	length_rs = RADCURV * (*t + MPIDIV2 + *v) + *u;
	return(length_rs);
}


/***********************************************************/
double csc2_cb(double x, double y, double phi, double rs, double rc, double* t, double *u, double* v)
{
	double a, b, u1, theta, length_rs;

	a = x + rs;
	b = y - rc;
	u1 = sqrt(a*a + b * b);
	if (u1 < RADCURVMUL2) return(INFINITY);
	theta = my_atan2(b, a);
	*t = mod2pi(theta);
	*u = u1 - RADCURVMUL2;
	*v = mod2pi(-*t - MPIDIV2 + phi);

	length_rs = RADCURV * (*t + MPIDIV2 + *v) + *u;
	return(length_rs);
}


/***********************************************************/
extern "C" __declspec(dllexport) double reed_shepp(double x1, double y1, double t1, double x2, double y2, double t2, int* numero, double* tr, double* ur, double* vr)
{
	double x, y, phi;
	double t, u, v, tn, un, vn;
	int num;
	double var, vard, theta, alpha, dx, dy, length;
	double sphi, cphi;
	double ap, am, b1, b2;

	/* coordinate change */
	dx = x2 - x1;
	dy = y2 - y1;
	theta = my_atan2(dy, dx);
	alpha = theta - t1;
	vard = sqrt(dx*dx + dy * dy);
	x = cos(alpha)*vard;
	y = sin(alpha)*vard;
	phi = t2 - t1;

	sphi = sin(phi);
	cphi = cos(phi);

	ap = RADCURV * sphi;
	am = -RADCURV * sphi;
	b1 = RADCURV * (cphi - 1);
	b2 = RADCURV * (cphi + 1);

	/*   C | C | C   */

	length = c_c_c(x, y, phi, ap, b1, &tn, &un, &vn);
	num = 1;
	t = tn; u = un; v = vn;

	var = c_c_c(-x, y, -phi, am, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 2;
		t = tn; u = un; v = vn;
	}

	var = c_c_c(x, -y, -phi, am, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 3;
		t = tn; u = un; v = vn;
	}

	var = c_c_c(-x, -y, phi, ap, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 4;
		t = tn; u = un; v = vn;
	}

	/*   C | C C   */

	var = c_cc(x, y, phi, ap, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 5;
		t = tn; u = un; v = vn;
	}

	var = c_cc(-x, y, -phi, am, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 6;
		t = tn; u = un; v = vn;
	}

	var = c_cc(x, -y, -phi, am, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 7;
		t = tn; u = un; v = vn;
	}

	var = c_cc(-x, -y, phi, ap, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 8;
		t = tn; u = un; v = vn;
	}

	/*   C S C   */

	var = csca(x, y, phi, ap, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 9;
		t = tn; u = un; v = vn;
	}

	var = csca(x, -y, -phi, am, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 10;
		t = tn; u = un; v = vn;
	}

	var = csca(-x, y, -phi, am, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 11;
		t = tn; u = un; v = vn;
	}

	var = csca(-x, -y, phi, ap, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 12;
		t = tn; u = un; v = vn;
	}

	var = cscb(x, y, phi, ap, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 13;
		t = tn; u = un; v = vn;
	}

	var = cscb(x, -y, -phi, am, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 14;
		t = tn; u = un; v = vn;
	}

	var = cscb(-x, y, -phi, am, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 15;
		t = tn; u = un; v = vn;
	}

	var = cscb(-x, -y, phi, ap, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 16;
		t = tn; u = un; v = vn;
	}

	/*   C Cu | Cu C   */

	var = ccu_cuc(x, y, phi, ap, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 17;
		t = tn; u = un; v = vn;
	}

	var = ccu_cuc(x, -y, -phi, am, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 18;
		t = tn; u = un; v = vn;
	}

	var = ccu_cuc(-x, y, -phi, am, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 19;
		t = tn; u = un; v = vn;
	}

	var = ccu_cuc(-x, -y, phi, ap, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 20;
		t = tn; u = un; v = vn;
	}

	/*   C | Cu Cu | C   */

	var = c_cucu_c(x, y, phi, ap, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 21;
		t = tn; u = un; v = vn;
	}

	var = c_cucu_c(x, -y, -phi, am, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 22;
		t = tn; u = un; v = vn;
	}

	var = c_cucu_c(-x, y, -phi, am, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 23;
		t = tn; u = un; v = vn;
	}

	var = c_cucu_c(-x, -y, phi, ap, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 24;
		t = tn; u = un; v = vn;
	}

	/*   C | C2 S C   */

	var = c_c2sca(x, y, phi, ap, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 25;
		t = tn; u = un; v = vn;
	}

	var = c_c2sca(x, -y, -phi, am, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 26;
		t = tn; u = un; v = vn;
	}

	var = c_c2sca(-x, y, -phi, am, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 27;
		t = tn; u = un; v = vn;
	}

	var = c_c2sca(-x, -y, phi, ap, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 28;
		t = tn; u = un; v = vn;
	}

	var = c_c2scb(x, y, phi, ap, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 29;
		t = tn; u = un; v = vn;
	}

	var = c_c2scb(x, -y, -phi, am, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 30;
		t = tn; u = un; v = vn;
	}

	var = c_c2scb(-x, y, -phi, am, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 31;
		t = tn; u = un; v = vn;
	}

	var = c_c2scb(-x, -y, phi, ap, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 32;
		t = tn; u = un; v = vn;
	}

	/*   C | C2 S C2 | C   */

	var = c_c2sc2_c(x, y, phi, ap, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 33;
		t = tn; u = un; v = vn;
	}

	var = c_c2sc2_c(x, -y, -phi, am, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 34;
		t = tn; u = un; v = vn;
	}

	var = c_c2sc2_c(-x, y, -phi, am, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 35;
		t = tn; u = un; v = vn;
	}

	var = c_c2sc2_c(-x, -y, phi, ap, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 36;
		t = tn; u = un; v = vn;
	}

	/*   C C | C   */

	var = cc_c(x, y, phi, ap, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 37;
		t = tn; u = un; v = vn;
	}

	var = cc_c(x, -y, -phi, am, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 38;
		t = tn; u = un; v = vn;
	}

	var = cc_c(-x, y, -phi, am, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 39;
		t = tn; u = un; v = vn;
	}

	var = cc_c(-x, -y, phi, ap, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 40;
		t = tn; u = un; v = vn;
	}

	/*   C S C2 | C   */

	var = csc2_ca(x, y, phi, ap, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 41;
		t = tn; u = un; v = vn;
	}

	var = csc2_ca(x, -y, -phi, am, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 42;
		t = tn; u = un; v = vn;
	}

	var = csc2_ca(-x, y, -phi, am, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 43;
		t = tn; u = un; v = vn;
	}

	var = csc2_ca(-x, -y, phi, ap, b1, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 44;
		t = tn; u = un; v = vn;
	}

	var = csc2_cb(x, y, phi, ap, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 45;
		t = tn; u = un; v = vn;
	}

	var = csc2_cb(x, -y, -phi, am, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 46;
		t = tn; u = un; v = vn;
	}

	var = csc2_cb(-x, y, -phi, am, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 47;
		t = tn; u = un; v = vn;
	}

	var = csc2_cb(-x, -y, phi, ap, b2, &tn, &un, &vn);
	if (var < length)
	{
		length = var;
		num = 48;
		t = tn; u = un; v = vn;
	}






	*tr = t; *ur = u; *vr = v;
	*numero = num;
	return(length);
}


/***********************************************************/
double min_length_rs(double x1, double y1, double t1, double x2, double y2, double t2, int* numero, double* t, double* u, double* v)
{
	double length_rs;

	if ((fabs(x1 - x2)<EPS1) && (fabs(y1 - y2)<EPS1)
		&& (fabs(t1 - t2)<EPS1))  length_rs = 0.0;
	else length_rs = reed_shepp(x1, y1, t1, x2, y2, t2, numero, t, u, v);

	return(length_rs);
}


/***********************************************************/
int fct_curve(int ty, int orientation, double val, double* x1, double* y1, double* t1, double delta, double* pathx, double* pathy, double* patht, int n)
{
	int i;
	double va1, va2, l, newval, incrt, remain;
	double center_x, center_y;
	double x2, y2, t2;
	int nnew;

	if (ty == 3)
		if (fabs(val / RADCURV)<EPS4) return(0);
	else
		if (fabs(val)<EPS4) return(0);

	switch (ty)
	{
	case 1: /* circular arc toward the right */
		center_x = *x1 + RADCURV * sin(*t1);
		center_y = *y1 - RADCURV * cos(*t1);
		va1 = *t1 + MPIDIV2;
		if (orientation == 1) va2 = va1 - val;
		else va2 = va1 + val;
		x2 = center_x + RADCURV * cos(va2);
		y2 = center_y + RADCURV * sin(va2);
		t2 = *t1 - orientation * val;

		nnew = val / delta;
		remain = val - nnew * delta;
		nnew = nnew + n;

		if (orientation == -1) delta = -delta;
		incrt = 0;
		for (i = n; i<nnew; i++)
		{
			va1 = va1 - delta;
			*(pathx + i) = center_x + RADCURV * cos(va1);
			*(pathy + i) = center_y + RADCURV * sin(va1);
			incrt = incrt - delta;
			*(patht + i) = mod2pi(*t1 + incrt);
		}
		n = nnew;
		if (remain > fabs(delta) / 5.)
		{
			*(pathx + nnew) = x2;
			*(pathy + nnew) = y2;
			*(patht + nnew) = mod2pi(t2);
			n++;
		}
		else
		{
			*(pathx + nnew - 1) = x2;
			*(pathy + nnew - 1) = y2;
			*(patht + nnew - 1) = mod2pi(t2);
		}
		break;

	case 2: /* circular arc toward the left */
		center_x = *x1 - RADCURV * sin(*t1);
		center_y = *y1 + RADCURV * cos(*t1);
		va1 = *t1 - MPIDIV2;
		if (orientation == 1) va2 = va1 + val;
		else va2 = va1 - val;
		x2 = center_x + RADCURV * cos(va2);
		y2 = center_y + RADCURV * sin(va2);
		t2 = *t1 + orientation * val;

		nnew = val / delta;
		remain = val - nnew * delta;
		nnew = nnew + n;

		if (orientation == -1) delta = -delta;
		incrt = 0;
		for (i = n; i<nnew; i++)
		{
			va1 = va1 + delta;
			*(pathx + i) = center_x + RADCURV * cos(va1);
			*(pathy + i) = center_y + RADCURV * sin(va1);
			incrt = incrt + delta;
			*(patht + i) = mod2pi(*t1 + incrt);
		}
		n = nnew;
		if (remain > fabs(delta) / 5.)
		{
			*(pathx + nnew) = x2;
			*(pathy + nnew) = y2;
			*(patht + nnew) = mod2pi(t2);
			n++;
		}
		else
		{
			*(pathx + nnew - 1) = x2;
			*(pathy + nnew - 1) = y2;
			*(patht + nnew - 1) = mod2pi(t2);
		}
		break;

	case 3: /* straight line */
		x2 = *x1 + orientation * val*cos(*t1);
		y2 = *y1 + orientation * val*sin(*t1);
		*t1 = mod2pi(*t1);
		t2 = *t1;

		va1 = sqrt((x2 - *x1)*(x2 - *x1) + (y2 - *y1)*(y2 - *y1));
		i = va1 / 1.2;
		remain = va1 - i * 1.2;
		nnew = n + i;
		newval = 1.2;
		va1 = orientation * cos(*t1);
		va2 = orientation * sin(*t1);
		for (i = n; i<nnew; i++)
		{
			*(pathx + i) = *x1 + va1 * newval;
			*(pathy + i) = *y1 + va2 * newval;
			*(patht + i) = *t1;
			newval = newval + 1.2;
		}
		if (remain > 0.4)
		{
			*(pathx + nnew) = x2;
			*(pathy + nnew) = y2;
			*(patht + nnew) = t2;
			n = nnew + 1;
		}
		else
		{
			*(pathx + nnew - 1) = x2;
			*(pathy + nnew - 1) = y2;
			*(patht + nnew - 1) = t2;
			n = nnew;
		}
	}

	*x1 = x2;
	*y1 = y2;
	*t1 = t2;

	return(n);
}


/***********************************************************/
extern "C" __declspec(dllexport) int constRS(int num, double t, double u, double v, double x1, double y1, double t1, double delta, double* pathx, double* pathy, double* patht)
{
	int left, right, straight, fwd, bwd;
	int n;

	*pathx = x1;
	*pathy = y1;
	*patht = t1;
	n = 1;

	right = 1; left = 2; straight = 3;
	fwd = 1; bwd = -1;

	switch (num)
	{

		/*   C | C | C   */

	case 1:
		n = fct_curve(left, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 2:
		n = fct_curve(left, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 3:
		n = fct_curve(right, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 4:
		n = fct_curve(right, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

		/*   C | C C   */

	case 5:
		n = fct_curve(left, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 6:
		n = fct_curve(left, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 7:
		n = fct_curve(right, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 8:
		n = fct_curve(right, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

		/*   C S C   */

	case 9:
		n = fct_curve(left, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 10:
		n = fct_curve(right, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 11:
		n = fct_curve(left, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 12:
		n = fct_curve(right, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 13:
		n = fct_curve(left, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 14:
		n = fct_curve(right, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 15:
		n = fct_curve(left, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 16:
		n = fct_curve(right, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

		/*   C Cu | Cu C   */

	case 17:
		n = fct_curve(left, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 18:
		n = fct_curve(right, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 19:
		n = fct_curve(left, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 20:
		n = fct_curve(right, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

		/*   C | Cu Cu | C   */

	case 21:
		n = fct_curve(left, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 22:
		n = fct_curve(right, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 23:
		n = fct_curve(left, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 24:
		n = fct_curve(right, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

		/*   C | C2 S C   */

	case 25:
		n = fct_curve(left, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, bwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 26:
		n = fct_curve(right, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, bwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 27:
		n = fct_curve(left, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, fwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 28:
		n = fct_curve(right, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, fwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 29:
		n = fct_curve(left, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, bwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 30:
		n = fct_curve(right, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, bwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 31:
		n = fct_curve(left, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, fwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 32:
		n = fct_curve(right, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, fwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

		/*   C | C2 S C2 | C   */

	case 33:
		n = fct_curve(left, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, bwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 34:
		n = fct_curve(right, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, bwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 35:
		n = fct_curve(left, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, fwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 36:
		n = fct_curve(right, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, fwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

		/*   C C | C   */

	case 37:
		n = fct_curve(left, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 38:
		n = fct_curve(right, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 39:
		n = fct_curve(left, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(right, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 40:
		n = fct_curve(right, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(left, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

		/*   C S C2 | C   */

	case 41:
		n = fct_curve(left, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 42:
		n = fct_curve(right, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 43:
		n = fct_curve(left, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 44:
		n = fct_curve(right, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 45:
		n = fct_curve(left, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 46:
		n = fct_curve(right, fwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, fwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 47:
		n = fct_curve(left, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, bwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;

	case 48:
		n = fct_curve(right, bwd, t, &x1, &y1, &t1, delta, pathx, pathy, patht, 1);
		n = fct_curve(straight, bwd, u, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(right, bwd, MPIDIV2, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		n = fct_curve(left, fwd, v, &x1, &y1, &t1, delta, pathx, pathy, patht, n);
		break;



	default:
		printf("Error: RS curve type %d unknown\n", num);
	}

	return n;
}



