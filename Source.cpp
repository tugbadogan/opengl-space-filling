/* sierpinski gasket with vertex arrays */
#define GLUI_FREEGLUT true

#include "Angel.h"
#include "GL/glui.h"

using namespace std;

vec2 points[1000000];
vec2 initial[100000];
vec2 current[100000];
int Index = 0;
bool Continue_draw = TRUE;
GLuint color;

int point_count = 0;
int initial_point_count = 0;

// windows
int mainWindow, settingsWindow;

// Settings variables
char filename[100];
int background_color;
int polygon_color;
float bgcolor[4] = { 1.0, 1.0, 1.0, 1.0 };
float pcolor[4] = { 0.0, 0.0, 0.0, 1.0 };

//----------------------------------------------------------------------------

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

//----------------------------------------------------------------------------

vec2
find_point(double distance, double m, double x1, double y1, int direction)
{
	if (m == 0) {
		return vec2(x1 + distance*direction, y1);
	}
	if (m == INT_MAX || m == -INT_MAX) {
		return vec2(x1, y1 + distance*direction);
	}
	double x = x1 + direction * distance * 1.0 / (sqrt(1.0 + m*m));
	double y = y1 + direction * distance * m / sqrt(1.0 + m*m);
	return  vec2(x, y);
}

//----------------------------------------------------------------------------

bool
counter_clock_wise(vec2 a, vec2 b, vec2 c)
{
	double x1 = a[0], y1 = a[1];
	double x2 = b[0], y2 = b[1];
	double x3 = c[0], y3 = c[1];
	double t = (x1*y2 + x2*y3 + x3*y1) - (y1*x2 + y2*x3 + y3*x1);
	// if counter clockwise t is positive
	return t > 0;
}

//----------------------------------------------------------------------------

char
check_point(vec2 a, vec2 b, vec2 c)
{
	if (c[0] < a[0] && c[0] < b[0])
		return false;
	if (c[0] >a[0] && c[0] > b[0])
		return false;
	if (c[1] < a[1] && c[1] < b[1])
		return false;
	if (c[1] > a[1] && c[1] > b[1])
		return false;
	return true;
}

//----------------------------------------------------------------------------

void
divideCurrentPolygon()
{
	point_count = Index;
	for (int i = 0; i < Index; i++)
		current[i] = points[i];
	Index = 0;
	int i;
	for (i = 0; i + 1 < point_count; i += 2) {
		//divide_line(current[i], current[i + 1]);

		//coordinates of lines
		double  x1 = current[i][0], x2 = current[i+1][0], y1 = current[i][1], y2 = current[i+1][1];

		double slope;

		if (x2 == x1)
			slope = INT_MAX;
		else
			slope = (y2 - y1) / (x2 - x1);

		double orthogonal_line_slope;
		if (x2 == x1)
			orthogonal_line_slope = 0;
		else if (slope != 0)
			orthogonal_line_slope = -1 / slope;
		else if (slope == 0)
			orthogonal_line_slope = INT_MAX;

		double distance = sqrt((y2 - y1)*(y2 - y1) + (x2 - x1)*(x2 - x1)) / 4.0;

		vec2 v0 = find_point(distance, slope, x1, y1, 1);
		if (!check_point(current[i], current[i+1], v0))
			v0 = find_point(distance, slope, x1, y1, -1);
		points[Index++] = current[i];
		points[Index++] = v0;

		vec2 v1 = find_point(distance, orthogonal_line_slope, v0[0], v0[1], -1);
		if (!counter_clock_wise(current[i], current[i+1], v1)) {
			v1 = find_point(distance, orthogonal_line_slope, v0[0], v0[1], 1);
		}
		points[Index++] = v0;
		points[Index++] = v1;

		vec2 v2 = find_point(distance, slope, v1[0], v1[1], 1);
		if (counter_clock_wise(v0, v1, v2)) {
			v2 = find_point(distance, slope, v1[0], v1[1], -1);
		}
		points[Index++] = v1;
		points[Index++] = v2;

		vec2 v3 = find_point(distance, orthogonal_line_slope, v2[0], v2[1], 1);
		if (counter_clock_wise(v1, v2, v3)) {
			v3 = find_point(distance, orthogonal_line_slope, v2[0], v2[1], -1);
		}
		points[Index++] = v2;
		points[Index++] = v3;


		vec2 v4 = find_point(distance, orthogonal_line_slope, v3[0], v3[1], 1);
		if (!counter_clock_wise(current[i+1], v3, v4)) {
			v4 = find_point(distance, orthogonal_line_slope, v3[0], v3[1], -1);
		}
		points[Index++] = v3;
		points[Index++] = v4;

		vec2 v5 = find_point(distance, slope, v4[0], v4[1], 1);
		if (!counter_clock_wise(v3, v4, v5)) {
			v5 = find_point(distance, slope, v4[0], v4[1], -1);
		}
		points[Index++] = v4;
		points[Index++] = v5;

		vec2 v6 = find_point(distance, orthogonal_line_slope, v5[0], v5[1], -1);
		if (!counter_clock_wise(v4, v5, v6)) {
			v6 = find_point(distance, orthogonal_line_slope, v5[0], v5[1], 1);
		}
		points[Index++] = v5;
		points[Index++] = v6;

		points[Index++] = v6;
		points[Index++] = current[i+1];

	}
}

