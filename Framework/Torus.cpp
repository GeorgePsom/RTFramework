#include "stdafx.h"
#ifndef CBRT
#define     cbrt(x)  ((x) > 0.0 ? pow((double)(x), 1.0/3.0) : \
			  		 ((x) < 0.0 ? -pow((double)-(x), 1.0/3.0) : 0.0))
#endif
Torus::~Torus()
{
}

bool Torus::Intersect(Ray& ray)
{
	double x1 = XMVectorGetX(ray.origin); double y1 = XMVectorGetY(ray.origin); double z1 = XMVectorGetZ(ray.origin);
	double d1 = XMVectorGetX(ray.direction); double d2 = XMVectorGetY(ray.direction); double d3 = XMVectorGetZ(ray.direction);

	double coeffs[5];	
	double roots[4];

	// define the coefficients of the quartic equation

	double sum_d_sqrd = d1 * d1 + d2 * d2 + d3 * d3;
	double e = x1 * x1 + y1 * y1 + z1 * z1 - a * a - b * b;
	double f = x1 * d1 + y1 * d2 + z1 * d3;
	double four_a_sqrd = 4.0 * a * a;

	coeffs[0] = e * e - four_a_sqrd * (b * b - y1 * y1); 	// constant term
	coeffs[1] = 4.0 * f * e + 2.0 * four_a_sqrd * y1 * d2;
	coeffs[2] = 2.0 * sum_d_sqrd * e + 4.0 * f * f + four_a_sqrd * d2 * d2;
	coeffs[3] = 4.0 * sum_d_sqrd * f;
	coeffs[4] = sum_d_sqrd * sum_d_sqrd;  					// coefficient of t^4

	// find roots of the quartic equation

	int num_real_roots = SolveQuartic(coeffs, roots);

	bool	intersected = false;
	double 	t = 10000000.0;

	if (num_real_roots == 0)  // ray misses the torus
		return(false);

	// find the smallest root greater than kEpsilon, if any
	// the roots array is not sorted

	for (int j = 0; j < num_real_roots; j++)
		if (roots[j] > ray.tMin) {
			intersected = true;
			if (roots[j] < t)
				t = roots[j];
		}

	if (!intersected)
		return (false);
	ray.t = t;
	return true;

	

}

