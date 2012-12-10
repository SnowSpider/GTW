#include <cmath>
#include <iostream>

#include <memory.h>
#include <assert.h>
#include <stdlib.h>

#include "mat.h"

using namespace std;

Mat3::Mat3(){ //default constructor
    memset(n, 0, sizeof(n));
}
Mat3::Mat3( const float m00, const float m10, const float m20,
            const float m01, const float m11, const float m21,
            const float m02, const float m12, const float m22 ){ //parametric constructor
    n[0] = m00;
    n[1] = m10;
    n[2] = m20;
    n[3] = m01;
    n[4] = m11;
    n[5] = m21;
    n[6] = m02;
    n[7] = m12;
    n[8] = m22;
}
float& Mat3::operator [](int i) { //subscript
    return n[i];
}
float Mat3::operator []( int i ) const {
    return n[i];
}
Mat3::Mat3( const Mat3& m ){ //copy constructor
    n[0] = m[0];
    n[1] = m[1];
    n[2] = m[2];
    n[3] = m[3];
    n[4] = m[4];
    n[5] = m[5];
    n[6] = m[6];
    n[7] = m[7];
    n[8] = m[8];
}
Mat3& Mat3::operator=( const Mat3& m ){ //copy-assinment operator
    n[0] = m[0];
    n[1] = m[1];
    n[2] = m[2];
    n[3] = m[3];
    n[4] = m[4];
    n[5] = m[5];
    n[6] = m[6];
    n[7] = m[7];
    n[8] = m[8];
    return *this;
}
/*
Mat3::~Mat3(){ //destructor
    delete[] n;
}
*/
bool Mat3::equals(const Mat3& m){
    for(int i=0;i<9;i++){
        if(n[i] != m[i]) return false;
    }
    return true;
}
void Mat3::clear(){
    memset(n, 0, sizeof(n));
}
void Mat3::setIdentity(){
    memset(n, 0, sizeof(n));
    m00 = m11 = m22 = 1.0;
}
Mat3 Mat3::operator+( const Mat3& m ) const { //addition
    Mat3 ret;
    for(int i=0;i<9;i++){
        ret[i] = n[i] + m[i];
    }
    return ret;
}
Mat3 Mat3::operator-( const Mat3& m ) const { //subtraction
    Mat3 ret;
    for(int i=0;i<9;i++){
        ret[i] = n[i] - m[i];
    }
    return ret;
}
Vec3 Mat3::operator*( const Vec3& v ) const { //multiplication
    Vec3 ret;
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            ret[i] += n[i*3 + j] * v[j]; 
        }
    }
    return ret;
}
Mat3 Mat3::operator*( const Mat3& m ) const {
    Mat3 ret;
    
    ret.m00 = (m00 * m.m00) + (m01 * m.m10) + (m02 * m.m20);
    ret.m01 = (m00 * m.m01) + (m01 * m.m11) + (m02 * m.m21);
    ret.m02 = (m00 * m.m02) + (m01 * m.m12) + (m02 * m.m22);

    ret.m10 = (m10 * m.m00) + (m11 * m.m10) + (m12 * m.m20);
    ret.m11 = (m10 * m.m01) + (m11 * m.m11) + (m12 * m.m21);
    ret.m12 = (m10 * m.m02) + (m11 * m.m12) + (m12 * m.m22);

    ret.m20 = (m20 * m.m00) + (m21 * m.m10) + (m22 * m.m20);
    ret.m21 = (m20 * m.m01) + (m21 * m.m11) + (m22 * m.m21);
    ret.m22 = (m20 * m.m02) + (m21 * m.m12) + (m22 * m.m22);
    
    return ret;
}
Mat3 Mat3::operator*( const float& m ) const { //scalar multiplication
    Mat3 ret;
    for(int i=0;i<9;i++){
        ret[i] = n[i] * m;
    }
    return ret;
}
Mat3 Mat3::operator+=( const Mat3& m ) { //addition assignment
    for(int i=0;i<9;i++){
        n[i] += m[i];
    }
}
Mat3 Mat3::operator-=( const Mat3& m ) { //subtraction assignment
    for(int i=0;i<9;i++){
        n[i] -= m[i];
    }
}
Mat3& Mat3::operator*=( const Mat3& m ) { //multiplication assignment
    Mat3 ret;
    
    ret.m00 = (m00 * m.m00) + (m01 * m.m10) + (m02 * m.m20);
    ret.m01 = (m00 * m.m01) + (m01 * m.m11) + (m02 * m.m21);
    ret.m02 = (m00 * m.m02) + (m01 * m.m12) + (m02 * m.m22);

    ret.m10 = (m10 * m.m00) + (m11 * m.m10) + (m12 * m.m20);
    ret.m11 = (m10 * m.m01) + (m11 * m.m11) + (m12 * m.m21);
    ret.m12 = (m10 * m.m02) + (m11 * m.m12) + (m12 * m.m22);

    ret.m20 = (m20 * m.m00) + (m21 * m.m10) + (m22 * m.m20);
    ret.m21 = (m20 * m.m01) + (m21 * m.m11) + (m22 * m.m21);
    ret.m22 = (m20 * m.m02) + (m21 * m.m12) + (m22 * m.m22);
    
    *this = ret;
    
    return *this;
}
Mat3 Mat3::transpose(){
    Mat3 ret;
    ret[0] = n[0];
    ret[1] = n[3];
    ret[2] = n[6];
    ret[3] = n[1];
    ret[4] = n[4];
    ret[5] = n[7];
    ret[6] = n[2];
    ret[7] = n[5];
    ret[8] = n[8];
}
float Mat3::determinant(){
    return n[0]*n[4]*n[8] + n[3]*n[7]*n[2] + n[6]*n[1]*n[5] - n[6]*n[4]*n[2] - n[3]*n[1]*n[8] - n[0]*n[7]*n[5];
}

