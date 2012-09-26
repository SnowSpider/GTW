#ifndef __MAT_H__
#define __MAT_H__

#include <math.h>

#include "vec.h"

class Mat3{
    public:
    float m[9]; 
    /* Column-major order
    M = [ m[0] m[3] m[6] ]
        [ m[1] m[4] m[7] ]
        [ m[2] m[5] m[8] ]
    */
    
    Mat3();
	Mat3( const float m0, const float m3, const float m6,
	      const float m1, const float m4, const float m7,
          const float m2, const float m5, const float m8 );
	float& operator []( int i );
    float operator []( int i ) const;
	Vec3 operator*( const Vec3& v ) const;
	void setIdentity();
};

/*
A 4x4 matrix

        | 0   4   8  12 |
  mat = | 1   5   9  13 |
        | 2   6  10  14 |
        | 3   7  11  15 |
*/
class Mat4
{
    public:
        float mat[16];

        Mat4();
        Mat4(float matrix[16]);
        Mat4(float a, float e, float i, float m,
             float b, float f, float j, float n,
             float c, float g, float k, float o,
             float d, float h, float l, float p);
        
        float& operator []( int i );
	    float operator []( int i ) const;

        //Basic transformations
        static Mat4 CreateIdentity();
        static Mat4 CreateRotationAxis(float degrees, float axis_x, float axis_y, float axis_z);
        static Mat4 CreateRotationPitchYawRoll(float pitch, float yaw, float roll);
        static Mat4 CreateScale(float x, float y, float z);
        static Mat4 CreateTranslation(float x, float y, float z);

        //Projection matrices
        static Mat4 CreatePerspectiveProjection(float fovY, float aspect, float zNear, float zFar);
        static Mat4 CreateOrthographicProjection(float left, float right, float bottom, float top, float nearVal, float farVal);
        //Camera Setup
        static Mat4 CreateLookAt(const Vec3& pEye, const Vec3& pCenter, const Vec3& pUp);

        Mat4 Inverse() const;
        bool IsIdentity() const;
        Mat4 Transpose() const;
        Mat4 Multiply(const Mat4& other) const; //returns this * other
        bool Equal(const Mat4& other, float threshold = 0.0) const;

        Vec3 GetUpVec3() const;
        Vec3 GetRightVec3() const;
        Vec3 GetForwardVec3() const;

        //Transform a vector through this matrix:
        Vec3 Multiply(const Vec3& v) const; //returns Vec3( this * Vec4(v,1.0) )
        Vec4 Multiply(const Vec4& v) const; //returns this * v
};

const Mat4 operator*(const Mat4& lhs, const Mat4& rhs);
const Vec3 operator*(const Mat4& lhs, const Vec3& rhs);
const Vec4 operator*(const Mat4& lhs, const Vec4& rhs);

#endif

