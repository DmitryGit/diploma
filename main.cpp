#include <GLUT/glut.h>
#include <iostream>
//#include <malloc.h>
#include "utils.h"
#include "drow_utils.h"




int main (int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitWindowPosition(350, 200);
    glutCreateWindow("Tie Fighter");
	glutDisplayFunc(display);
    glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keypress);
	glutMouseFunc(mouse);
	glutMotionFunc(rotate);



    init(WindowWidth, WindowHeight, argv);
	glutMainLoop();
	return 0;
}
