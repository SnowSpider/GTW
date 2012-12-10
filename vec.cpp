#include <math.h>
#include "vec.h"
#include "mat.h"

using namespace std;

Vec2::Vec2(){ //default constructor
    x = 0.0;
    y = 0.0;
}
Vec2::Vec2( const float a, const float b ){ //parametric constructor
    x = a;
    y = b;
}
Vec2::Vec2( const Vec2& v ){ //copy constructor
    x = v.x;
    y = v.y;
}
Vec2& Vec2::operator=( const Vec2& v ){ //copy-assinment operator
    x = v.x;
    y = v.y;
    return *this;
}
/*
Vec2::~Vec2(){ //destructor
    delete[] n;
}
*/
float& Vec2::operator []( int i ){ //subscript
    return n[i];
}
float Vec2::operator []( int i ) const {
    return n[i];
}
bool Vec2::equals(const Vec2& v){
    if(x==v.x && y==v.y) return true;
    else return false;
}
void Vec2::clear(){
    x = 0.0;
    y = 0.0;
}
Vec2 Vec2::operator+( const Vec2& v ) const { //addition
    return Vec2(x+v.x, y+v.y);
}
Vec2 Vec2::operator-( const Vec2& v ) const { //subtraction
    return Vec2(x-v.x, y-v.y);
}
Vec2 Vec2::operator*( const float& m ) const { //multiplication
    Vec2 ret;
    ret.x = x*m;
    ret.y = y*m;
    return ret;
}
Vec2 operator*(const float m, const Vec2& v) {
    return v * m;
}
Vec2 Vec2::operator/( const float& d ){ //division
    Vec2 ret;
    ret.x = x/d;
    ret.y = y/d;
    return ret;
}
Vec2& Vec2::operator+=( const Vec2& v ){
    x += v.x;
    y += v.y;
    return *this;
}
Vec2& Vec2::operator-=( const Vec2& v ){
    x -= v.x;
    y -= v.y;
    return *this;
}
Vec2& Vec2::operator*=( const float m ){
    x *= m;
    y *= m;
    return *this;
}
Vec2& Vec2::operator/=( const float d ){
    x /= d;
    y /= d;
    return *this;
}
double Vec2::length2() const { 
    return x*x + y*y;
}
double Vec2::length() const { 
    return sqrt( length2() );
}
std::ostream &operator<<(std::ostream &output, const Vec2 &v){
    output << '(' << v[0] << ", " << v[1] << ')';
    return output;
}



