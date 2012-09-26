#include <cmath>
#include <iostream>

#include <memory.h>
#include <assert.h>
#include <stdlib.h>

#include "vec.h"
#include "mat.h"
using namespace std;

Mat3::Mat3(){ 
    m[0][0] = 0.0;
	m[0][1] = 0.0;
    m[0][2] = 0.0;
    m[1][0] = 0.0;
    m[1][1] = 0.0;
    m[1][2] = 0.0;
    m[2][0] = 0.0;
    m[2][1] = 0.0;
    m[2][2] = 0.0;
}
    
Mat3::Mat3( const float m0, const float m1, const float m2,
	        const float m3, const float m4, const float m5,
            const float m6, const float m7, const float m8 ){ 
    m[0][0] = m0; 
    m[0][1] = m3; 
    m[0][2] = m6;
    m[1][0] = m1; 
    m[1][1] = m4; 
    m[1][2] = m7;
    m[2][0] = m2; 
    m[2][1] = m5; 
    m[2][2] = m8;
}
	
float* Mat3::operator [](int i) { 
	return m[i];
}
	
Vec3 Mat3::operator*( const Vec3& v ) const {
    Vec3 result;
    for( int i=0; i<3; i++ ){
        for( int j=0; j<3; j++ ){
            result[i] += m[i][j] * v[j];
        }
    }
    return result;
}


Mat4::Mat4(){
    memset(mat, 0, sizeof(float) * 16);
    mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
}

Mat4::Mat4(float matrix[16]){
    memcpy((void*)mat, (void*)matrix, sizeof(float) * 16);
}

/*
A 4x4 matrix

        | 0   4   8  12 |
  mat = | 1   5   9  13 |
        | 2   6  10  14 |
        | 3   7  11  15 |
*/

Mat4::Mat4(float a, float e, float i, float m,
           float b, float f, float j, float n,
           float c, float g, float k, float o,
           float d, float h, float l, float p){
    mat[0] = a; //mat[0][0]
    mat[1] = b; //mat[0][1]
    mat[2] = c; //mat[0][2]
    mat[3] = d; //mat[0][3]
    mat[4] = e; //mat[1][0]
    mat[5] = f; //mat[1][1]
    mat[6] = g; //mat[1][2]
    mat[7] = h; //mat[1][3]
    mat[8] = i; //mat[2][0]
    mat[9] = j; //mat[2][1]
    mat[10] = k; //mat[2][2]
    mat[11] = l; //mat[2][3]
    mat[12] = m; //mat[3][0]
    mat[13] = n; //mat[3][1]
    mat[14] = o; //mat[3][2]
    mat[15] = p; //mat[3][3]
}

float& Mat4::operator []( int i ){
    return mat[i];
}
float Mat4::operator []( int i ) const {
    return mat[i];
}

//Basic transformations
Mat4 Mat4::CreateIdentity(){
    //is already an identity matrix..
    Mat4 pOut;
    return pOut;
}

Mat4 Mat4::CreateRotationAxis(float degrees, float axis_x, float axis_y, float axis_z){
    float radians = degrees*M_PI / 180.0;
    float rcos = cosf(radians);
    float rsin = sinf(radians);

    Mat4 out;

    out.mat[0] = rcos + axis_x * axis_x * (1 - rcos);
    out.mat[1] = axis_z * rsin + axis_y * axis_x * (1 - rcos);
    out.mat[2] = -axis_y * rsin + axis_z * axis_x * (1 - rcos);
    out.mat[3] = 0.0f;

    out.mat[4] = -axis_z * rsin + axis_x * axis_y * (1 - rcos);
    out.mat[5] = rcos + axis_y * axis_y * (1 - rcos);
    out.mat[6] = axis_x * rsin + axis_z * axis_y * (1 - rcos);
    out.mat[7] = 0.0f;

    out.mat[8] = axis_y * rsin + axis_x * axis_z * (1 - rcos);
    out.mat[9] = -axis_x * rsin + axis_y * axis_z * (1 - rcos);
    out.mat[10] = rcos + axis_z * axis_z * (1 - rcos);
    out.mat[11] = 0.0f;

    out.mat[12] = 0.0f;
    out.mat[13] = 0.0f;
    out.mat[14] = 0.0f;
    out.mat[15] = 1.0f;

    return out;
}

