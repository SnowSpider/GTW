#ifndef __Vec_H__
#define __Vec_H__

#include <iostream>

class Vec2{
    public:
    union {
        float n[2];
        struct{ float x,y;};
    };
    Vec2(); //default constructor
    Vec2( const float a, const float b ); //parametric constructor
    Vec2( const Vec2& v ); //copy constructor
    Vec2& operator=( const Vec2& v ); //copy-assinment operator
    //~Vec2(); //destructor
    
    float& operator []( int i ); //subscript
    float operator []( int i ) const;
    
    bool equals(const Vec2& v);
    void clear();
    
    Vec2 operator+( const Vec2& v ) const; //addition
    Vec2 operator-( const Vec2& v ) const; //subtraction
    Vec2 operator*( const float& m ) const; //multiplication
    Vec2 operator/( const float& d ); //division
    
    Vec2& operator+=( const Vec2& v ); //addition assignment
    Vec2& operator-=( const Vec2& v ); //subtraction assignment
    Vec2& operator*=( const float m ); //multiplication assignment
    Vec2& operator/=( const float d ); //division assignment
    
    double length2() const;
    double length() const;
    
};
Vec2 operator*(const float m, const Vec2& v);
std::ostream &operator<<(std::ostream &output, const Vec2 &v);

class Vec3{
    public:
    union {
        float n[3];
        struct{ float x,y,z;};
    };
    Vec3(); //default constructor
    Vec3( const float a, const float b, const float c ); //parametric constructor
    Vec3( const Vec3& v ); //copy constructor
    Vec3& operator=( const Vec3& v ); //copy-assinment operator
    //~Vec3(); //destructor
    
    float& operator []( int i ); //subscript
    float operator []( int i ) const;
    
    bool equals(const Vec3& v);
    void clear();
    
    Vec3 operator+( const Vec3& v ) const; //addition
    Vec3 operator-( const Vec3& v ) const; //subtraction
    Vec3 operator*( const float& m ) const; //multiplication
    Vec3 operator/( const float& d ); //division
    
    Vec3& operator+=( const Vec3& v ); //addition assignment
    Vec3& operator-=( const Vec3& v ); //subtraction assignment
    Vec3& operator*=( const float m ); //multiplication assignment
    Vec3& operator/=( const float d ); //division assignment
    
    double length2() const;
    double length() const;
    
    void normalize();
    float operator*( const Vec3& v ) const; //dot product
    Vec3 operator^( const Vec3& v ) const; //cross product
    float angle( const Vec3& v ) const;
    
    float min() const;
    float max() const;
    
    void rotateX(float t);
    void rotateY(float t);
    void rotateZ(float t);
};
Vec3 normalize(const Vec3& v);
float dot( const Vec3& a, const Vec3& b);
Vec3 cross( const Vec3& a, const Vec3& b );
Vec3 min(const Vec3& a, const Vec3& b);
Vec3 max(const Vec3& a, const Vec3& b);
Vec3 midpoint (Vec3& a, Vec3& b);
Vec3 midpoint (Vec3& a, Vec3& b, Vec3& c);
Vec3 operator*(const float m, const Vec3& v);
std::ostream &operator<<(std::ostream &output, const Vec3 &v);

class Vec4{
    public:
    union {
        float n[4];
        struct{ float x,y,z,w;};
    };
    Vec4(); //default constructor
    Vec4( const float a, const float b, const float c, const float d ); //parametric constructor
    Vec4( const Vec4& v ); //copy constructor
    Vec4& operator=( const Vec4& v ); //copy-assinment operator
    //~Vec4(); //destructor
    
    float& operator []( int i ); //subscript
    float operator []( int i ) const;
    
    bool equals(const Vec4& v);
    void clear();
    
    Vec4 operator+( const Vec4& v ) const; //addition
    Vec4 operator-( const Vec4& v ) const; //subtraction
    Vec4 operator*( const float& m ) const; //multiplication
    Vec4 operator/( const float& d ); //division
    
    Vec4& operator+=( const Vec4& v ); //addition assignment
    Vec4& operator-=( const Vec4& v ); //subtraction assignment
    Vec4& operator*=( const float m ); //multiplication assignment
    Vec4& operator/=( const float d ); //division assignment
    
    //double length2() const;
    //double length() const;
    
};
Vec4 operator*(const float m, const Vec4& v);
std::ostream &operator<<(std::ostream &output, const Vec4 &v);

#endif
