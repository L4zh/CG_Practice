
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#include <iostream>
#include <fstream>
#include <vector>


std::vector<float> vec;
const int POINTNUM = 7;


// 定义活性边表AET和新边表NET
typedef struct XET
{
    float x;
    float dx, ymax;
    XET * next;
} AET, NET;


// 定义点结构体point
struct point
{
    float x;
    float y;
} pointPoly[POINTNUM] = {
    250,  50,
    550, 150,
    550, 400,
    250, 200,
    100, 350,
    50, 200,
    120, 30
};


// 键盘响应
void keyBoard(unsigned char key, int x, int y)
{
    if (key == 'q' || key == 27 || key == 'Q')
        exit(0);
}


// 多边形扫描算法
void PolyScan()
{
    // 计算最高点的y坐标
    int MaxY = 0;
    int i;
    for (i = 0; i < POINTNUM; i++)
    {
        if (pointPoly[i].y > MaxY)
            MaxY = pointPoly[i].y;
    }
    
    // 初始化AET表
    AET *pAET = new AET;
    pAET->next = NULL;
    
    // 初始化NET表
    NET *pNET[1024];
    for (i = 0; i <= MaxY; i++)
    {
        pNET[i] = new NET;
        pNET[i]->next = NULL;
    }
    
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    
    glBegin(GL_POINTS);
    
    // 扫描并建立NET表
    for (i = 0; i <= MaxY; i++)
    {
        for (int j = 0; j < POINTNUM; j++)
        {
            if (pointPoly[j].y == i)
            {
                // 一个点跟前面的一个点形成一条线段，跟后面的点也形成线段
                if (pointPoly[(j - 1 + POINTNUM) % POINTNUM].y > pointPoly[j].y)
                {
                    NET *p = new NET;
                    p->x = pointPoly[j].x;
                    p->ymax = pointPoly[(j - 1 + POINTNUM) % POINTNUM].y;
                    p->dx = (pointPoly[(j - 1 + POINTNUM) % POINTNUM].x - pointPoly[j].x) / (pointPoly[(j - 1 + POINTNUM) % POINTNUM].y - pointPoly[j].y);
                    p->next = pNET[i]->next;
                    pNET[i]->next = p;
                }
                
                if (pointPoly[(j + 1 + POINTNUM) % POINTNUM].y > pointPoly[j].y)
                {
                    NET *p = new NET;
                    p->x = pointPoly[j].x;
                    p->ymax = pointPoly[(j + 1 + POINTNUM) % POINTNUM].y;
                    p->dx = (pointPoly[(j + 1 + POINTNUM) % POINTNUM].x - pointPoly[j].x) / (pointPoly[(j + 1 + POINTNUM) % POINTNUM].y - pointPoly[j].y);
                    p->next = pNET[i]->next;
                    pNET[i]->next = p;
                }
            }
        }
    }
    
    // 建立并更新活性边表AET
    for (i = 0; i <= MaxY; i++)
    {
        // 计算新的交点x,更新AET
        NET *p = pAET->next;
        
        while (p)
        {
            p->x = p->x + p->dx;
            p = p->next;
        }
        
        // 新AET排序
        AET *tq = pAET;
        p = pAET->next;
        tq->next = NULL;
        
        while (p)
        {
            while (tq->next && p->x >= tq->next->x)
                tq = tq->next;
            
            NET *s = p->next;
            p->next = tq->next;
            tq->next = p;
            p = s;
            tq = pAET;
        }
        // 从AET表中删除ymax==i的结点
        AET *q = pAET;
        p = q->next;
        while (p)
        {
            if (p->ymax == i)
            {
                q->next = p->next;
                delete p;
                p = q->next;
            }
            else
            {
                q = q->next;
                p = q->next;
            }
        }
        
        // 将NET中的新点加入AET
        p = pNET[i]->next;
        q = pAET;
        while (p)
        {
            while (q->next && p->x >= q->next->x)
                q = q->next;
            NET *s = p->next;
            p->next = q->next;
            q->next = p;
            p = s;
            q = pAET;
        }
        
        // 配对填充颜色
        p = pAET->next;
        while (p && p->next)
        {
            for (float j = p->x; j <= p->next->x; j++)
                glVertex2i(static_cast<int>(j), i);
            p = p->next->next;
        }
    }
    
    glEnd();
    glFlush();
}


// 初始化
void init()
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0,600.0,0.0,450.0);
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(600, 600);
    glutCreateWindow("X-Scanline");
    
    init();
    glutDisplayFunc(PolyScan);
    
    glutKeyboardFunc(keyBoard);
    
    glutMainLoop();
    return 0;
}
