#include <stdio.h>
#include <iostream>
#include <GL/glut.h>
#include <GL/glu.h>
#include "GL/glui.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "src/Mesh.h"
#include "src/Camera.h"
#include "src/Renderer.h"

#define MAX_WINDOWS 2
#define OPENGL_WINDOW 0
#define CLOSE2GL_WINDOW 1

// Global variables
int windows[MAX_WINDOWS];
int screenW[MAX_WINDOWS], screenH[MAX_WINDOWS];
Camera camera;
Renderer renderer;
Screen screen;
Mesh *mesh = nullptr;
int mouseX, mouseY;
bool mouseDown = false;
bool mouseLeft = false;

// GLUI variables
GLUI *glui;
#define GLUI_EVENT_TRANSLATION 0
#define GLUI_EVENT_TRANSLATION_AXIS 1
#define GLUI_EVENT_TRANSLATION_LOCK 2
#define GLUI_EVENT_ROTATION 3
#define GLUI_EVENT_ROTATION_AXIS 4
#define GLUI_PROJECTION 5
#define GLUI_FRONT_FACE 6
#define GLUI_COLOR_CHANGE 7
#define GLUI_RESET 8
#define GLUI_RENDER_MODE 9
#define GLUI_LOAD_FILENAME 10
#define GLUI_SHADE_MODE 11
#define GLUI_LOAD_TEXTURE 12
float GLUITranslation = 0.0f;
int GLUITranslationAxis = 0;
int GLUITranslationLock = 0;
float GLUIRotation = 0.0f;
int GLUIRotationAxis = 0;
int GLUIFrontFace = 1;
int GLUIRenderMode = 2;
char filename[255];
char textureFilename[255];

Mesh* readFile(const char *fileName)
{
	FILE *fp = fopen(fileName, "r");
	if (fp == NULL)
	{
		std::cerr << "Could not open file: " << fileName << std::endl;
		exit(-1);
	}
	
	char ch = '0';
	while (ch != '\n') fscanf(fp, "%c", &ch);

	int numTriangles, materialCount;
	fscanf(fp, "# triangles = %d\n", &numTriangles);	
	fscanf(fp, "Material count = %d\n", &materialCount);

	Vector3f *ambient = new Vector3f[materialCount];
	Vector3f *diffuse = new Vector3f[materialCount];
	Vector3f *specular = new Vector3f[materialCount];
	float *shine = new float[materialCount];

	for (int i = 0; i < materialCount; i++)
	{
		fscanf(fp, "ambient color %f %f %f\n", &ambient[i].x, &ambient[i].y, &ambient[i].z);
		fscanf(fp, "diffuse color %f %f %f\n", &diffuse[i].x, &diffuse[i].y, &diffuse[i].z);
		fscanf(fp, "specular color %f %f %f\n", &specular[i].x, &specular[i].y, &specular[i].z);
		fscanf(fp, "material shine %f\n", &shine[i]);
	}

	char text[3];
	fscanf(fp, "Texture = %s\n", text);
	bool hasTexture = text[0] == 'Y';
	
	ch = '0';
	while (ch != '\n') fscanf(fp, "%c", &ch);
	
	Triangle **triangles = new Triangle*[numTriangles];
	int colorIndex[3];
	for (int i = 0; i < numTriangles; i++)
	{
		triangles[i] = new Triangle;
		fscanf(fp, "v0 %f %f %f %f %f %f %d", &triangles[i]->vertices[0].x, &triangles[i]->vertices[0].y, &triangles[i]->vertices[0].z,
							&triangles[i]->normals[0].x, &triangles[i]->normals[0].y, &triangles[i]->normals[0].z, &triangles[i]->colorIndex[0]);
		if (hasTexture)
		{
			fscanf(fp, "%f %f", &triangles[i]->textureCoords[0].x, &triangles[i]->textureCoords[0].y);
		}
		fscanf(fp, "\nv1 %f %f %f %f %f %f %d", &triangles[i]->vertices[1].x, &triangles[i]->vertices[1].y, &triangles[i]->vertices[1].z, 								
							&triangles[i]->normals[1].x, &triangles[i]->normals[1].y, &triangles[i]->normals[1].z, &triangles[i]->colorIndex[1]);
		if (hasTexture)
		{
			fscanf(fp, "%f %f", &triangles[i]->textureCoords[1].x, &triangles[i]->textureCoords[1].y);
		}
		fscanf(fp, "\nv2 %f %f %f %f %f %f %d", &triangles[i]->vertices[2].x, &triangles[i]->vertices[2].y, &triangles[i]->vertices[2].z,
							&triangles[i]->normals[2].x, &triangles[i]->normals[2].y, &triangles[i]->normals[2].z, &triangles[i]->colorIndex[2]);
		if (hasTexture)
		{
			fscanf(fp, "%f %f", &triangles[i]->textureCoords[2].x, &triangles[i]->textureCoords[2].y);
		}
		fscanf(fp, "\nface normal %f %f %f\n", &triangles[i]->faceNormal.x, &triangles[i]->faceNormal.y, &triangles[i]->faceNormal.z);
	}
	fclose(fp);
	return new Mesh(triangles, numTriangles, ambient, diffuse, specular, shine, materialCount, hasTexture);
}

