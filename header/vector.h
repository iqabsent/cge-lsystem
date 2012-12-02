////////////////////////////////////////////////////////////////////////////////




// ---------- vector.h ----------

/*!

\file vector.h
\brief interface for the Vector3D class

The Vector3D class: represents a 3 dimensional vector.

Only has 3 variables: x, y, and z.

*/




#ifndef _vector3D_
#define _vector3D_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




////////////////////////////////////////////////////////////////////////////////




class Vector3D {

 public:

	/*
	
	Variables:
	
	I didn't make x, y, and z private, as the programmer should already know the
	inner workings of a vector, and would figure that there would be an x, y, and
	z variables.
	
	Plus, I hate the look of setX(int x) and getX() :D .
	
	*/

	double x, y, z;

	/*
	
	Constructors:
	
	Vector3D v: x, y, and z are all set to 0.
	Vector3D v2(1, 2, 3): x, y, and z are set to 1, 2, and 3, respectivly.
	Vector3D v3(Vector3D v4): v3.x, v3.y, and v3.z are all set to v4.x, v4.y, and v4.z, respectivly.
	
	*/
	
	Vector3D() {
		x = y = z = 0;
	}

	Vector3D(double a, double b, double c) {
		x = a;
		y = b;
		z = c;
	}

	Vector3D(const Vector3D &v) {
		x = v.x;
		y = v.y;
		z = v.z;
	}

	/*
	
	Operator overloadings:
	
	Vector3D operator+(Vector v): x, y, and z are added to v.x, v.y, and v.z, respectivly.
	Vector3D operator-(Vector3D v): x, y, and z are subtracted from v.x, v.y, and v.z, respectivly.
	Vector3D operator*(double w): x, y, and z are all multiplied by w.
	Vector3D operator/(double w): x, y, and z are all divided by w.
	Vector3D operator=(Vector3D v): this vector is set to v.
	
	*/

	Vector3D operator+(Vector3D v);
	Vector3D operator-(Vector3D v);
	Vector3D operator*(double w);
	Vector3D operator/(double w);
	Vector3D operator=(Vector3D v);


	/*

	Other functions:

	void negate(): negate the vector - x, y, and z are set to -x, -y, and -z, respectivly
	double length(): returns the length of the vector.
	void normalize(): normalize the vector, vector / length of vector
	
	*/

	void	negate();
	double	length();
	void	normalize();

	void	crossProduct(Vector3D);
	double	dotProduct(Vector3D);

};




////////////////////////////////////////////////////////////////////////////////




#endif // _vector3D_