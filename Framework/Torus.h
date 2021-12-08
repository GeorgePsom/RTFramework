#pragma once

class Torus : public Intersectable
{
public:
	Torus(float A, float B, Material& m) : a(A), b(B), mat(m){}

	~Torus();
	bool Intersect(Ray& ray)  override;
	void GetSurfaceData(Surface& surf, Ray& ray) const override;
	
private:
	int SolveQuartic(double c[5], double s[4]);
	int	SolveQuadric(double c[3], double s[2]);
	int	SolveCubic(double c[4], double s[3]);
	inline bool isZero(double x)
	{
		if (x < 1e-30 && x > -1e-30)
			return true;
		return false;
	}

public:
	float a, b;
	Material mat;
};