Mat4::Mat4(){ //default constructor
    memset(n, 0, sizeof(n));
}
Mat4::Mat4(float n00, float n10, float n20, float n30,
           float n01, float n11, float n21, float n31,
           float n02, float n12, float n22, float n32,
           float n03, float n13, float n23, float n33){ //parametric constructor
    n[0] = n00;
    n[1] = n10; 
    n[2] = n20; 
    n[3] = n30; 
    n[4] = n01; 
    n[5] = n11; 
    n[6] = n21; 
    n[7] = n31; 
    n[8] = n02; 
    n[9] = n12; 
    n[10] = n22; 
    n[11] = n32; 
    n[12] = n03; 
    n[13] = n13; 
    n[14] = n23; 
    n[15] = n33; 
}
float& Mat4::operator []( int i ){ //subscript
    return n[i];
}
float Mat4::operator []( int i ) const {
    return n[i];
}
Mat4::Mat4( const Mat4& m ){ //copy constructor
    n[0] = m[0]; 
    n[1] = m[1]; 
    n[2] = m[2]; 
    n[3] = m[3]; 
    n[4] = m[4]; 
    n[5] = m[5]; 
    n[6] = m[6]; 
    n[7] = m[7]; 
    n[8] = m[8]; 
    n[9] = m[9]; 
    n[10] = m[10]; 
    n[11] = m[11]; 
    n[12] = m[12]; 
    n[13] = m[13]; 
    n[14] = m[14]; 
    n[15] = m[15]; 
}
Mat4& Mat4::operator=( const Mat4& m ){ //copy-assinment operator
    n[0] = m[0]; 
    n[1] = m[1]; 
    n[2] = m[2]; 
    n[3] = m[3]; 
    n[4] = m[4]; 
    n[5] = m[5]; 
    n[6] = m[6]; 
    n[7] = m[7]; 
    n[8] = m[8]; 
    n[9] = m[9]; 
    n[10] = m[10]; 
    n[11] = m[11]; 
    n[12] = m[12]; 
    n[13] = m[13]; 
    n[14] = m[14]; 
    n[15] = m[15]; 
    return *this;
}
/*
Mat4::~Mat4(){ //destructor
    delete[] n;
}
*/
bool Mat4::equals(const Mat4& m){
    for(int i=0;i<16;i++){
        if(n[i] != m[i]) return false;
    }
    return true;
}
void Mat4::clear(){
    memset(n, 0, sizeof(n));
}
void Mat4::setIdentity(){
    memset(n, 0, sizeof(n));
    m00 = m11 = m22 = m33 = 1.0;
}






