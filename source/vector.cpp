////////////////////////////////////////////////////////////////////////////////




// ---------- vector.cpp ----------

/*!

\file vector.cpp
\brief implementation of the Vector3D class

*/




// ---------- include ----------

#include <math.h>

#include "../header/vector.h"




////////////////////////////////////////////////////////////////////////////////




Vector3D Vector3D::operator+(Vector3D v) {
	return Vector3D((x + v.x), (y + v.y), (z + v.z));
}


Vector3D Vector3D::operator-(Vector3D v) {
	return Vector3D((x - v.x), (y - v.y), (z - v.z));
}


Vector3D Vector3D::operator*(double w) {
	return Vector3D((x * w), (y * w), (z * w));
}


Vector3D Vector3D::operator/(double w) {
 return Vector3D((x / w), (y / w), (z / w));
}


Vector3D Vector3D::operator=(Vector3D v) {
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}


void Vector3D::negate() {
	x = -x;
	y = -y;
	z = -z;
}


double Vector3D::length() {
	return sqrt((x * x) + (y * y) + (z * z));
}


void Vector3D::normalize() {
	double l = length();
	x /= l;
	y /= l;
	z /= l;
}

void Vector3D::crossProduct( Vector3D a )
{

	double x1, y1, z1;

	x1 = ( a.y * z ) - ( a.z * y );
	y1 = ( a.z * x ) - ( a.x * z );
	z1 = ( a.x * y ) - ( a.y * x );

	x = x1;
	y = y1;
	z = z1;

}

double Vector3D::dotProduct( Vector3D a )
{

	return ( a.x * x ) + ( a.y * y ) + ( a.z * z );

}




////////////////////////////////////////////////////////////////////////////////