void Torus::GetSurfaceData(Surface& surf, Ray& ray) const
{
	surf.position = ray.origin + ray.t * ray.direction;
	double param_squared = a * a + b * b;

	double x = XMVectorGetX(surf.position);
	double y = XMVectorGetY(surf.position);
	double z = XMVectorGetZ(surf.position);
	double sum_squared = x * x + y * y + z * z;

	surf.normal = XMVectorSet(4.0 * x * (sum_squared - param_squared), 4.0 * y * (sum_squared - param_squared + 2.0 * a * a), 4.0 * z * (sum_squared - param_squared), 0.0f);
	surf.normal = XMVector3Normalize(surf.normal);
	XMVECTOR tangent = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	
	float theta = XMVectorGetX(XMVector3Dot(surf.normal, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)));
	
	float phi = XMVectorGetX(XMVector3Dot(surf.normal, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	theta = acosf(theta) * 180.0f / XM_PI;
	phi = acosf(phi) * 180.0f / XM_PI;
	surf.tex = XMVectorSet(theta / 360.0f, phi / 360.0f, 0.0f, 0.0f );
}

int Torus::SolveQuartic(double c[5], double s[4])
{

	double  coeffs[4];
	double  z, u, v, sub;
	double  A, B, C, D;
	double  sq_A, p, q, r;
	int     i, num;

	/* normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0 */

	A = c[3] / c[4];
	B = c[2] / c[4];
	C = c[1] / c[4];
	D = c[0] / c[4];

	/*  substitute x = y - A/4 to eliminate cubic term:
	x^4 + px^2 + qx + r = 0 */

	sq_A = A * A;
	p = -3.0 / 8 * sq_A + B;
	q = 1.0 / 8 * sq_A * A - 1.0 / 2 * A * B + C;
	r = -3.0 / 256 * sq_A * sq_A + 1.0 / 16 * sq_A * B - 1.0 / 4 * A * C + D;

	if (isZero(r)) {
		/* no absolute term: y(y^3 + py + q) = 0 */

		coeffs[0] = q;
		coeffs[1] = p;
		coeffs[2] = 0;
		coeffs[3] = 1;

		num = SolveCubic(coeffs, s);

		s[num++] = 0;
	}
	else {
		/* solve the resolvent cubic ... */

		coeffs[0] = 1.0 / 2 * r * p - 1.0 / 8 * q * q;
		coeffs[1] = -r;
		coeffs[2] = -1.0 / 2 * p;
		coeffs[3] = 1;

		(void)SolveCubic(coeffs, s);

		/* ... and take the one real solution ... */

		z = s[0];

		/* ... to build two quadric equations */

		u = z * z - r;
		v = 2 * z - p;

		if (isZero(u))
			u = 0;
		else if (u > 0)
			u = sqrt(u);
		else
			return 0;

		if (isZero(v))
			v = 0;
		else if (v > 0)
			v = sqrt(v);
		else
			return 0;

		coeffs[0] = z - u;
		coeffs[1] = q < 0 ? -v : v;
		coeffs[2] = 1;

		num = SolveQuadric(coeffs, s);

		coeffs[0] = z + u;
		coeffs[1] = q < 0 ? v : -v;
		coeffs[2] = 1;

		num += SolveQuadric(coeffs, s + num);
	}

	/* resubstitute */

	sub = 1.0 / 4 * A;

	for (i = 0; i < num; ++i)
		s[i] -= sub;

	return num;
}


int Torus::SolveQuadric(double c[3], double s[2]) {
	double p, q, D;

	/* normal form: x^2 + px + q = 0 */

	p = c[1] / (2 * c[2]);
	q = c[0] / c[2];

	D = p * p - q;

	if (isZero(D)) {
		s[0] = -p;
		return 1;
	}
	else if (D > 0) {
		double sqrt_D = sqrt(D);

		s[0] = sqrt_D - p;
		s[1] = -sqrt_D - p;
		return 2;
	}
	else /* if (D < 0) */
		return 0;
}


int Torus::SolveCubic(double c[4], double s[3]) {
	int     i, num;
	double  sub;
	double  A, B, C;
	double  sq_A, p, q;
	double  cb_p, D;

	/* normal form: x^3 + Ax^2 + Bx + C = 0 */

	A = c[2] / c[3];
	B = c[1] / c[3];
	C = c[0] / c[3];

	/*  substitute x = y - A/3 to eliminate quadric term:
	x^3 +px + q = 0 */

	sq_A = A * A;
	p = 1.0 / 3 * (-1.0 / 3 * sq_A + B);
	q = 1.0 / 2 * (2.0 / 27 * A * sq_A - 1.0 / 3 * A * B + C);

	/* use Cardano's formula */

	cb_p = p * p * p;
	D = q * q + cb_p;

	if (isZero(D)) {
		if (isZero(q)) { /* one triple solution */
			s[0] = 0;
			num = 1;
		}
		else { /* one single and one double solution */
			double u = cbrt(-q);
			s[0] = 2 * u;
			s[1] = -u;
			num = 2;
		}
	}
	else if (D < 0) { /* Casus irreducibilis: three real solutions */
		double phi = 1.0 / 3 * acos(-q / sqrt(-cb_p));
		double t = 2 * sqrt(-p);

		s[0] = t * cos(phi);
		s[1] = -t * cos(phi + XM_PI / 3);
		s[2] = -t * cos(phi - XM_PI / 3);
		num = 3;
	}
	else { /* one real solution */
		double sqrt_D = sqrt(D);
		double u = cbrt(sqrt_D - q);
		double v = -cbrt(sqrt_D + q);

		s[0] = u + v;
		num = 1;
	}

	/* resubstitute */

	sub = 1.0 / 3 * A;

	for (i = 0; i < num; ++i)
		s[i] -= sub;

	return num;
}
