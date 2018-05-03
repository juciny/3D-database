#define _USE_MATH_DEFINES
#define _SCL_SECURE_NO_WARNINGS

#include <iostream>  
#include <OpenMesh/Core/IO/MeshIO.hh>       //读取文件
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>  //操作文件
#include <opencv2\opencv.hpp>
#include <fstream>
#include "GL\glut.h"
#include <math.h>
#include <Windows.h>
#include <string>
#include<stdio.h>
#include <mysql.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib")

using namespace std;

//------------------------数据库操作----------------------
class conndba
{
private:
	MYSQL mydata;
public:
	//--------------------------数据库连接------------------------------------
	conndba()
	{
		//初始化数据库
		if (0 == mysql_library_init(0, NULL, NULL))
		{
	      cout << "mysql_library_init() succeed" << endl;
		}
		else 
		{ 
			cout << "mysql_library_init() failed" << endl; 
		}
		//初始化数据结构
	    if (NULL != mysql_init(&mydata))
		{
			  cout << "mysql_init() succeed" << endl;
		}
		else 
		{
			cout << "mysql_init() failed" << endl;
		}

		//在连接数据库之前，设置额外的连接选项
		//可以设置的选项很多，这里设置字符集，否则无法处理中文
		if (0 == mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk")) 
		{
			cout << "mysql_options() succeed" << endl;
		}
		else 
		{
			cout << "mysql_options() failed" << endl;
		}
		//连接数据库
		if (NULL!= mysql_real_connect(&mydata, "localhost", "root", "", "db001", 3306, NULL, 0))
			//这里的地址，用户名，密码，端口可以根据自己本地的情况更改
		{
			cout << "mysql_real_connect() succeed" << endl;
		}
		else 
		{
			cout << "mysql_real_connect() failed" << endl;
		}
	}

	//------------------------数据库查询（根据name）-----------------------
	string select_file(string name)
	{
		string file_name;
		string sqlstr;
		sqlstr = "SELECT model FROM models WHERE name='" + name + "'";
		MYSQL_RES *result = NULL;
		if (0 == mysql_query(&mydata, sqlstr.c_str())) 
		{
			cout << "mysql_query() select data succeed" << endl;
			//一次性取得数据集
			result = mysql_store_result(&mydata);
			//取得并打印行数
			int rowcount = mysql_num_rows(result);    //获取行数
			cout << "row count: " << rowcount << endl;

			if (rowcount == 0)
			{
				file_name = "NULL";
				cout << file_name << endl;
			}
			else 
			{
				//取得并打印各字段的名称
				unsigned int fieldcount = mysql_num_fields(result);
				MYSQL_ROW row = NULL;
				row = mysql_fetch_row(result);
				file_name = row[0];
				cout << file_name << endl;
			}
			return file_name;
		}
		else 
		{
			cout << "mysql_query() select data failed" << endl;
			mysql_close(&mydata);
			return NULL;
		}
		mysql_free_result(result);
		mysql_close(&mydata);
		mysql_server_end();
	}

	//------------------------数据库添加模型-----------------------
	void insert(string number) 
	{
		//string sqlstr = "INSERT INTO models(name,model)VALUES('m"+number+"','F:\\\\3DModels\\\\psb_db0-3\\\\benchmark\\\\db\\\\0\\\\m"+number+"\\\\m"+number+".off')";
		string sqlstr = "INSERT INTO models(name,model)VALUES('m" + number + "','F:\\\\\\\\3DModels\\\\\\\\psb_db14-18\\\\\\\\benchmark\\\\\\\\db\\\\\\\\18\\\\\\\\m" + number + "\\\\\\\\m" + number + ".off')";
		if (0 == mysql_query(&mydata, sqlstr.c_str())) 
		{
			cout << "insert sucess" << endl;
		}
		else 
		{
			cout << "insert failed" << endl;
			mysql_close(&mydata);
		}
		//mysql_close(&mydata);
		//mysql_server_end();
	}

	//-------------------------将ID自增设置为1----------------------------------------
	void incrementToZero() {
		string sqlstr = "ALTER TABLE models AUTO_INCREMENT=1";
		if (0 == mysql_query(&mydata, sqlstr.c_str()))
		{
			cout << "insert sucess" << endl;
		}
		else
		{
			cout << "insert failed" << endl;
			mysql_close(&mydata);
		}
		//mysql_close(&mydata);
		//mysql_server_end();
	}

};

typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;
string edgeName;
string edgeName_1= ".jpg";//图片的后缀
char edgeName_2[8];//图片的前缀
int edgeNum = 0;


void saveMat() {
	//viewport中包含窗口离左上角的坐标（x，y），以及窗口的width和height
	//初始化viewport
	GLint viewPort[4] = { 0 };    

	//获取viewport的参数
	glGetIntegerv(GL_VIEWPORT, viewPort);
	//获取窗口的宽和高
	int nowcols= viewPort[2];
	int nowrows= viewPort[3];
	cout<<viewPort[0]<<" "<<viewPort[1]<<" " << nowrows << " " << nowcols << endl;
	GLbyte* colorArr = (GLbyte*)new unsigned char[nowrows*nowcols * 3];  //新建一个数组
/*
      opengl中有一个非常有用的函数：glReadPixels（）函数，可以读取各种缓冲区的数值（比如，深度，颜色）
	  这个函数比较奇葩，是从窗口的左下角作为第一个像素开始读，并且是按RGB读入
      将opengl的绘制场景保存成图片。
*/
	glReadPixels(viewPort[0], viewPort[1], viewPort[2], viewPort[3], GL_RGB, GL_UNSIGNED_BYTE, colorArr);

   //数组初始化
	for (int i = 0; i<nowcols * nowrows* 3; i++)
	{
		if (colorArr[i] == -1) { colorArr[i] = 255; }
	}
	//初始化一个Mat矩阵，其所有元素均为0
	cv::Mat show= cv::Mat(nowrows, nowcols, CV_8UC3, cv::Scalar::all(0));
	//将数组赋给mat
	for(int i = 0; i< nowrows; i++)
	{
		for(int j  = 0; j< nowcols; j++)
		{
	     show.at<cv::Vec3b>(nowrows - i - 1, j) = cv::Vec3b(colorArr[(i*nowcols + j) * 3 + 2], colorArr[(i*nowcols + j) * 3 + 1], colorArr[(i*nowcols + j) * 3]);
		}
	}
	
	cv::Mat gray, edge;
	cvtColor(show, gray, CV_BGR2GRAY);
	blur(gray, edge, cv::Size(3, 3));
	Canny(edge, edge, 31, 31 * 3, 3);
   
	_itoa(edgeNum, edgeName_2, 10);
	edgeName = edgeName_2 + edgeName_1;
	cout << edgeName << endl;
	cv::imwrite(edgeName,edge);
	edgeNum++;
	delete colorArr;
}

/*
按键盘上的“C”键，就可以将读取的图像缓冲区存储成为Mat
*/

void keyboard(unsigned char key, int x, int y) {	
	switch (key) 
	{
	case 'c':
	case 'C':
		saveMat();
		cout << "color data read !" << endl;;
	default:
		break;
	}
}

//鼠标交互有关的
int mousetate = 0; //鼠标当前的状态
GLfloat Oldx = 0.0; // 点击之前的位置
GLfloat Oldy = 0.0;
//与实现角度大小相关的参数，只需要两个就可以完成
float xRotate = 0.0f;
float yRotate = 0.0f;
float zRotate = 0.0f;

float xTransform = 0.0f;
float yTransform = 0.0f;
float zTransform = 0.0f;

float scale = 2;

//文件读取有关的
MyMesh mesh;
GLuint showFaceList, showWireList;


//初始化顶点和面   
void initGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(2.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST); //用来开启深度缓冲区的功能，启动后OPengl就可以跟踪Z轴上的像素，那么它只有在
	                           //前面没有东西的情况下才会绘制这个像素，在绘制3d时，最好启用，视觉效果会比较真实
							 // ------------------- Lighting  
	glEnable(GL_LIGHTING); // 如果enbale那么就使用当前的光照参数去推导顶点的颜色
	glEnable(GL_LIGHT0); //第一个光源，而GL_LIGHT1表示第二个光源
						 // ------------------- Display List  
	showFaceList = glGenLists(1);
	int temp = mesh.n_edges();

