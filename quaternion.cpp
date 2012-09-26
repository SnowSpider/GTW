#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "quaternion.h"
#include "vec.h"
#include "mat.h"
#include <string.h>

#define TOLERANCE 0.00001f
#define PIOVER180 0.01745329252

Quaternion::Quaternion(){
    x = y = z = 0.0f;
    w = 1;
}
Quaternion::Quaternion( const float x, const float y, const float z, const float w ){
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}
Quaternion::Quaternion( const Quaternion &v ){
    *this = v;
}
bool Quaternion::operator==(const Quaternion&v) const {
    return memcmp(array, v.array, sizeof(float)*4)==0;
}
float& Quaternion::operator[](const int v){
    return array[v];
}

// normalising a quaternion works similar to a vector. This method will not do anything
// if the quaternion is close enough to being unit-length. define TOLERANCE as something
// small like 0.00001f to get accurate results
void Quaternion::normalize()
{
	// Don't normalize if we don't have to
	float mag2 = w * w + x * x + y * y + z * z;
	if (fabs(mag2) > TOLERANCE && fabs(mag2 - 1.0f) > TOLERANCE) {
		float mag = sqrt(mag2);
		w /= mag;
		x /= mag;
		y /= mag;
		z /= mag;
	}
}

// We need to get the inverse of a quaternion to properly apply a quaternion-rotation to a vector
// The conjugate of a quaternion is the same as the inverse, as long as the quaternion is unit-length
Quaternion Quaternion::getConjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

// Multiplying q1 with q2 applies the rotation q2 to q1
Quaternion Quaternion::operator* (const Quaternion &rq) const
{
	// the constructor takes its arguments as (x, y, z, w)
	return Quaternion(w * rq.x + x * rq.w + y * rq.z - z * rq.y,
	                  w * rq.y + y * rq.w + z * rq.x - x * rq.z,
	                  w * rq.z + z * rq.w + x * rq.y - y * rq.x,
	                  w * rq.w - x * rq.x - y * rq.y - z * rq.z);
}

// Multiplying a quaternion q with a vector v applies the q-rotation to v
Vec3 Quaternion::operator* (const Vec3 &vec) const
{
	Vec3 vn(vec);
	vn.normalize();
 
	Quaternion vecQuat, resQuat;
	vecQuat.x = vn.x;
	vecQuat.y = vn.y;
	vecQuat.z = vn.z;
	vecQuat.w = 0.0f;
 
	resQuat = vecQuat * getConjugate();
	resQuat = *this * resQuat;
 
	return (Vec3(resQuat.x, resQuat.y, resQuat.z));
}

// Convert from Axis Angle
void Quaternion::FromAxis(const Vec3 &v, float angle)
{
	float sinAngle;
	angle *= 0.5f;
	Vec3 vn(v);
	vn.normalize();
 
	sinAngle = sin(angle);
 
	x = (vn.x * sinAngle);
	y = (vn.y * sinAngle);
	z = (vn.z * sinAngle);
	w = cos(angle);
}

// Convert from Euler Angles
void Quaternion::FromEuler(float pitch, float yaw, float roll)
{
	// Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll
	// and multiply those together.
	// the calculation below does the same, just shorter
 
	float p = pitch * PIOVER180 / 2.0;
	float y = yaw * PIOVER180 / 2.0;
	float r = roll * PIOVER180 / 2.0;
 
	float sinp = sin(p);
	float siny = sin(y);
	float sinr = sin(r);
	float cosp = cos(p);
	float cosy = cos(y);
	float cosr = cos(r);
 
	this->x = sinr * cosp * cosy - cosr * sinp * siny;
	this->y = cosr * sinp * cosy + sinr * cosp * siny;
	this->z = cosr * cosp * siny - sinr * sinp * cosy;
	this->w = cosr * cosp * cosy + sinr * sinp * siny;
 
	normalize();
}

// Convert from Mat4
void Quaternion::fromMatrix( const Mat4& m ){ 
    float tr, s, q[4];
    int i, j, k;
    int next[3] = {1, 2, 0};
    tr = m[0] + m[5] + m[10];
    // Check the diagonal
    if(tr > 0.0){
        s = sqrt(tr + 1.0);
        w = s / 2.0;
        s  = 0.5 / s;
        x = (m[6] - m[9]) * s;
        y = (m[8] - m[2]) * s;
        z = (m[1] - m[4]) * s;
    }
    else{
        i = 0;
        if(m[5] > m[0]){
            i = 1;
        }
        if(m[10] > m[i*4]){
            i = 2;
        }
        j = next[i];
        k = next[j];
        s = sqrt((m[i*4] - m[j*4] + m[k*4]) + 1.0);
        
        q[i] = s * 0.5;
        if(s != 0.0){
            s = 0.5 / s;
        }
        q[3] = (m[j*4+k] + m[k*4+j]) * s;
        q[j] = (m[i*4+j] + m[j*4+i]) * s;
        q[k] = (m[i*4+k] + m[k*4+i]) * s;
        x = q[0];
        y = q[1];
        z = q[2];
        w = q[3];
    }
}

// Convert to Matrix
Mat4 Quaternion::getMatrix() const
{
	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;
 
	// This calculation would be a lot more complicated for non-unit length quaternions
	// Note: The constructor of Mat4 expects the Matrix in column-major format like expected by
	// OpenGL
	return Mat4( 1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
			     2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
			     2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
			     0.0f, 0.0f, 0.0f, 1.0f );
}

// Convert to Axis/Angles
void Quaternion::getAxisAngle(Vec3 *axis, float *angle)
{
	float scale = sqrt(x * x + y * y + z * z);
	axis->x = x / scale;
	axis->y = y / scale;
	axis->z = z / scale;
	*angle = acos(w) * 2.0f;
}



