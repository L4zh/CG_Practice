
#include <cstdio>
#include <cstdlib>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#define TrianglePoint 1000

GLfloat v[3][2] = {
    {-1.0, -0.58},
    {1.0, -0.58},
    {1.0, 1.15}
};

int n;

void Triangle(GLfloat *a, GLfloat *b, GLfloat *c)
{
    glVertex2fv(a);
    glVertex2fv(b);
    glVertex2fv(c);
}

void DivideTriangle(GLfloat *a, GLfloat *b, GLfloat *c, int m)
{
    GLfloat v0[2], v1[2], v2[2];
    int j;
    if (m > 0)
    {
        for (j = 0; j < 2; j++)
            v0[j] = (a[j] + b[j]) / 2;
        for (j = 0; j < 2; j++)
            v1[j] = (a[j] + c[j]) / 2;
        for (j = 0; j < 2; j++)
            v2[j] = (b[j] + c[j]) / 2;
        DivideTriangle(a, v0, v1, m - 1);
        DivideTriangle(c, v1, v2, m - 1);
        DivideTriangle(b, v2, v0, m - 1);
    }
    else
        Triangle(a, b, c);
}

void Display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    DivideTriangle(v[0], v[1], v[2], n);
    glEnd();
    glFlush();
}

void MyInit()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-2.0, 2.0, -2.0, 2.0);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);
}

void Display_Points()
{
    int j, k;
    int rand();
    GLfloat p[2] = {0.0, 1.0};
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBegin(GL_POINTS);
    for (k = 0; k < TrianglePoint * n; k++)
    {
        j = rand() % 3;
        p[0] = (p[0] + v[j][0]) / 2.0;
        p[1] = (p[1] + v[j][1]) / 2.0;
        glVertex2fv(p);
    }
    glEnd();
    glFlush();
}

void DrawMenu(int index)
{
    switch (index)
    {
        case 1:
            Display_Points();
            break;
        case 2:
            Display();
            break;
        default:
            break;
    }
}

void MousePlot(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
    if ((action == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON))
    {
        printf("xMouse: %d\nyMouse: %d\n\n", xMouse, yMouse);
    }
}

void MyKey(unsigned char key, int x, int y)
{
    if (key == 27 || key == 'q' || key == 'Q')
        exit(0);
    else if (key >= '1' && key <= '9')
    {
        n = key - 48;
        Display();
    }
}

int main(int argc, char **argv)
{
    n = 5;
    
    /*
     init and create window
     */
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(800, 800);
    glutCreateWindow("2D Gasket");
    glutDisplayFunc(Display);
    
    MyInit();
    
    /*
     add menu entry
     */
    glutCreateMenu(DrawMenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutAddMenuEntry("点画", 1);
    glutAddMenuEntry("线画", 2);
    
    /*
     mouse operation:
     */
    glutMouseFunc(MousePlot);
    
    /*
     key board operation
     */
    glutKeyboardFunc(MyKey);
    
    glutMainLoop();
    return 0;
}