	// 绘制flat
	glNewList(showFaceList, GL_COMPILE);
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{
		glBegin(GL_TRIANGLES);
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			glNormal3fv(mesh.normal(*fv_it).data());
			glVertex3fv(mesh.point(*fv_it).data());
		}
		glEnd();
	}
	glEndList();
}

// 当窗体改变大小的时候
void myReshape(GLint w, GLint h)
{
	glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w > h)
		glOrtho(-static_cast<GLdouble>(w) / h, static_cast<GLdouble>(w) / h, -1.0, 1.0, -100.0, 100.0);
	else
		glOrtho(-1.0, 1.0, -static_cast<GLdouble>(h) / w, static_cast<GLdouble>(h) / w, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// 读取文件的函数
void readfile(string file) {
	// 请求顶点法线 vertex normals
	mesh.request_vertex_normals();
	//如果不存在顶点法线，则报错 
	if (!mesh.has_vertex_normals())
	{
		cout << "错误：标准定点属性 “法线”不存在" << endl;
		return;
	}
	// 如果有顶点发现则读取文件 
	OpenMesh::IO::Options opt;
	if (!OpenMesh::IO::read_mesh(mesh, file, opt))
	{
		cout << "无法读取文件:" << file << endl;
		return;
	}
	else 
		cout << "成功读取文件:" << file << endl;
	cout << endl; // 为了ui显示好看一些
				  //如果不存在顶点法线，则计算出
	if (!opt.check(OpenMesh::IO::Options::VertexNormal))
	{
		// 通过面法线计算顶点法线
		mesh.request_face_normals();
		// mesh计算出顶点法线
		mesh.update_normals();
		// 释放面法线
		mesh.release_face_normals();
	}
}

//  xyz 旋转  每次旋转45度
void mySpecial(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_F4:
		xRotate += 45;
		cout << xRotate << endl;
		break;
	case GLUT_KEY_F5:
		yRotate += 45;
		cout << yRotate << endl;
		break;
	case GLUT_KEY_F6:
		zRotate += 45;
		cout << zRotate << endl;
		break;
	case GLUT_KEY_UP:
		yTransform += 0.2f;
		cout << "向上移动" << 0.2<<endl;
		break;
	case GLUT_KEY_DOWN:
		yTransform -= 0.2f;
		cout << "向下移动" << 0.2 << endl;
		break;
	case GLUT_KEY_LEFT:
		xTransform -= 0.2f;
		cout << "向左移动" << 0.2 << endl;
		break;
	case GLUT_KEY_RIGHT:
		xTransform += 0.2f;
		cout << "向右移动" << 0.2 << endl;
		break;
	case GLUT_KEY_PAGE_DOWN:
		zTransform -= 0.2f;
		cout << "向后移动" << 0.2 << endl;
		break;
	case GLUT_KEY_PAGE_UP:
		zTransform += 0.2f;
		cout << "向前移动" << 0.2 << endl;
		break;
	default:
		break;
	}
	
	glutPostRedisplay();
}

