#include "utils.h"
#include "drow_utils.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <fstream>
#include <unistd.h>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <limits>
#include <pthread.h>
#include <signal.h>


#define min(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

Triangle *triangles;
Point * points;
Line * lines;
Point MAX_POINT;
Point MIN_POINT;
Point ORIGIN_POINT;

double POINT_MASS = 46.6502*pow(real(10),real(-26));//4,6502 х 10-26
double POINT_MASS_MOLAR = 28;//4,6502 х 10-26
//double POINT_MASS = 4.6502*pow(real(10),real(-26));
namespace Constants {
    double P0 = 101325.0; //стандартное атмосферное давление на уровне моря (Па)
//    double P0 = 101325.0; //стандартное атмосферное давление на уровне моря (Па)
    double T0 = 288.15; //стандартная температура на уровне моря (K)
    double g = 9.80665; //ускорение свободного падения над поверхностью Земли (m/sec2)
    double L = 0.0065; //скорость падения температуры с высотой, в пределах тропосферы (L/m)
    double R = 8.31447; // универсальная газовая постоянная Дж⁄(Мол·K)
    double M = 0.0289644; //молярная масса сухого воздуха кг⁄Мол
    double T = 250; //температура в стратосфере (−56,5 - 0,8 °С) (К)
    double k = 1.38006488*pow(real(10),real(-23)); //постоянноая Больцмана (Дж/К)
    double n0 = 2.7*pow(real(10),real(25)); //кол-во молекул газа в единице обьема при НУ
    double h0 = 0; //высота над уровнет моря при НУ ?????
    double m0 = POINT_MASS;
    double pi = 3.14159265359;
}
double rotX = 0;
double rotY = 0;
double rotZ = 0;
double ZOOM = -20.0;
double moveX = 0.0;
double moveY = 0.0;
int WindowWidth = 640;
int WindowHeight = 480;
int points_area = 1000;
int TRIANGLES_NUMBER;
unsigned int POINTS_NUMBER = 10000;
int POINT_SPEED = 8000;
double POINTS_RADIUS;



double getConcentration (double H) {
    using namespace Constants;
    double n, V;
    double a = m0*g*H;
    double b = k*T;
    n = n0*exp(-a/b); //  кол-во молекул в м3 на высоте H
    V = 4/3*pi*pow(POINTS_RADIUS,real(3));//*POINTS_RADIUS*POINTS_RADIUS;
//    cout<<"N: "<<n<<endl<<"V: "<<V<<endl;
    cout<<"Concentration (in m3): "<<n<<endl;
    cout<<"V: "<<V<<endl;

    return n*V;
}

double calculatePointsRadius(double distance) {
    return getDistanceBetweenPoints(MAX_POINT,ORIGIN_POINT) + distance;
}

Point maxPoint(){
    Point m;
    m = triangles[0].a;
    for(int i=0;i<TRIANGLES_NUMBER;i++){
        if (triangles[i].a.distanceToOriginPoint()>=getDistanceBetweenPoints(m, ORIGIN_POINT)){m = triangles[i].a;}
        if (triangles[i].b.distanceToOriginPoint()>=getDistanceBetweenPoints(m, ORIGIN_POINT)){m = triangles[i].b;}
        if (triangles[i].c.distanceToOriginPoint()>=getDistanceBetweenPoints(m, ORIGIN_POINT)){m = triangles[i].c;}
    }
    return m;
}

Point Point::operator+(Vector v) {
    return Point(x + v.x,y + v.y, z + v.z);
}

Point Point::operator-(Vector v) {
    return Point(x - v.x,y - v.y, z - v.z);
}


Point minPoint(){
    Point m;
    m = triangles[0].a;
    for(int i=0;i<TRIANGLES_NUMBER;i++){
        if (triangles[i].a<=m){m = triangles[i].a;}
        if (triangles[i].b<=m){m = triangles[i].b;}
        if (triangles[i].c<=m){m = triangles[i].c;}
    }
    return m;
}

bool isPointInSphare(Point p, Sphere s) {
    return (p.distanceToOriginPoint() < s.radius);
}


void movePointsArray(void *_args){
    pair<Line*,unsigned long long> *args = (pair<Line*,unsigned long long>*)_args;
    Line *points = args->first;
    int num = args->second;
    cout<<"in thread!";
    for (int i=0; i< num; ++i){
        Sphere big_sphape(ORIGIN_POINT, POINTS_RADIUS);
        if (!isPointInSphare(points[i].set[0], big_sphape)){
            Line *tmp = GenerateRandomLine();
            memcpy(lines + i,tmp ,sizeof(Line));
            delete tmp;
        }
        points[i].set[0] = points[i].set[0]+points[i].directionVector*0.5;
    }


}

