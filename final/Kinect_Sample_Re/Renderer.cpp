#include "Renderer.h"

void draw_center(void)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); /* R */
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.2f, 0.0f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'x');

	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f); /* G */
	glVertex3f(0.0f, 0.2f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.0f, 0.2f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'y');

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f); /* B */
	glVertex3f(0.0f, 0.0f, -0.2f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.0f, 0.0f, -0.2f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'z');
}

void idle() {
	static GLuint previousClock = glutGet(GLUT_ELAPSED_TIME);
	static GLuint currentClock = glutGet(GLUT_ELAPSED_TIME);
	static GLfloat deltaT;

	currentClock = glutGet(GLUT_ELAPSED_TIME);
	deltaT = currentClock - previousClock;
	if (deltaT < 1000.0 / 20.0) { return; }
	else { previousClock = currentClock; }

	//char buff[256];
	//sprintf_s(buff, "Frame Rate = %f", 1000.0 / deltaT);
	//frameRate = buff;

	glutPostRedisplay();
}

void close()
{
	glDeleteTextures(1, &dispBindIndex);
	glutLeaveMainLoop();
	CloseHandle(hMutex);
}

void add_quats(float q1[4], float q2[4], float dest[4])
{
	static int count = 0;
	float t1[4], t2[4], t3[4];
	float tf[4];

	vcopy(q1, t1);
	vscale(t1, q2[3]);

	vcopy(q2, t2);
	vscale(t2, q1[3]);

	vcross(q2, q1, t3);
	vadd(t1, t2, tf);
	vadd(t3, tf, tf);
	tf[3] = q1[3] * q2[3] - vdot(q1, q2);

	dest[0] = tf[0];
	dest[1] = tf[1];
	dest[2] = tf[2];
	dest[3] = tf[3];

	if (++count > RENORMCOUNT) {
		count = 0;
		normalize_quat(dest);
	}
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(58, (double)width / height, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	GLfloat spin_quat[4];
	float gain;
	gain = 2.0; /* trackball gain */

	if (drag_state == GLUT_DOWN)
	{
		if (button_state == GLUT_LEFT_BUTTON)
		{
			trackball(spin_quat,
				(gain * rot_x - 500) / 500,
				(500 - gain * rot_y) / 500,
				(gain * x - 500) / 500,
				(500 - gain * y) / 500);
			add_quats(spin_quat, quat, quat);
		}
		else if (button_state == GLUT_RIGHT_BUTTON)
		{
			t[0] -= (((float)trans_x - x) / 500);
			t[1] += (((float)trans_y - y) / 500);
		}
		else if (button_state == GLUT_MIDDLE_BUTTON)
			t[2] -= (((float)trans_z - y) / 500 * 4);
		else if (button_state == 3 || button_state == 4) // scroll
		{

		}
		//glutPostRedisplay();
	}

	rot_x = x;
	rot_y = y;

	trans_x = x;
	trans_y = y;
	trans_z = y;
}

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			rot_x = x;
			rot_y = y;

			//t[0] = t[0] + 1;


		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			trans_x = x;
			trans_y = y;
		}
		else if (button == GLUT_MIDDLE_BUTTON)
		{
			//trcon = trcon + 1;
			trans_z = y;
		}
		else if (button == 3 || button == 4)
		{
			const float sign = (static_cast<float>(button)-3.5f) * 2.0f;
			t[2] -= sign * 500 * 0.00015f;
		}
	}

	drag_state = state;
	button_state = button;
}

void vzero(float* v)
{
	v[0] = 0.0f;
	v[1] = 0.0f;
	v[2] = 0.0f;
}