//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		// If left button is clicked, add point to points array.
		case GLUT_LEFT_BUTTON:    
			if (Continue_draw)
			{
				if (Index == 0)
					points[Index++] = vec2((x - 256) / 256.0, (256 - y) / 256.0);
				else
					points[Index++] = points[Index - 1];

				points[Index++] = vec2((x - 256) / 256.0, (256 - y) / 256.0);
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------

void
mouseMovement(int x, int y)
{
	if (!Continue_draw || Index == 0)
		return;
	points[Index] = points[Index - 1];
	points[Index + 1] = vec2((x - 256) / 256.0, (256 - y) / 256.0);
}

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		// Complete polygon
		case 'c':
			points[Index++] = points[Index - 1];
			points[Index++] = points[0];
			Continue_draw = FALSE;
			initial_point_count = Index;
			int i;
			for (i = 0; i < Index; i++)
			{
				initial[i] = points[i];
			}
			break;
		// Divide polygon
		case 'd':
			divideCurrentPolygon();
			break;
	}
}

//---------------------------------------------------------------------------

void
idle(void)
{
	int currentWindow = glutGetWindow();
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutSetWindow(settingsWindow);
	glutPostRedisplay();
	glutSetWindow(currentWindow);
}

//----------------------------------------------------------------------------

void
completePolygon()
{
	points[Index++] = points[Index - 1];
	points[Index++] = points[0];

	// Disables drawing line
	Continue_draw = FALSE;

	initial_point_count = Index;

	int i;
	// Copy initial points
	for (i = 0; i < Index; i++)
	{
		initial[i] = points[i];
	}
}

//----------------------------------------------------------------------------

void
change_background_color()
{	
	// White background
	if (background_color == 1)
	{
		bgcolor[0] = 1.0;
		bgcolor[1] = 1.0;
		bgcolor[2] = 1.0;
		bgcolor[3] = 1.0;
	}
	// Black background
	else if (background_color == 2)
	{
		bgcolor[0] = 0.0;
		bgcolor[1] = 0.0;
		bgcolor[2] = 0.0;
		bgcolor[3] = 1.0;
	}
	// Red background
	else if (background_color == 3)
	{
		bgcolor[0] = 1.0;
		bgcolor[1] = 0.0;
		bgcolor[2] = 0.0;
		bgcolor[3] = 1.0;
	}
	//Green background
	else if (background_color == 4)
	{
		bgcolor[0] = 0.0;
		bgcolor[1] = 1.0;
		bgcolor[2] = 0.0;
		bgcolor[3] = 1.0;
	}
	//Blue background
	else if (background_color == 5)
	{
		bgcolor[0] = 0.0;
		bgcolor[1] = 0.0;
		bgcolor[2] = 1.0;
		bgcolor[3] = 1.0;
	}
	//Cyan background
	else if (background_color == 6)
	{
		bgcolor[0] = 0.0;
		bgcolor[1] = 1.0;
		bgcolor[2] = 1.0;
		bgcolor[3] = 1.0;
	}
	//Magenta background
	else if (background_color == 7)
	{
		bgcolor[0] = 1.0;
		bgcolor[1] = 0.0;
		bgcolor[2] = 1.0;
		bgcolor[3] = 1.0;
	}
	//Yellow background
	else
	{
		bgcolor[0] = 1.0;
		bgcolor[1] = 1.0;
		bgcolor[2] = 0.0;
		bgcolor[3] = 1.0;
	}
}