void close2GLRenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderer.renderClose2GL(mesh, camera, screen, screenW[CLOSE2GL_WINDOW], screenH[CLOSE2GL_WINDOW]);
	glutSwapBuffers();
}

void close2GLChangeSize(int w, int h)
{
	screenW[CLOSE2GL_WINDOW] = w;
	screenH[CLOSE2GL_WINDOW] = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.loadModelViewMatrix();
	renderer.renderGL(mesh);
	glutSwapBuffers();
}

void changeSize(int w, int h)
{
	screenW[OPENGL_WINDOW] = w;
	screenH[OPENGL_WINDOW] = h;
	screen.loadViewportMatrix(w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	screen.loadProjectionMatrix();
}

void updateWindows()
{
	glui->sync_live();
	for (int i = 0; i < MAX_WINDOWS; i++)
	{
		glutSetWindow(windows[i]);
		glutPostRedisplay();
	}	
}

void mouseMove(int x, int y)
{
	Vector3f min, max;
	mesh->getMinMax(min, max);
	int diffX = x - mouseX;
	int diffY = y - mouseY;
	mouseX = x;
	mouseY = y;
	if (mouseLeft)
	{
		camera.getRotation().y -= diffX;
		camera.getRotation().x += diffY;
		if (GLUIRotationAxis == 0)
		{
			GLUIRotation += diffY;
		}
		else if (GLUIRotationAxis == 1)
		{
			GLUIRotation -= diffX;
		}
	}
	else
	{
		camera.getTranslation().x += diffX * (max.x - min.x) / 100.0f;
		camera.getTranslation().y -= diffY * (max.y - min.y) / 100.0f;
		if (GLUITranslationAxis == 0)
		{
			GLUITranslation += diffX * (max.x - min.x) / 100.0f;
		}
		else if (GLUITranslationAxis == 1)
		{
			GLUITranslation += diffY * (max.y - min.y) / 100.0f;
		}
	}
	updateWindows();
}

void mouseClick(int button, int state, int x, int y)
{
	// zoom
	if ((button == 3 || button == 4) && mesh != nullptr)
	{
		Vector3f min, max;
		mesh->getMinMax(min, max);
		camera.getTranslation().z += (max.z - min.z) / 10.0f * (button == 3 ? - 1 : 1);
	}
	else
	{
		mouseLeft = button == GLUT_LEFT_BUTTON;
		if (button == GLUT_LEFT_BUTTON)
		{
			mouseDown = state == GLUT_DOWN;
			mouseX = x;
			mouseY = y;
		}
	}
	updateWindows();
}

void reset()
{
	camera.reset();
	renderer.reset();
	screen.reset();
	GLUIRotation = 0.0f;
	GLUITranslation = 0.0f;
	GLUITranslationLock = false;
}

inline float getCameraZ(Vector3f &meshMin, Vector3f &meshMax)
{
	float length = std::max(meshMax.x - meshMin.x, meshMax.y - meshMin.y) / 2.0f;
	return -length / std::tan(30.0f * M_PI / 180.0);
}

void init(const std::string &filename)
{
	if (mesh != nullptr)
	{
		delete mesh;
		mesh = nullptr;
	}
	
	mesh = readFile(filename.c_str());
	
	Vector3f meshMin(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3f meshMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	mesh->getMinMax(meshMin, meshMax);
	Vector3f target = (meshMin + meshMax) * 0.5f;
	Vector3f origin = Vector3f(target.x, target.y, meshMin.z + getCameraZ(meshMin, meshMax) * 1.1f);	
	camera = Camera(origin, target);
	
	reset();
}

float* readTextureImage(const char *filename, int &width, int &height)
{
	cv::Mat img = cv::imread(filename);
	if (!img.data)
	{
		std::cerr << "Could not read texture: " << textureFilename;
		exit(-1);
	}
	width = img.cols;
	height = img.rows;
	float *texture = new float[img.rows * img.cols * 3];
	for (int ch = 0; ch < 3; ch++)
	{
		for (int y = 0; y < img.rows; y++)
		{
			for (int x = 0; x < img.cols; x++)
			{
				texture[y * img.cols * 3 + x * 3 + ch] = static_cast<float>(img.at<cv::Vec3b>(y, x)[2 - ch]) / 255.0f;
			}
		}
	}
	return texture;
}

void loadTexture(const char *filename)
{
	if (std::string(filename) == "") return;
	
	glutSetWindow(windows[OPENGL_WINDOW]);
	
	if (renderer.getTexture() != nullptr)
	{
		renderer.removeTexture();
	}
	glEnable(GL_TEXTURE_2D);
	
	int width, height;
	float *texture = readTextureImage(filename, width, height);

	GLuint textName;
	
	glGenTextures(1, &textName); 
	glBindTexture(GL_TEXTURE_2D, textName); 
	
	if (renderer.getTextureFiltering() == TEXTURE_FILTERING_NEAREST_NEIGHBORS)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if (renderer.getTextureFiltering() == TEXTURE_FILTERING_BILINEAR)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, texture);
	
	if (renderer.getTextureFiltering() == TEXTURE_FILTERING_MIPMAP)
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_FLOAT, texture); 
	}
	
	renderer.setTexture(texture, textName, width, height);
}

