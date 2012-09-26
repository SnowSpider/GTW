#ifndef __QUAT_H__
#define __QUAT_H__

#include "vec.h"
#include "mat.h"
using namespace std;

class Quat{
    public:
    union {
        float array[4]; // The 4D low level array representation to pass the vector as pointer parameter
        struct{ float x,y,z,w; }; // Components X, Y, Z and W to be used by the application
    };
    Quat();
    Quat( const float x, const float y, const float z, const float w );
    Quat( const Quat& q );
    
    void clear();
    void set( const Vec3& v );
    void set( const Quat& q );
    void negative(const Quat& q);
    Quat& operator=( const Quat& q );
    bool operator==(const Quat& q) const ;
    float& operator[](const int i);
    float operator []( int i ) const;
    
    void normalize();
    Quat getConjugate() const; 
    Quat operator* (const Quat& rq) const;
    Vec3 operator* (const Vec3& vec) const;
    Quat& operator* (const float m);
    Quat& operator*=(const float m);
    Quat operator^( const Quat& q ) const;
    Quat operator+( const Quat& q ) const;
    Quat& operator+=( const Quat& q );
    void fromAxis (const Vec3& v, float angle);
    void fromEuler (float pitch, float yaw, float roll);
    void fromMatrix( const Mat4& m );
    Mat3 toRotMat() const;
    Mat4 getMatrix() const;
    void getAxisAngle (Vec3& axis, float& angle);
    Quat slerp(Quat& q, float t);
};

#endif