void vset(float* v, float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

void vsub(const float *src1, const float *src2, float *dst)
{
	dst[0] = src1[0] - src2[0];
	dst[1] = src1[1] - src2[1];
	dst[2] = src1[2] - src2[2];
}

void vcopy(const float *v1, float *v2)
{
	register int i;
	for (i = 0; i < 3; i++)
		v2[i] = v1[i];
}

void vcross(const float *v1, const float *v2, float *cross)
{
	float temp[3];

	temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
	vcopy(temp, cross);
}

float vlength(const float *v)
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void vscale(float *v, float div)
{
	v[0] *= div;
	v[1] *= div;
	v[2] *= div;
}

void vnormal(float *v)
{
	vscale(v, 1.0f / vlength(v));
}

float vdot(const float *v1, const float *v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void vadd(const float *src1, const float *src2, float *dst)
{
	dst[0] = src1[0] + src2[0];
	dst[1] = src1[1] + src2[1];
	dst[2] = src1[2] + src2[2];
}

void trackball(float q[4], float p1x, float p1y, float p2x, float p2y)
{
	float a[3]; /* Axis of rotation */
	float phi;  /* how much to rotate about axis */
	float p1[3], p2[3], d[3];
	float t;

	if (p1x == p2x && p1y == p2y) {
		/* Zero rotation */
		vzero(q);
		q[3] = 1.0;
		return;
	}

	/*
	 * First, figure out z-coordinates for projection of P1 and P2 to
	 * deformed sphere
	 */
	vset(p1, p1x, p1y, tb_project_to_sphere(TRACKBALLSIZE, p1x, p1y));
	vset(p2, p2x, p2y, tb_project_to_sphere(TRACKBALLSIZE, p2x, p2y));

	/*
	 *  Now, we want the cross product of P1 and P2
	 */
	vcross(p2, p1, a);

	/*
	 *  Figure out how much to rotate around that axis.
	 */
	vsub(p1, p2, d);
	t = vlength(d) / (2.0f*TRACKBALLSIZE);

	/*
	 * Avoid problems with out-of-control values...
	 */
	if (t > 1.0) t = 1.0;
	if (t < -1.0) t = -1.0;
	phi = 2.0f * asin(t);

	axis_to_quat(a, phi, q);
}

void axis_to_quat(float a[3], float phi, float q[4])
{
	vnormal(a);
	vcopy(a, q);
	vscale(q, sin(phi / 2.0f));
	q[3] = cos(phi / 2.0f);
}

float tb_project_to_sphere(float r, float x, float y)
{
	float d, t, z;

	d = sqrt(x*x + y*y);
	if (d < r * 0.70710678118654752440f) {    /* Inside sphere */
		z = sqrt(r*r - d*d);
	}
	else {           /* On hyperbola */
		t = r / 1.41421356237309504880f;
		z = t*t / d;
	}
	return z;
}

void normalize_quat(float q[4])
{
	int i;
	float mag;

	mag = (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
	for (i = 0; i < 4; i++) q[i] /= mag;
}

void build_rotmatrix(float m[4][4], float q[4])
{
	m[0][0] = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
	m[0][1] = 2.0f * (q[0] * q[1] - q[2] * q[3]);
	m[0][2] = 2.0f * (q[2] * q[0] + q[1] * q[3]);
	m[0][3] = 0.0f;

	m[1][0] = 2.0f * (q[0] * q[1] + q[2] * q[3]);
	m[1][1] = 1.0f - 2.0f * (q[2] * q[2] + q[0] * q[0]);
	m[1][2] = 2.0f * (q[1] * q[2] - q[0] * q[3]);
	m[1][3] = 0.0f;

	m[2][0] = 2.0f * (q[2] * q[0] - q[1] * q[3]);
	m[2][1] = 2.0f * (q[1] * q[2] + q[0] * q[3]);
	m[2][2] = 1.0f - 2.0f * (q[1] * q[1] + q[0] * q[0]);
	m[2][3] = 0.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

void InitializeWindow(int argc, char* argv[])
{
	// initialize glut settings
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);

	glutInitWindowPosition(0, 0);

	dispWindowIndex = glutCreateWindow("3D Model");

	trackball(quat, 90.0, 0.0, 0.0, 0.0);

	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutCloseFunc(close);
	//GLuint image = load   ("./my_texture.bmp");
	
	//glBindTexture(1,)

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// bind textures
	glClearColor(0.9f, 0.98f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	reshape(1000, 1000);

	/*glGenTextures(1, &dispBindIndex);
	glBindTexture(GL_TEXTURE_2D, dispBindIndex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
}



void drawing(int mymesh_len, Vertex* vertex, Vertex* vertex_color, MMesh* mymesh) {
	glBegin(GL_TRIANGLES);
	for (int jj = 0; jj < mymesh_len; jj = jj + 1)
	{
		glTexCoord2d(vertex_color[mymesh[jj].T1 - 1].X, vertex_color[mymesh[jj].T1 - 1].Y);
		glVertex3f(vertex[mymesh[jj].V1 - 1].X, vertex[mymesh[jj].V1 - 1].Y, vertex[mymesh[jj].V1 - 1].Z);
		glTexCoord2d(vertex_color[mymesh[jj].T2 - 1].X, vertex_color[mymesh[jj].T2 - 1].Y);
		glVertex3f(vertex[mymesh[jj].V2 - 1].X, vertex[mymesh[jj].V2 - 1].Y, vertex[mymesh[jj].V2 - 1].Z);
		glTexCoord2d(vertex_color[mymesh[jj].T3 - 1].X, vertex_color[mymesh[jj].T3 - 1].Y);
		glVertex3f(vertex[mymesh[jj].V3 - 1].X, vertex[mymesh[jj].V3 - 1].Y, vertex[mymesh[jj].V3 - 1].Z);
		//glTexCoord2d(vertex_color[mymesh[jj].T4 - 1].X, vertex_color[mymesh[jj].T4 - 1].Y);
		//glVertex3f(vertex[mymesh[jj].V4 - 1].X, vertex[mymesh[jj].V4 - 1].Y, vertex[mymesh[jj].V4 - 1].Z);
	}
	glEnd();

}

void drawingColor(int mymesh_len, Vertex* vertex, MMesh* mymesh) {
	
	glBegin(GL_TRIANGLES);
	for (int jj = 0; jj < mymesh_len; jj = jj + 1)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(vertex[mymesh[jj].V1 - 1].X, vertex[mymesh[jj].V1 - 1].Y, vertex[mymesh[jj].V1 - 1].Z);
		glVertex3f(vertex[mymesh[jj].V2 - 1].X, vertex[mymesh[jj].V2 - 1].Y, vertex[mymesh[jj].V2 - 1].Z);
		glVertex3f(vertex[mymesh[jj].V3 - 1].X, vertex[mymesh[jj].V3 - 1].Y, vertex[mymesh[jj].V3 - 1].Z);
	}
	glEnd();

}
void readBMP(char* filepath) {
	int i, j, k = 0;
	FILE* f = fopen(filepath, "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
											   // extract image height and width from header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];

	int size = 3 * width * height;
	unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
	fclose(f);
	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
		{
			mytexels[j][i][0] = data[k * 3 + 2];
			mytexels[j][i][1] = data[k * 3 + 1];
			mytexels[j][i][2] = data[k * 3];
			k++;
		}
}
void drawCloud(int mymesh_len, Vertex* vertex, MMesh* mymesh) {
	glBegin(GL_TRIANGLES);
	for (int jj = 0; jj < mymesh_len; jj = jj + 1)
	{
		glTexCoord2d(1.0, 1.0);
		glVertex3f(vertex[mymesh[jj].V1 - 1].X, vertex[mymesh[jj].V1 - 1].Y, vertex[mymesh[jj].V1 - 1].Z);
		glVertex3f(vertex[mymesh[jj].V2 - 1].X, vertex[mymesh[jj].V2 - 1].Y, vertex[mymesh[jj].V2 - 1].Z);
		glVertex3f(vertex[mymesh[jj].V3 - 1].X, vertex[mymesh[jj].V3 - 1].Y, vertex[mymesh[jj].V3 - 1].Z);
	}
	glEnd();
}


void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 0.1, 200);		
	glTranslatef(t[0], t[1], t[2] - 1.0f);
	glScalef(1, 1, 1);	
	GLfloat m[4][4],m1[4][4];
	build_rotmatrix(m, quat);
	gluLookAt(3.0, 1.5, -0.3, 0, 0, 0, 0, 1.0, 0);

	GLfloat r, g, b;
	glMultMatrixf(&m[0][0]);



	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat diffuse0[4] = { 1.1, 1.3, 1.3, 1.0 };
	GLfloat ambient0[4] = { 0.4, 0.4, 0.5, 1.0 };
	GLfloat specular0[4] = { 0.4, 0.8, 1.0, 1.0 };
	GLfloat light0_pos[4] = { -0.5, -1.0, 3.8, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos); 
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.2);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);

	glShadeModel(GL_FLAT);


	//// 폴리곤의 앞면의 재질을 설정 
	//glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	
	readBMP("texture/grass.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLES); //바닥
	glTexCoord2d(0.0, 1.0);
	glVertex3f(vertex[mymesh[0].V1 - 1].X, vertex[mymesh[0].V1 - 1].Y, vertex[mymesh[0].V1 - 1].Z); //2
	glTexCoord2d(1.0, 0.0);
	glVertex3f(vertex[mymesh[0].V2 - 1].X, vertex[mymesh[0].V2 - 1].Y, vertex[mymesh[0].V2 - 1].Z); //3
	glTexCoord2d(0.0, 0.0);
	glVertex3f(vertex[mymesh[0].V3 - 1].X, vertex[mymesh[0].V3 - 1].Y, vertex[mymesh[0].V3 - 1].Z); //1
	glTexCoord2d(0.0, 1.0);
	glVertex3f(vertex[mymesh[1].V1 - 1].X, vertex[mymesh[1].V1 - 1].Y, vertex[mymesh[1].V1 - 1].Z); //2
	glTexCoord2d(1.0, 1.0);
	glVertex3f(vertex[mymesh[1].V2 - 1].X, vertex[mymesh[1].V2 - 1].Y, vertex[mymesh[1].V2 - 1].Z); //4
	glTexCoord2d(1.0, 0.0);
	glVertex3f(vertex[mymesh[1].V3 - 1].X, vertex[mymesh[1].V3 - 1].Y, vertex[mymesh[1].V3 - 1].Z); //3
	glEnd();

	readBMP("texture/grasssub.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	glBegin(GL_QUADS);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(vertex[mymesh[0].V3 - 1].X, vertex[mymesh[0].V3 - 1].Y, vertex[mymesh[0].V3 - 1].Z);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(vertex[mymesh[0].V1 - 1].X, vertex[mymesh[0].V1 - 1].Y, vertex[mymesh[0].V1 - 1].Z);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(vertex[mymesh[0].V1 - 1].X, vertex[mymesh[0].V1 - 1].Y - 0.5f, vertex[mymesh[0].V1 - 1].Z);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(vertex[mymesh[0].V3 - 1].X, vertex[mymesh[0].V3 - 1].Y -0.5f, vertex[mymesh[0].V3 - 1].Z);

	glTexCoord2d(1.0, 0.0);
	glVertex3f(vertex[mymesh[0].V3 - 1].X, vertex[mymesh[0].V3 - 1].Y, vertex[mymesh[0].V3 - 1].Z);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(vertex[mymesh[0].V2 - 1].X, vertex[mymesh[0].V2 - 1].Y, vertex[mymesh[0].V2 - 1].Z);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(vertex[mymesh[0].V2 - 1].X, vertex[mymesh[0].V2 - 1].Y - 0.5f, vertex[mymesh[0].V2 - 1].Z);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(vertex[mymesh[0].V3 - 1].X, vertex[mymesh[0].V3 - 1].Y - 0.5f, vertex[mymesh[0].V3 - 1].Z);

	glTexCoord2d(1.0, 0.0);
	glVertex3f(vertex[mymesh[0].V2 - 1].X, vertex[mymesh[0].V2 - 1].Y, vertex[mymesh[0].V2 - 1].Z);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(vertex[mymesh[1].V2 - 1].X, vertex[mymesh[1].V2 - 1].Y, vertex[mymesh[1].V2 - 1].Z);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(vertex[mymesh[1].V2 - 1].X, vertex[mymesh[1].V2 - 1].Y - 0.5f, vertex[mymesh[1].V2 - 1].Z);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(vertex[mymesh[0].V2 - 1].X, vertex[mymesh[0].V2 - 1].Y - 0.5f, vertex[mymesh[0].V2 - 1].Z);

	glTexCoord2d(1.0, 0.0);
	glVertex3f(vertex[mymesh[1].V1 - 1].X, vertex[mymesh[1].V1 - 1].Y, vertex[mymesh[1].V1 - 1].Z);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(vertex[mymesh[1].V2 - 1].X, vertex[mymesh[1].V2 - 1].Y, vertex[mymesh[1].V2 - 1].Z);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(vertex[mymesh[1].V2 - 1].X, vertex[mymesh[1].V2 - 1].Y - 0.5f, vertex[mymesh[1].V2 - 1].Z);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(vertex[mymesh[1].V1 - 1].X, vertex[mymesh[1].V1 - 1].Y - 0.5f, vertex[mymesh[1].V1 - 1].Z);

	glTexCoord2d(1.0, 0.0); //밑바닥
	glVertex3f(vertex[mymesh[0].V3 - 1].X, vertex[mymesh[0].V3 - 1].Y - 0.5f, vertex[mymesh[0].V3 - 1].Z);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(vertex[mymesh[1].V1 - 1].X, vertex[mymesh[1].V1 - 1].Y - 0.5f, vertex[mymesh[1].V1 - 1].Z);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(vertex[mymesh[1].V2 - 1].X, vertex[mymesh[1].V2 - 1].Y - 0.5f, vertex[mymesh[1].V2 - 1].Z);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(vertex[mymesh[1].V3 - 1].X, vertex[mymesh[1].V3 - 1].Y - 0.5f, vertex[mymesh[1].V3 - 1].Z);
	
	glEnd();

	//readBMP("texture/sky2.bmp");
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	//glBegin(GL_QUADS);
	//glTexCoord2d(0.0, 0.0);
	//glVertex3f(wall[0].X, wall[0].Y, wall[0].Z);
	//glTexCoord2d(1.0, 0.0);
	//glVertex3f(wall[1].X, wall[1].Y, wall[1].Z);
	//glTexCoord2d(1.0, 1.0);
	//glVertex3f(wall[5].X, wall[5].Y, wall[5].Z);
	//glTexCoord2d(0.0, 1.0);
	//glVertex3f(wall[4].X, wall[4].Y, wall[4].Z);
	//glEnd();

	readBMP("texture/green2.bmp"); //덤불
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	drawing(len2, vertex2, vertex_color2, mymesh2);
	drawing(len3, vertex3, vertex_color3, mymesh3);
	drawing(len4, vertex4, vertex_color4, mymesh4);
	drawing(len5, vertex5, vertex_color5, mymesh5);
	drawing(len6, vertex6, vertex_color6, mymesh6);
	drawing(len7, vertex7, vertex_color7, mymesh7);
	drawing(len8, vertex8, vertex_color8, mymesh8);
	drawing(len9, vertex9, vertex_color9, mymesh9);
	drawing(len10, vertex10, vertex_color10, mymesh10);
	drawing(len11, vertex11, vertex_color11, mymesh11);

	readBMP("texture/rock.bmp"); //돌맹이
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	drawing(len12, rock1, rock_color1, rockmesh1);
	drawing(len13, rock2, rock_color2, rockmesh2);
	drawing(len14, rock3, rock_color3, rockmesh3);
	drawing(len15, rock4, rock_color4, rockmesh4);

	readBMP("texture/wood.bmp"); //나뭇가지, 통나무 등
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	drawing(lwood1, wood1, wood_color1, woodmesh1);
	drawing(lwood2, wood2, wood_color2, woodmesh2);
	drawing(lwood13, wood13, wood_color13, woodmesh13);
	drawing(lwood4, wood4, wood_color4, woodmesh4);
	drawing(lwood6, wood6, wood_color6, woodmesh6);
	drawing(lwood7, wood7, wood_color7, woodmesh7);
	drawing(lwood11, wood11, wood_color11, woodmesh11);

	readBMP("texture/wood2.bmp"); //나뭇가지, 통나무 등
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	drawing(lwood8, wood8, wood_color8, woodmesh8);
	drawing(lwood9, wood9, wood_color9, woodmesh9);
	drawing(lwood10, wood10, wood_color10, woodmesh10);
	drawing(lwood5, wood5, wood_color5, woodmesh5);
	drawing(lwood12, wood12, wood_color12, woodmesh12);
	drawing(lwood3, wood3, wood_color3, woodmesh3);

	readBMP("texture/texture_gradient.bmp"); //나무
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	drawing(ltree1, tree1, tree_color1, treemesh1);
	drawing(ltree2, tree2, tree_color2, treemesh2);
	drawing(ltree3, tree3, tree_color3, treemesh3);
	drawing(ltree4, tree4, tree_color4, treemesh4);

	drawing(ltree5, tree5, tree_color5, treemesh5);
	drawing(ltree6, tree6, tree_color6, treemesh6);
	drawing(ltree7, tree7, tree_color7, treemesh7);
	drawing(ltree8, tree8, tree_color8, treemesh8);

	readBMP("texture/texture_gradient2.bmp"); //나무
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	drawing(ltree9, tree9, tree_color9, treemesh9);
	drawing(ltree10, tree10, tree_color10, treemesh10);
	drawing(ltree11, tree11, tree_color11, treemesh11);
	drawing(ltree12, tree12, tree_color12, treemesh12);

	readBMP("texture/stone.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	drawing(lstone1, stone1, stone_color1, stonemesh1);
	drawing(lstone2, stone2, stone_color2, stonemesh2);
	drawing(lstone3, stone3, stone_color3, stonemesh3);
	drawing(lstone4, stone4, stone_color4, stonemesh4);
	drawing(lstone5, stone5, stone_color5, stonemesh5);

	drawing(lstone6, stone6, stone_color6, stonemesh6);
	drawing(lstone7, stone7, stone_color7, stonemesh7);
	drawing(lstone8, stone8, stone_color8, stonemesh8);
	drawing(lstone9, stone9, stone_color9, stonemesh9);
	drawing(lstone10, stone10, stone_color10, stonemesh10);

	readBMP("texture/bird.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	drawing(birdl, bird, bird_color, birdmesh);

	readBMP("texture/cloud.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);

	glPushMatrix();
	trcon = trcon - 1;
	if (trcon <= -40)
		trcon = -trcon;
	glTranslatef(0, 0, trcon / 250.0);
	drawCloud(lcloud1, cloud1, cloudmesh1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, trcon / 50.0);
	drawCloud(lcloud2, cloud2, cloudmesh2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, trcon / 100.0);
	drawCloud(lcloud3, cloud3, cloudmesh3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, trcon / 200.0);
	drawCloud(lcloud4, cloud4, cloudmesh4);
	glPopMatrix();

	glutSwapBuffers();
}


void deleteVertex() {
	delete[] vertex;
	delete[] mymesh;
	delete[] vertex_color;
	delete[] vertex2;
	delete[] mymesh2;
	delete[] vertex_color2;
	delete[] vertex3;
	delete[] mymesh3;
	delete[] vertex_color3;
	delete[] vertex4;
	delete[] mymesh4;
	delete[] vertex_color4;
	delete[] vertex5;
	delete[] mymesh5;
	delete[] vertex_color5;
	delete[] vertex6;
	delete[] mymesh6;
	delete[] vertex_color6;
	delete[] vertex7;
	delete[] mymesh7;
	delete[] vertex_color7;
	delete[] vertex8;
	delete[] mymesh8;
	delete[] vertex_color8;
	delete[] vertex9;
	delete[] mymesh9;
	delete[] vertex_color9;
	delete[] vertex10;
	delete[] mymesh10;
	delete[] vertex_color10;
	delete[] vertex11;
	delete[] mymesh11;
	delete[] vertex_color11;


	delete[] rock1;
	delete[] rockmesh1;
	delete[] rock_color1;
	delete[] rock2;
	delete[] rockmesh2;
	delete[] rock_color2;
	delete[] rock3;
	delete[] rockmesh3;
	delete[] rock_color3;
	delete[] rock4;
	delete[] rockmesh4;
	delete[] rock_color4;

	delete[] wood1;
	delete[] wood2;
	delete[] wood3;
	delete[] wood4;
	delete[] wood5;
	delete[] wood6;
	delete[] wood7;
	delete[] wood8;
	delete[] wood9;
	delete[] wood10;
	delete[] wood11;
	delete[] wood12;
	delete[] wood13;

	delete[] wood_color1;
	delete[] wood_color2;
	delete[] wood_color3;
	delete[] wood_color4;
	delete[] wood_color5;
	delete[] wood_color6;
	delete[] wood_color7;
	delete[] wood_color8;
	delete[] wood_color9;
	delete[] wood_color10;
	delete[] wood_color11;
	delete[] wood_color12;
	delete[] wood_color13;
	
	delete[] woodmesh1;
	delete[] woodmesh2;
	delete[] woodmesh3;
	delete[] woodmesh4;
	delete[] woodmesh5;
	delete[] woodmesh6;
	delete[] woodmesh7;
	delete[] woodmesh8;
	delete[] woodmesh9;
	delete[] woodmesh10;
	delete[] woodmesh11;
	delete[] woodmesh12;
	delete[] woodmesh13;

	delete[] tree1;
	delete[] tree2;
	delete[] tree3;
	delete[] tree4;
	delete[] tree5;
	delete[] tree6;
	delete[] tree7;
	delete[] tree8;
	delete[] tree9;
	delete[] tree10;
	delete[] tree11;
	delete[] tree12;

	delete[] tree_color1;
	delete[] tree_color2;
	delete[] tree_color3;
	delete[] tree_color4;
	delete[] tree_color5;
	delete[] tree_color6;
	delete[] tree_color7;
	delete[] tree_color8;
	delete[] tree_color9;
	delete[] tree_color10;
	delete[] tree_color11;
	delete[] tree_color12;

	delete treemesh1;
	delete treemesh2;
	delete treemesh3;
	delete treemesh4;
	delete treemesh5;
	delete treemesh6;
	delete treemesh7;
	delete treemesh8;
	delete treemesh9;
	delete treemesh10;
	delete treemesh11;
	delete treemesh12;


	delete[] stone1;
	delete[] stone2;
	delete[] stone3;
	delete[] stone4;
	delete[] stone5;
	delete[] stone6;
	delete[] stone7;
	delete[] stone8;
	delete[] stone9;
	delete[] stone10;

	delete[] stone_color1;
	delete[] stone_color2;
	delete[] stone_color3;
	delete[] stone_color4;
	delete[] stone_color5;
	delete[] stone_color6;
	delete[] stone_color7;
	delete[] stone_color8;
	delete[] stone_color9;
	delete[] stone_color10;
	
	delete[] stonemesh1;
	delete[] stonemesh2;
	delete[] stonemesh3;
	delete[] stonemesh4;
	delete[] stonemesh5;
	delete[] stonemesh6;
	delete[] stonemesh7;
	delete[] stonemesh8;
	delete[] stonemesh9;
	delete[] stonemesh10;

	delete[] cloud1;
	delete[] cloud2;
	delete[] cloud3;
	delete[] cloud4;

	delete[] cloudmesh1;
	delete[] cloudmesh2;
	delete[] cloudmesh3;
	delete[] cloudmesh4;



}

void makefile(string filepath) {
	ifstream inputFile(filepath);
	if (!inputFile.is_open()) {
		cout << "파일을 열 수 없습니다." << std::endl;
		return;
	}

	map<string, ofstream> outputFileMap;
	string line;

	while (getline(inputFile, line)) {
		if (line.length() >= 2) {
			string firstTwoChars = line.substr(0, 2);

			// 새로운 문자열이 나타나면 새로운 출력 파일 열기
			if (outputFileMap.find(firstTwoChars) == outputFileMap.end() && (firstTwoChars == "f " || firstTwoChars =="vt")) {
				string outputFileName = filepath.substr(0, filepath.length() - 4) + firstTwoChars + ".txt";
				outputFileMap[firstTwoChars].open(outputFileName);
			}

			// 해당 문자열의 출력 파일로 줄 쓰기
			outputFileMap[firstTwoChars] << line << std::endl;
		}
	}

	// 모든 출력 파일 닫기
	for (auto& pair : outputFileMap) {
		pair.second.close();
	}

	inputFile.close();

}


void readOBJ(string filepath, Vertex* vertex) {

	ifstream file(filepath);
	float x, y, z;
	int j;
	char ch;
	if (file.is_open()) {
		string line;
		j = 0;
		while (j < 100000 && getline(file, line)) {
			istringstream iss(line);
			if (iss >> ch >> x >> y >> z && ch == 'v') {
				vertex[j].X = x / scale;
				vertex[j].Y = y / scale;
				vertex[j].Z = z / scale;
				if (vertex[j].Z < zmin)
					zmin = vertex[j].Z;
				if (vertex[j].Z > zmax)
					zmax = vertex[j].Z;
				j++;

			}
		}
	}
	file.close();

}
void readVT(string filepath, Vertex* vertex_color) {
	ifstream file2(filepath);
	if (file2.is_open())
	{
		int j = 0;
		while (j < 10000)
		{
			string line;
			if (getline(file2, line))
			{
				if (line.substr(0, 2) == "vt")
				{
					istringstream iss(line);
					string prefix;
					float x, y;
					if (iss >> prefix >> x >> y)
					{
						vertex_color[j].X = x;
						vertex_color[j].Y = y;
						j++;
					}
				}
			}
			else
			{
				break;
			}
		}
		file2.close();
		cout << filepath << " \n";
	}
	else
	{
		cout << filepath << "파일을 열 수 없습니다." << endl;
	}
}
int readF(string filepath, MMesh* mymesh) {
	ifstream file3(filepath);
	if (file3.is_open())
	{
		int j = 0;
		string line;
		while (j < 10000)
		{

			if (getline(file3, line))
			{
				istringstream iss(line);
				string prefix;
				string v1, v2, v3;
				if (iss >> prefix >> v1 >> v2 >> v3)
				{
					if (prefix == "f")
					{
						int pos1 = v1.find('/');
						int pos2 = v2.find('/');
						int pos3 = v3.find('/');

						int post1_y = v1.find('/', pos1 + 1);
						int post2_y = v1.find('/', pos2 + 1);
						int post3_y = v1.find('/', pos3 + 1);

						// Parse vertex indices
						mymesh[j].V1 = stoi(v1.substr(0, pos1));
						mymesh[j].V2 = stoi(v2.substr(0, pos2));
						mymesh[j].V3 = stoi(v3.substr(0, pos3));

						mymesh[j].T1 = stoi(v1.substr(pos1 + 1, post1_y - pos1 - 1));
						mymesh[j].T2 = stoi(v2.substr(pos2 + 1, post2_y - pos1 - 1));
						mymesh[j].T3 = stoi(v3.substr(pos3 + 1, post3_y - pos1 - 1));
						//cout << mymesh[j].V1 << " " << mymesh[j].V2 << " " << mymesh[j].V3 << "\n";
						j++;
					}
				}

			}
			else
			{
				cout << filepath << " \n";
				return j;
				break;
			}
		}
		file3.close();
	}
	else
	{
		cout << filepath << "파일을 열 수 없습니다." << endl;
	}
}
int readFcloud(string filepath, MMesh* mymesh) {
	ifstream file3(filepath);
	if (file3.is_open())
	{
		int j = 0;
		string line;
		while (j < 10000)
		{

			if (getline(file3, line))
			{
				istringstream iss(line);
				string prefix;
				string v1, v2, v3;
				if (iss >> prefix >> v1 >> v2 >> v3)
				{
					if (prefix == "f")
					{
						int pos1 = v1.find('/');
						int pos2 = v2.find('/');
						int pos3 = v3.find('/');

						// Parse vertex indices
						mymesh[j].V1 = stoi(v1.substr(0, pos1));
						mymesh[j].V2 = stoi(v2.substr(0, pos2));
						mymesh[j].V3 = stoi(v3.substr(0, pos3));
						j++;
					}
				}

			}
			else
			{
				cout << filepath << " \n";
				return j;
				break;
			}
		}
		file3.close();
	}
	else
	{
		cout << filepath << "파일을 열 수 없습니다." << endl;
	}
}

int main(int argc, char* argv[])
{

	vertex = new Vertex[1000];
	vertex_color = new Vertex[1000];
	mymesh = new MMesh[1000];


	readOBJ("trees/base.obj", vertex);
	readVT("trees/basevt.txt", vertex_color);
	len = readF("trees/basef .txt", mymesh);

	/*FILE* fp;
	fp = fopen("apple.obj", "r");
	int count = 0;
	int num = 0;
	char ch;
	float x, y, z;

	for (j = 0; j < 100000; j = j + 1)
	{
		count = fscanf(fp, "v %f %f %f /n",&x, &y, &z);
		if (count == 3 )
		{
			vertex[j].X = x / scale;
			vertex[j].Y = y / scale;
			vertex[j].Z = z / scale;
			if (vertex[j].Z < zmin)
				zmin = vertex[j].Z;
			if (vertex[j].Z > zmax)
				zmax = vertex[j].Z;
		}
		else
			break;
	}
	fclose(fp);


	fp = fopen("applet.txt", "r");
	
	for (j = 0; j < 100000; j = j + 1)
	{
		count = fscanf(fp, "vt %f %f %f /n", &x, &y, &z);
		if (count == 3)
		{
			vertex_color[j].X = x;
			vertex_color[j].Y = y;
			vertex_color[j].Z = z;
		}
		else
			break;
	}
	fclose(fp);

	FILE* fpp;
	fpp = fopen("applef2.txt", "r");
	float x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;

	for (j = 0; j < 100000; j = j + 1)
	{
		count = fscanf(fp, "f %f/%f/%f %f/%f/%f %f/%f/%f %f/%f/%f /n", &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3, &x4, &y4, &z4);
		if (count == 12 )
		{
			mymesh[j].V1 = x1;
			mymesh[j].V2 = x2;
			mymesh[j].V3 = x3;
			mymesh[j].V4 = x4;
			mymesh[j].T1 = y1;
			mymesh[j].T2 = y2;
			mymesh[j].T3 = y3;
			mymesh[j].T4 = y4;
		}
		else
			break;
	}
	fclose(fpp);*/

	vertex2 = new Vertex[1000];
	vertex_color2 =  new Vertex[1000];
	mymesh2 = new MMesh[1000];

	readOBJ("trees/grass1.obj", vertex2);
	readVT("trees/grass1vt.txt", vertex_color2);
	len2 = readF("trees/grass1f .txt", mymesh2);



	vertex3 = new Vertex[1000];
	vertex_color3 = new Vertex[1000];
	mymesh3 = new MMesh[1000];

	readOBJ("trees/grass2.obj", vertex3);
	readVT("trees/grass2vt.txt", vertex_color3);
	len3 = readF("trees/grass2f .txt", mymesh3);



	vertex4 = new Vertex[1000];
	vertex_color4 = new Vertex[1000];
	mymesh4 = new MMesh[1000];

	readOBJ("trees/grass3.obj", vertex4);
	readVT("trees/grass3vt.txt", vertex_color4);
	len4 = readF("trees/grass3f .txt", mymesh4);



	vertex5 = new Vertex[1000];
	vertex_color5 = new Vertex[1000];
	mymesh5 = new MMesh[1000];

	readOBJ("trees/grass4.obj", vertex5);
	readVT("trees/grass4vt.txt", vertex_color5);
	len5 = readF("trees/grass4f .txt", mymesh5);


	vertex6 = new Vertex[1000];
	vertex_color6 = new Vertex[1000];
	mymesh6 = new MMesh[1000];

	readOBJ("trees/grass5.obj", vertex6);
	readVT("trees/grass5vt.txt", vertex_color6);
	len6 = readF("trees/grass5f .txt", mymesh6);


	vertex7 = new Vertex[1000];
	vertex_color7 = new Vertex[1000];
	mymesh7 = new MMesh[1000];

	readOBJ("trees/grass6.obj", vertex7);
	readVT("trees/grass6vt.txt", vertex_color7);
	len7 = readF("trees/grass6f .txt", mymesh7);


	vertex8 = new Vertex[1000];
	vertex_color8 = new Vertex[1000];
	mymesh8 = new MMesh[1000];

	readOBJ("trees/grass7.obj", vertex8);
	readVT("trees/grass7vt.txt", vertex_color8);
	len8 = readF("trees/grass7f .txt", mymesh8);


	vertex9 = new Vertex[1000];
	vertex_color9 = new Vertex[1000];
	mymesh9 = new MMesh[1000];

	readOBJ("trees/grass8.obj", vertex9);
	readVT("trees/grass8vt.txt", vertex_color9);
	len9 = readF("trees/grass8f .txt", mymesh9);


	vertex10 = new Vertex[1000];
	vertex_color10 = new Vertex[1000];
	mymesh10 = new MMesh[1000];

	readOBJ("trees/grass9.obj", vertex10);
	readVT("trees/grass9vt.txt", vertex_color10);
	len10 = readF("trees/grass9f .txt", mymesh10);


	vertex11 = new Vertex[1000];
	vertex_color11 = new Vertex[1000];
	mymesh11 = new MMesh[1000];

	readOBJ("trees/grass10.obj", vertex11);
	readVT("trees/grass10vt.txt", vertex_color11);
	len11 = readF("trees/grass10f .txt", mymesh11);


	rock1 = new Vertex[1000];
	rock_color1 = new Vertex[1000];
	rockmesh1 = new MMesh[1000];

	readOBJ("trees/rock.obj", rock1);
	readVT("trees/rockvt.txt", rock_color1);
	len12 = readF("trees/rockf .txt", rockmesh1);

	rock2 = new Vertex[1000];
	rock_color2 = new Vertex[1000];
	rockmesh2 = new MMesh[1000];

	readOBJ("trees/rock2.obj", rock2);
	readVT("trees/rock2vt.txt", rock_color2);
	len13 = readF("trees/rock2f .txt", rockmesh2);

	rock3 = new Vertex[1000];
	rock_color3 = new Vertex[1000];
	rockmesh3 = new MMesh[1000];

	readOBJ("trees/rock3.obj", rock3);
	readVT("trees/rock3vt.txt", rock_color3);
	len14 = readF("trees/rock3f .txt", rockmesh3);

	rock4 = new Vertex[1000];
	rock_color4 = new Vertex[1000];
	rockmesh4 = new MMesh[1000];

	readOBJ("trees/rock4.obj", rock4);
	readVT("trees/rock4vt.txt", rock_color4);
	len15 = readF("trees/rock4f .txt", rockmesh4);


	wood1 = new Vertex[1000];
	wood_color1 = new Vertex[1000];
	woodmesh1 = new MMesh[1000];
	readOBJ("trees/wood.obj", wood1);
	readVT("trees/woodvt.txt", wood_color1);
	lwood1 = readF("trees/woodf .txt", woodmesh1);

	wood2 = new Vertex[1000];
	wood_color2 = new Vertex[1000];
	woodmesh2 = new MMesh[1000];
	readOBJ("trees/wood2.obj", wood2);
	readVT("trees/wood2vt.txt", wood_color2);
	lwood2 = readF("trees/wood2f .txt", woodmesh2);

	wood3 = new Vertex[1000];
	wood_color3 = new Vertex[1000];
	woodmesh3 = new MMesh[1000];
	readOBJ("trees/wood3.obj", wood3);
	readVT("trees/wood3vt.txt", wood_color3);
	lwood3 = readF("trees/wood3f .txt", woodmesh3);

	wood4 = new Vertex[1000];
	wood_color4 = new Vertex[1000];
	woodmesh4 = new MMesh[1000];
	readOBJ("trees/wood4.obj", wood4);
	readVT("trees/wood4vt.txt", wood_color4);
	lwood4 = readF("trees/wood4f .txt", woodmesh4);

	wood5 = new Vertex[1000];
	wood_color5 = new Vertex[1000];
	woodmesh5 = new MMesh[1000];
	readOBJ("trees/wood5.obj", wood5);
	readVT("trees/wood5vt.txt", wood_color5);
	lwood5 = readF("trees/wood5f .txt", woodmesh5);

	wood6 = new Vertex[1000];
	wood_color6 = new Vertex[1000];
	woodmesh6 = new MMesh[1000];
	readOBJ("trees/wood6.obj", wood6);
	readVT("trees/wood6vt.txt", wood_color6);
	lwood6 = readF("trees/wood6f .txt", woodmesh6);

	wood7 = new Vertex[1000];
	wood_color7 = new Vertex[1000];
	woodmesh7 = new MMesh[1000];
	readOBJ("trees/wood7.obj", wood7);
	readVT("trees/wood7vt.txt", wood_color7);
	lwood7 = readF("trees/wood7f .txt", woodmesh7);

	wood8 = new Vertex[1000];
	wood_color8 = new Vertex[1000];
	woodmesh8 = new MMesh[1000];
	readOBJ("trees/wood8.obj", wood8);
	readVT("trees/wood8vt.txt", wood_color8);
	lwood8 = readF("trees/wood8f .txt", woodmesh8);

	wood9 = new Vertex[1000];
	wood_color9 = new Vertex[1000];
	woodmesh9 = new MMesh[1000];
	readOBJ("trees/wood9.obj", wood9);
	readVT("trees/wood9vt.txt", wood_color9);
	lwood9 = readF("trees/wood9f .txt", woodmesh9);

	wood10 = new Vertex[1000];
	wood_color10 = new Vertex[1000];
	woodmesh10 = new MMesh[1000];
	readOBJ("trees/wood10.obj", wood10);
	readVT("trees/wood10vt.txt", wood_color10);
	lwood10 = readF("trees/wood10f .txt", woodmesh10);

	wood11 = new Vertex[1000];
	wood_color11 = new Vertex[1000];
	woodmesh11 = new MMesh[1000];
	readOBJ("trees/wood11.obj", wood11);
	readVT("trees/wood11vt.txt", wood_color11);
	lwood11 = readF("trees/wood11f .txt", woodmesh11);
	wood12 = new Vertex[1000];
	wood_color12 = new Vertex[1000];
	woodmesh12 = new MMesh[1000];
	readOBJ("trees/wood12.obj", wood12);
	readVT("trees/wood12vt.txt", wood_color12);
	lwood12 = readF("trees/wood12f .txt", woodmesh12);
	wood13 = new Vertex[1000];
	wood_color13 = new Vertex[1000];
	woodmesh13 = new MMesh[1000];
	readOBJ("trees/wood13.obj", wood13);
	readVT("trees/wood13vt.txt", wood_color13);
	lwood13 = readF("trees/wood13f .txt", woodmesh13);


	tree1 = new Vertex[1000];
	tree_color1 = new Vertex[1000];
	treemesh1 = new MMesh[1000];
	readOBJ("trees/tree1.obj", tree1);
	readVT("trees/tree1vt.txt", tree_color1);
	ltree1 = readF("trees/tree1f .txt", treemesh1);
	tree2 = new Vertex[1000];
	tree_color2 = new Vertex[1000];
	treemesh2 = new MMesh[1000];
	readOBJ("trees/tree2.obj", tree2);
	readVT("trees/tree2vt.txt", tree_color2);
	ltree2 = readF("trees/tree2f .txt", treemesh2);
	tree3 = new Vertex[1000];
	tree_color3 = new Vertex[1000];
	treemesh3 = new MMesh[1000];
	readOBJ("trees/tree3.obj", tree3);
	readVT("trees/tree3vt.txt", tree_color3);
	ltree3 = readF("trees/tree3f .txt", treemesh3);

	tree4 = new Vertex[1000];
	tree_color4 = new Vertex[1000];
	treemesh4 = new MMesh[1000];
	readOBJ("trees/tree4.obj", tree4);
	readVT("trees/tree4vt.txt", tree_color4);
	ltree4 = readF("trees/tree4f .txt", treemesh4);
	tree5 = new Vertex[1000];
	tree_color5 = new Vertex[1000];
	treemesh5 = new MMesh[1000];
	readOBJ("trees/tree5.obj", tree5);
	readVT("trees/tree5vt.txt", tree_color5);
	ltree5 = readF("trees/tree5f .txt", treemesh5);
	tree6 = new Vertex[1000];
	tree_color6 = new Vertex[1000];
	treemesh6 = new MMesh[1000];
	readOBJ("trees/tree6.obj", tree6);
	readVT("trees/tree6vt.txt", tree_color6);
	ltree6 = readF("trees/tree6f .txt", treemesh6);

	tree7 = new Vertex[1000];
	tree_color7 = new Vertex[1000];
	treemesh7 = new MMesh[1000];
	readOBJ("trees/tree7.obj", tree7);
	readVT("trees/tree7vt.txt", tree_color7);
	ltree7 = readF("trees/tree7f .txt", treemesh7);
	tree8 = new Vertex[1000];
	tree_color8 = new Vertex[1000];
	treemesh8 = new MMesh[1000];
	readOBJ("trees/tree8.obj", tree8);
	readVT("trees/tree8vt.txt", tree_color8);
	ltree8 = readF("trees/tree8f .txt", treemesh8);
	tree9 = new Vertex[1000];
	tree_color9 = new Vertex[1000];
	treemesh9 = new MMesh[1000];
	readOBJ("trees/tree9.obj", tree9);
	readVT("trees/tree9vt.txt", tree_color9);
	ltree9 = readF("trees/tree9f .txt", treemesh9);

	tree10 = new Vertex[1000];
	tree_color10 = new Vertex[1000];
	treemesh10 = new MMesh[1000];
	readOBJ("trees/tree10.obj", tree10);
	readVT("trees/tree10vt.txt", tree_color10);
	ltree10 = readF("trees/tree10f .txt", treemesh10);
	tree11 = new Vertex[1000];
	tree_color11 = new Vertex[1000];
	treemesh11 = new MMesh[1000];
	readOBJ("trees/tree11.obj", tree11);
	readVT("trees/tree1vt.txt", tree_color11);
	ltree11 = readF("trees/tree11f .txt", treemesh11);
	tree12 = new Vertex[1000];
	tree_color12 = new Vertex[1000];
	treemesh12 = new MMesh[1000];
	readOBJ("trees/tree12.obj", tree12);
	readVT("trees/tree12vt.txt", tree_color12);
	ltree12 = readF("trees/tree12f .txt", treemesh12);

	stone1 = new Vertex[1000];
	stone_color1 = new Vertex[1000];
	stonemesh1 = new MMesh[1000];
	readOBJ("trees/stone1.obj", stone1);
	readVT("trees/stone1vt.txt", stone_color1);
	lstone1 = readF("trees/stone1f .txt", stonemesh1);
	stone2 = new Vertex[1000];
	stone_color2 = new Vertex[1000];
	stonemesh2 = new MMesh[1000];
	readOBJ("trees/stone2.obj", stone2);
	readVT("trees/stone2vt.txt", stone_color2);
	lstone2 = readF("trees/stone2f .txt", stonemesh2);
	stone3 = new Vertex[1000];
	stone_color3 = new Vertex[1000];
	stonemesh3 = new MMesh[1000];
	readOBJ("trees/stone3.obj", stone3);
	readVT("trees/stone3vt.txt", stone_color3);
	lstone3 = readF("trees/stone3f .txt", stonemesh3);

	stone4 = new Vertex[1000];
	stone_color4 = new Vertex[1000];
	stonemesh4 = new MMesh[1000];
	readOBJ("trees/stone4.obj", stone4);
	readVT("trees/stone4vt.txt", stone_color4);
	lstone4 = readF("trees/stone1f .txt", stonemesh4);
	stone5 = new Vertex[1000];
	stone_color5 = new Vertex[1000];
	stonemesh5 = new MMesh[1000];
	readOBJ("trees/stone5.obj", stone5);
	readVT("trees/stone5vt.txt", stone_color5);
	lstone5 = readF("trees/stone5f .txt", stonemesh5);
	stone6 = new Vertex[1000];
	stone_color6 = new Vertex[1000];
	stonemesh6 = new MMesh[1000];
	readOBJ("trees/stone6.obj", stone6);
	readVT("trees/stone6vt.txt", stone_color6);
	lstone6 = readF("trees/stone6f .txt", stonemesh6);

	stone7 = new Vertex[1000];
	stone_color7 = new Vertex[1000];
	stonemesh7 = new MMesh[1000];
	readOBJ("trees/stone7.obj", stone1);
	readVT("trees/stone7vt.txt", stone_color7);
	lstone7 = readF("trees/stone7f .txt", stonemesh7);
	stone8 = new Vertex[1000];
	stone_color8 = new Vertex[1000];
	stonemesh8 = new MMesh[1000];
	readOBJ("trees/stone8.obj", stone8);
	readVT("trees/stone8vt.txt", stone_color8);
	lstone8 = readF("trees/stone8f .txt", stonemesh8);
	stone9 = new Vertex[1000];
	stone_color9 = new Vertex[1000];
	stonemesh9 = new MMesh[1000];
	readOBJ("trees/stone9.obj", stone9);
	readVT("trees/stone9vt.txt", stone_color9);
	lstone9 = readF("trees/stone9f .txt", stonemesh9);
	stone10 = new Vertex[1000];
	stone_color10 = new Vertex[1000];
	stonemesh10 = new MMesh[1000];
	readOBJ("trees/stone10.obj", stone10);
	readVT("trees/stone10vt.txt", stone_color10);
	lstone10 = readF("trees/stone10f .txt", stonemesh10);

	cloud1 = new Vertex[1000];
	cloudmesh1 = new MMesh[1000];
	readOBJ("trees/cloud1.obj", cloud1);
	lcloud1 = readFcloud("trees/cloud1f .txt", cloudmesh1);

	cloud2 = new Vertex[1000];
	cloudmesh2 = new MMesh[1000];
	readOBJ("trees/cloud2.obj", cloud2);
	lcloud2 = readFcloud("trees/cloud2f .txt", cloudmesh2);


	cloud3 = new Vertex[1000];
	cloudmesh3 = new MMesh[1000];
	readOBJ("trees/cloud3.obj", cloud3);
	lcloud3 = readFcloud("trees/cloud3f .txt", cloudmesh3);


	cloud4 = new Vertex[1000];
	cloudmesh4 = new MMesh[1000];
	readOBJ("trees/cloud4.obj", cloud4);
	lcloud4 = readFcloud("trees/cloud4f .txt", cloudmesh4);

	wall = new Vertex[100];
	readOBJ("trees/wall.obj", wall);

	bird = new Vertex[1000];
	bird_color = new Vertex[1000];
	birdmesh = new MMesh[1000];
	readOBJ("trees/bullfinch.obj", bird);
	readVT("trees/buulfinchvt.txt", bird_color);
	birdl = readF("trees/bullfinchf .txt", birdmesh);

	InitializeWindow(argc, argv);

	display();

	glutMainLoop();
	deleteVertex();
	return 0;
}