Mat4& Mat4::operator=( const Mat3& m ){
    m00 = m.m00; m10 = m.m10; m20 = m.m20;
    m01 = m.m01; m11 = m.m11; m21 = m.m21;
    m02 = m.m02; m12 = m.m12; m22 = m.m22;
    return *this;
}

Mat4& Mat4::operator*=( const float scale ){
    m00 *= scale; m10 *= scale; m20 *= scale;
    m01 *= scale; m11 *= scale; m21 *= scale;
    m02 *= scale; m12 *= scale; m22 *= scale;
    return *this;
}

float Mat4::SVD(){ 
    float scale = sqrt( ( (m00 * m00) + (m01 * m01) + (m02 * m02) + 
                          (m10 * m10) + (m11 * m11) + (m12 * m12) +
                          (m20 * m20) + (m21 * m21) + (m22 * m22) ) / 3.0f );
    return scale;
}

float Mat4::SVD(Mat3& rot3){
    //this->getRotationScale(rot3);
    rot3.m00 = m00; rot3.m01 = m01; rot3.m02 = m02;
    rot3.m10 = m10; rot3.m11 = m11; rot3.m12 = m12;
    rot3.m20 = m20; rot3.m21 = m21; rot3.m22 = m22;

    // zero-div may occur.
    float s = 1.0f / sqrt( (m00 * m00) +
                           (m01 * m01) +
                           (m02 * m02) );
    rot3.m00 *= s;
    rot3.m01 *= s;
    rot3.m02 *= s;

    s = 1.0f / sqrt( (m10 * m10) +
                     (m11 * m11) +
                     (m12 * m12) );
    rot3.m10 *= s;
    rot3.m11 *= s;
    rot3.m12 *= s;

    s = 1.0f / sqrt( (m20 * m20) +
                     (m21 * m21) +
                     (m22 * m22) );
    rot3.m20 *= s;
    rot3.m21 *= s;
    rot3.m22 *= s;
    
    return SVD();
}

float Mat4::SVD(Mat4& rot4){
    rot4 = *this;

    // zero-div may occur.
    float s = 1.0f / sqrt( (m00 * m00) +
                           (m01 * m01) +
                           (m02 * m02) );
    rot4.m00 *= s;
    rot4.m01 *= s;
    rot4.m02 *= s;

    s = 1.0f / sqrt( (m10 * m10) +
                     (m11 * m11) +
                     (m12 * m12) );
    rot4.m10 *= s;
    rot4.m11 *= s;
    rot4.m12 *= s;

    s = 1.0f / sqrt( (m20 * m20) +
                     (m21 * m21) +
                     (m22 * m22) );
    rot4.m20 *= s;
    rot4.m21 *= s;
    rot4.m22 *= s;
    
    return SVD();
}

void Mat4::setRot(Mat3& rot3){
    float scale = SVD();
    *this = rot3;
    *this *= scale;
}

//Basic transformations
Mat4 Mat4::CreateIdentity(){
    /*
    Mat4 pOut(1,0,0,0,
              0,1,0,0,
              0,0,1,0,
              0,0,0,1);
    */
    Mat4 pOut; //already identity
    return pOut;
}

