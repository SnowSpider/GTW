#ifndef __VEC3_HEADER__
#define __VEC3_HEADER__

#include <cmath>

class Vec3{
    public:
    float n[3];
    
    Vec3(){ 
        n[0] = 0.0; 
        n[1] = 0.0; 
        n[2] = 0.0;
    }
	Vec3( const float x, const float y, const float z ){ 
	    n[0] = x; 
	    n[1] = y; 
	    n[2] = z; 
	}
	Vec3( const Vec3& v ){ 
	    n[0] = v[0]; 
	    n[1] = v[1]; 
	    n[2] = v[2];
	}
	
	float& operator []( int i ){ 
	    return n[i];
	}
	
	float operator []( int i ) const { 
	    return n[i]; 
	}
    
    Vec3 operator-( const Vec3& v ) const { 
        return Vec3(n[0]-v[0], n[1]-v[1], n[2]-v[2]); 
    }
	
	Vec3 operator+( const Vec3& v ) const { 
	    return Vec3(v[0]+n[0], v[1]+n[1], v[2]+n[2]); 
	}
	
	const float* getPointer() const { 
	    return n; 
	}
	
	double length2() const { 
	    return n[0]*n[0] + n[1]*n[1] + n[2]*n[2];
	}
	
	double length() const { 
	    return sqrt( length2() );
	}

    void normalize() { 
		double len = length();
		n[0] /= len; n[1] /= len; n[2] /= len;
	}
	
	//bool iszero() { return ( (n[0]==0 && n[1]==0 && n[2]==0) ? true : false); };
	//void zeroElements() { memset(n,0,sizeof(float)*3); }
    
    float dot( const Vec3& v ) const {
        return n[0] * v[0] + n[1] * v[1] +n[2] * v[2];
    }
    
    Vec3 cross( const Vec3& v ) const {
        return Vec3( n[1] * v[2] - n[2] * v[1],
			         n[2] * v[0] - n[0] * v[2],
			         n[0] * v[1] - n[1] * v[0] );
    }
    
    float angle( const Vec3& v ) const {
        float s = sqrt(length2() * v.length2());
		//btFullAssert(s != btScalar(0.0));
		if(s == 0.0) exit(0);
		return acos(dot(v) / s);
    }
    
    Vec3 operator/( const float d ) {
	    Vec3 result;
	    for( int i=0; i<3; i++ ){
		    result[i] = n[i] / d;
	    }
	    return result;
    }
    
    Vec3 operator*( const float m ) {
	    Vec3 result;
	    for( int i=0; i<3; i++ ){
		    result[i] = n[i] * m;
	    }
	    return result;
    }
    
};

#endif
