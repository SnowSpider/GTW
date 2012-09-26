#ifndef __MAT3_HEADER__
#define __MAT3_HEADER__

class Mat3{
    public:
    float m[3][3]; 
    /* Column-major order
    M = [ m[0] m[3] m[6] ]
        [ m[1] m[4] m[7] ]
        [ m[2] m[5] m[8] ]
    */
    
    Mat3();
	Mat3( const float m0, const float m1, const float m2,
	      const float m3, const float m4, const float m5,
          const float m6, const float m7, const float m8 );
	float* operator []( int i );
	Vec3 operator*( const Vec3& v ) const;
};

#endif