void movePoints(){
    for (int i=0; i< POINTS_NUMBER; i++){
        Sphere big_sphape(ORIGIN_POINT, POINTS_RADIUS);
        if (!isPointInSphare(lines[i].set[0], big_sphape)){
            Line *tmp = GenerateRandomLine();
            memcpy(lines + i,tmp ,sizeof(Line));
            delete tmp;
        }
        lines[i].set[0] = lines[i].set[0]+lines[i].directionVector*0.1;

    }
//    int threadNum = 2;
//    if(threadNum == 1) {
////        pair<Particle*,unsigned long long> args(particles, electronsNumber + ionsNumber);
////        processParticlesArray(&args);
//    } else {
//        pthread_t *threads = new pthread_t[threadNum];


//        int pointsPerThread = ceil(1.0*(POINTS_NUMBER)/threadNum);
//        int firtsPointForCurrentThread = 0;
//        int threadsStarted = 0;
////        COUT("---------------------------------------------------------------------------");
////        COUT("num = " << electronsNumber+ionsNumber);
//        pair<Line*,int> **threadArgs = new pair<Line*,int>*[threadNum];
//        for(;threadsStarted < threadNum;++threadsStarted) {
//            if (firtsPointForCurrentThread >= POINTS_NUMBER)
//                break;
//            threadArgs[threadsStarted] =
//                    new pair<Point*,int>(lines + firtsPointForCurrentThread,
//                                                           min(POINTS_NUMBER - firtsPointForCurrentThread,(int)pointsPerThread));
//            assert(pthread_create(threads + threadsStarted,NULL,movePointsArray,threadArgs[threadsStarted]) == 0);
//            firtsPointForCurrentThread += pointsPerThread;
//        }

//        // wait for all threads
//        for(int t = 0;t < threadsStarted;++t)
//            pthread_join(threads[t], NULL);

//        // clean threads args
//        for(int t = 0;t < threadsStarted;++t)
//            delete threadArgs[t];

//        delete threadArgs;
//        delete threads;
//    }
}

void ShowPoints(){
    glColor3d(0,0,1);
    for (int i=0; i<POINTS_NUMBER; i++){
        glBegin(GL_POINTS);
        glVertex3f(lines[i].set[0].x , lines[i].set[0].y,lines[i].set[0].z);
        glEnd();
    }
}

void read_file(char* filename)
{
    filebuf fb;
    Point a, b, c;
    int i=0;
    istream fileInputStream(&fb);
    TRIANGLES_NUMBER = 0;
    triangles = new Triangle[10000000];

    if (!fb.open(filename,ios::in)) {
        cerr << "An error occurred while opening file" << endl;
        return;
    }

    while(!fileInputStream.eof()) {
        // read 3 points
        fileInputStream >> a.x >> a.y >> a.z
                        >> b.x >> b.y >> b.z
                        >> c.x >> c.y >> c.z;
        triangles[i] = Triangle(a,b,c);
        triangles[i].color = 0;
        i++;
        TRIANGLES_NUMBER++;
        if (fileInputStream.fail()) {
            if (!fileInputStream.eof())
                fileInputStream.clear();
            break;
        }
    }
        // scipping remaining characters in current string
        while (!fileInputStream.eof() && fileInputStream.get() != '\n');
    fb.close();
    MAX_POINT = maxPoint();
    MIN_POINT = minPoint();

    return;
}

