
#include <cstdlib>
#include <cmath>

#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>


int xm, ym, xmm, ymm;
int first = 0;


// 画点函数
void draw_pixel(int ix, int iy)
{
    glBegin(GL_POINTS);
    glVertex2i(ix, iy);
    glEnd();
}


// 画圆辅助
void circle_support(int xc, int yc, int x, int y)
{
    draw_pixel(xc + x, yc + y);
    draw_pixel(xc + x, yc - y);
    draw_pixel(xc - x, yc + y);
    draw_pixel(xc - x, yc - y);
    draw_pixel(xc + y, yc + x);
    draw_pixel(xc + y, yc - x);
    draw_pixel(xc - y, yc + x);
    draw_pixel(xc - y, yc - x);
}


// 中点画圆算法
void circle(int xc, int yc, int r)
{
    int x = 0, y = r;
    int d = 3 - 2 * r;
    draw_pixel(xc, yc);
    
    while (x < y)
    {
        if (d < 0)
            d = d + 4 * x + 6;
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
        circle_support(xc, yc, x, y);
    }
}


// 键盘控制
void keyBoard(unsigned char key, int x ,int y)
{
    if (key == 'q' || key == 'Q' || key == 27)
        exit(0);
}


// 鼠标位置
void mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        xm = x;
        ym = (700 - y);
        glColor3f(1.0, 1.0, 1.0);
        first = 0;
    }
    
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        // 先用XOR模式清除刚才的圆
        glLogicOp(GL_XOR);
        circle(xm, ym, xmm);
        glFlush();
        
        // 绘制确定的圆
        xmm = x;
        ymm = (700 - y);
        glColor3f(1.0, 1.0, 1.0);
        circle(xm, ym, xmm);
        glFlush();
        glLogicOp(GL_COPY);
    }
}


// 鼠标移动
void move(int x, int y)
{
    // 非0，需要XOR清除刚才画的圆
    if (first == 1)
    {
        glLogicOp(GL_XOR);
        circle(xm, ym, xmm);
        glFlush();
    }
    
    // 获取新的移动坐标，绘制圆，并计数器标志定为1
    xmm = x;
    ymm = (700 - y);
    glColor3f(1.0, 1.0, 1.0);
    circle(xm, ym, xmm);
    glFlush();
    first = 1;
}


// 显示函数
void display()
{
    glFlush();
}


// 初始化函数
void init()
{
    // 设置背景色是黑色
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 开启逻辑运算功能
    glEnable(GL_COLOR_LOGIC_OP);
    
    // 设置缓冲模式
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // 裁剪窗口是一个单位正方形，坐标原点位于正方形的左下角
    gluOrtho2D(0, 700, 0, 700);
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Line");
    
    init();
    glutDisplayFunc(display);
    
    // 注册需要的两个鼠标回调函数
    glutMouseFunc(mouse);
    glutMotionFunc(move);
    glutKeyboardFunc(keyBoard);
    
    glutMainLoop();
    return 0;
}
