#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <cmath>
#include <cstring>
typedef double real;

struct Vector;
struct Point;
struct Triangle;
struct Sphere;

using namespace std;

template <unsigned int T>
struct Locus { // collection of points
    Point set[T];
    friend ostream& operator<<(ostream &os, const Locus &l) {
        for(unsigned int i = 0;i < T - 1;i++)
            os << l.set[i] << ", ";
        os << l.set[T - 1];
        return os;
    }
};



struct Point {
    real x;
    real y;
    real z;


    Point(): x(0),y(0),z(0) {}
    Point(real _x, real _y, real _z): x(_x),y(_y),z(_z) {}
    Point(const Point &p) {x = p.x;y = p.y;z = p.z;}

    Point operator+(Vector v);
    Point operator-(Vector v);
    friend ostream& operator<<(ostream &os, const Point &p) {
        os << '(' << p.x << ',' << p.y << ',' << p.z << ')';
        return os;
    }
    bool operator==(Point a) const {
        return (x == a.x && y == a.y && z == a.z);
    }
    bool operator!=(Point b) const {
        return !(*this == b);
    }
    bool operator>=(Point a) const {
        return (x >= a.x && y >= a.y && z >= a.z);
    }
    bool operator<=(Point a) const {
        return (x <= a.x && y <= a.y && z <= a.z);
    }

    Point operator+(int n) const {
        return Point(x+n, y+n, z+n);
    }

    void print() {
        std::cout<<'('<<x<<';'<<y<<';'<<z<<')'<<std::endl;
    }
    bool isInArea(Point Min, Point Max) {
        return ((x <= Max.x && y <= Max.y && z <= Max.z) && (x >= Min.x && y >= Min.y && z >= Min.z));
    }
};

struct Sphere {
    Point center;
    int radius;
    Sphere(): center(), radius(0) {}
    Sphere(Point _p, real _r): center(_p), radius(_r) {}
    Sphere(const Sphere &_s): center(_s.center), radius(_s.radius) {}
};

struct Vector: public Point {
    Vector(): Point() {}
    Vector(Point p): Point(p) {}
    Vector(real _x, real _y, real _z): Point(_x,_y,_z) {}
    Vector(Point b,Point a): Point(a.x - b.x,a.y - b.y,a.z - b.z) {}

    real operator*(Vector right) {
        return x*right.x + y*right.y + z*right.z;
    }
    Vector operator*(double k) {
        return Vector(k*x,k*y,k*z);
    }
    Vector operator/(double k) {
        return Vector(x/k,y/k,z/k);
    }
    Vector operator+(Vector v) {
        return Vector(x + v.x,y + v.y, z + v.z);
    }
    Vector operator-(Vector v) {
        return Vector(x - v.x,y - v.y, z - v.z);
    }
    real length() {
        return sqrt(x*x + y*y + z*z);
    }
    double cos(Vector right) {
        return ((*this)*right)/(this->length()*right.length());
    }
    Vector vectorProduct(Vector left) {
        return Vector(y*left.z - z*left.y, -x*left.z +
                      z*left.x, x*left.y - y*left.x);
    }
    Vector normalized() {
        double len = length();
       ////// assert(len != 0);
        return Vector(x/len,y/len,z/len);
    }
    Vector resized(real _length) {
        double coef = _length/length();
        if (std::isnan(coef))
            return Vector(x,y,z);
        return Vector(x*coef,y*coef,z*coef);
    }
    void resize(real _length) {
        double coef = _length/length();
        if (!std::isnan(coef)) {
            x *= coef;
            y *= coef;
            z *= coef;
        }
    }
};



struct Triangle : public Locus<3> {
    Triangle(): a(set[0]), b(set[1]), c(set[2]) {}
    Triangle(const Triangle &tP): a(set[0]), b(set[1]), c(set[2]) {
        set[0] = tP.set[0]; set[1] = tP.set[1]; set[2] = tP.set[2];
    }
    Triangle(Point _a,Point _b,Point _c): a(set[0]), b(set[1]), c(set[2]) {
        set[0] = _a; set[1] = _b; set[2] = _c;
    }
    Triangle& operator=(const Triangle& right) {
        if (this != &right) {
            memcpy(set, right.set, 3*sizeof(Point));
        }
        return *this;
    }

    virtual Vector getNormal() {
        return Vector(a,b).vectorProduct(Vector(a,c) );
    }

    Point centerOfMass() {
        return Point( (a.x + b.x + c.x) / 3.0,
                      (a.y + b.y + c.y) / 3.0,
                      (a.z + b.z + c.z) / 3.0);
    }

    double area() {
        Vector ab(a,b), ac(a,c);
        return 0.5*sqrt(1 - pow(ab.cos(ac),2))*ab.length()*ac.length();
    }
    Point &a;
    Point &b;
    Point &c;

    double color;
    void print() {
        a.print();
        b.print();
        c.print();
    }
};

struct Line: public Locus<2> {
    Vector directionVector;
    Line(Point _a,Point _b): a(set[0]), b(set[1]) {
        set[0] = _a; set[1] = _b;
        directionVector = Vector(_a,_b);
    }
    Line(Point _a,Vector v): a(set[0]), b(set[1]) {
        directionVector = v;
        set[0] = _a;
        set[1] = _a + v;
    }
    Line& operator=(const Line& right) {
        if (this != &right) {
            memcpy(set, right.set, 2*sizeof(Point));
        }
        return *this;
    }


    Point& a;
    Point& b;
    Point pointByCoef(real coef) {
        return Point(a.x + coef*directionVector.x,
                     a.y + coef*directionVector.y,
                     a.z + coef*directionVector.z);
    }
};







#endif
