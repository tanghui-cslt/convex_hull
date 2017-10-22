
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL\glut.h>
#include "viewer\Arcball.h"                           /*  Arc Ball  Interface         */
#include "Mesh\BaseMesh.h"
#include "MyMesh.h"
#include <Eigen/Dense>

using namespace MeshLib;
using  namespace std;
using namespace Eigen;



void draw_mesh();
void processNormalKeys(unsigned char key, int x, int y);

/*判断这个点是否在面外
* 1  在面的外面
* -1 不在面的外面
* 0  共面
*/
int is_outside(CPoint *arr_point);

/* global mesh */
CBaseMesh<CVertex, CEdge, CFace, CHalfEdge>  mesh;

#define PI 3.1415926535
static int du = 90, OriX = -1, OriY = -1;   //du是视点和x轴的夹角
static float r = 1.5, h = 0.0;   //r是视点绕y轴的半径，h是视点高度即在y轴上的坐标
static float c = PI / 180.0;    //弧度和角度转换参数
int num_vertex = 1;
int num_face = 1;
/*! display call back function
*/
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(r*cos(c*du), h, r*sin(c*du), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);   //从视点看远点
	
	
	//glutWireTeapot(0.5f);
	draw_mesh();
	glutSwapBuffers();                                      //交换两个缓冲区指针

}


void draw_mesh()
{
	glBegin(GL_LINES);
	for (MeshEdgeIterator<CVertex, CEdge, CFace, CHalfEdge>fiter(&mesh); !fiter.end(); ++fiter)
	{
		CEdge * pf = *fiter;
		
		CPoint p1 = mesh.edgeVertex1(pf)->point();
		CPoint p2 = mesh.edgeVertex2(pf)->point();

		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(p1[0], p1[1], p1[2]);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(p2[0], p2[1], p2[2]);
	}
	
	glEnd();
	glFlush();

}
void Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)  //记录鼠标按下位置
		OriX = x, OriY = y;
}

void onMouseMove(int x, int y)   //处理鼠标拖动
{
	du += x - OriX; //鼠标在窗口x轴方向上的增量加到视点与x轴的夹角上，就可以左右转
	h += 0.03*(y - OriY);  //鼠标在窗口y轴方向上的改变加到视点y的坐标上，就可以上下转
	if (h > 1.0)   h = 1.0;  //对视点y坐标作一些限制，不会使视点太奇怪
	else if (h < -1.0) h = -1.0;
	OriX = x, OriY = y;  //将此时的坐标作为旧值，为下一次计算增量做准备
}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -3.6);
	glRotatef(45, 1.0, 0.0, 0.0);
}
void init(void)
{
	glEnable(GL_DEPTH_TEST);
}

void init_openGL(int argc, char * argv[])
{
	/* glut stuff */
	glutInit(&argc, argv);                /* Initialize GLUT */
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Mesh Viewer");	  /* Create window with given title */
	//glViewport(0, 0, 800, 600);
	
	init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);             /* Set-up callback functions */
	glutIdleFunc(display);          //设置不断调用显示函数
	glutMouseFunc(Mouse);
	glutMotionFunc(onMouseMove);
	
	glutKeyboardFunc(processNormalKeys);
	glutMainLoop();                       /* Start GLUT event-processing loop */
}

void creat_new_mesh()
{
	CBaseMesh<CVertex, CEdge, CFace, CHalfEdge> ::tVertex t[4] = { NULL,NULL,NULL ,NULL};
	CBaseMesh<CVertex, CEdge, CFace, CHalfEdge> ::tVertex v[3] = { NULL,NULL,NULL };
	t[0] = mesh.createVertex(num_vertex++);
	t[1] = mesh.createVertex(num_vertex++);
	t[2] = mesh.createVertex(num_vertex++);
	t[3] = mesh.createVertex(num_vertex++);
	CPoint a(0.05, 0, 0);
	CPoint b(0, 0.05, 0);
	CPoint c(0, 0, 0.05);
	CPoint d(0, 0, 0);
	t[0]->point() = a;
	t[1]->point() = b;
	t[2]->point() = c;
	t[3]->point() = d;

	v[0] = t[0];
	v[1] = t[1];
	v[2] = t[2];
	mesh.createFace(v, num_face++);

	v[0] = t[1];
	v[1] = t[0];
	v[2] = t[3];
	mesh.createFace(v, num_face++);

	v[0] = t[0];
	v[1] = t[2];
	v[2] = t[3];
	mesh.createFace(v, num_face++);

	v[0] = t[2];
	v[1] = t[1];
	v[2] = t[3];
	mesh.createFace(v, num_face++);

}
CPoint random_point()
{

	double x = rand() / (RAND_MAX + 1.0) * 1 - 0.5;
	double y = rand() / (RAND_MAX + 1.0) * 1 - 0.5;
	double z = rand() / (RAND_MAX + 1.0) * 1 - 0.5;
	
	CPoint a(x,y,z);
	return a;
}
void  create_face(vector<CHalfEdge *> & half_edge, CVertex * cv)
{

	//for (int i = 0; i< half_edge.size(); i++)
	//{
	//	cout << "face point = " << half_edge[i]->source()->point() << "\t" << half_edge[i]->target()->point() << "\t";
	//}
	//cout << endl;

	for (int i = 0; i < half_edge.size(); i++)
	{
		CHalfEdge *temp_edge = half_edge[i];
		vector <CVertex *>  v;				// push_back the vertex  in ccw order
		v.push_back(temp_edge->target());
		v.push_back(temp_edge->source());
		v.push_back(cv);
		mesh.createFace(v, num_face++);
		v.clear();
	}
	
}

