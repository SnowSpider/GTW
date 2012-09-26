#ifndef __VEC3_HEADER__
#define __VEC3_HEADER__

#include <cmath>
#include <iostream>
using namespace std;

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

    Vec3 normalize() { 
		double len = length();
		n[0] /= len; n[1] /= len; n[2] /= len;
		return *this;
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
    
    float operator*( const Vec3& v ) const {
	    return n[0] * v[0] + n[1] * v[1] +n[2] * v[2];
    }
    
    Vec3 operator^( const Vec3& v ) const {
	    return Vec3( n[1] * v[2] - n[2] * v[1],
			         n[2] * v[0] - n[0] * v[2],
			         n[0] * v[1] - n[1] * v[0] );
    }
    
    float angle( const Vec3& v ) const {
        float s = sqrt(length2() * v.length2());
		//btFullAssert(s != btScalar(0.0));
		/*
		if(s == 0.0){
		    cout << "n = (" << n[0] << ", "
		                    << n[1] << ", "
		                    << n[2] << ")" << endl;
		    cout << "v = (" << v[0] << ", "
		                    << v[1] << ", "
		                    << v[2] << ")" << endl << endl;
		    return 666;
		}
		*/
		//exit(0);
		return acos(dot(v) / s);
    }
    
    Vec3 operator/( const float d ) {
	    Vec3 result;
	    for( int i=0; i<3; i++ ){
		    result[i] = n[i] / d;
	    }
	    return result;
    }
    
    Vec3 operator*( const float m ) const {
	    Vec3 result;
	    for( int i=0; i<3; i++ ){
		    result[i] = n[i] * m;
	    }
	    return result;
    }
    
    void setValue( const float x, const float y, const float z ){
        n[0] = x; 
	    n[1] = y; 
	    n[2] = z;
    }
    
    void setValue( const Vec3& v ){ 
	    n[0] = v[0]; 
	    n[1] = v[1]; 
	    n[2] = v[2];
	}
    
    static Vec3 min(const Vec3& a, const Vec3& b){
		Vec3 result;
		for(int i=0;i<3;i++){
			result[i]=(a[i]<b[i]?a[i]:b[i]);
		}
		return result;
	}
	
	static Vec3 max(const Vec3& a, const Vec3& b){
		Vec3 result;
		for(int i=0;i<3;i++){
			result[i]=(a[i]>b[i]?a[i]:b[i]);
		}
		return result;
	}
	
	float min() const {
		float result=n[0];
		for(int i=0;i<3;i++){
			result=(result<n[i]?result:n[i]);
		}
		return result;
	}

	float max() const { 
		float result=n[0];
		for(int i=0;i<3;i++){
			result=(result>n[i]?result:n[i]);
		}
		return result;
	}
    
};

/*
Vec3 operator*(const float& s, const Vec3& v) {
    return v * s; 
}

ostream & operator<<(ostream & out, const Vec3& v) {
	out << "("; 
	for(int i=0;i<3;i++)
	{	
		out << v[i];
		if(i<3-1) out << ",";
	}
	out << ")";
	return out;
}
*/

#endif
