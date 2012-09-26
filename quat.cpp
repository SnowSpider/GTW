#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "quat.h"
#include "vec.h"
#include "mat.h"
#include <string.h>

#define TOLERANCE 0.00001f
#define PIOVER180 0.01745329252

Quat::Quat(){
    x = y = z = 0;
    w = 1;
}
Quat::Quat( const float x, const float y, const float z, const float w ){
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}
Quat::Quat( const Quat& q ){
    *this = q;
}
void Quat::clear(){
    x = y = z = 0;
}
void Quat::set( const Vec3& v ){
    x = v.x;
    y = v.y;
    z = v.z;
}
void Quat::set( const Quat& q ){
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}
void Quat::negative(const Quat& q){
    x = -q.x;
    y = -q.y;
    z = -q.z;
    w = q.w;
}
Quat& Quat::operator=( const Quat& q ){
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}
bool Quat::operator==(const Quat& q) const {
    return memcmp(array, q.array, sizeof(float)*4)==0;
}
float& Quat::operator[](const int i){
    return array[i];
}
float Quat::operator []( int i ) const {
    return array[i];
}

// normalising a Quat works similar to a vector. This method will not do anything
// if the Quat is close enough to being unit-length. define TOLERANCE as something
// small like 0.00001f to get accurate results
void Quat::normalize() {
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

// We need to get the inverse of a Quat to properly apply a Quat-rotation to a vector
// The conjugate of a Quat is the same as the inverse, as long as the Quat is unit-length
Quat Quat::getConjugate() const {
	return Quat(-x, -y, -z, w);
}

// Multiplying q1 with q2 applies the rotation q2 to q1
Quat Quat::operator* (const Quat &rq) const {
	// the constructor takes its arguments as (x, y, z, w)
	return Quat(w * rq.x + x * rq.w + y * rq.z - z * rq.y,
	            w * rq.y + y * rq.w + z * rq.x - x * rq.z,
	            w * rq.z + z * rq.w + x * rq.y - y * rq.x,
	            w * rq.w - x * rq.x - y * rq.y - z * rq.z);
}

// Multiplying a Quat q with a vector v applies the q-rotation to v
Vec3 Quat::operator* (const Vec3 &vec) const {
	Vec3 vn(vec);
	vn.normalize();
 
	Quat vecQuat, resQuat;
	vecQuat.x = vn.x;
	vecQuat.y = vn.y;
	vecQuat.z = vn.z;
	vecQuat.w = 0.0f;
 
	resQuat = vecQuat * getConjugate();
	resQuat = *this * resQuat;
 
	return (Vec3(resQuat.x, resQuat.y, resQuat.z));
}

Quat& Quat::operator* (const float m){
    x *= m;
    y *= m;
    z *= m;
    return *this;
}

Quat& Quat::operator*=( const float m ){
    x *= m;
    y *= m;
    z *= m;
    return *this;
}

Quat Quat::operator^( const Quat& q ) const {
    return Quat( y * q.z - z * q.y,
                 z * q.x - x * q.z,
                 x * q.y - y * q.x , w);
}

Quat Quat::operator+( const Quat& q ) const { 
    return Quat(q.x+x, q.y+y, q.z+z, w); 
}

Quat& Quat::operator+=( const Quat& q ){
    x += q.x;
    y += q.y;
    z += q.z;
    return *this;
}

// Convert from Axis Angle
void Quat::fromAxis(const Vec3 &v, float angle){
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
void Quat::fromEuler(float pitch, float yaw, float roll){
	// Basically we create 3 Quats, one for pitch, one for yaw, one for roll
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

// Convert from Matrix
void Quat::fromMatrix( const Mat4& m ){ 
    float tr, s, q[4];
    int i, j, k;
    int next[3] = {1, 2, 0};
    tr = m[0] + m[5] + m[10];
    if(tr > 0.0){ // Check the diagonal
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
Mat4 Quat::getMatrix() const {
	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;
 
	// This calculation would be a lot more complicated for non-unit length Quats
	// Note: The constructor of Mat4 expects the Matrix in column-major format like expected by
	// OpenGL
	return Mat4( 1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
			     2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
			     2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
			     0.0f, 0.0f, 0.0f, 1.0f );
}

// Convert to Axis/Angles
void Quat::getAxisAngle(Vec3& axis, float& angle){
	float scale = sqrt(x * x + y * y + z * z);
	axis.x = x / scale;
	axis.y = y / scale;
	axis.z = z / scale;
	angle = acos(w) * 2.0f;
}

Quat Quat::slerp(Quat& rq, float t){
    Quat ret;
    float temp[4];
    double omega, cosom, sinom, scale0, scale1;
 
    // calc cosine
    cosom = x * rq.x + 
            y * rq.y + 
            z * rq.z + 
            w * rq.w;
 
    // adjust signs (if necessary)
    if ( cosom < 0.0 ){
        cosom = -cosom; 
        temp[0] = - rq.x;
        temp[1] = - rq.y;
        temp[2] = - rq.z;
        temp[3] = - rq.w;
    }
    else{
        temp[0] = rq.x;
        temp[1] = rq.y;
        temp[2] = rq.z;
        temp[3] = rq.w;
    }

    // calculate coefficients
    if ( (1.0 - cosom) > TOLERANCE ){
        // standard case (slerp)
        omega = acos(cosom);
        sinom = sin(omega);
        scale0 = sin((1.0 - t) * omega) / sinom;
        scale1 = sin(t * omega) / sinom;
    }
    else{
        // "from" and "to" Quats are very close 
        //  ... so we can do a linear interpolation
        scale0 = 1.0 - t;
        scale1 = t;
    } 

    // calculate final values
    ret.x = scale0 * x + scale1 * temp[0];
    ret.y = scale0 * y + scale1 * temp[1];
    ret.z = scale0 * z + scale1 * temp[2];
    ret.w = scale0 * w + scale1 * temp[3];
}