Mat4 Mat4::CreateRotationAxis(float degrees, float axis_x, float axis_y, float axis_z){
    float radians = degrees*M_PI / 180.0;
    float rcos = cosf(radians);
    float rsin = sinf(radians);

    Mat4 out;

    out[0] = rcos + axis_x * axis_x * (1 - rcos);
    out[1] = axis_z * rsin + axis_y * axis_x * (1 - rcos);
    out[2] = -axis_y * rsin + axis_z * axis_x * (1 - rcos);
    out[3] = 0.0f;

    out[4] = -axis_z * rsin + axis_x * axis_y * (1 - rcos);
    out[5] = rcos + axis_y * axis_y * (1 - rcos);
    out[6] = axis_x * rsin + axis_z * axis_y * (1 - rcos);
    out[7] = 0.0f;

    out[8] = axis_y * rsin + axis_x * axis_z * (1 - rcos);
    out[9] = -axis_x * rsin + axis_y * axis_z * (1 - rcos);
    out[10] = rcos + axis_z * axis_z * (1 - rcos);
    out[11] = 0.0f;

    out[12] = 0.0f;
    out[13] = 0.0f;
    out[14] = 0.0f;
    out[15] = 1.0f;

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

    out[0] = (float) cp * cy;
    out[4] = (float) cp * sy;
    out[8] = (float) - sp;

    out[1] = (float) srsp * cy - cr * sy;
    out[5] = (float) srsp * sy + cr * cy;
    out[9] = (float) sr * cp;

    out[2] = (float) crsp * cy + sr * sy;
    out[6] = (float) crsp * sy - sr * cy;
    out[10] = (float) cr * cp;

    out[3] = out[7] = out[11] = 0.0;
    out[15] = 1.0;

    return out;
}

Mat4 Mat4::CreateScale(float x, float y, float z){
    Mat4 out;

    out[0] = x;
    out[5] = y;
    out[10] = z;
    out[15] = 1.0f;

    return out;
}

Mat4 Mat4::CreateTranslation(float x, float y, float z){
    Mat4 out;

    out[12] = x;
    out[13] = y;
    out[14] = z;
    out[15] = 1.0f;

    return out;
}


//Projection matrices
Mat4 Mat4::CreatePerspectiveProjection(float fovY, float aspect, float zNear, float zFar){
    float r = fovY * M_PI / 360.0;
    float deltaZ = zFar - zNear;
    float s = sin(r);
    float cotangent = 0;

    if (deltaZ == 0 || s == 0 || aspect == 0) {
        //return NULL;
        return Mat4();
    }

    //cos(r) / sin(r) = cot(r)
    cotangent = cos(r) / s;

    Mat4 out;

    out[0] = cotangent / aspect;
    out[5] = cotangent;
    out[10] = -(zFar + zNear) / deltaZ;
    out[11] = -1;
    out[14] = -2 * zNear * zFar / deltaZ;
    out[15] = 0;

    return out;
}

/** Creates an orthographic projection matrix like glOrtho */
Mat4 Mat4::CreateOrthographicProjection(float left, float right, float bottom, float top, float nearVal, float farVal){
    Mat4 out;
    float tx = -((right + left) / (right - left));
    float ty = -((top + bottom) / (top - bottom));
    float tz = -((farVal + nearVal) / (farVal - nearVal));

    out[0] = 2 / (right - left);
    out[5] = 2 / (top - bottom);
    out[10] = -2 / (farVal - nearVal);
    out[12] = tx;
    out[13] = ty;
    out[14] = tz;

    return out;
}

/**
 * Builds a translation matrix in the same way as gluLookAt()
 */

//Camera Setup
Mat4 Mat4::CreateLookAt(const Vec3& pEye, const Vec3& pCenter, const Vec3& pUp){
    Vec3 f, up, s, u;
    Mat4 translate, out;

    f = normalize(pCenter - pEye);

    up = normalize(pUp);

    s = normalize(f^up);

    u = normalize(s^f);

    out[0] = s.x;
    out[4] = s.y;
    out[8] = s.z;

    out[1] = u.x;
    out[5] = u.y;
    out[9] = u.z;

    out[2] = -f.x;
    out[6] = -f.y;
    out[10] = -f.z;

    translate = Mat4::CreateTranslation(-pEye.x, -pEye.y, -pEye.z);
    out = out.Multiply(translate);

    return out;
}