Mat4 Mat4::CreateRotationPitchYawRoll(float pitch, float yaw, float roll){
    Mat4 out;
    double cr = cos(pitch);
    double sr = sin(pitch);
    double cp = cos(yaw);
    double sp = sin(yaw);
    double cy = cos(roll);
    double sy = sin(roll);
    double srsp = sr * sp;
    double crsp = cr * sp;

    out.mat[0] = (float) cp * cy;
    out.mat[4] = (float) cp * sy;
    out.mat[8] = (float) - sp;

    out.mat[1] = (float) srsp * cy - cr * sy;
    out.mat[5] = (float) srsp * sy + cr * cy;
    out.mat[9] = (float) sr * cp;

    out.mat[2] = (float) crsp * cy + sr * sy;
    out.mat[6] = (float) crsp * sy - sr * cy;
    out.mat[10] = (float) cr * cp;

    out.mat[3] = out.mat[7] = out.mat[11] = 0.0;
    out.mat[15] = 1.0;

    return out;
}

Mat4 Mat4::CreateScale(float x, float y, float z){
    Mat4 out;

    out.mat[0] = x;
    out.mat[5] = y;
    out.mat[10] = z;
    out.mat[15] = 1.0f;

    return out;
}

Mat4 Mat4::CreateTranslation(float x, float y, float z){
    Mat4 out;

    out.mat[12] = x;
    out.mat[13] = y;
    out.mat[14] = z;
    out.mat[15] = 1.0f;

    return out;
}


//Projection matrices
Mat4 Mat4::CreatePerspectiveProjection(float fovY, float aspect, float zNear, float zFar){
    float r = fovY * M_PI / 360.0;
    float deltaZ = zFar - zNear;
    float s = sin(r);
    float cotangent = 0;

    if (deltaZ == 0 || s == 0 || aspect == 0)
    {
        return NULL;
    }

    //cos(r) / sin(r) = cot(r)
    cotangent = cos(r) / s;

    Mat4 out;

    out.mat[0] = cotangent / aspect;
    out.mat[5] = cotangent;
    out.mat[10] = -(zFar + zNear) / deltaZ;
    out.mat[11] = -1;
    out.mat[14] = -2 * zNear * zFar / deltaZ;
    out.mat[15] = 0;

    return out;
}

/** Creates an orthographic projection matrix like glOrtho */
Mat4 Mat4::CreateOrthographicProjection(float left, float right, float bottom, float top, float nearVal, float farVal){
    Mat4 out;
    float tx = -((right + left) / (right - left));
    float ty = -((top + bottom) / (top - bottom));
    float tz = -((farVal + nearVal) / (farVal - nearVal));

    out.mat[0] = 2 / (right - left);
    out.mat[5] = 2 / (top - bottom);
    out.mat[10] = -2 / (farVal - nearVal);
    out.mat[12] = tx;
    out.mat[13] = ty;
    out.mat[14] = tz;

    return out;
}

/**
 * Builds a translation matrix in the same way as gluLookAt()
 */

//Camera Setup
Mat4 Mat4::CreateLookAt(const Vec3& pEye, const Vec3& pCenter, const Vec3& pUp){
    Vec3 f, up, s, u;
    Mat4 translate, out;

    f = (pCenter - pEye).normalized();

    up = pUp.normalized();

    s = (f^up).normalized();

    u = (s^f).normalized();

    out.mat[0] = s.x;
    out.mat[4] = s.y;
    out.mat[8] = s.z;

    out.mat[1] = u.x;
    out.mat[5] = u.y;
    out.mat[9] = u.z;

    out.mat[2] = -f.x;
    out.mat[6] = -f.y;
    out.mat[10] = -f.z;

    translate = Mat4::CreateTranslation(-pEye.x, -pEye.y, -pEye.z);
    out = out.Multiply(translate);

    return out;
}