//----------------------------------------------------------------------------

void
change_polygon_color(int color_option)
{
	// Black polygon
	if (polygon_color == 1)
	{
		pcolor[0] = 0.0;
		pcolor[1] = 0.0;
		pcolor[2] = 0.0;
		pcolor[3] = 1.0;
	}
	// White polygon
	else if (polygon_color == 2)
	{
		pcolor[0] = 1.0;
		pcolor[1] = 1.0;
		pcolor[2] = 1.0;
		pcolor[3] = 1.0;
	}
	// Red polygon
	else if (polygon_color == 3)
	{
		pcolor[0] = 1.0;
		pcolor[1] = 0.0;
		pcolor[2] = 0.0;
		pcolor[3] = 1.0;
	}
	// Green polygon
	else if (polygon_color == 4)
	{
		pcolor[0] = 0.0;
		pcolor[1] = 1.0;
		pcolor[2] = 0.0;
		pcolor[3] = 1.0;
	}
	// Blue polygon
	else if (polygon_color == 5)
	{
		pcolor[0] = 0.0;
		pcolor[1] = 0.0;
		pcolor[2] = 1.0;
		pcolor[3] = 1.0;
	}
	// Cyan polygon
	else if (polygon_color == 6)
	{
		pcolor[0] = 0.0;
		pcolor[1] = 1.0;
		pcolor[2] = 1.0;
		pcolor[3] = 1.0;
	}
	// Magenta polygon
	else if (polygon_color == 7)
	{
		pcolor[0] = 1.0;
		pcolor[1] = 0.0;
		pcolor[2] = 1.0;
		pcolor[3] = 1.0;
	}
	// Yellow polygon
	else
	{
		pcolor[0] = 1.0;
		pcolor[1] = 1.0;
		pcolor[2] = 0.0;
		pcolor[3] = 1.0;
	}
}

//----------------------------------------------------------------------------

void
loadFile()
{
	FILE *f = fopen(filename, "r");
	// Get point count from file
	fscanf(f, "%d", &initial_point_count);
	
	Index = initial_point_count;

	// Get points from file
	for (int i = 0; i < Index; i++)
	{
		fscanf(f, "%f %f", &initial[i][0], &initial[i][1]);
		points[i] = initial[i];
	}
	Continue_draw = FALSE;

	// Get background color from file
	fscanf(f, "%f %f %f %f", &bgcolor[0], &bgcolor[1], &bgcolor[2], &bgcolor[3]);

	// Get polygon color from file
	fscanf(f, "%f %f %f %f", &pcolor[0], &pcolor[1], &pcolor[2], &pcolor[3]);

	fclose(f);
}

//----------------------------------------------------------------------------

void
saveFile()
{
	FILE *f = fopen(filename, "w");

	// Save point counts
	fprintf(f, "%d\n", initial_point_count);

	// Save point coordinates
	for (int i = 0; i < initial_point_count; i++)
		fprintf(f, "%f %f\n", initial[i][0], initial[i][1]);
	
	// Save background color
	fprintf(f, "%f %f %f %f\n", bgcolor[0], bgcolor[1], bgcolor[2], bgcolor[3]);

	// Save polygon color
	fprintf(f, "%f %f %f %f\n", pcolor[0], pcolor[1], pcolor[2], pcolor[3]);
	
	fclose(f);
}

//----------------------------------------------------------------------------

void
exitScreen()
{
	exit(EXIT_SUCCESS);
}

//----------------------------------------------------------------------------

