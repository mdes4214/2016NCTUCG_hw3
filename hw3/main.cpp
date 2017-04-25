#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /* offsetof */
#include <math.h>
#include "../GL/glew.h"
#include "../GL/freeglut.h"
#include "../shader_lib/shader.h"

#include "glm/glm.h"
#include "glm_helper.h"

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512
#define MOUSE_MOVE_SPEED 1

void init(void);
void display(void);
void displayText(GLfloat x, GLfloat y, GLfloat r, GLfloat g, GLfloat b, const char *string);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void mouseWheel(int wheel, int direction, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMove(int x, int y);
void idle(void);
float cos_clamp(float x);
float dot(const GLfloat *v1, const GLfloat *v2);
void cross(GLfloat *v, const GLfloat *v1, const GLfloat *v2);
glm::mat4 getViewMatrix(glm::vec3 eye, glm::vec3 target, glm::vec3 up);

GLMmodel *XiaoPingGuo;
GLuint groupNum;
GLfloat *vntArray;
GLuint *vao, *vbo;
GLfloat light_pos[] = { 10.0, 10.0, 0.0 };
GLfloat mouse_pos0[3], mouse_pos1[3];
GLfloat mouse_move[16] = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
};
const GLfloat identity[16] = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
};
float eyex = 0.0, eyey = 0.0, eyez = 3.0;
float lightx = 10.0, lighty = 10.0, lightz = 0.0;
float rot = 0.0;
GLuint program;
int objChoose = 0, mode = 0, rot_toggle = 1;
char text[20] = "default";

float cos_clamp(float x)
{
	if (x < -1)
		return -1.0;
	else if (x > 1)
		return 1.0;
	else
		return x;
}