void GLUICallback(int eventID)
{
	if (eventID == GLUI_LOAD_FILENAME)
	{
		init(filename);
	}
	else if (eventID == GLUI_EVENT_TRANSLATION)
	{
		if (GLUITranslationAxis == 0)
		{
			camera.getTranslation().x = GLUITranslation;
		}
		else if (GLUITranslationAxis == 1)
		{
			camera.getTranslation().y = GLUITranslation;
		}
		else if (GLUITranslationAxis == 2)
		{
			camera.getTranslation().z = GLUITranslation;
		}
	}
	else if (eventID == GLUI_EVENT_TRANSLATION_AXIS)
	{
		if (GLUITranslationAxis == 0)
		{
			GLUITranslation = camera.getTranslation().x;
		}
		else if (GLUITranslationAxis == 1)
		{
			GLUITranslation = camera.getTranslation().y;
		}
		else if (GLUITranslationAxis == 2)
		{
			GLUITranslation = camera.getTranslation().z;
		}
	}
	else if (eventID == GLUI_EVENT_ROTATION)
	{
		if (GLUIRotationAxis == 0)
		{
			camera.getRotation().x = GLUIRotation;
		}
		else if (GLUIRotationAxis == 1)
		{
			camera.getRotation().y = GLUIRotation;
		}
		else if (GLUIRotationAxis == 2)
		{
			camera.getRotation().z = GLUIRotation;
		}
	}
	else if (eventID == GLUI_EVENT_ROTATION_AXIS)
	{
		if (GLUIRotationAxis == 0)
		{
			GLUIRotation = camera.getRotation().x;
		}
		else if (GLUIRotationAxis == 1)
		{
			GLUIRotation = camera.getRotation().y;
		}
		else if (GLUIRotationAxis == 2)
		{
			GLUIRotation = camera.getRotation().z;
		}
	}
	else if (eventID == GLUI_PROJECTION)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		screen.loadProjectionMatrix();
	}
	else if (eventID == GLUI_RESET)
	{
		reset();
	}
	else if (eventID == GLUI_LOAD_TEXTURE)
	{
		loadTexture(textureFilename);
	}
	if (mesh != nullptr && GLUITranslationLock)
	{
		camera.lookAtTarget();
		GLUIRotation = 0.0f;
	}
	updateWindows();
}

