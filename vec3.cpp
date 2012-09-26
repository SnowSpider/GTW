#include <cmath>
#include <iostream>
#include "vec3.h"
#include "mat3.h"

using namespace std;

Vec3::Vec3(){ 
    n[0] = 0.0; 
    n[1] = 0.0; 
    n[2] = 0.0;
}
Vec3::Vec3( const float x, const float y, const float z ){ 
    n[0] = x; 
    n[1] = y; 
    n[2] = z; 
}
Vec3::Vec3( const Vec3& v ){ 
    n[0] = v[0]; 
    n[1] = v[1]; 
    n[2] = v[2];
}
	
float& Vec3::operator []( int i ){ 
    return n[i];
}
	
float Vec3::operator []( int i ) const { 
    return n[i]; 
}
    
bool Vec3::equals(const Vec3& v) {
    if(n[0]==v[0] && n[1]==v[1] && n[2]==v[2]) return true;
    else return false;
}
    
Vec3 Vec3::operator-( const Vec3& v ) const { 
    return Vec3(n[0]-v[0], n[1]-v[1], n[2]-v[2]); 
}
	
Vec3 Vec3::operator+( const Vec3& v ) const { 
    return Vec3(v[0]+n[0], v[1]+n[1], v[2]+n[2]); 
}
	
const float* Vec3::getPointer() const { 
    return n; 
}
	
double Vec3::length2() const { 
    return n[0]*n[0] + n[1]*n[1] + n[2]*n[2];
}
	
double Vec3::length() const { 
    return sqrt( length2() );
}

Vec3 Vec3::normalize() { 
	double len = length();
	n[0] /= len; n[1] /= len; n[2] /= len;
	return *this;
}
	
//bool iszero() { return ( (n[0]==0 && n[1]==0 && n[2]==0) ? true : false); };
//void zeroElements() { memset(n,0,sizeof(float)*3); }
    
float Vec3::dot( const Vec3& v ) const {
    return n[0] * v[0] + n[1] * v[1] +n[2] * v[2];
}
    
Vec3 Vec3::cross( const Vec3& v ) const {
    return Vec3( n[1] * v[2] - n[2] * v[1],
			     n[2] * v[0] - n[0] * v[2],
			     n[0] * v[1] - n[1] * v[0] );
}
    
float Vec3::operator*( const Vec3& v ) const {
    return n[0] * v[0] + n[1] * v[1] + n[2] * v[2];
}
    
Vec3 Vec3::operator^( const Vec3& v ) const {
    return Vec3( n[1] * v[2] - n[2] * v[1],
			     n[2] * v[0] - n[0] * v[2],
			     n[0] * v[1] - n[1] * v[0] );
}
    
float Vec3::angle( const Vec3& v ) const {
    float s = sqrt(length2() * v.length2());
	//btFullAssert(s != btScalar(0.0));
	//if(s == 0.0) cout << "FFFFFUUUUUUUU-" << endl;//exit(0);
	return acos(dot(v) / s); //expressed in radians
}
    
Vec3 Vec3::operator/( const float d ) {
    Vec3 result;
    for( int i=0; i<3; i++ ){
	    result[i] = n[i] / d;
    }
    return result;
}
    
Vec3 Vec3::operator*( const float m ) const {
    Vec3 result;
    for( int i=0; i<3; i++ ){
	    result[i] = n[i] * m;
    }
    return result;
}
    
void Vec3::setValue( const float x, const float y, const float z ){
    n[0] = x; 
    n[1] = y; 
    n[2] = z;
}
    
void Vec3::setValue( const Vec3& v ){ 
    n[0] = v[0]; 
    n[1] = v[1]; 
    n[2] = v[2];
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
	float result=n[0];
	for(int i=0;i<3;i++){
		result=(result<n[i]?result:n[i]);
	}
	return result;
}

float Vec3::max() const { 
	float result=n[0];
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
    n[0] = temp[0];
    n[1] = temp[1];
    n[2] = temp[2];
    //return rotmat * (*this);
}
	
void Vec3::rotateY(float t){
    Mat3 rotmat(cos(t), 0, sin(t),
                0, 1, 0,
                -sin(t), 0, cos(t));
    Vec3 temp = rotmat * (*this);
    n[0] = temp[0];
    n[1] = temp[1];
    n[2] = temp[2];
    //return rotmat * (*this);
}
	
void Vec3::rotateZ(float t){
    Mat3 rotmat(cos(t), -sin(t), 0,
                sin(t), cos(t), 0,
                0, 0, 1);
    Vec3 temp = rotmat * (*this);
    n[0] = temp[0];
    n[1] = temp[1];
    n[2] = temp[2];
    //return rotmat * (*this);
}
	
    
