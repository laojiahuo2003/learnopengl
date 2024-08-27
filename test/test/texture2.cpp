
///////////////////////////
#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#define    MAXPOINTS    20

int* C;//二项式系数数组指针
int control_points_n = 0;//控制点数量
int specific_p;//特定点标志
int click_state = 0;//0为新增控制点，1为移动控制点，2为已到底控制点最大值
int screen_width, screen_height;//屏幕宽度，长度

struct point
{
	int x;
	int y;
};
point* P;


//函数声明
void reshape(int w, int h);//自适应屏幕窗口大小的改变
void keyboard(unsigned char button, int x, int y);//控制按键函数
void mouse_click(int button, int state, int x, int y);//处理鼠标click事件函数
void mouse_motion(int x, int y);//鼠标在窗口中按下并移动时调用此函数
void display(void);//显示函数,需要显示时调用此函数


//定位坐标函数
void gotoxy(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int main(int argc, char** argv)
{
	P = new point[MAXPOINTS];//控制点数组指针

	printf("按下\"ESC\"键退出。按下\"R\"键重新绘制\r\n");
	printf("最多添加20个点");

	glutInit(&argc, argv);//gult初始化

	/*在创建窗口的时候，指定其显示模式的类型。GLUT_DOUBLE代表双缓冲*/
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);//初始化窗口距离屏幕左边和上面的像素数
	glutInitWindowSize(500, 500);//初始化窗口宽度和高度
	glutCreateWindow("Bézier curve");//创建窗口，"Bézier curve"为窗口名字

	//清除颜色缓冲区,防止缓冲区中原有的颜色信息影响本次绘图
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_LINE_STIPPLE);//启用直线点画功能
	glutReshapeFunc(reshape);//若屏幕窗口大小改变则调用reshape函数
	glutKeyboardFunc(keyboard);//处理普通按键消息。详见https://blog.csdn.net/xie_zi/article/details/1911891
	glutMouseFunc(mouse_click);//处理鼠标click事件
	glutMotionFunc(mouse_motion);//当鼠标在窗口中按下并移动时调用glutMotionFunc注册的回调函数
	glutDisplayFunc(display);//当窗口内容被绘制时调用display函数
	glutMainLoop();//进入gult死循环

	delete[] P;
	return 1;
}

//画正方形，用来标记点
void Draw_Square(int x, int y)
{
	glColor3f(0, 1, 0);//设置颜色，三个参数分别代表red, green, blue
	glBegin(GL_POLYGON);//绘制了一个凸多边形。顶点1到n定义了这个多边形。
	glVertex2f(x + 2, y + 2);
	glVertex2f(x - 2, y + 2);
	glVertex2f(x - 2, y - 2);
	glVertex2f(x + 2, y - 2);
	glEnd();
}

/*计算二项式系数
* n为次数，C为存放二项式系数的数组
* 利用了C(n-k)(n)=C(k)(n)
* 即C[k] = C(k)(n),k在右上角，n在左下角（不知道怎么打那个符号。。）*/
void Culculate_Binomial_Coefficient(int n, int* C)
{
	int k, j;

	for (k = 0; k <= n; k++)
	{
		C[k] = 1;
		for (j = n; j >= k + 1; j--)//1*n*(n-1)*(n-2)*...*(k+2)*(k+1)
		{
			C[k] *= j;
		}
		for (j = n - k; j >= 2; j--)//上式 / ( (n-k)*(n-k-1)*...*3*2*1 )
		{
			C[k] /= j;
		}
	}
}

//画贝塞尔曲线
//n为维度（即控制点数减1），p为控制点数组，m为密度
void Draw_Bézier_curve(int n, point* p, int m)
{
	int i, j;
	float t;
	int C[MAXPOINTS - 1];//二项式系数数组。这样定义有些浪费空间，但不知道更好的办法
	//算出来的曲线轨迹点x坐标。用float型相比int型曲线会更平滑
	float Bézier_curve_pointx;
	float Bézier_curve_pointy;

	Culculate_Binomial_Coefficient(n, C);
	glLineStipple(1, 0xffff);//设置为实线
	glBegin(GL_LINE_STRIP);
	//这里实际上是画了m条短直线来接近贝塞尔曲线
	for (i = 0; i <= m; i++)
	{
		t = (float)i / (float)m;
		Bézier_curve_pointx = 0;
		Bézier_curve_pointy = 0;
		for (j = 0; j < control_points_n; j++)
		{
			Bézier_curve_pointx += C[j] * pow(1 - t, control_points_n - j - 1) * pow(t, j) * p[j].x;
			Bézier_curve_pointy += C[j] * pow(1 - t, control_points_n - j - 1) * pow(t, j) * p[j].y;
		}
		glVertex2f(Bézier_curve_pointx, Bézier_curve_pointy);
	}
	glEnd();

}