Vec3::Vec3(){ //default constructor
    x = 0.0;
    y = 0.0;
    z = 0.0;
}
Vec3::Vec3( const float a, const float b, const float c ){ //parametric constructor
    x = a;
    y = b;
    z = c;
}
Vec3::Vec3( const Vec3& v ){ //copy constructor
    x = v.x;
    y = v.y;
    z = v.z;
}
Vec3& Vec3::operator=( const Vec3& v ){ //copy-assinment operator
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}
/*
Vec3::~Vec3(){ //destructor
    delete[] n;
}
*/
float& Vec3::operator []( int i ){ //subscript
    return n[i];
}
float Vec3::operator []( int i ) const {
    return n[i];
}
bool Vec3::equals(const Vec3& v){
    if(x==v.x && y==v.y && z==v.z) return true;
    else return false;
}
void Vec3::clear(){
    x = 0.0;
    y = 0.0;
    z = 0.0;
}
Vec3 Vec3::operator+( const Vec3& v ) const { //addition
    return Vec3(x+v.x, y+v.y, z+v.z);
}
Vec3 Vec3::operator-( const Vec3& v ) const { //subtraction
    return Vec3(x-v.x, y-v.y, z-v.z);
}
Vec3 Vec3::operator*( const float& m ) const { //multiplication
    Vec3 ret;
    ret.x = x*m;
    ret.y = y*m;
    ret.z = z*m;
    return ret;
}
Vec3 operator*(const float m, const Vec3& v) {
    return v * m;
}
Vec3 Vec3::operator/( const float& d ){ //division
    Vec3 ret;
    ret.x = x/d;
    ret.y = y/d;
    ret.z = z/d;
    return ret;
}
Vec3& Vec3::operator+=( const Vec3& v ){ //addition assignment
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}
Vec3& Vec3::operator-=( const Vec3& v ){ //subtraction assignment
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}
Vec3& Vec3::operator*=( const float m ){ //multiplication assignment
    x *= m;
    y *= m;
    z *= m;
    return *this;
}
Vec3& Vec3::operator/=( const float d ){ //division assignment
    x /= d;
    y /= d;
    z /= d;
    return *this;
}
double Vec3::length2() const { 
    return x*x + y*y + z*z;
}
double Vec3::length() const { 
    return sqrt( length2() );
}
void Vec3::normalize(){
    double len = length();
    x /= len;y /= len;z /= len;
}
Vec3 normalize(const Vec3& v){
    Vec3 temp(v);
    temp.normalize();
    return temp;
}
float Vec3::operator*( const Vec3& v ) const { //dot product
    return x * v.x + y * v.y + z * v.z;
}
Vec3 Vec3::operator^( const Vec3& v ) const { //cross product
    return Vec3( y * v.z - z * v.y,
                 z * v.x - x * v.z,
                 x * v.y - y * v.x );
}
float dot( const Vec3& a, const Vec3& b ){
    return a*b;
}
Vec3 cross( const Vec3& a, const Vec3& b ){
    return a^b;
}
float Vec3::angle( const Vec3& v ) const {
    float s = sqrt(length2() * v.length2());
    //if(s == 0.0) cout << "FFFFFUUUUUUUU-" << endl; //exit(0);
    return acos((*this*v) / s); //expressed in radians
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
Vec3 min(const Vec3& a, const Vec3& b){
    Vec3 result;
    for(int i=0;i<3;i++){
        result[i]=(a[i]<b[i]?a[i]:b[i]);
    }
    return result;
}
Vec3 max(const Vec3& a, const Vec3& b){
    Vec3 result;
    for(int i=0;i<3;i++){
        result[i]=(a[i]>b[i]?a[i]:b[i]);
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
Vec3 midpoint (Vec3& a, Vec3& b){
    Vec3 midpoint = (a + b) * 0.5;
    return midpoint;
}
Vec3 midpoint (Vec3& a, Vec3& b, Vec3& c){
    Vec3 midpoint = (a + b + c)/3.0;
    return midpoint;
}
std::ostream &operator<<(std::ostream &output, const Vec3 &v){
    output << '(' << v[0] << ", " << v[1] << ", " << v[2] << ')';
    return output;
}




Vec4::Vec4(){ //default constructor
    x = 0.0;
    y = 0.0;
    z = 0.0;
    w = 0.0;
}
Vec4::Vec4( const float a, const float b, const float c, const float d ){ //parametric constructor
    x = a;
    y = b;
    z = c;
    w = d;
}
Vec4::Vec4( const Vec4& v ){ //copy constructor
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
}
Vec4& Vec4::operator=( const Vec4& v ){ //copy-assinment operator
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return *this;
}
/*
Vec4::~Vec4(){ //destructor
    delete[] n;
}
*/
float& Vec4::operator []( int i ){ //subscript
    return n[i];
}
float Vec4::operator []( int i ) const {
    return n[i];
}
bool Vec4::equals(const Vec4& v){
    if(x==v.x && y==v.y && z==v.z && w==v.w) return true;
    else return false;
}
void Vec4::clear(){
    x = 0.0;
    y = 0.0;
    z = 0.0;
    w = 0.0;
}
Vec4 Vec4::operator+( const Vec4& v ) const { //addition
    return Vec4(x+v.x, y+v.y, z+v.z, w+v.w);
}
Vec4 Vec4::operator-( const Vec4& v ) const { //subtraction
    return Vec4(x-v.x, y-v.y, z-v.z, w-v.w);
}
Vec4 Vec4::operator*( const float& m ) const { //multiplication
    Vec4 ret;
    ret.x = x*m;
    ret.y = y*m;
    ret.z = z*m;
    ret.w = w*m;
    return ret;
}
Vec4 operator*(const float m, const Vec4& v) {
    return v * m;
}
Vec4 Vec4::operator/( const float& d ){ //division
    Vec4 ret;
    ret.x = x/d;
    ret.y = y/d;
    ret.z = z/d;
    ret.w = w/d;
    return ret;
}
Vec4& Vec4::operator+=( const Vec4& v ){ //addition assignment
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}
Vec4& Vec4::operator-=( const Vec4& v ){ //subtraction assignment
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}
Vec4& Vec4::operator*=( const float m ){ //multiplication assignment
    x *= m;
    y *= m;
    z *= m;
    w *= m;
    return *this;
}
Vec4& Vec4::operator/=( const float d ){ //division assignment
    x /= d;
    y /= d;
    z /= d;
    w /= d;
    return *this;
}
std::ostream &operator<<(std::ostream &output, const Vec4 &v){
    output << '(' << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ')';
    return output;
}


