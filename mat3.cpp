#include <cmath>
#include <iostream>
#include "vec3.h"
#include "mat3.h"

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