//自适应屏幕窗口大小的改变
void reshape(int w, int h)
{
	screen_width = w;
	screen_height = h;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, w, 0.0, h);
}

//控制按键函数
void keyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case 82://"R"
	case 114://"r"
		//重新绘图
		//清空之前显示的坐标
		gotoxy(0, 3);
		for (int i = 0; i < control_points_n; i++)
		{
			printf("                   \r\n");
		}

		specific_p = control_points_n = 0;
		click_state = 0;

		/*glutPostRedisplay函数用来标记当前窗口需要重新绘制。通过glutMainLoop
		* 下一次循环时，窗口显示将被回调以重新显示窗口的正常面板*/
		glutPostRedisplay();
		break;
	case 27://"ESC"
		exit(0);//退出glut循环
	}
}

/*
* 处理鼠标click事件函数
* button表明哪个鼠标键被按下或松开,第二个参数表明函数被调用发生时鼠标的
* 状态（按下或松开），x,y表示鼠标在当前窗口的坐标（以左上角为原点）
*/
void mouse_click(int button, int state, int x, int y)
{
	int i;
	int distance;//鼠标点击处于任意一点距离

	//如果鼠标左键按下
	if (button == GLUT_LEFT_BUTTON)
	{
		//不是移动状态时计算距离
		if (state == GLUT_DOWN && click_state != 1)
		{
			for (i = 0; i < control_points_n; i++)
			{
				//按控制点依次计算距离
				distance = (x - P[i].x) * (x - P[i].x) +
					(screen_height - y - P[i].y) * (screen_height - y - P[i].y);
				if (distance < 15)//如果距离小于特定值
				{
					click_state = 1;//进入状态1，即移动点
					specific_p = i;
					break;//跳出循环
				}
			}
		}

		if (state == GLUT_DOWN && click_state == 0)//新增点
		{
			if (control_points_n < MAXPOINTS)
			{
				P[control_points_n].x = x;
				P[control_points_n].y = screen_height - y;
				specific_p = control_points_n;
				control_points_n++;
				glutPostRedisplay();
			}
			else click_state = 2;//进入状态2，无法新增控制点
		}
		else if (state == GLUT_DOWN && click_state == 1)//移动点
		{
			P[specific_p].x = x;
			P[specific_p].y = screen_height - y;
			glutPostRedisplay();
		}
		else if (state == GLUT_UP)
		{
			//判断控制点是否已达上限
			click_state = control_points_n < MAXPOINTS ? 0 : 2;
		}
	}
}

//鼠标在窗口中按下并移动时调用此函数
void mouse_motion(int x, int y)
{
	if (click_state == 1)
	{
		P[specific_p].x = x;
		P[specific_p].y = screen_height - y;
		glutPostRedisplay();
	}
}

//显示函数,需要显示时调用此函数
void display(void)
{
	int i;

	glClear(GL_COLOR_BUFFER_BIT);//清缓存。GL_COLOR_BUFFER_BIT代表当前可写的颜色缓冲

	for (i = 0; i < control_points_n; i++)//画控制点
	{
		Draw_Square(P[i].x, P[i].y);
	}
	//显示控制点坐标
	gotoxy(0, specific_p + 3);
	printf("P%d:(%d, %d)  ", specific_p, P[specific_p].x, P[specific_p].y);

	glLineStipple(1, 0x0f0f);
	glBegin(GL_LINE_STRIP);//启用直线点画功能
	glColor3f(1, 1, 1);//设置颜色，三个参数分别代表red, green, blue
	for (i = 0; i < control_points_n; i++)//画虚线
	{
		glVertex2f(P[i].x, P[i].y);
	}
	glEnd();

	if (control_points_n > 1)//有大于1个控制点就画出贝塞尔曲线
	{
		Draw_Bézier_curve(control_points_n - 1, P, 200);
	}

	glutSwapBuffers();//交换两个缓冲区指针
}
