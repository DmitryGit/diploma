#include "utils.h"
#include "drow_utils.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstring>
#include <assert.h>
#include <algorithm>
#include <cmath>
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <fstream>


Triangle *triangles;
Point * points;
Line * lines;
Point MAX_POINT;
Point MIN_POINT;
Point ORIGIN_POINT;

double POINT_MASS = 2.7*pow(real(10),real(-27));//4,6502 х 10-26
//double POINT_MASS = 4.6502*pow(real(10),real(-26));
namespace Constans {
    double P0 = 101325.0; //стандартное атмосферное давление на уровне моря (Па)
    double T0 = 288.15; //стандартная температура на уровне моря (K)
    double g = 9.80665; //ускорение свободного падения над поверхностью Земли (m/sec2)
    double L = 0.0065; //скорость падения температуры с высотой, в пределах тропосферы (L/m)
    double R = 8.31447; // универсальная газовая постоянная Дж⁄(Мол·K)
    double M = 0.0289644; //молярная масса сухого воздуха кг⁄Мол
    double T = 250; //температура в стратосфере (−56,5 - 0,8 °С) (К)
    double k = 1.38006488*pow(real(10),real(-23)); //постоянноая Больцмана (Дж/К)
    double n0 = 2.7*pow(real(10),real(25)); //кол-во молекул газа в единице обьема при НУ
    double h0 = 1000; //высота над уровнет моря при НУ ?????
    double pi = 3.14;
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
int POINTS_NUMBER = 10000;
int POINT_SPEED = 3874; //первая космичческая (м/c)
int POINTS_RADIUS = 2;



double getConcentration (double H) {
    double n, V;

    n = Constans::n0*exp(-POINT_MASS*Constans::g*(H-Constans::h0)/Constans::k*Constans::T); //  кол-во молекул в м3 на высоте H

    V = 4/3*Constans::pi*POINTS_RADIUS*POINTS_RADIUS*POINTS_RADIUS;
    cout<<"N: "<<-POINT_MASS*Constans::g*(H-Constans::h0)/Constans::k*Constans::T<<endl<<"V: "<<V<<endl;
    cout<<setprecision(220)<<"n*V: "<<n*V;
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
        m.print();
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
    return (sqrt(pow(p.x - s.center.x,real(2)) + pow(p.y - s.center.y,real(2)) + pow(p.z - s.center.z,real(2))) < s.radius);
}

void MovePoints(){
    for (int i=0; i< POINTS_NUMBER; i++){
        Sphere body_sphare(ORIGIN_POINT, getDistanceBetweenPoints(MAX_POINT,ORIGIN_POINT));
        Sphere big_sphape(ORIGIN_POINT, POINTS_RADIUS);
        if (!(isPointInSphare(lines[i].set[0], big_sphape)) || (isPointInSphare(lines[i].set[0], body_sphare))){
//        if ((isPointInSphare(lines[i].set[0], body_sphare))){
            Line *tmp = GenerateLine(GenerateRandomPoint(big_sphape,body_sphare),GenerateRandomPoint(big_sphape,body_sphare));
            memcpy(lines + i,tmp ,sizeof(Line));
            delete tmp;
//            cout<<"Delete point"<<endl;
        }
        lines[i].set[0] = lines[i].set[0]+lines[i].directionVector*0.001;


//        lines[i].set[1] = lines[i].set[1]+lines[i].directionVector;
//        lines[i].set[0].x -=0.01*lines[i].directionVector.x;
//        lines[i].set[0].y -=0.01*lines[i].directionVector.y;
//        lines[i].set[0].z -=0.01*lines[i].directionVector.z;
//        lines[i].set[1].x -=0.01*lines[i].directionVector.x;
//        lines[i].set[1].y -=0.01*lines[i].directionVector.y;
//        lines[i].set[1].z -=0.01*lines[i].directionVector.z;

    }
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
    cout<<triangles;
    MAX_POINT = maxPoint();
    MIN_POINT = minPoint();

}

void init(double Width, double Height, char *filename)
{
    srand((unsigned)time(NULL));
    int altitude =1117;
    double distance = 3;
//    read_file_assimp(filename);
    read_file(filename);

    POINTS_RADIUS = calculatePointsRadius(distance);
    POINTS_NUMBER = getConcentration(altitude);
    cout<<endl<<endl<<endl<<"Radius"<< POINTS_RADIUS <<endl<<endl;

//    cout<<endl<<endl<<rand()<<endl<<time(NULL);
    cout<<endl<<endl<<endl<<endl<<"Points Number: "<<POINTS_NUMBER<<endl;
    glClearColor(1, 1, 1, 0.0);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    // включается освещение:
    // включается нулевой источник света (всего их 8):
    // включается управление материалом (цвет и отражающие способности) предметов при помощи функции glColor:
    glEnable(GL_COLOR_MATERIAL);
    // параметры изображения объектов в зависимости от размеров окна:
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();// # восстанавливаем координаты мира
    gluPerspective(180, Width/Height, 0.1, 10000.0);
    glMatrixMode(GL_MODELVIEW);

    GenerateLines();
}

void display(){   
    Sphere body_sphare(ORIGIN_POINT, getDistanceBetweenPoints(MAX_POINT,ORIGIN_POINT));
    double current_impulse;
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
    //# цвет тела, параметры RGBA:
    glColor4f(0.0, 0.7, 0.1, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // см. выше
//    DrowAxis();
//    glBegin(GL_TRIANGLES);			// рисуем треугольники

    glBegin(GL_LINES);			// рисуем треугольники
    for (int i=0;i<TRIANGLES_NUMBER;i++)
    {
        glColor3d(0,1,0);      // цвет треугольника
        glVertex3f(triangles[i].a.x, triangles[i].set[0].y, triangles[i].set[0].z);
        glVertex3f(triangles[i].set[1].x, triangles[i].set[1].y, triangles[i].set[1].z);
        glVertex3f(triangles[i].set[2].x, triangles[i].set[2].y, triangles[i].set[2].z);
    }
    glEnd();

    // подсчет количества столкновений с молекулами
    for (int i=0; i<POINTS_NUMBER; i++){
        if (isPointInSphare(lines[i].set[0], body_sphare)) {
            for (int j=0; j<TRIANGLES_NUMBER; j++){
                if (doesLineIntersectTriangle(triangles[j], lines[i])){
                    all_count++;
                    count++;
//                    cout<<"Point intersect"<<endl;
//                    cout<<"triangle"<<endl;
//                    triangles[j].print();
                }
            }
        }
    }

    if (!(step%100)){
        current_impulse = all_count*pow(real(POINT_SPEED),real(2))*POINT_MASS;
        cout.setf( ios::fixed);
//        cout<<setprecision (22)<<"Impuls on current step "<<": "<<current_impulse<<endl;
//        cout<<"points on current step "<<": "<<all_count<<endl;
        all_count = 0;
    }
    MovePoints();
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

Line* GenerateLine(Point a, Point b){
    return new Line(a, b);
}

void GenerateLines(){
    Point a,b;
    Sphere body_sphare(ORIGIN_POINT, getDistanceBetweenPoints(MAX_POINT,ORIGIN_POINT));
    Sphere big_sphape(ORIGIN_POINT, POINTS_RADIUS);
    Vector dir_v;

    lines = (Line*)malloc(POINTS_NUMBER*sizeof(Line));
    for (int i=0; i<POINTS_NUMBER; i++){
//        a = GenerateRandomPoint(big_sphape, body_sphare);
//        b = GenerateRandomPoint(big_sphape, body_sphare);
        a = getRandomPointOnSphere(big_sphape);
        b = getRandomPointOnSphere(big_sphape);
        dir_v = Vector(a,b);
        Line *tmp = GenerateLine(a,dir_v);
        memcpy(lines + i,tmp ,sizeof(Line));
        delete tmp;
    }
}