Mat4 Mat4::Inverse() const {
    float temp[4];
    float tmat[16];
    int i, j, k;

    for (i = 0; i < 16; i++){
        tmat[i] = this->mat[i];
    }

    Mat4 pOut;

    for (j = 0; j < 4; ++j){ // Find largest pivot in column j among rows j..3
        int i1 = j;              // Row with largest pivot candidate
        for (i = j + 1; i < 4; ++i){
            if (fabs(tmat[i*4 + j]) > fabs(tmat[i1*4 + j])){
                i1 = i;
            }
        }

        // Swap rows i1 and j in a and b to put pivot on diagonal
        for (k = 0; k < 4; k++){
            temp[k] = tmat[i1 * 4 + k];
        }

        for (k = 0; k < 4; k++){
            tmat[i1 * 4 + k] = tmat[j * 4 + k];
            tmat[j * 4 + k] = temp[k];
        }

        for (k = 0; k < 4; k++){
            temp[k] = pOut.mat[i1 * 4 + k];
        }

        for (k = 0; k < 4; k++){
            pOut.mat[i1 * 4 + k] = pOut.mat[j * 4 + k];
            pOut.mat[j * 4 + k] = temp[k];
        }

        // Scale row j to have a unit diagonal
        if (!tmat[j*4 + j]){
            // Singular matrix - can't invert
            return NULL;
        }

        for (k = 0; k < 4; k++){
            pOut.mat[j * 4 + k] /= tmat[j * 4 + j];
            tmat[j * 4 + k] /= tmat[j * 4 + j];
        }

        // Eliminate off-diagonal elems in col j of a, doing identical ops to b
        for (i = 0; i < 4; ++i){
            if (i != j){
                for (k = 0; k < 4; k++){
                    pOut.mat[i*4 + k] -= tmat[i*4 + j] * pOut.mat[j*4 + k];
                    tmat[i*4 + k] -= tmat[i*4 + j] * tmat[j*4 + k];
                }
            }
        }
    }

    return pOut;
}

bool Mat4::IsIdentity() const {
    static const float identity [] = { 1.0f, 0.0f, 0.0f, 0.0f,
                                       0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f, 0.0f, 1.0f, 0.0f,
                                       0.0f, 0.0f, 0.0f, 1.0f };

    return (memcmp(identity, this->mat, sizeof(float) * 16) == 0);
}

Mat4 Mat4::Transpose() const {
    int x, z;
    Mat4 pOut;

    for (z = 0; z < 4; ++z)
    {
        for (x = 0; x < 4; ++x)
        {
            pOut.mat[(z * 4) + x] = mat[(x * 4) + z];
        }
    }

    return pOut;
}

Mat4 Mat4::Multiply(const Mat4& other) const//returns this * other
{
    Mat4 out;

    const float *m1 = mat, *m2 = other.mat;

    out.mat[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
    out.mat[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
    out.mat[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
    out.mat[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

    out.mat[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
    out.mat[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
    out.mat[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
    out.mat[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

    out.mat[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
    out.mat[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
    out.mat[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
    out.mat[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

    out.mat[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
    out.mat[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
    out.mat[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
    out.mat[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

    return out;
}

bool Mat4::Equal(const Mat4& other, float threshold) const {
    int i = 0;

    for (i = 0; i < 16; ++i)
    {
        if (!(mat[i] + threshold > other.mat[i] &&
                mat[i] - threshold < other.mat[i]))
        {
            return false;
        }
    }

    return true;
}

Vec3 Mat4::GetUpVec3() const {
    Vec3 out;
    out.x = mat[4];
    out.y = mat[5];
    out.z = mat[6];

    return out;
}

Vec3 Mat4::GetRightVec3() const {
    Vec3 out;
    out.x = mat[0];
    out.y = mat[1];
    out.z = mat[2];

    return out;
}

Vec3 Mat4::GetForwardVec3() const {
    Vec3 out;
    out.x = mat[8];
    out.y = mat[9];
    out.z = mat[10];

    return out;
}


//Transform a vector through this matrix:
Vec3 Mat4::Multiply(const Vec3& v) const {
    Vec3 out;
    out.x = mat[0]*v.x + mat[4]*v.y + mat[8]*v.z;
    out.y = mat[1]*v.x + mat[5]*v.y + mat[9]*v.z;
    out.z = mat[2]*v.x + mat[6]*v.y + mat[10]*v.z;
    return out; //returns Vec3( this * Vec4(v,1.0) )
}

Vec4 Mat4::Multiply(const Vec4& v) const {
    Vec4 out;
    out.x = mat[0]*v.x + mat[4]*v.y + mat[ 8]*v.z + mat[12]*v.w;
    out.y = mat[1]*v.x + mat[5]*v.y + mat[ 9]*v.z + mat[13]*v.w;
    out.z = mat[2]*v.x + mat[6]*v.y + mat[10]*v.z + mat[14]*v.w;
    out.w = mat[3]*v.x + mat[7]*v.y + mat[11]*v.z + mat[15]*v.w;
    return out; //returns this * v
}


const Mat4 operator*(const Mat4& lhs, const Mat4& rhs){
                return lhs.Multiply(rhs);
}

const Vec3 operator*(const Mat4& lhs, const Vec3& rhs){
                return lhs.Multiply(rhs);
}

const Vec4 operator*(const Mat4& lhs, const Vec4& rhs){
                return lhs.Multiply(rhs);
}

