#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "vec.h"
#include "mat.h"
using namespace std;

class Quaternion{
    public:
    union {
        float array[4];///<The 4D low level array representation to pass the vector as pointer parameter
        struct{ float x,y,z,w; };///<Components X, Y, Z and W to be used by the application
    };
    Quaternion();
    Quaternion( const float x, const float y, const float z, const float w );
    Quaternion( const Quaternion& v );
    
    bool operator==(const Quaternion& v) const ;
    float& operator[](const int v);
    void normalize();
    Quaternion getConjugate() const; 
    Quaternion operator* (const Quaternion& rq) const;
    Vec3 operator* (const Vec3& vec) const;
    void FromAxis (const Vec3& v, float angle);
    void FromEuler (float pitch, float yaw, float roll);
    void fromMatrix( const Mat4& m );
    Mat4 getMatrix () const;
    void getAxisAngle (Vec3* axis, float* angle);
};

#endif