void read_file_assimp(char* file_name)
{
    Point a, b, c;
    Assimp::Importer importer;
    aiScene *aiscene = (aiScene*)importer
        .ReadFile(file_name,aiProcess_Triangulate|aiProcess_FixInfacingNormals|aiProcess_FindDegenerates
                  |aiProcess_PreTransformVertices|aiProcess_OptimizeMeshes|aiProcess_FindInvalidData|aiProcess_RemoveRedundantMaterials);
    if (aiscene == NULL) {
        cerr << "An error occurred while opening file: "<< file_name << endl;
        return;
    }

    for(unsigned int i = 0;i < aiscene->mNumMeshes;++i) {
        if (aiscene->mMeshes[i]->HasFaces())
            TRIANGLES_NUMBER+=aiscene->mMeshes[i]->mNumFaces;
    }
    triangles = new Triangle[TRIANGLES_NUMBER];
    for(unsigned int i = 0, k = 0;i < aiscene->mNumMeshes;++i) {
        if (aiscene->mMeshes[i]->HasFaces()) {
            for(unsigned int j = 0;j < aiscene->mMeshes[i]->mNumFaces;++j) {
                    a.x = aiscene->mMeshes[i]->mVertices[aiscene->mMeshes[i]->mFaces[j].mIndices[0]].x;
                    a.y = aiscene->mMeshes[i]->mVertices[aiscene->mMeshes[i]->mFaces[j].mIndices[0]].y;
                    a.z = aiscene->mMeshes[i]->mVertices[aiscene->mMeshes[i]->mFaces[j].mIndices[0]].z;
                    b.x = aiscene->mMeshes[i]->mVertices[aiscene->mMeshes[i]->mFaces[j].mIndices[1]].x;
                    b.y = aiscene->mMeshes[i]->mVertices[aiscene->mMeshes[i]->mFaces[j].mIndices[1]].y;
                    b.z = aiscene->mMeshes[i]->mVertices[aiscene->mMeshes[i]->mFaces[j].mIndices[1]].z;
                    c.x = aiscene->mMeshes[i]->mVertices[aiscene->mMeshes[i]->mFaces[j].mIndices[2]].x;
                    c.y = aiscene->mMeshes[i]->mVertices[aiscene->mMeshes[i]->mFaces[j].mIndices[2]].y;
                    c.z = aiscene->mMeshes[i]->mVertices[aiscene->mMeshes[i]->mFaces[j].mIndices[2]].z;
                    triangles[k] = Triangle(a,b,c);
                    ++k;
            }
        }
    }
//    cout<<triangles;
    MAX_POINT = maxPoint();
    MIN_POINT = minPoint();

}

void init(char *filename, double height, double radius)
{
    srand((unsigned)time(NULL));
    read_file_assimp(filename);
//    read_file(filename);

    POINTS_RADIUS = calculatePointsRadius(radius);
    POINTS_NUMBER = getConcentration(height);
    cout<<endl<<endl<<endl<<"Radius "<< POINTS_RADIUS <<endl<<endl;

    cout<<endl<<endl<<endl<<endl<<"Points Number: "<<POINTS_NUMBER<<endl;

    GenerateLines();
}

void display(){   
    Sphere body_sphare(ORIGIN_POINT, getDistanceBetweenPoints(MAX_POINT,ORIGIN_POINT));
    static int index;
    static double current_impulse;
    static unsigned int all_count=0;
    unsigned int count=0;
    static int step;
    step++;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// # очищаем экран
    glLoadIdentity(); // # восстанавливаем координаты мира
    //# перемещаем систему координат в точку, заданную параметрами X, Y, Z:
    glTranslatef(moveX, moveY, ZOOM);
    //# вращаем тело вокруг вектора, заданного тремя параметрами:
    glRotatef(rotX, 1.0, 0.0, 0.0);// # вокруг оси X
    glRotatef(rotY, 0.0, 1.0, 0.0);// # вокруг оси Y
    glRotatef(rotZ, 0.0, 0.0, 1.0);// # вокруг оси Z
    glBegin(GL_LINES);
    glColor3d(0,1,0);      // цвет треугольника
    for (int i=0;i<TRIANGLES_NUMBER;i++)
    {
        for (int j=0; j<3; j++){
            glVertex3f(triangles[i].set[j].x, triangles[i].set[j].y, triangles[i].set[j].z);
        }
    }
    glEnd();
    // подсчет количества столкновений с молекулами
    for (int i=0; i<POINTS_NUMBER; i++){
        if (isPointInSphare(lines[i].set[0], body_sphare)) {
            for (int j=0; j<TRIANGLES_NUMBER; j++){
                if (doesLineIntersectTriangle(triangles[j], lines[i])){
                    all_count++;
                    count++;
                    triangles[j].color = 1;
                    Line *tmp = GenerateRandomLine();
                    memcpy(lines + i,tmp ,sizeof(Line));
                    delete tmp;
                    break;
                }
            }
        }
    }
    if (!(step%20)){
        current_impulse = all_count*pow(real(POINT_SPEED),real(2))*POINT_MASS*100000000000000;
//        impulse += current_impulse;
        cout.setf( ios::fixed);
//        cout<<setprecision (22)<<"Impuls on current step "<<": "<<current_impulse<<endl;
        cout<<setprecision (22)<<current_impulse<<endl;
        index++;
//        cout<<"current step "<<": "<<step<<endl;
        all_count = 0;
    }
    movePoints();
    ShowPoints();
    glutSwapBuffers();
}

bool isPointInsideTriangle(Triangle &t,Point k) {
    Vector v0(Point(0,0,0)), v1(k,t.a), v2(k,t.b), v3(k,t.c);
    if (v1 == v0 || v2 == v0 || v3 == v0)
        return true;
    real cos1 = v1.cos(v2), cos2 = v1.cos(v3), cos3 = v2.cos(v3);
    switch( sign(cos1) + sign(cos2) + sign(cos3) ) {
    case -3:
        return true;
    case 3:
    case 1:
        return false;
    default:
        // delta is required to prevent mashine imprecision
        real delta = 0.0001;
        return !(acos(cos1) + acos(cos2) + acos(cos3) < M_PI*2 - delta);
    }
}