// 鼠标交互
void myMouse(int button, int state, int x, int y)
{
	//如果左键按下，表示操作开始
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mousetate = 1;
		Oldx = x;
		Oldy = y;
	}
	//左键抬起，表示操作结束
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		mousetate = 0;
	//滚轮事件
	if (state == GLUT_UP && button == 3) {
			scale -= 0.1;
	}
	if (state == GLUT_UP && button == 4) {
			scale += 0.1;
	}
	glutPostRedisplay();
}

// 鼠标运动时控制物体旋转
void onMouseMove(int x, int y) {
	if (mousetate) {
		//x对应y是因为对应的是法向量
		yRotate += x - Oldx;
		cout << yRotate << endl;
		glutPostRedisplay();//用来标记当前窗口需要重新绘制
		Oldx = x;
		xRotate += y - Oldy;
		cout << xRotate << endl;
		glutPostRedisplay();
		Oldy = y;
	}
}

void myDisplay()
{
	//要清除之前的深度缓存
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	//与显示相关的函数
	//让物体旋转的函数 第一个参数是角度大小，后面的参数是旋转的法向量
	//物体的坐标系：X轴横，y轴竖，z轴垂直于纸面

	
	//glTranslaterf（a，b，c）控制物体的位移
	//对于Benchmark中的模型
	//a控制前后移动，b控制上下移动，c控制左右移动
	glTranslatef(xTransform, yTransform, zTransform);

	//控制模型的旋转
	glRotatef(xRotate, 1.0f, 0.0f, 0.0f);
	glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
	glRotatef(zRotate, 0.0f, 0.0f, 1.0f);
	
	// 缩放
	glScalef(scale, scale, scale); 

   //每次display都要使用glcalllist回调函数显示想显示的顶点列表

	glCallList(showFaceList);
	glutSwapBuffers(); //这是Opengl中用于实现双缓存技术的一个重要函数
}