int find_face(int *face_arr,CPoint c);
void  find_boundary(int *face_arr, int face_num, CVertex * random_vertex);

void insert_point()
{
	// 需要修改面的个数， 具体是哪些面
	int face_num = 0, *face_arr  = new int[100];

	CVertex * random_vertex = mesh.createVertex(num_vertex++);

	random_vertex->point() = random_point();		//randomly generate a point  

	//cout << "random point :" << random_vertex->point() << endl;

	face_num = find_face(face_arr,random_vertex->point());

	if (face_num > 0)
	{
		//delete these faces;
		static int num_time = 0;
		num_time++;
	//	cout << "time = " << num_time << endl;
		find_boundary(face_arr, face_num, random_vertex);
	}
	
	//释放空间
	//delete face_arr;
}
void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27)
	{
		insert_point();
		//cout << "times: = "<<num_vertex << endl;
		glutPostRedisplay();
	}

}
/*! main function for viewer
*/
int main(int argc, char * argv[]){
	srand((unsigned)time(NULL));
	creat_new_mesh();

	//for (size_t i = 0; i < 1; i++)
	//{
	//	insert_point();
	//}
	init_openGL(argc, argv);
	return 0;
}

void find_boundary(int *face_arr, int face_num, CVertex * random_vertex)
{
	vector <int > face_vec;
	vector<CHalfEdge *> half_vec;
	for (int i = 0; i < face_num; i++)
	{
		face_vec.push_back(face_arr[i]);
	}
	for (int i = 0; i< face_num; i++)
	{
		int face_id = face_arr[i];
		CFace *cf = mesh.idFace(face_id);
		CHalfEdge *fg = cf->halfedge();
		
		for (int j = 0; j <3; j++)
		{
			CHalfEdge *sym = fg->he_sym();

			int sym_id = mesh.faceId(mesh.halfedgeFace(fg->he_sym()));
			
			if (find(face_vec.begin(),face_vec.end(),sym_id) == face_vec.end())
			{
				half_vec.push_back(fg->he_sym());
				//cout << "id = "<<face_id<<" sym id = " << sym_id << " " << j << endl;
			}
			fg = fg->he_next();
		}
		
	}

	cout << "delete num = " << face_num<<endl;
	for (int i = 0; i < face_num; i++)
	{
		CFace *dele_cf = mesh.idFace(face_arr[i]);
		mesh.deleteFace(dele_cf);
	}
	cout << "num face :" << mesh.numFaces() << endl;

	create_face(half_vec, random_vertex);
	half_vec.clear();
	face_vec.clear();
}

//找这个点在哪些面的外面
int find_face(int *face_arr, CPoint c)
{
	CPoint * arr_point = new CPoint [4];
	int face_num = 0;
	

	for (MeshFaceIterator<CVertex, CEdge, CFace, CHalfEdge>fiter(&mesh); !fiter.end(); ++fiter)
	{
		CFace * pf = *fiter;
		CHalfEdge * pcf = mesh.faceHalfedge(pf);
	
		arr_point[0] = pcf->source()->point();
		arr_point[1] = pcf->target()->point();
		arr_point[2] = pcf->he_next()->target()->point();
		arr_point[3] = c;
		int judge_side = is_outside(arr_point);
		if (judge_side == 1)
		{
			face_arr[face_num++] = mesh.faceId(pf);
		}
		else if (judge_side == 0)
		{
			return 0;
		}
	}
	return face_num;
}

int is_outside(CPoint *arr_point)
{
	MatrixXd m(4, 4);
	double det = 0.0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j <3; j++)
		{
			m(i, j) = arr_point[i][j];
		}
		m(i, 3) = 1;
	}
	det = m.determinant();
	if (fabs(det) < 1e-6)
	{
		return 0;
	}
	else if (det < 0)
	{
		return 1;
	}
	else {
		return -1;
	}
}