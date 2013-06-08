#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include <GLUT/glut.h>
//#include <GLUT/glu.h>
//#include <malloc.h>
#include "drow_utils.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstring>
#include <assert.h>
#include <algorithm>
#include <cmath>

#include <stdlib.h>
#include <time.h>
//#include <string.h>
//#include <assimp/Importer.hpp> // C++ importer interface
//#include <assimp/scene.h> // Output data structure
//#include <assimp/postprocess.h> // Post processing flags

//#define POINTS_NUMBER 100000


extern double rotX;
extern double rotY;
extern double rotZ;
extern double ZOOM;
extern double moveX;
extern double moveY;
extern int mouseOldX;
extern int mouseOldY;
extern int TRIANGLES_NUMBER;
extern int WindowWidth;
extern int WindowHeight;
extern int points_area;

using namespace std;

template <typename T>
inline char sign(T t) {
    return (t > 0)? 1: (t < 0)? -1: 0;
}

void init(double Width, double Height, char *filename);
void display();
void read_file(char* file_name);
void read_file_assimp(char* file_name);


Point GenerateRandomPoint(Sphere S, Sphere s);
Point getPlaneAndLineIntersection2(Triangle &plane,Line line);
Line* GenerateLine(Point a, Point b);
void GenerateLines();
Point maxPoint();
Point minPoint();
bool isPointInSphare(Point p, Sphere s);
void MovePoints();
void ShowPoints();

bool doesLineIntersectTriangle(Triangle &triangle,Line line);
bool isPointInsideTriangle2(Triangle &t,Point k);
bool isPointInsideTriangle(Triangle &t,Point k);
Point getPointOnLineProjection(Line line,Point point);
Point getPlaneAndLineIntersection2(Triangle &plane,Line line);
bool isPointInsideTriangle2(Triangle &t,Point k);
bool doesLineIntersectTriangle(Triangle &triangle,Line line);
double getDistanceBetweenPoints(Point a,Point b);
Line* GenerateLine(Point a, Point b);
void GenerateLines();

double getDistanceBetweenPoints(Point a,Point b);

#endif // UTILS_H