void
display(void)
{
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw lines
	glDrawArrays(GL_LINES, 0, Index+2);

	// Background color
	glClearColor(bgcolor[0], bgcolor[1], bgcolor[2], bgcolor[3]);

	// Polygon color
	glUniform4f(color, pcolor[0], pcolor[1], pcolor[2], pcolor[3]);

	glFlush();
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
init(void)
{
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader22.glsl", "fshader22.glsl");
	glUseProgram(program);
	
	// Initialize the vertex position attribute from the vertex shader    
	GLuint loc = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	
	color = glGetUniformLocation(program, "color");

	glClearColor(1.0, 1.0, 1.0, 1.0); /* white background */
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	int b_color_menu_id, p_color_menu_id, f_color_menu_id;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	mainWindow = glutCreateWindow("Space Filling Curves");
	// 26.08.2011 ATES - GLEW kutuphanesiyle ilgili bir sorundan oturu asagidaki satiri eklemezsek init() metodu icerisinde
	// GlGenVertexArrays cagirildiginda access violation hatasi veriyor
	glewExperimental = GL_TRUE;
	glewInit();
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(mouseMovement);
	glutIdleFunc(idle);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(10, 10);
	glutInitWindowPosition(1024, 0);
	glutInitContextVersion(2, 1);
	glutInitContextProfile(GLUT_INIT_PROFILE);

	// Drawing settings
	settingsWindow = glutCreateWindow("Drawing Settings");
	GLUI *glui = GLUI_Master.create_glui("GLUI");
	glui->set_main_gfx_window(settingsWindow);

	// Bakcground color list
	GLUI_Listbox *bgcolorlist = glui->add_listbox("Background Color", &background_color, 0, (GLUI_Update_CB)change_background_color);
	bgcolorlist->add_item(1, "White");
	bgcolorlist->add_item(2, "Black");
	bgcolorlist->add_item(3, "Red");
	bgcolorlist->add_item(4, "Green");
	bgcolorlist->add_item(5, "Blue");
	bgcolorlist->add_item(6, "Cyan");
	bgcolorlist->add_item(7, "Magenta");
	bgcolorlist->add_item(8, "Yellow");

	// Polygon color list
	GLUI_Listbox *pcolorlist = glui->add_listbox("Polygon Color", &polygon_color, 1, (GLUI_Update_CB)change_polygon_color);
	pcolorlist->add_item(1, "Black");
	pcolorlist->add_item(2, "White");
	pcolorlist->add_item(3, "Red");
	pcolorlist->add_item(4, "Green");
	pcolorlist->add_item(5, "Blue");
	pcolorlist->add_item(6, "Cyan");
	pcolorlist->add_item(7, "Magenta");
	pcolorlist->add_item(8, "Yellow");

	// Get filename to store or load polygon
	glui->add_edittext("File Name", GLUI_EDITTEXT_TEXT, filename);

	// Complete polygon
	glui->add_button("Complete Polygon", 0, (GLUI_Update_CB)completePolygon);

	// Divide polygon
	glui->add_button("Divide Polygon", 1, (GLUI_Update_CB)divideCurrentPolygon);

	// Save and load polygon
	glui->add_button("Save Polygon", 2, (GLUI_Update_CB)saveFile);
	glui->add_button("Load Polygon", 3, (GLUI_Update_CB)loadFile);

	// Exit
	glui->add_button("Exit", 4, (GLUI_Update_CB)exitScreen);
	
	glui->add_separator();
	glui->add_separator();
	glui->add_separator();
	glui->add_column(true);
	glui->add_separator();

	/*
	b_color_menu_id = glutCreateMenu(background_color_menu);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Cyan", 4);
	glutAddMenuEntry("Magenta", 5);
	glutAddMenuEntry("Yellow", 6);
	glutAddMenuEntry("White", 7);
	glutAddMenuEntry("Black", 8);

	p_color_menu_id = glutCreateMenu(polygon_color_menu);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Cyan", 4);
	glutAddMenuEntry("Magenta", 5);
	glutAddMenuEntry("Yellow", 6);
	glutAddMenuEntry("White", 7);
	glutAddMenuEntry("Black", 8);

	f_color_menu_id = glutCreateMenu(fill_color_menu);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Cyan", 4);
	glutAddMenuEntry("Magenta", 5);
	glutAddMenuEntry("Yellow", 6);
	glutAddMenuEntry("White", 7);
	glutAddMenuEntry("Black", 8);

	glutCreateMenu(main_menu);
	// Set the menu values to the relevant rotation axis values (or Quit)
	glutAddSubMenu("Background Color", b_color_menu_id);
	glutAddSubMenu("Polygon Color", p_color_menu_id);
	glutAddSubMenu("Filling Color", f_color_menu_id);
	//glutAddSubMenu("renk", menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	*/

	glutMainLoop();
	return 0;
}