float dot(const GLfloat *v1, const GLfloat *v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void cross(GLfloat *v, const GLfloat *v1, const GLfloat *v2)
{
	v[0] = v1[1] * v2[2] - v1[2] * v2[1];
	v[1] = -(v1[0] * v2[2] - v1[2] * v2[0]);
	v[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void normalize(GLfloat *v)
{
	float norm = sqrtf(dot(v, v));
	v[0] /= norm;
	v[1] /= norm;
	v[2] /= norm;
}

glm::mat4 getViewMatrix(glm::vec3 eye, glm::vec3 target, glm::vec3 up)
{
	glm::vec3 zaxis = glm::normalize(eye - target); // forward
	glm::vec3 xaxis = glm::normalize(glm::cross(up, zaxis)); // right
	glm::vec3 yaxis = glm::cross(zaxis, xaxis); // up

	glm::mat4 viewMatrix = {
		glm::vec4(xaxis.x,			   yaxis.x,				  zaxis.x, 0),
		glm::vec4(xaxis.y,			   yaxis.y,				  zaxis.y, 0),
		glm::vec4(xaxis.z,			   yaxis.z,				  zaxis.z, 0),
		glm::vec4(-glm::dot(xaxis, eye), -glm::dot(yaxis, eye), -glm::dot(zaxis, eye), 1)
	};

	return viewMatrix;
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("OpenGL HW3 - 0556619 - default");
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	glewInit();

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(mouseWheel);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	glutMainLoop();

	glmDelete(XiaoPingGuo);
	free(vao);
	free(vbo);
	return 0;
}

void init(void) {
	// OpenGL, GLSL version
	printf("OpenGL version: %s\n", glGetString(GL_VERSION));
	printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("\n");

	// choose object
	printf("Please choose one object:\n");
	printf("1 apple\n");
	printf("2 wolf\n");
	printf("3 falcon\n");
	printf("4 sponza\n");
	scanf("%d", &objChoose);
	switch (objChoose) {
	case 2:
		XiaoPingGuo = glmReadOBJ("Model/wolf/wolf.obj");
		break;
	case 3:
		XiaoPingGuo = glmReadOBJ("Model/falcon/falcon.obj");
		break;
	case 4:
		XiaoPingGuo = glmReadOBJ("Model/sponza/sponza.obj");
		break;
	default:
		XiaoPingGuo = glmReadOBJ("Model/apple/apple.obj");
	}

	glmUnitize(XiaoPingGuo);
	glmFacetNormals(XiaoPingGuo);
	glmVertexNormals(XiaoPingGuo, 90.0, GL_FALSE);

	//print_model_info(XiaoPingGuo);
	//printf("v num: %d\nn num: %d\ntriangle num: %d\ntexcoords num: %d\n", XiaoPingGuo->numvertices, XiaoPingGuo->numnormals, XiaoPingGuo->numtriangles, XiaoPingGuo->numtexcoords);
	//printf("tex num: %d\n", XiaoPingGuo->numtextures);
	/*printf("group num: %d\n", XiaoPingGuo->numgroups);
	GLMgroup *p = NULL;
	for (int i = 0; i < XiaoPingGuo->numgroups; ++i) {
	if (i == 0) {
	printf("%d: %d\n", i, XiaoPingGuo->groups->material);
	p = XiaoPingGuo->groups->next;
	}
	else
	printf("%d: %d\n", i, p->material);
	}
	printf("%f %f %f\n", XiaoPingGuo->vertices[3 * 5231], XiaoPingGuo->vertices[3 * 5231 + 1], XiaoPingGuo->vertices[3 * 5231 + 2]);*/
	//printf("name: %s\n", XiaoPingGuo->materials[7].name);
	//printf("tex index: %d\n", XiaoPingGuo->materials[7].map_diffuse);
	//printf("tex id: %d\n", XiaoPingGuo->textures[XiaoPingGuo->materials[7].map_diffuse].id);
	//system("PAUSE");

	// shader
	GLuint vert = createShader("Shaders/hw3.vert", "vertex");
	GLuint frag = createShader("Shaders/hw3.frag", "fragment");
	program = createProgram(vert, frag);

	// vao, vbo
	// 1 triangle has 3 vertices(including v, n, t)
	// 1 vertex has 3 data(x, y, z)
	// 1 normal has 3 data(x, y, z)
	// 1 texcoordinate has 2 data(x, y)
	GLMgroup *pGroup = NULL;
	groupNum = XiaoPingGuo->numgroups;
	vao = (GLuint*)malloc(sizeof(GLuint) * groupNum);
	vbo = (GLuint*)malloc(sizeof(GLuint) * groupNum);
	for (int ivao = 0; ivao < groupNum; ++ivao) {
		if (ivao == 0)
			pGroup = XiaoPingGuo->groups;
		else
			pGroup = pGroup->next;
		int vntSize = pGroup->numtriangles * 3 * 3 * 2 + pGroup->numtriangles * 3 * 2;
		vntArray = (GLfloat*)malloc(sizeof(GLfloat) * vntSize);
		for (int i = 0, k = 0; i < pGroup->numtriangles; ++i) {
			int indexTri = pGroup->triangles[i];
			int indexV[3] = { XiaoPingGuo->triangles[indexTri].vindices[0], XiaoPingGuo->triangles[indexTri].vindices[1], XiaoPingGuo->triangles[indexTri].vindices[2] };
			int indexN[3] = { XiaoPingGuo->triangles[indexTri].nindices[0], XiaoPingGuo->triangles[indexTri].nindices[1], XiaoPingGuo->triangles[indexTri].nindices[2] };
			int indexT[3] = { XiaoPingGuo->triangles[indexTri].tindices[0], XiaoPingGuo->triangles[indexTri].tindices[1], XiaoPingGuo->triangles[indexTri].tindices[2] };
			for (int j = 0; j < 3; ++j, ++k) {
				vntArray[8 * k] = XiaoPingGuo->vertices[3 * indexV[j]];
				vntArray[8 * k + 1] = XiaoPingGuo->vertices[3 * indexV[j] + 1];
				vntArray[8 * k + 2] = XiaoPingGuo->vertices[3 * indexV[j] + 2];
				vntArray[8 * k + 3] = XiaoPingGuo->normals[3 * indexN[j]];
				vntArray[8 * k + 4] = XiaoPingGuo->normals[3 * indexN[j] + 1];
				vntArray[8 * k + 5] = XiaoPingGuo->normals[3 * indexN[j] + 2];
				vntArray[8 * k + 6] = XiaoPingGuo->texcoords[2 * indexT[j]];
				vntArray[8 * k + 7] = XiaoPingGuo->texcoords[2 * indexT[j] + 1];
			}
		}
		glGenBuffers(1, &vbo[ivao]);
		glGenVertexArrays(1, &vao[ivao]);
		glBindVertexArray(vao[ivao]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[ivao]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vntSize, vntArray, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(NULL);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	free(vntArray);
}

void displayText(GLfloat x, GLfloat y, GLfloat r, GLfloat g, GLfloat b, const char *string) {
	int l = strlen(string);

	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	for (int i = 0; i < l; ++i) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}

void display(void)
{
	light_pos[0] = lightx;
	light_pos[1] = lighty;
	light_pos[2] = lightz;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1e-2, 1e2);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);

	//GLfloat view[16];
	//glGetFloatv(GL_MODELVIEW_MATRIX, view);

	glMultMatrixf(mouse_move);
	glRotatef(rot, 0.0, 1.0, 0.0);

	// ModelView, Projection, View Matrix 
	GLfloat modelView[16], projection[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
	glGetFloatv(GL_PROJECTION_MATRIX, projection);
	glm::mat4 viewMatrix = getViewMatrix(glm::vec3(eyex, eyey, eyez), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	GLfloat *view = glm::value_ptr(viewMatrix);
	
	// uniform
	glUseProgram(program);
	GLint loc = glGetUniformLocation(program, "ModelView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, modelView);
	loc = glGetUniformLocation(program, "Projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection);
	loc = glGetUniformLocation(program, "View");
	glUniformMatrix4fv(loc, 1, GL_FALSE, view);
	loc = glGetUniformLocation(program, "lightPos_in");
	glUniform3fv(loc, 1, light_pos);
	loc = glGetUniformLocation(program, "mode");
	glUniform1i(loc, mode);

	// texture
	GLint locTex = glGetUniformLocation(program, "texture");
	GLMgroup *pGroup = NULL;
	GLuint *tex = (GLuint*)malloc(sizeof(GLuint) * groupNum);
	for (int ivao = 0; ivao < groupNum; ++ivao) {
		if (ivao == 0)
			pGroup = XiaoPingGuo->groups;
		else
			pGroup = pGroup->next;
		GLuint indexMtl = pGroup->material;
		GLuint indexTex = XiaoPingGuo->materials[indexMtl].map_diffuse;
		tex[ivao] = XiaoPingGuo->textures[indexTex].id;	
	}
	glActiveTexture(GL_TEXTURE0 + 0);

	// draw object
	pGroup = NULL;
	for (int ivao = 0; ivao < groupNum; ++ivao) {
		if (ivao == 0)
			pGroup = XiaoPingGuo->groups;
		else
			pGroup = pGroup->next;
		glBindTexture(GL_TEXTURE_2D, tex[ivao]);
		glUniform1i(locTex, 0);
		glBindVertexArray(vao[ivao]);
		glDrawArrays(GL_TRIANGLES, 0, pGroup->numtriangles * 3);
		glBindVertexArray(NULL);
		glBindTexture(GL_TEXTURE_2D, NULL);
	}
	glUseProgram(NULL);
	/*glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glmDraw(XiaoPingGuo, GLM_SMOOTH | GLM_TEXTURE);*/

	// window itle & text
	switch (mode) {
	case 1:
		glutSetWindowTitle("OpenGL HW3 - 0556619 - Flat shading");
		sprintf(text, "Flat shading");
		break;
	case 2:
		glutSetWindowTitle("OpenGL HW3 - 0556619 - Phong shading");
		sprintf(text, "Phong shading");
		break;
	case 3:
		glutSetWindowTitle("OpenGL HW3 - 0556619 - Gouraud shading");
		sprintf(text, "Gouraud shading");
		break;
	case 4:
		glutSetWindowTitle("OpenGL HW3 - 0556619 - Gooch shading");
		sprintf(text, "Gooch shading");
		break;
	default:
		glutSetWindowTitle("OpenGL HW3 - 0556619 - default");
		sprintf(text, "default");
	}
	displayText(0.5, 0.7, 1.0, 1.0, 1.0, text);

	glutSwapBuffers();
	rot += rot_toggle * 0.1;
	free(tex);
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	//ESC
		exit(0);
		break;
	}
	case 'd':
	{
		eyex += 0.1;
		break;
	}
	case 'a':
	{
		eyex -= 0.1;
		break;
	}
	case 'w':
	{
		eyey += 0.1;
		break;
	}
	case 's':
	{
		eyey -= 0.1;
		break;
	}
	case 'z':
	{
		eyez -= 0.1;
		break;
	}
	case 'x':
	{
		eyez += 0.1;
		break;
	}
	case 'k':
	{
		lightx += 0.1;
		break;
	}
	case 'h':
	{
		lightx -= 0.1;
		break;
	}
	case 'u':
	{
		lighty += 0.1;
		break;
	}
	case 'j':
	{
		lighty -= 0.1;
		break;
	}
	case 'n':
	{
		lightz -= 0.1;
		break;
	}
	case 'm':
	{
		lightz += 0.1;
		break;
	}
	case 'c':
	{
		if (mode < 4)
			mode++;
		else
			mode = 0;
		break;
	}
	case 'f':
	{
		rot_toggle = !rot_toggle;
		break;
	}
	case 'r':
	{
		for (int i = 0; i < 16; ++i)
			mouse_move[i] = identity[i];
		eyex = 0.0f;
		eyey = 0.0f;
		eyez = 3.0f;
		rot = 0.0f;
		lightx = 10.0f;
		lighty = 10.0f;
		lightz = 0.0f;
		break;
	}
	default:
	{
		break;
	}
	}
}


void mouseWheel(int wheel, int direction, int x, int y)
{
	if (direction > 0)
		eyez -= 0.1;
	else
		eyez += 0.1;
}

void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state != GLUT_UP) {
			mouse_pos0[0] = 2.0 * x / glutGet(GLUT_WINDOW_WIDTH) - 1;
			mouse_pos0[1] = -2.0 * y / glutGet(GLUT_WINDOW_HEIGHT) + 1;
			mouse_pos0[2] = 1;
			normalize(mouse_pos0);
		}
	}
}

void mouseMove(int x, int y)
{
	mouse_pos1[0] = 2.0 * x / glutGet(GLUT_WINDOW_WIDTH) - 1;
	mouse_pos1[1] = -2.0 * y / glutGet(GLUT_WINDOW_HEIGHT) + 1;
	mouse_pos1[2] = 1;
	normalize(mouse_pos1);

	if (dot(mouse_pos0, mouse_pos1) > 0.9999)
		return;

	GLfloat axis[3];
	float angle;

	cross(axis, mouse_pos0, mouse_pos1);
	normalize(axis);
	angle = acosf(cos_clamp(dot(mouse_pos0, mouse_pos1)));

	glPushMatrix();
	glLoadIdentity();
	glRotatef(angle * 180 / M_PI * MOUSE_MOVE_SPEED, axis[0], axis[1], axis[2]);
	glMultMatrixf(mouse_move);
	glGetFloatv(GL_MODELVIEW_MATRIX, mouse_move);
	glPopMatrix();

	mouse_pos0[0] = mouse_pos1[0];
	mouse_pos0[1] = mouse_pos1[1];
	mouse_pos0[2] = mouse_pos1[2];
}

void idle(void)
{
	glutPostRedisplay();
}