//将图片转为像素
void image2txt() {
	cv::Mat srcImage = cv::imread("ss.jpg", cv::IMREAD_COLOR);
	cv::imshow("001", srcImage);
	fstream file("image.txt", ios::out);
	cv::Vec3i p;
	int a = int(srcImage.rows);
	int b = int(srcImage.cols);
	
	cout << "图片高：" << srcImage.rows << "----图片宽：" << srcImage.cols << endl;
	for (int nrows = 0; nrows < srcImage.rows; nrows++)
	{
		for (int ncols = 0; ncols < srcImage.cols; ncols++)
		{
			p = srcImage.at<cv::Vec3b>(nrows, ncols);
			int a =12;
			cout << p.val[0] << "," << p.val[1] << "," << p.val[2] << ",";
			file << p.val[0] << "," << p.val[1] << "," << p.val[2] << ",";

		}

		
	}
	file.close();
}

//将像素转为图片
void txt2image() {
	unsigned char k[17][36] = {
		2,0,0,4,2,2,8,6,6,8,6,6,5,3,3,3,1,1,5,0,1,7,2,3,5,0,1,6,0,1,6,0,1,6,0,1,
		2,2,2,1,1,1,0,0,0,2,0,0,2,0,0,4,2,2,6,4,4,9,4,5,5,0,1,5,0,1,5,0,1,6,0,1,
		6,6,6,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,2,0,0,4,0,0,4,0,0,4,0,0,
		1,3,3,1,3,3,1,3,3,2,4,4,3,5,5,4,4,4,1,1,1,0,0,0,2,0,0,2,0,0,2,0,0,2,0,0,
		0,2,1,0,3,2,0,4,3,0,4,3,2,4,4,3,5,5,5,7,7,7,9,9,1,1,1,1,1,1,1,1,1,3,1,1,
		4,10,9,1,7,6,0,4,3,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,1,3,3,1,3,3,3,3,3,3,3,3,
		4,10,9,2,8,7,4,10,9,12,18,17,20,26,25,18,23,22,4,9,8,0,1,0,0,5,4,2,4,4,2,
		4,4,2,4,4,0,1,0,0,2,1,12,18,17,41,47,46,65,71,70,63,69,68,37,43,42,12,17,16,
		1,6,5,1,6,5,3,5,5,3,5,5,0,7,6,0,1,0,22,28,27,113,119,118,166,172,171,159,165,
		164,123,129,128,50,55,54,14,19,18,3,8,7,0,0,0,0,2,2,3,9,8,12,18,17,59,65,64,162,
		168,167,228,234,233,230,235,234,190,195,194,116,121,120,55,60,59,20,22,22,0,0,0,
		0,0,0,0,1,0,4,10,9,44,50,49,135,140,139,211,216,215,242,247,246,233,238,237,189,
		191,191,122,124,124,52,54,54,10,10,10,8,8,8,0,5,4,1,6,5,20,25,24,76,81,80,148,150,
		150,209,211,211,242,244,244,243,245,245,192,192,192,108,108,108,54,54,54,50,48,48,
		0,1,1,0,2,2,11,13,13,36,38,38,88,90,90,153,153,153,208,208,208,249,249,249,242,240,
		240,178,176,176,134,132,132,126,124,124,0,0,0,0,0,0,2,2,2,3,3,3,32,32,32,83,83,83,
		144,142,142,214,212,212,247,245,245,234,229,230,214,209,210,203,198,199,12,12,12,5,
		5,5,7,7,7,2,0,0,9,7,7,37,35,35,83,81,81,167,162,163,208,203,204,250,245,246,255,250,
		251,239,233,234,5,3,3,2,0,0,13,11,11,5,3,3,6,4,4,8,6,6,30,25,26,106,101,102,164,159,
		160,243,237,238,255,254,255,239,233,234,7,2,3,9,4,5,4,0,0,4,0,0,6,1,2,5,0,1,45,40,41,
		116,111,112,190,185,186,252,247,248,255,250,251,243,238,239,
	};
	cv::Mat outImage = cv::Mat(17, 12, CV_8UC3, (unsigned char*)k);
	cv::Mat gray, edge;
	cv::imshow("001", outImage);
	cvtColor(outImage, gray, CV_BGR2GRAY);
	blur(gray, edge, cv::Size(3, 3));
	Canny(edge, edge, 31, 31*3, 3);
	imshow("002", edge);
	vector <int> params;
	imwrite("011.jpg", edge, params);
}