Mat4 Mat4::Inverse() const {
    float temp[4];
    float tmat[16];
    int i, j, k;

    for (i = 0; i < 16; i++){
        tmat[i] = this->n[i];
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
            temp[k] = pOut[i1 * 4 + k];
        }

        for (k = 0; k < 4; k++){
            pOut[i1 * 4 + k] = pOut[j * 4 + k];
            pOut[j * 4 + k] = temp[k];
        }

        // Scale row j to have a unit diagonal
        if (!tmat[j*4 + j]){
            // Singular matrix - can't invert
            //return NULL;
            return Mat4();
        }

        for (k = 0; k < 4; k++){
            pOut[j * 4 + k] /= tmat[j * 4 + j];
            tmat[j * 4 + k] /= tmat[j * 4 + j];
        }

        // Eliminate off-diagonal elems in col j of a, doing identical ops to b
        for (i = 0; i < 4; ++i){
            if (i != j){
                for (k = 0; k < 4; k++){
                    pOut[i*4 + k] -= tmat[i*4 + j] * pOut[j*4 + k];
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

    return (memcmp(identity, this->n, sizeof(float) * 16) == 0);
}

Mat4 Mat4::Transpose() const {
    int x, z;
    Mat4 pOut;

    for (z = 0; z < 4; ++z){
        for (x = 0; x < 4; ++x){
            pOut[(z * 4) + x] = n[(x * 4) + z];
        }
    }

    return pOut;
}

Mat4 Mat4::Multiply(const Mat4& other) const//returns this * other
{
    Mat4 out;

    const float *m1 = n, *m2 = other.n;

    out[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
    out[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
    out[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
    out[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

    out[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
    out[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
    out[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
    out[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

    out[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
    out[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
    out[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
    out[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

    out[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
    out[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
    out[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
    out[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

    return out;
}

bool Mat4::Equal(const Mat4& other, float threshold) const {
    int i = 0;

    for (i = 0; i < 16; ++i){
        if (!(n[i] + threshold > other.n[i] &&
                n[i] - threshold < other.n[i])){
            return false;
        }
    }

    return true;
}

Vec3 Mat4::GetUpVec3() const {
    Vec3 out;
    out.x = n[4];
    out.y = n[5];
    out.z = n[6];

    return out;
}

Vec3 Mat4::GetRightVec3() const {
    Vec3 out;
    out.x = n[0];
    out.y = n[1];
    out.z = n[2];

    return out;
}

Vec3 Mat4::GetForwardVec3() const {
    Vec3 out;
    out.x = n[8];
    out.y = n[9];
    out.z = n[10];

    return out;
}


//Transform a vector through this matrix:
Vec3 Mat4::Multiply(const Vec3& v) const {
    Vec3 out;
    out.x = n[0]*v.x + n[4]*v.y + n[8]*v.z;
    out.y = n[1]*v.x + n[5]*v.y + n[9]*v.z;
    out.z = n[2]*v.x + n[6]*v.y + n[10]*v.z;
    return out; //returns Vec3( this * Vec4(v,1.0) )
}

Vec4 Mat4::Multiply(const Vec4& v) const {
    Vec4 out;
    out.x = n[0]*v.x + n[4]*v.y + n[ 8]*v.z + n[12]*v.w;
    out.y = n[1]*v.x + n[5]*v.y + n[ 9]*v.z + n[13]*v.w;
    out.z = n[2]*v.x + n[6]*v.y + n[10]*v.z + n[14]*v.w;
    out.w = n[3]*v.x + n[7]*v.y + n[11]*v.z + n[15]*v.w;
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

