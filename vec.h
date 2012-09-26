#ifndef __Vec3_H__
#define __Vec3_H__

class Vec3{
    public:
    float x, y, z;
    //float n[3];
    
    Vec3();
	Vec3( const float a, const float b, const float c );
	Vec3( const Vec3& v );
	Vec3& operator=( const Vec3& v );
	float& operator []( int i );
	float operator []( int i ) const;
    bool equals(const Vec3& v);
    Vec3 operator-( const Vec3& v ) const;
	Vec3 operator+( const Vec3& v ) const;
	Vec3& operator+=( const Vec3& v );
	Vec3& operator-=( const Vec3& v );
    Vec3& operator*=( const float m );
    Vec3& operator/=( const float d );
	//const float* getPointer() const;
	double length2() const;
	double length() const;
    Vec3 normalize();
    Vec3 normalized() const;
    float dot( const Vec3& v ) const;
    Vec3 cross( const Vec3& v ) const;
    float operator*( const Vec3& v ) const;
    Vec3 operator^( const Vec3& v ) const;
    float angle( const Vec3& v ) const;
    Vec3 operator/( const float d );
    Vec3 operator*( const float m ) const;
    void setValue( const float a, const float b, const float c );
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

class Vec4 {
    public:
    float x, y, z, w;

    Vec4(float a, float b, float c, float d);
    Vec4();
    Vec4(const Vec3& v, float w);

    float Length() const;

    //compute the dot product which is cos(alpha) * this.Length * a.Length
    //where alpha is the (smaller) angle between the vectors
    float Dot(const Vec4& a) const;
    Vec4 Subtract(const Vec4& a) const;
    Vec4 Add(const Vec4& a) const;
};

const Vec4 operator+(const Vec4& lhs, const Vec4& rhs);
const Vec4 operator-(const Vec4& lhs, const Vec4& rhs);
const float operator*(const Vec4& lhs, const Vec4& rhs);


#endif
