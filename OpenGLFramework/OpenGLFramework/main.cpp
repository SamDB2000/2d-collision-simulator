/**
 *	CS 344 - Fundamentals of Computer Graphics
 *	Basic 2D framework using OpenGL/FreeGLUT
 *
 *	Instructions:
 *	- Right click on window for showing the menu and change speed and color of the square
 *	- Press ESC to exit (option also available on menu)
 */

#include <iostream>
#include "GL/glut.h"
#include <chrono>
#include <thread>
#include <vector>
#include "GL/geometry.h"


 /* Window information */
float windowWidth = 800;
float windowHeight = 600;
int windowID = -1;

//Square *square;
//std::vector<Square *> squares;
std::vector<Shape*> shapes;
std::vector<Square*> vecSquares;
std::vector<Circle*> vecCircles;
int _id = 0;

/* IDs of the menu and the submenus */
int menuID;
bool circles = false;
bool squares = true;
bool run = false;

// Maybe a function like this will be used to draw a line from the center of the square 
// to the mouse when creating an initial motion vector
/**
 * Draws a line connecting the two squares
 */
void drawLine() {
	float centers[2] = {
		//squareX, squareY,
	};
	float lineColor[3] = {
		1.0, 0.0, 0.0 // red
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, centers);
	glColorPointer(3, GL_FLOAT, 0, lineColor);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

/**
 *	This will update the objects and redraw
 */
void refresh()
{
	/* Clear the window */
	glClear(GL_COLOR_BUFFER_BIT);

	// Start a timer to limit to (around) 60fps
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	start = std::chrono::steady_clock::now();

	// Update position and display
	for (auto& shape : shapes) {
		shape->update();
		shape->display();
	}

	end = std::chrono::steady_clock::now();
	int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::this_thread::sleep_for(std::chrono::milliseconds((1000 / 60) - elapsed));

	/* Force execution of OpenGL commands */
	glFlush();

	/* Swap buffers for animation */
	glutSwapBuffers();
}


/**
 *	Function invoked when window system events are not being received
 */
void idle()
{
	/* Redraw the window */
	glutPostRedisplay();
}


/**
 *	Function invoked when an event on regular keys occur
 */
void keyboard(unsigned char k, int x, int y)
{
	/* Show which key was pressed */
	std::cout << "Pressed \"" << k << "\" ASCII: " << (int)k << std::endl;

	/* Close application if ESC is pressed */
	if (k == 27)
	{
		exit(0);
	}
	else if (k == 'r') {
		run = true;
	}
}


/**
 *	Set OpenGL initial state
 */
void init()
{
	/* Set clear color */
	glClearColor(1.0, 1.0, 1.0, 0.0);

	/* Set 2D orthogonal projection */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, windowWidth, windowHeight, 0.0);

}


/**
 *	Function invoked when a mouse event occur
 */
void mouse(int button, int state, int x, int y)
{
	/* Show the button and the event on the mouse */
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// create a square object whenever the mouse gets clicked
		x = (float)x;
		y = (float)y;

		if (!circles) {
			Square* square = new Square(20.0, Vec2f(x, y), Vec2f(0, 0), Vec2f(0.0, 0.0), _id);
			shapes.push_back(square);
			vecSquares.push_back(square); // This was the easiest way, though not pretty, to ensure that I had a list of just squares
			_id++;
		}
		else {
			Circle* circle = new Circle(20.0, Vec2f(x, y), Vec2f(-2, -10), Vec2f(0.0, 0.3), _id);
			shapes.push_back(circle);
			vecCircles.push_back(circle); // This was the easiest way, though not pretty, to ensure that I had a list of just circles
			_id++;
		}
		// This adds a new square to the list of squares, eventually this will just be a list of objects

	}
	/*else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		std::cout << "Mouse: Left button up" << std::endl;
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		std::cout << "Mouse: Middle button down" << std::endl;
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
	{
		std::cout << "Mouse: Middle button up" << std::endl;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		std::cout << "Mouse: Right button down" << std::endl;
		// create a square object whenever the mouse gets clicked
		x = (float)x;
		y = (float)y;
		// This adds a new square to the list of squares, eventually this will just be a list of objects

	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		std::cout << "Mouse: Right button up" << std::endl;
	}*/
}


/**
 *	Function called when the an option of the menu is selected
 */
void menu(int value)
{
	if (value == 0)
	{
		run = true;
	}
	if (value == 1)
	{
		circles = true;
		squares = false;
	}
	if (value == 2)
	{
		squares = true;
		circles = false;
	}
	if (value == 3) {
		exit(0);
	}
}


/**
 *	Generates the menu
 */
void makeMenu()
{
	/* Generate the menu */
	menuID = glutCreateMenu(menu);
	glutAddMenuEntry("Run", 0);
	glutAddMenuEntry("Circles", 1);
	glutAddMenuEntry("Squares", 2);
	glutAddMenuEntry("Exit", 3);

	/* Attach menu to the right click */
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/**
 *	Main function
 */
int main(int argc, char** argv)
{
	/* Initialize the GLUT window */
	glutInit(&argc, argv);
	glutInitWindowSize((int)windowWidth, (int)windowHeight);
	glutInitWindowPosition(30, 30);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	windowID = glutCreateWindow("2D Physics Sandbox");

	/* Set OpenGL initial state */
	init();

	/* Callback functions */
	glutDisplayFunc(refresh);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	/* Generate the menu */
	makeMenu();

	/* Start the main GLUT loop */
	/* NOTE: No code runs after this */
	glutMainLoop();
}