void setupGLUI()
{
	glui = GLUI_Master.create_glui("GLUI");
	glui->add_edittext("Filename:", GLUI_EDITTEXT_TEXT, filename);
	glui->add_button("Load file", GLUI_LOAD_FILENAME, GLUICallback);
	glui->add_separator();
	glui->add_statictext("Translation");
	GLUI_Spinner *translation_spinner = glui->add_spinner("Amount:", GLUI_SPINNER_FLOAT, &GLUITranslation, GLUI_EVENT_TRANSLATION, GLUICallback);
	GLUI_Panel *translation_axis_panel = glui->add_panel("Axis");
	GLUI_RadioGroup *translation_axis_group = glui->add_radiogroup_to_panel(translation_axis_panel, &GLUITranslationAxis, GLUI_EVENT_TRANSLATION_AXIS, GLUICallback);
	glui->add_radiobutton_to_group(translation_axis_group, "U");
	glui->add_radiobutton_to_group(translation_axis_group, "V");
	glui->add_radiobutton_to_group(translation_axis_group, "N");
	glui->add_checkbox("Lock to object", &GLUITranslationLock, GLUI_EVENT_TRANSLATION_LOCK, GLUICallback);
	glui->add_separator();
	glui->add_statictext("Rotation");
	GLUI_Spinner *rotation_spinner = glui->add_spinner("Amount:", GLUI_SPINNER_FLOAT, &GLUIRotation, GLUI_EVENT_ROTATION, GLUICallback);
	rotation_spinner->set_float_limits(-360, 360);
	GLUI_Panel *rotation_axis_panel = glui->add_panel("Axis");
	GLUI_RadioGroup *rotation_axis_group = glui->add_radiogroup_to_panel(rotation_axis_panel, &GLUIRotationAxis, GLUI_EVENT_ROTATION_AXIS, GLUICallback);
	glui->add_radiobutton_to_group(rotation_axis_group, "U");
	glui->add_radiobutton_to_group(rotation_axis_group, "V");
	glui->add_radiobutton_to_group(rotation_axis_group, "N");
	glui->add_separator();
	glui->add_statictext("Projection and culling");
	GLUI_Spinner *znear_spinner = glui->add_spinner("Znear:", GLUI_SPINNER_FLOAT, &screen.getZNear(), GLUI_PROJECTION, GLUICallback);
	znear_spinner->set_float_limits(0.0f, 10000.0f);
	GLUI_Spinner *zfar_spinner = glui->add_spinner("Zfar:", GLUI_SPINNER_FLOAT, &screen.getZFar(), GLUI_PROJECTION, GLUICallback);
	zfar_spinner->set_float_limits(0.0f, 10000.0f);
	GLUI_Spinner *fov_spinner_h = glui->add_spinner("Field of view (H):", GLUI_SPINNER_FLOAT, &screen.getFovH(), GLUI_PROJECTION, GLUICallback);
	fov_spinner_h->set_float_limits(0.0f, 180.0f);
	GLUI_Spinner *fov_spinner_v = glui->add_spinner("Field of view (V):", GLUI_SPINNER_FLOAT, &screen.getFovV(), GLUI_PROJECTION, GLUICallback);
	fov_spinner_v->set_float_limits(0.0f, 180.0f);
	GLUI_Panel *front_face_panel = glui->add_panel("Front face");
	GLUI_RadioGroup *front_face_group = glui->add_radiogroup_to_panel(front_face_panel, &renderer.getFrontFace(), GLUI_FRONT_FACE, GLUICallback);
	glui->add_radiobutton_to_group(front_face_group, "CW");
	glui->add_radiobutton_to_group(front_face_group, "CCW");
	glui->add_separator();
	glui->add_button("Reset", GLUI_RESET, GLUICallback);
	glui->add_column(true);
	
	glui->add_statictext("Rasterization");
	GLUI_Panel *render_mode_panel = glui->add_panel("Polygon mode");
	GLUI_RadioGroup *render_mode_group = glui->add_radiogroup_to_panel(render_mode_panel, &renderer.getRenderMode(), GLUI_RENDER_MODE, GLUICallback);
	glui->add_radiobutton_to_group(render_mode_group, "Vertices");
	glui->add_radiobutton_to_group(render_mode_group, "Wireframe");
	glui->add_radiobutton_to_group(render_mode_group, "Solid");
	glui->add_separator();
	glui->add_statictext("Diffuse color");
	GLUI_Spinner *diffuse_r_spinner = glui->add_spinner("R:", GLUI_SPINNER_FLOAT, &renderer.getObjectColor().x, GLUI_COLOR_CHANGE, GLUICallback);
	diffuse_r_spinner->set_float_limits(0.0f, 1.0f);
	GLUI_Spinner *diffuse_g_spinner = glui->add_spinner("G:", GLUI_SPINNER_FLOAT, &renderer.getObjectColor().y, GLUI_COLOR_CHANGE, GLUICallback);
	diffuse_g_spinner->set_float_limits(0.0f, 1.0f);
	GLUI_Spinner *diffuse_b_spinner = glui->add_spinner("B:", GLUI_SPINNER_FLOAT, &renderer.getObjectColor().z, GLUI_COLOR_CHANGE, GLUICallback);
	diffuse_b_spinner->set_float_limits(0.0f, 1.0f);
	glui->add_separator();
	glui->add_statictext("Ambient color");
	GLUI_Spinner *ambient_r_spinner = glui->add_spinner("R:", GLUI_SPINNER_FLOAT, &renderer.getAmbientColor().x, GLUI_COLOR_CHANGE, GLUICallback);
	ambient_r_spinner->set_float_limits(0.0f, 1.0f);
	GLUI_Spinner *ambient_g_spinner = glui->add_spinner("G:", GLUI_SPINNER_FLOAT, &renderer.getAmbientColor().y, GLUI_COLOR_CHANGE, GLUICallback);
	ambient_g_spinner->set_float_limits(0.0f, 1.0f);
	GLUI_Spinner *ambient_b_spinner = glui->add_spinner("B:", GLUI_SPINNER_FLOAT, &renderer.getAmbientColor().z, GLUI_COLOR_CHANGE, GLUICallback);
	ambient_b_spinner->set_float_limits(0.0f, 1.0f);
	glui->add_separator();
	glui->add_statictext("Specular color");
	GLUI_Spinner *specular_r_spinner = glui->add_spinner("R:", GLUI_SPINNER_FLOAT, &renderer.getSpecularColor().x, GLUI_COLOR_CHANGE, GLUICallback);
	specular_r_spinner->set_float_limits(0.0f, 1.0f);
	GLUI_Spinner *specular_g_spinner = glui->add_spinner("G:", GLUI_SPINNER_FLOAT, &renderer.getSpecularColor().y, GLUI_COLOR_CHANGE, GLUICallback);
	specular_g_spinner->set_float_limits(0.0f, 1.0f);
	GLUI_Spinner *specular_b_spinner = glui->add_spinner("B:", GLUI_SPINNER_FLOAT, &renderer.getSpecularColor().z, GLUI_COLOR_CHANGE, GLUICallback);
	specular_b_spinner->set_float_limits(0.0f, 1.0f);
	GLUI_Spinner *shininess_spinner = glui->add_spinner("Shininess", GLUI_SPINNER_FLOAT, &renderer.getShininess(), GLUI_COLOR_CHANGE, GLUICallback);
	shininess_spinner->set_float_limits(0.0f, 1.0f);
	glui->add_column(true);
	
	GLUI_Panel *shading_mode_panel = glui->add_panel("Shading mode");
	GLUI_RadioGroup *shading_mode_group = glui->add_radiogroup_to_panel(shading_mode_panel, &renderer.getShadingMode(), GLUI_SHADE_MODE, GLUICallback);
	glui->add_radiobutton_to_group(shading_mode_group, "Flat");
	glui->add_radiobutton_to_group(shading_mode_group, "Gouraud");
	glui->add_radiobutton_to_group(shading_mode_group, "None");
	glui->add_separator();
	glui->add_statictext("Light color");
	GLUI_Spinner *light_r_spinner = glui->add_spinner("R:", GLUI_SPINNER_FLOAT, &renderer.getLightColor().x, GLUI_COLOR_CHANGE, GLUICallback);
	light_r_spinner->set_float_limits(0.0f, 1.0f);
	GLUI_Spinner *light_g_spinner = glui->add_spinner("G:", GLUI_SPINNER_FLOAT, &renderer.getLightColor().y, GLUI_COLOR_CHANGE, GLUICallback);
	light_g_spinner->set_float_limits(0.0f, 1.0f);
	GLUI_Spinner *light_b_spinner = glui->add_spinner("B:", GLUI_SPINNER_FLOAT, &renderer.getLightColor().z, GLUI_COLOR_CHANGE, GLUICallback);
	light_b_spinner->set_float_limits(0.0f, 1.0f);
	glui->add_separator();
	glui->add_statictext("Light position");
	GLUI_Spinner *light_position_x_spinner = glui->add_spinner("X:", GLUI_SPINNER_FLOAT, &renderer.getLightPosition().x, GLUI_COLOR_CHANGE, GLUICallback);
	GLUI_Spinner *light_position_y_spinner = glui->add_spinner("Y:", GLUI_SPINNER_FLOAT, &renderer.getLightPosition().y, GLUI_COLOR_CHANGE, GLUICallback);
	GLUI_Spinner *light_position_z_spinner = glui->add_spinner("Z:", GLUI_SPINNER_FLOAT, &renderer.getLightPosition().z, GLUI_COLOR_CHANGE, GLUICallback);
	glui->add_separator();
	glui->add_statictext("Attenuation factor");
	GLUI_Spinner *attenuation_factor_constant_spinner = glui->add_spinner("Constant:", GLUI_SPINNER_FLOAT, &renderer.getAttenuationConstant(), GLUI_COLOR_CHANGE, GLUICallback);
	attenuation_factor_constant_spinner->set_float_limits(0.0f, 1.0f);
	GLUI_Spinner *attenuation_factor_linear_spinner = glui->add_spinner("Linear:", GLUI_SPINNER_FLOAT, &renderer.getAttenuationLinear(), GLUI_COLOR_CHANGE, GLUICallback);
	attenuation_factor_linear_spinner->set_float_limits(0.0f, 1.0f);
	GLUI_Spinner *attenuation_factor_quadratic_spinner = glui->add_spinner("Quadratic:", GLUI_SPINNER_FLOAT, &renderer.getAttenuationQuadratic(), GLUI_COLOR_CHANGE, GLUICallback);
	attenuation_factor_quadratic_spinner->set_float_limits(0.0f, 1.0f);
	glui->add_column(true);
	
	glui->add_edittext("Texture filename:", GLUI_EDITTEXT_TEXT, textureFilename);
	glui->add_button("Load texture", GLUI_LOAD_TEXTURE, GLUICallback);
	glui->add_separator();
	GLUI_Panel *texture_filtering_panel = glui->add_panel("Texture filtering");
	GLUI_RadioGroup *texture_filtering_group = glui->add_radiogroup_to_panel(texture_filtering_panel, &renderer.getTextureFiltering(), GLUI_LOAD_TEXTURE, GLUICallback);
	glui->add_radiobutton_to_group(texture_filtering_group, "Nearest-Neighbors");
	glui->add_radiobutton_to_group(texture_filtering_group, "Bilinear filtering");
	glui->add_radiobutton_to_group(texture_filtering_group, "Mipmap");
	glui->add_separator();
	glui->add_checkbox("Perspectively correct", &renderer.getPerspectiveCorrect(), GLUI_LOAD_TEXTURE, GLUICallback);
	glui->set_main_gfx_window(windows[OPENGL_WINDOW]);
}

void setupGL()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_CULL_FACE);	
	//glCullFace(GL_BACK);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	windows[OPENGL_WINDOW] = glutCreateWindow("OpenGL");
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	setupGL();
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(640, 0);
	windows[CLOSE2GL_WINDOW] = glutCreateWindow("Close2GL");
	glutDisplayFunc(close2GLRenderScene);
	glutReshapeFunc(close2GLChangeSize);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	
	setupGLUI();
	
	if (argc > 2)
	{
		std::string inputTextureFilename(argv[2]);
		std::memcpy(textureFilename, inputTextureFilename.c_str(), inputTextureFilename.size() * sizeof(char));
		loadTexture(textureFilename);
		glui->sync_live();
	}
	if (argc > 1)
	{
		std::string inputFilename(argv[1]);
		std::memcpy(filename, inputFilename.c_str(), inputFilename.size() * sizeof(char));
		init(filename);
		glui->sync_live();
	}
	
	glutMainLoop();
	return 0;
}

