#ifndef DROW_UTILS_H
#define DROW_UTILS_H

#include "types.h"


#if !defined(GLUT_WHEEL_UP)
#  define GLUT_WHEEL_UP   3
#  define GLUT_WHEEL_DOWN 4
#endif


void ShowPoints();
void MovePoints();
void DrowAxis();
void keypress(unsigned char key, int x, int y);
void reshape(int Width, int Height);
void keypress(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void rotate(int x, int y);




#endif // DROW_UTILS_H