// коэффициент точки пересечения находим из условия перпендикулярности напрвляющего вектора
// прямой и вектора, образованного заданной точкой и ее проекией (в качестве координат последней
// берем параметрические уравнения прямой)
Point getPointOnLineProjection(Line line,Point point) {
    real coef = line.directionVector*Vector(line.a,point) /
            (line.directionVector*line.directionVector);
    return line.pointByCoef(coef);
}

// коэффициент точки пересечения находим подставляя параметрические уравнения прямой
// в векторное уравнение плоскости
Point getPlaneAndLineIntersection2(Triangle &plane,Line line) {
    double coef = plane.getNormal()*Vector(line.a,plane.a) /
            (plane.getNormal()*line.directionVector);
    return line.pointByCoef(coef);
}

bool isPointInsideTriangle2(Triangle &t,Point k) {
    Point p = getPointOnLineProjection(Line(t.a,t.b),t.c);
    if (Vector(p,t.c).cos(Vector(p,k)) < 0)
        return false;
    p = getPointOnLineProjection(Line(t.c,t.b),t.a);
    if (Vector(p,t.a).cos(Vector(p,k)) < 0)
        return false;
    p = getPointOnLineProjection(Line(t.c,t.a),t.b);
    if (Vector(p,t.b).cos(Vector(p,k)) < 0)
        return false;
    return true;
}

bool doesLineIntersectTriangle(Triangle &triangle,Line line) {
    Point intersection = getPlaneAndLineIntersection2(triangle,line);
//    if (isinf(intersection.x)) {
//        cerr << "INF" << endl;
//        return false;
//    }
//    if (isnan(intersection.x)) {
//        cerr << "NAN" << endl;
//        return false;
//    }
    bool retVal = isPointInsideTriangle2(triangle,intersection);
//    cout<<retVal;
    return retVal;
}

double getDistanceBetweenPoints(Point a,Point b) {
    return sqrt(pow(a.x - b.x,2) + pow(a.y - b.y,2) + pow(a.z - b.z,2));
}

Point GenerateRandomPoint(Sphere S, Sphere s) {
    Point P;
    do {
        P.x = (rand()*2.0/RAND_MAX - 1)*S.radius;
        P.y = (rand()*2.0/RAND_MAX - 1)*S.radius;
        P.z = (rand()*2.0/RAND_MAX - 1)*S.radius;
    } while(getDistanceBetweenPoints(P,ORIGIN_POINT) < s.radius);
    return P;
}

double getRandom(){
    return rand()*1.0/RAND_MAX;
}

Point getRandomPointOnSphere(Sphere s) {
    return s.center + Vector(getRandom() - 0.5,getRandom() - 0.5,getRandom() - 0.5)
            .resized(s.radius);
}

Point getRandomBetweenSpheres(Sphere S, Sphere s) {
    Point P;
    do {
        P.x = (rand()*2.0/RAND_MAX - 1)*S.radius;
        P.y = (rand()*2.0/RAND_MAX - 1)*S.radius;
        P.z = (rand()*2.0/RAND_MAX - 1)*S.radius;
    } while(getDistanceBetweenPoints(P,ORIGIN_POINT) < s.radius);
    return P;
}

Line* GenerateLine(Point a, Point b){
    return new Line(a, b);
}

Line* GenerateLine(Point a, Vector v){
    return new Line(a, v);
}

Line* GenerateRandomLine(){
    Point a,b;
    Vector dir_v;
    Sphere body_sphare(ORIGIN_POINT, getDistanceBetweenPoints(MAX_POINT,ORIGIN_POINT));
    Sphere big_sphape(ORIGIN_POINT, POINTS_RADIUS);
    a = getRandomPointOnSphere(big_sphape);
    b = getRandomPointOnSphere(big_sphape);
    dir_v = Vector(a,b);
    dir_v = dir_v/dir_v.length();
    return GenerateLine(a,dir_v);
//    return GenerateLine(getRandomBetweenSpheres(big_sphape, body_sphare),dir_v);
}

void GenerateLines(){
    lines = (Line*)malloc(POINTS_NUMBER*sizeof(Line));
    for (int i=0; i<POINTS_NUMBER; i++){
        Line *tmp = GenerateRandomLine();
        memcpy(lines + i,tmp ,sizeof(Line));
        delete tmp;
    }
}


