#include <GLUT/glut.h>
#include <iostream>
//#include <malloc.h>
#include "utils.h"
#include "drow_utils.h"


const char usage[] = "Usage:\n\nprogram [OPTIONS] <filename>\n\n\
    -A ALTITUDE - altitude\
    -r RADIUS - radius of generative sphere [not used]\n\
    -s TIME - time to sleep in microseconds\n\
    -m - model particles\n\
    -o NUM - modeling type; 1, 2 or 3\n\
        1 - modeling without field affecting\n\
        2 - (default) most optimized modeling\n\
        3 - modeling best applicable for drawing\n\
    -v - verbose mode\n\
    -d - draw\n\
    -j - draw trajectories of particles\n\
    -a - draw axes\n\
    -c CHARGE - initial charge of spacecraft (default -0.0000005)\n\
    -f SF - scale factor for coordinates in file to reduce them to SI\n\
        (default 1)\n\
    -n N - total number of particles at time momentn\n\
    -i INTERVAL - interval to print measurings\n\
        (use 'i' prefix for number of steps or 's' for seconds)\n\
    -p STEP - step of particle measured in spacecrafts length\n\
        (default 0.25)\n\
    -x - file with complex data format\n\
    -h NUM - number of posix threads (default 1)\n";


int main (int argc, char * argv[])
{
    int altitude = 0;
    char *filename = NULL;
    int c;
    while ((c = getopt (argc, argv, ":vdjamxgt:A:s:f:t:n:i:p:o:c:h:")) != -1) {
        switch(c) {
        case 'A':
            altitude = strtold(optarg,NULL);
            break;
        case 'h':
            break;
        case 'i':
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

    std::cout<<"ALtitude"<<altitude<<endl;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(WindowWidth, WindowHeight);
    glutInitWindowPosition(500, 500);
    glutCreateWindow("Tie Fighter");
	glutDisplayFunc(display);
    glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keypress);
	glutMouseFunc(mouse);
	glutMotionFunc(rotate);

    init(WindowWidth, WindowHeight, filename);
	glutMainLoop();
	return 0;
}