conndba *demo = new conndba();

//将数据库的自增设置为1
void reset() {
	demo->incrementToZero();
}
//在数据库中插入模型
void insert_models() {
	for (int i = 1800; i < 1815; ++i)
	{
		string s = to_string(i);
		demo->insert(s);
	}
}

int main(int argc, char** argv)
{
    
	//reset();            //重置，自增从1开始
	//insert_models();    //插入模型

	string file_name;
	cout << "************************************" << endl;
	cout << "请输入3D模型的名字：";
	cin >> file_name;
	string file_path = demo->select_file(file_name);
	cout << "模型的名字是：\t" << file_name << endl;
	cout << "模型的路径是：\t" << file_path << endl;
	cout << "开始读" << file_name << endl;
	cout << "************************************" << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // GLUT_Double 表示使用双缓存而非单缓存
	glutInitWindowPosition(500, 500);
	glutInitWindowSize(600, 400);
	glutCreateWindow("3D模型库——简陋版");

	readfile(file_path);
	scale = 1.0;
	initGL();                     //初始化顶点和面 
	
    glutMouseFunc(myMouse);        //鼠标操控：1 用鼠标旋转  2 用滚轮控制缩放
	glutMotionFunc(onMouseMove);   //// 鼠标运动时控制物体旋转
	glutSpecialFunc(&mySpecial);   //  位移 旋转 
	glutReshapeFunc(&myReshape);   // 当窗体改变大小的时候
	glutDisplayFunc(&myDisplay);    
	glutKeyboardFunc(keyboard);

	glutMainLoop();     //循环下一次
	
	
	return 0;
}