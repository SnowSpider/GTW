#ifndef __QUAT_H__
#define __QUAT_H__

#include "vec.h"
#include "mat.h"
using namespace std;

class Quat{
    public:
    union {
        float array[4]; 
        struct{ float x,y,z,w; }; 
    };
    Quat(); //default constructor
    Quat( const float x, const float y, const float z, const float w ); //parametric constructor
    Quat( const Quat& q ); //copy constructor
    Quat& operator=( const Quat& q ); //copy-assinment operator
    
    float& operator []( int i ); //subscript
    float operator []( int i ) const;
    
    bool equals(const Quat& q);
    void clear();
    
    //addition
    //subtraction
    //multiplication
    //division
    
    //addition assignment
    //subtraction assignment
    //multiplication assignment
    //division assignment
    
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
