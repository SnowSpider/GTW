#include <math.h>
#include "vec.h"
#include "mat.h"

using namespace std;

Vec3::Vec3(){ 
    x = 0.0; 
    y = 0.0; 
    z = 0.0;
}
Vec3::Vec3( const float a, const float b, const float c ){ 
    x = a; 
    y = b; 
    z = c; 
}
Vec3::Vec3( const Vec3& v ){ 
    x = v.x; 
    y = v.y; 
    z = v.z;
}
Vec3& Vec3::operator=( const Vec3& v ){
    x = v.x; 
    y = v.y; 
    z = v.z;
}

float& Vec3::operator []( int i ){ 
    return n[i];
}
    
float Vec3::operator []( int i ) const { 
    return n[i];
}
    
bool Vec3::equals(const Vec3& v) {
    if(x==v.x && y==v.y && z==v.z) return true;
    else return false;
}
    
Vec3 Vec3::operator-( const Vec3& v ) const { 
    return Vec3(x-v.x, y-v.y, z-v.z); 
}
    
Vec3 Vec3::operator+( const Vec3& v ) const { 
    return Vec3(v.x+x, v.y+y, v.z+z); 
}

Vec3& Vec3::operator+=( const Vec3& v ){
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vec3& Vec3::operator-=( const Vec3& v ){
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vec3& Vec3::operator*=( const float m ){
    x *= m;
    y *= m;
    z *= m;
    return *this;
}

Vec3& Vec3::operator/=( const float d ){
    x /= d;
    y /= d;
    z /= d;
    return *this;
}

/*
const float* Vec3::getPointer() const { 
    return n; 
}
*/
double Vec3::length2() const { 
    return x*x + y*y + z*z;
}
    
double Vec3::length() const { 
    return sqrt( length2() );
}

Vec3 Vec3::normalize() { 
    double len = length();
    x /= len; y /= len; z /= len;
    return *this;
}

Vec3 Vec3::normalized() const { 
    double len = length();
    return Vec3(x/len, y/len, z/len);
}
    
//bool iszero() { return ( (x==0 && y==0 && z==0) ? true : false); };
//void zeroElements() { memset(n,0,sizeof(float)*3); }
    
float Vec3::dot( const Vec3& v ) const {
    return x * v.x + y * v.y +z * v.z;
}
    
Vec3 Vec3::cross( const Vec3& v ) const {
    return Vec3( y * v.z - z * v.y,
                 z * v.x - x * v.z,
                 x * v.y - y * v.x );
}
    
float Vec3::operator*( const Vec3& v ) const {
    return x * v.x + y * v.y + z * v.z;
}
    
Vec3 Vec3::operator^( const Vec3& v ) const {
    return Vec3( y * v.z - z * v.y,
                 z * v.x - x * v.z,
                 x * v.y - y * v.x );
}
    
float Vec3::angle( const Vec3& v ) const {
    float s = sqrt(length2() * v.length2());
    //btFullAssert(s != btScalar(0.0));
    //if(s == 0.0) cout << "FFFFFUUUUUUUU-" << endl;//exit(0);
    return acos(dot(v) / s); //expressed in radians
}
    
Vec3 Vec3::operator/( const float d ) {
    Vec3 ret;
    ret.x = x/d;
    ret.y = y/d;
    ret.z = z/d;
    return ret;
}
    
Vec3 Vec3::operator*( const float m ) const {
    Vec3 ret;
    ret.x = x*m;
    ret.y = y*m;
    ret.z = z*m;
    return ret;
}
    
void Vec3::setValue( const float a, const float b, const float c ){
    x = a; 
    y = b; 
    z = c;
}
    
void Vec3::setValue( const Vec3& v ){ 
    x = v.x; 
    y = v.y; 
    z = v.z;
}
    
Vec3 Vec3::min(const Vec3& a, const Vec3& b){
    Vec3 result;
    for(int i=0;i<3;i++){
        result[i]=(a[i]<b[i]?a[i]:b[i]);
    }
    return result;
}
    
Vec3 Vec3::max(const Vec3& a, const Vec3& b){
    Vec3 result;
    for(int i=0;i<3;i++){
        result[i]=(a[i]>b[i]?a[i]:b[i]);
    }
    return result;
}
    
float Vec3::min() const {
    float n[3];
    n[0] = x;
    n[1] = y;
    n[2] = z;
    float result=x;
    for(int i=0;i<3;i++){
        result=(result<n[i]?result:n[i]);
    }
    return result;
}

float Vec3::max() const { 
    float n[3];
    n[0] = x;
    n[1] = y;
    n[2] = z;
    float result=x;
    for(int i=0;i<3;i++){
        result=(result>n[i]?result:n[i]);
    }
    return result;
}

void Vec3::rotateX(float t){
    Mat3 rotmat(1, 0, 0,
                0, cos(t), -sin(t),
                0, sin(t), cos(t));
    Vec3 temp = rotmat * (*this);
    x = temp[0];
    y = temp[1];
    z = temp[2];
    //return rotmat * (*this);
}
    
void Vec3::rotateY(float t){
    Mat3 rotmat(cos(t), 0, sin(t),
                0, 1, 0,
                -sin(t), 0, cos(t));
    Vec3 temp = rotmat * (*this);
    x = temp[0];
    y = temp[1];
    z = temp[2];
    //return rotmat * (*this);
}
    
void Vec3::rotateZ(float t){
    Mat3 rotmat(cos(t), -sin(t), 0,
                sin(t), cos(t), 0,
                0, 0, 1);
    Vec3 temp = rotmat * (*this);
    x = temp[0];
    y = temp[1];
    z = temp[2];
    //return rotmat * (*this);
}

Vec3 operator*(const float& s, const Vec3& v) {
    return v * s; 
}

Vec3 midpoint (Vec3& a, Vec3& b){
    Vec3 midpoint = (a + b) * 0.5;
    return midpoint;
}

Vec3 midpoint (Vec3& a, Vec3& b, Vec3& c){
    Vec3 midpoint = (a + b + c)/3.0;
    return midpoint;
}

ostream &operator<<(ostream &output, const Vec3 &v) {
	cout << '(' << v[0] << ", " << v[1] << ", " << v[2] << ')';
	return output;
}

Vec4::Vec4(float a, float b, float c, float d){
    x = a;
    y = b;
    z = c;
    w = d;
}

Vec4::Vec4(){
    x = y = z = w = 0.0;
}

Vec4::Vec4(const Vec3& v, float w){
    x = v.x;
    y = v.y;
    z = v.z;
    this->w = w;
}

float Vec4::Length() const {
    return sqrt(x*x + y*y + z*z + w*w);
}

float Vec4::Dot(const Vec4& a) const {
    return x*a.x + y*a.y + z*a.z + w*a.w;
}


Vec4 Vec4::Subtract(const Vec4& a) const {
    Vec4 result = *this;
    result.x -= a.x;
    result.y -= a.y;
    result.z -= a.z;
    result.w -= a.w;
    return result;
}

Vec4 Vec4::Add(const Vec4& a) const {
    Vec4 result = *this;
    result.x += a.x;
    result.y += a.y;
    result.z += a.z;
    result.w += a.w;
    return result;
}

const Vec4 operator+(const Vec4& lhs, const Vec4& rhs){
    return lhs.Add(rhs);
}

const Vec4 operator-(const Vec4& lhs, const Vec4& rhs){
    return lhs.Subtract(rhs);
}

const float operator*(const Vec4& lhs, const Vec4& rhs){
    return lhs.Dot(rhs);
}


