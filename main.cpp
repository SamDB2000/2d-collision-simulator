/**
 *	CS 344 - Fundamentals of Computer Graphics
 *	Basic 2D framework using OpenGL/FreeGLUT
 *
 *	Instructions:
 *	- Right click on window for showing the menu and change speed and color of the square
 *	- Press ESC to exit (option also available on menu)
 */

// Dear ImGui: standalone example application for GLUT/FreeGLUT + OpenGL2, using legacy fixed pipeline
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "backends/imgui_impl_glut.h"
#include "backends/imgui_impl_opengl2.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed
#endif

#include <iostream>
#include "GL/glut.h"
#include <chrono>
#include <thread>
#include <vector>
#include <stdlib.h>
#include "geometry.h"

 /* Window information */
int windowWidth = 800;
int windowHeight = 600;

std::vector<Shape*> shapes;
std::vector<Square*> vecSquares;
std::vector<Circle*> vecCircles;
int _id = 1;
float bounceFactor = 0.3f;
bool isClicked = false;

/* IDs of the menu and the submenus */
int menuID;
bool run = false;
bool draww = false;
bool del = false;
bool was_clicked = false;
bool line = false;
int type = 0;
float prevx, prevy;
float vel = 1.0f;
float acc = 0.0;
int sizee = 10;
bool past = false;

// Maybe a function like this will be used to draw a line from the center of the square 
// to the mouse when creating an initial motion vector
/**
 * Draws a line connecting the two squares
 */
void drawLine(float x, float y) {
    float centers[4] = {
        prevx, prevy, x, y
    };
    float lineColor[6] = {
        0.0, 0.0, 0.0, // red
        0.0, 0.0, 0.0 // red
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
 *	Function invoked when window system events are not being received
 */
void idle() {
    /* Redraw the window */
    glutPostRedisplay();
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

    /* Add floor square */
    Square* floor = new Square(400, Vec2f(400, 1200), Vec2f(0, 0), Vec2f(0, 0), 0);
    vecSquares.push_back(floor);
    shapes.push_back(floor);
}


/**
 *	Function invoked when a mouse event occur
 */

void mouse(ImGuiIO& io) {
    float x = io.MousePos.x;
    float y = io.MousePos.y;
    bool now = io.MouseDown[GLUT_LEFT_BUTTON];
    if (!now && past && !was_clicked) {

        // create a square object whenever the mouse gets clicked
        if (draww) {

            if (line) {
                if (shapes.back()->access().compare("Circle") == 0) {
                    Vec2f inputDir = Vec2f(x - prevx, y - prevy) / 10.0f;
                    vecCircles.back()->velocity = inputDir * vel;
                    vecCircles.back()->acceleration = inputDir * acc / 100.0f;
                    vecCircles.back()->acceleration.y += 0.3;
                    shapes.back()->velocity = inputDir * vel;
                    shapes.back()->acceleration = inputDir * acc / 100.0f;
                    shapes.back()->acceleration.y += 0.3;
                }

                line = false;
            }
            else {
                if (type == 0) {
                    Square* square = new Square((float)sizee, Vec2f(x, y), Vec2f(0, 0), Vec2f(0.0, 0.3), _id);
                    shapes.push_back(square);
                    vecSquares.push_back(square); // This was the easiest way, though not pretty, to ensure that I had a list of just squares
                }
                else {
                    Circle* circle = new Circle((float)sizee, Vec2f(x, y), Vec2f(2, -10), Vec2f(0.0, 0.3), _id);
                    shapes.push_back(circle);
                    vecCircles.push_back(circle); // This was the easiest way, though not pretty, to ensure that I had a list of just circles
                }
                prevx = x;
                prevy = y;
                _id++;
                if (!run && type != 0) {
                    line = true;
                }
            }
        }
        else if (del) {
            //iterate backwards
            for (int i = shapes.size() - 1; i >= 0; i--) {

                if (shapes[i]->del(x, y)) {
                    int id = shapes[i]->id;
                    if (shapes[i]->access().compare("Circle") == 0) {
                        for (int j = vecCircles.size() - 1; j >= 0; j--) {

                            if (vecCircles[j]->id == id) {
                                vecCircles.erase(vecCircles.begin() + j);
                                break;
                            }
                        }
                    }
                    else {
                        for (int j = vecSquares.size() - 1; j >= 0; j--) {
                            if (vecSquares[j]->id == id) {
                                vecSquares.erase(vecSquares.begin() + j);
                                break;
                            }
                        }
                    }
                    shapes.erase(shapes.begin() + i);
                    break;
                }
            }
        }
    }
    else {
        if (line) {
            drawLine(x, y);
        }
    }
   if (!now && was_clicked) {
        was_clicked = false;
    }
    past = now;
}

// Our state

void my_display_code() {
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {

        ImGui::Begin("Menu");
        if (ImGui::Button("Play")) {
            run = !run;
            was_clicked = true;
            line = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear All")) {
            // Buttons return true when clicked (most widgets return true when edited/activated)
            shapes.clear();
            vecCircles.clear();
            vecSquares.clear();
            was_clicked = true; line = false;
        }

        ImGui::Text("Draw");

        if (ImGui::Button("Square")) {                         // Buttons return true when clicked
            del = false; draww = true; line = false; type = 0; was_clicked = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Circle")) {                         // Buttons return true when clicked
            del = false; draww = true; line = false; type = 1; was_clicked = true;
        }

        ImGui::Text("Tools");
        if (ImGui::Button("Erase")) {                         // Buttons return true when clicked 
            del = true; draww = false; line = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cursor")) {                         // Buttons return true when clicked
            del = false; draww = false; line = false;
        }

        if (ImGui::SliderInt("Size", &sizee, 1, 200)) {
            line = false; was_clicked = true; 
        }
        if (ImGui::SliderFloat("Vel", &vel, 0.0f, 100.0f)) {
            line = false; was_clicked = true;
        }
        if (ImGui::SliderFloat("Acc", &acc, 0.0f, 10.0f)) {
            line = false; was_clicked = true;
        }
        if (ImGui::SliderFloat("Bounce", &bounceFactor, 0.0f, 2.0f)) {
            line = false; was_clicked = true; 
        }


        ImGui::End();
    }
}

void refresh() {
    glutReshapeWindow(windowWidth, windowHeight);
    // Start the Dear ImGui frame
    glClear(GL_COLOR_BUFFER_BIT);
    // This is for capping at 60fps
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    start = std::chrono::steady_clock::now();

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();

    my_display_code();

    // Rendering
    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO();

    io.DisplaySize = ImVec2((float)windowWidth, (float)windowHeight);
    
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);

    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.

    // Update position and display
    for (auto& shape : shapes) {
        shape->update();
        shape->display();
    }

    mouse(io);

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    /* Force execution of OpenGL commands */

    end = std::chrono::steady_clock::now();
    int elapsed = (int)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::this_thread::sleep_for(std::chrono::milliseconds((1000 / 60) - elapsed));


    glutSwapBuffers();
    glutPostRedisplay();

}

// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

int main(int argc, char** argv)
{
    // Create GLUT window
    glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("2D Physics Sandbox");

    // Setup GLUT display function
    // We will also call ImGui_ImplGLUT_InstallFuncs() to get all the other functions installed for us,
    // otherwise it is possible to install our own functions and call the imgui_impl_glut.h functions ourselves.
    /* Set OpenGL initial state */
    init();

    glutDisplayFunc(refresh);
    glutIdleFunc(idle);


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGLUT_Init();
    ImGui_ImplGLUT_InstallFuncs();
    ImGui_ImplOpenGL2_Init();

    glutMainLoop();

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
