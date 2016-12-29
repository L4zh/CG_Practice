
#include <cstdlib>
#include <cmath>

#include <openGL/glut.h>


#define M_PI 3.14159
#define bool int
#define false 0
#define true 1


int near = 3;
int far = 10;

int Zeye = 5;

int projectStyle = 1;  // 1-平行正投影 2-透视


int winWidth = 500, winHeight = 500;

bool redrawContinue = false;

float lastPos[3] = { 0.0f, 0.0f, 0.0f };

float angle = 0.0;

GLfloat axis[3] = { 0.0, 0.0, 0.1 };

int curX, curY;
int startX, startY;


// 顶点值
GLfloat v[4][3] = {
    {  0.0,       0.0,       1.0 },
    {  0.0,       0.942809, -0.333333 },
    { -0.816497, -0.471405, -0.333333 },
    {  0.816497, -0.471405, -0.333333 }
};


// 颜色值
GLfloat colors[4][3] = {
    { 1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    { 1.0, 1.0, 0.0 },
};


// 分形次数
int n;


// 三角形顶点
void triangle(GLfloat *va, GLfloat *vb, GLfloat *vc)
{
    glVertex3fv(va);
    glVertex3fv(vb);
    glVertex3fv(vc);
}


// 画一个三角形
void tetra(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d)
{
    glColor3fv(colors[0]);
    triangle(a, b, c);
    glColor3fv(colors[1]);
    triangle(a, b, d);
    glColor3fv(colors[2]);
    triangle(a, d, c);
    glColor3fv(colors[3]);
    triangle(b, c, d);
}


// 递归分形
void divide_tetra(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d, int m)
{
    GLfloat ab[3], bc[3], ac[3], ad[3], bd[3], cd[3];
    int j;
    if (m > 0)
    {
        for (j=0; j<3; j++) ab[j] = (a[j] + b[j]) / 2.0;
        for (j=0; j<3; j++) bc[j] = (b[j] + c[j]) / 2.0;
        for (j=0; j<3; j++) ac[j] = (a[j] + c[j]) / 2.0;
        for (j=0; j<3; j++) ad[j] = (a[j] + d[j]) / 2.0;
        for (j=0; j<3; j++) bd[j] = (b[j] + d[j]) / 2.0;
        for (j=0; j<3; j++) cd[j] = (c[j] + d[j]) / 2.0;

        divide_tetra(a, ab, ac, ad, m - 1);
        divide_tetra(ab, b, bc, bd, m - 1);
        divide_tetra(ac, bc, c, cd, m - 1);
        divide_tetra(ad, bd, cd, d, m - 1);
    }
    else(tetra(a, b, c, d));
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
        // 做叉乘
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


float CompositeTransMatrix[4][4] {
    { 1.0, 0.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 0.0, 1.0 }
};

float *p_CompositeTransMatrix = *CompositeTransMatrix;

float Mlookup[4][4] {
    { 1.0, 0.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 0.0, 1.0 }
};

float *p_Mlookup = *Mlookup;


// 显示
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    // 设置视点相机位置
    gluLookAt(0, 0, Zeye,  0, 0, 0,  0, 1, 0);
    glGetFloatv(GL_MODELVIEW_MATRIX, p_Mlookup);

    glLoadIdentity();
    glRotatef(angle, axis[0], axis[1], axis[2]);
    glMultMatrixf(p_CompositeTransMatrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, p_CompositeTransMatrix);

    glLoadMatrixf(p_Mlookup);
    glMultMatrixf(p_CompositeTransMatrix);

    glBegin(GL_TRIANGLES);
    divide_tetra(v[0], v[1], v[2], v[3], n);
    glEnd();

    // 立即显示
    glFlush();
}


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


// 透视投影矩阵
void Perspective(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glFrustum(-2.0, 2.0, -2.0 * (GLfloat)h / (GLfloat)w,
                  2.0 * (GLfloat)h / (GLfloat)w, near, far);
    else
        glFrustum(-2.0 * (GLfloat)w / (GLfloat)h,
                  2.0 * (GLfloat)w / (GLfloat)h, -2.0, 2.0, near, far);
    glutPostRedisplay();

    // 使得跟踪球交互操作正确
    winWidth = w;
    winHeight = h;
}


// 正射投影矩阵
void Orthographic(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-2.0, 2.0, -2.0 * (GLfloat)h / (GLfloat)w,
                2.0 * (GLfloat)h / (GLfloat)w, near, far);
    else
        glOrtho(-2.0 * (GLfloat)w / (GLfloat)h,
                2.0 * (GLfloat)w / (GLfloat)h, -2.0, 2.0, near,far);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();

    // 使得跟踪球交互操作正确
    winWidth = w;
    winHeight = h;
}


// 绘制菜单
void Draw_menu(int index)
{
    switch (index)
    {
        case (1) :
        {
            Orthographic(winWidth, winHeight);
            projectStyle = 1;
            glutPostRedisplay();
            break;
        }
        case (2) :
        {
            Perspective(winWidth, winHeight);
            projectStyle = 2;
            glutPostRedisplay();
            break;
        }
    }
}


void myReshape(int w, int h)
{
    if (projectStyle == 1)
        Orthographic(w, h);
    else
        Perspective(w, h);
}


// 键盘退出
void keyboard(unsigned char key, int x, int y)
{
    if (key == 'q' || key == 'Q' || key == 27)
        exit(0);
}



int main(int argc, char **argv)
{
    n = 3;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Gasket");

    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);

    glutIdleFunc(myidle);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(myMouse);
    glutMotionFunc(mouseMotion);

    glutCreateMenu(Draw_menu);
    glutAddMenuEntry("Orthographic", 1);
    glutAddMenuEntry("Perspective", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutMainLoop();
}
