#ifndef __VEC3_HEADER__
#define __VEC3_HEADER__

class Vec3{
    public:
    //float x, y, z;
    float n[3];
    
    Vec3();
	Vec3( const float x, const float y, const float z );
	Vec3( const Vec3& v );
	float& operator []( int i );
	float operator []( int i ) const;
    bool equals(const Vec3& v);
    Vec3 operator-( const Vec3& v ) const;
	Vec3 operator+( const Vec3& v ) const;
	const float* getPointer() const;
	double length2() const;
	double length() const;
    Vec3 normalize();
    float dot( const Vec3& v ) const;
    Vec3 cross( const Vec3& v ) const;
    float operator*( const Vec3& v ) const;
    Vec3 operator^( const Vec3& v ) const;
    float angle( const Vec3& v ) const;
    Vec3 operator/( const float d );
    Vec3 operator*( const float m ) const;
    void setValue( const float x, const float y, const float z );
    void setValue( const Vec3& v );
    Vec3 min(const Vec3& a, const Vec3& b);
	Vec3 max(const Vec3& a, const Vec3& b);
	float min() const;
	float max() const;
	void rotateX(float t);
	void rotateY(float t);
	void rotateZ(float t);
	    
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
