
#define GLUT_DISABLE_ATEXIT_HACK

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <ctime>

#include <openGL/glut.h>

#define M_PI 3.14159
#define bool int
#define false 0
#define true 1

// mode 0 = wire frame, mode 1 = constant shading,
// mode 3 = interpolative shading


typedef float point[4];

point v[] =
{
    {  0.0, 0.0, 1.0 },
    {  0.0, 0.942809, -0.33333 },
    { -0.816497, -0.471405, -0.333333 },
    {  0.816497, -0.471405, -0.333333 }
};

int n;
int mode;

int winWidth = 500, winHeight = 500;
bool redrawContinue = false;
float lastPos[3] = { 0.0f, 0.0f, 0.0f };
float angle = 0.0;

GLfloat axis[3] = { 0.0, 0.0, 0.1 };

int curX, curY;
int startX, startY;


void triangle(point a, point b, point c)
{
    if (mode == 0)
        glBegin(GL_LINE_LOOP);
    else
        glBegin(GL_POLYGON);
    
    if (mode == 1)
        glNormal3fv(a);
    
    if (mode == 2)
        glNormal3fv(a);
    
    glVertex3fv(a);
    
    if (mode == 2)
        glNormal3fv(b);
    
    glVertex3fv(b);
    
    if (mode == 2)
        glNormal3fv(c);
    
    glVertex3fv(c);
    
    glEnd();
}


// 规范化
void normal(point p)
{
    float d = 0.0;
    int i;
    for (i = 0; i < 3; i++)
        d += p[i] * p[i];
    
    d = sqrtf(d);
    
    if (d > 0.0)
        for (i = 0; i < 3; i++)
            p[i] /= d;
}


// 三角形分形
void divide_triangle(point a, point b, point c, int m)
{
    point v1, v2, v3;
    int j;
    if (m > 0)
    {
        for (j = 0; j < 3; j++) v1[j] = a[j] + b[j];
        normal(v1);
        for (j = 0; j < 3; j++) v2[j] = a[j] + c[j];
        normal(v2);
        for (j = 0; j < 3; j++) v3[j] = b[j] + c[j];
        normal(v3);
        divide_triangle(a, v1, v2, m - 1);
        divide_triangle(c, v2, v3, m - 1);
        divide_triangle(b, v3, v1, m - 1);
        divide_triangle(v1, v3, v2, m - 1);
    }
    else(triangle(a, b, c));
}


void tetrahedron(int m)
{
    // Apply triangle subdivision to faces of tetrahedron
    divide_triangle(v[0], v[1], v[2], m);
    divide_triangle(v[3], v[2], v[1], m);
    divide_triangle(v[0], v[3], v[1], m);
    divide_triangle(v[0], v[2], v[3], m);
}


// 轨迹控制
void track_ptov(int x, int y, int width, int height, float v[3])
{
    float d, a;
    
    v[0] = (2.0f * x - width) / width;
    v[1] = (height - 2.0f * y) / height;
    d = (float)sqrt(v[0] * v[0] + v[1] * v[1]);
    v[2] = (float)cos((M_PI / 2.0f) * ((d < 1.0f) ? d : 1.0f));
    a = 1.0f / (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] *= a;
    v[1] *= a;
    v[2] *= a;
}


// 鼠标拖动响应
void mouseMotion(int x, int y)
{
    float curPos[3], dx, dy, dz;
    track_ptov(x, y, winWidth, winHeight, curPos);
    
    dx = curPos[0] - lastPos[0];
    dy = curPos[1] - lastPos[1];
    dz = curPos[2] - lastPos[2];
    
    if (dx || dy || dz)  // 鼠标拖动
    {
        angle = 90.0f * sqrt(dx * dx + dy * dy + dz * dz);
        axis[0] = lastPos[1] * curPos[2] - lastPos[2] * curPos[1];
        axis[1] = lastPos[2] * curPos[0] - lastPos[0] * curPos[2];
        axis[2] = lastPos[0] * curPos[1] - lastPos[1] * curPos[0];
        
        lastPos[0] = curPos[0];
        lastPos[1] = curPos[1];
        lastPos[2] = curPos[2];
    }
    
    glutPostRedisplay();
}


