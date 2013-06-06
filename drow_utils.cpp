#include <GLUT/glut.h>
#include "utils.h"
#include "drow_utils.h"


void DrowAxis(){
    glColor3d(0,255,0);
    glBegin(GL_LINES);
    glVertex3f(-5000,0,0);
    glVertex3f(5000,0,0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0,-5000,0);
    glVertex3f(0,5000,0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0,0,-5000);
    glVertex3f(0,0,5000);
    glEnd();
}


void rotate(int x, int y){
    static int mouseOldX;
    static int mouseOldY;
    if (((x - mouseOldX) > 0)&&((y - mouseOldY) > 0)) {
        rotX +=10;
        rotY +=10;
    }
    if (((x - mouseOldX) < 0)&&((y - mouseOldY) < 0)) {
        rotX -=10;
        rotY -=10;
    }
    if (((x - mouseOldX) > 0)&&((y - mouseOldY) < 0)) {
        rotX +=10;
        rotY -=10;
    }
    if (((x - mouseOldX) < 0)&&((y - mouseOldY) < 0)) {
        rotX -= 1;
        rotY -= 1;
    }
    if (((x - mouseOldX) < 0)&&((y - mouseOldY) > 0)) {
        rotX -= 1;
        rotY += 1;
    }
    if (((x - mouseOldX) < 0)&&((y - mouseOldY) == 0)) {
        rotY -= 1;
    }
    if (((x - mouseOldX) > 0)&&((y - mouseOldY) == 0)) {
        rotY += 1;
    }
    mouseOldX = x;
    mouseOldY = y;
    glutPostRedisplay();
}


void reshape(int Width, int Height){
    if (Height == 0){ Height = 1;}
    glViewport(0, 0, Width, Height);
    //# параметры изображения объектов в зависимости от размеров окна:
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); //# восстанавливаем координаты мира
    gluPerspective(45.0, Width/Height, 0.1, 10000000.0);
    glMatrixMode(GL_MODELVIEW);
}



void keypress(unsigned char key, int x, int y) {

    switch (key){
    case '\033':
        exit(0);
        break;
    case 'a':
        rotY += 10;
        glutPostRedisplay();
        break;
    case 'd':
        rotY -= 10;
        glutPostRedisplay();
        break;
    case 'w':
        rotX  += 10;
        glutPostRedisplay();
        break;
    case 's':
        rotX  -= 10;
        glutPostRedisplay();
        break;
    case 'z':
        ZOOM +=100;
        glutPostRedisplay();
        break;
    case 'x':
        ZOOM -=100;
        glutPostRedisplay();
        break;
    }
}

void mouse(int button, int state, int x, int y) {
    if (state == GLUT_UP ){
        if (button == GLUT_WHEEL_UP) {
            ZOOM +=100;
            glutPostRedisplay ();
        }
        else if (button == GLUT_WHEEL_DOWN) {
            ZOOM -=100;
            glutPostRedisplay ();
        }
    }
}
