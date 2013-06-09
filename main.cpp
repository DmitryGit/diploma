#include <GLUT/glut.h>
#include <iostream>
//#include <malloc.h>
#include "utils.h"
#include "drow_utils.h"


const char usage[] = "Usage:\n\nprogram [OPTIONS] <filename>\n\n\
    -h ALTITUDE - altitude\n\
    -r RADIUS - radius of field around body\n\
        -d - draw\n";
//    -a - draw axes\n\

int main (int argc, char * argv[])
{
    double H = 0;
    double radius = 1;
    char *filename = NULL;
    int c;
    bool drow = true;
    while ((c = getopt (argc, argv, ":h:d:r:")) != -1) {
        switch(c) {
        case 'h':
            H = strtold(optarg,NULL);
            break;
        case 'd':
            drow = true;
            break;
        case 'r':
            radius = strtold(optarg,NULL);
            break;
        case '?':
        default:
            break;
        }
    }
    if (optind == argc) {
        exit(1);
    }
    filename = argv[optind];
    init(filename, H, radius);

    if (drow){
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
        glutInitWindowSize(WindowWidth, WindowHeight);
        glutInitWindowPosition(700, 700);
        glutCreateWindow("MRO_13");
        glutDisplayFunc(display);
        glutIdleFunc(display);
        glutReshapeFunc(reshape);
        glutKeyboardFunc(keypress);
        glutMouseFunc(mouse);
        glutMotionFunc(rotate);
        glClearColor(1, 1, 1, 0.0);
        glMatrixMode(GL_MODELVIEW);
        glutMainLoop();
    }
	return 0;
}