// 鼠标开始响应
void startMotion(int x, int y)
{
    redrawContinue = false;
    startX = x; startY = y;
    curX = x; curY = y;
    track_ptov(x, y, winWidth, winHeight, lastPos);
}


// 鼠标结束响应
void stopMotion(int x, int y)
{
    redrawContinue = true;
}


// 混合矩阵
float CompositeTransMatrix[4][4]=
{
    { 1.0, 0.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 0.0, 1.0 }
};

float *p_CompositeTransMatrix = *CompositeTransMatrix;


// LookUp矩阵
float Mlookup[4][4]=
{
    { 1.0, 0.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 0.0, 1.0 }
};

float *p_Mlookup = *Mlookup;


// 时间值
GLfloat timeValue = 0.0;


void display(void)
{
    GLfloat radius = 5.0f;
    timeValue = time(NULL);
    
    GLfloat camX = sin((GLfloat)timeValue) * radius;
    GLfloat camZ = cos((GLfloat)timeValue) * radius;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // 改变视角坐标产生绕y轴漫游效果
    gluLookAt(camX, 0.0, camZ,  0.0, 0.0, 0.0,  0.0, 1.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, p_Mlookup);
    
    glLoadIdentity();
    glRotatef(angle, axis[0], axis[1], axis[2]);
    glMultMatrixf(p_CompositeTransMatrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, p_CompositeTransMatrix);
    
    glLoadMatrixf(p_Mlookup);
    glMultMatrixf(p_CompositeTransMatrix);
    
    //mode = 0;
    //tetrahedron(n);
    mode = 1;
    glTranslatef(-2.0, 0.0, 0.0);
    tetrahedron(n);
    mode = 2;
    glTranslatef(4.0, 0.0, 0.0);
    tetrahedron(n);
    
    glutSwapBuffers();
}


// 无响应事件
void myidle()
{
    if (redrawContinue == true)
    {
        angle = 0.1;
        glutPostRedisplay();
    }
}


// 传递鼠标值
void myMouse(int Botton, int State, int MouseX, int MouseY)
{
    if (Botton == GLUT_LEFT_BUTTON)
    {
        switch (State)
        {
            case GLUT_DOWN:
                startMotion(MouseX, MouseY);
                break;
            case GLUT_UP:
                stopMotion(MouseX, MouseY);
                break;
        }
    }
}


// 回显调用
void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-4.0, 4.0, -4.0 * (GLfloat)h / (GLfloat)w,
                4.0 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
    else
        glOrtho(-4.0 * (GLfloat)w / (GLfloat)h,
                4.0 * (GLfloat)w / (GLfloat)h, -4.0, 4.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    display();
}


// 初始化
void myinit()
{
    // 多边形材质
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess = { 75.0 };
    // 光源材质
    GLfloat light_ambient[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    // 光源位置
    GLfloat light_posotion[] = { 1.0, 0.0, 0.0, 1.0 };
    
    // set up ambient, diffuse, and specular components for light 0
    glLightfv(GL_LIGHT0, GL_POSITION, light_posotion);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    // define material proerties for front face of all polygons
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    
    glShadeModel(GL_SMOOTH); // enable smooth shading
    glEnable(GL_LIGHTING); // enable lighting
    glEnable(GL_LIGHT0);  // enable light 0
    glEnable(GL_DEPTH_TEST); // enable z buffer
    
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glColor3f(0.0, 0.0, 0.0);
    
    glutSolidSphere(2.0, 40, 40);
}


// 键盘响应
void keyboard(unsigned char key, int x, int y)
{
    if (key == 'q' || key == 'Q' || key == 27)
        exit(0);
    if (key >= '0' && key <= '7')
    {
        n = key - 48;
        display();
    }
}


int main(int argc, char **argv)
{
    n = 5;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(200, 0);
    glutInitWindowSize(700, 700);
    glutCreateWindow("Sphere");
    myinit();
    
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    
    glutKeyboardFunc(keyboard);
    glutIdleFunc(myidle);
    glutMouseFunc(myMouse);
    glutMotionFunc(mouseMotion);
    
    glutMainLoop();
}
