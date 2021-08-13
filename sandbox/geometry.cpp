#include "GL/glut.h"
#include <iostream>
#include <iostream>
#include <time.h>
#include "geometry.h"

extern bool run;
extern float bounceFactor;
extern std::vector<Square*> vecSquares;
extern std::vector<Circle*> vecCircles;


/**
* Default constructor for an Square, just initializes everything to 0
* Add something for creating the ID which will be helpful for collision
*/
Shape::Shape() : center(Vec2f(0.0, 0.0)), velocity(Vec2f(0.0, 0.0)), acceleration(Vec2f(0.0, 0.0)), id(0) {}

/**
* Normal constructor
*/
Shape::Shape(Vec2f c, Vec2f v, Vec2f a, int i) : center(c), velocity(v), acceleration(a), id(i) {}

/**
* Square constructor
* right now only takes width but could take in colors too
* Sets up vertices based off of center, initializes vertices and colors
* I'm sorry that it looks ugly, i'm tired
*/
Square::Square(float w, Vec2f c, Vec2f v, Vec2f a, int i) : width(w), Shape(c, v, a, i),
squareVertices{
    center.x - width, center.y - width,
    center.x + width, center.y - width,
    center.x + width, center.y + width,
    center.x - width, center.y + width },
    squareColors{
        1.0, 0.0, 0.0,			// Red
        0.0, 1.0, 0.0,			// Green
        0.0, 0.0, 1.0,			// Blue
        1.0, 0.0, 1.0			// Magenta
}
{}

/**
* Prints square info
*/
void Square::printInfo() {
    std::cout << "width: " << width << "\n";
    std::cout << "center: " << center.x << " " << center.y << "\n";
    std::cout << "velocity: " << velocity.x << " " << velocity.y << "\n";
    std::cout << "acceleration: " << acceleration.x << " " << acceleration.y << "\n";
    std::cout << "top left: " << squareVertices[0] << " " << squareVertices[1] << "\n";
}

std::string Square::access() {
    return std::string("Square");
}

/**
* Updates position of square, used to do more when squares moved
*/
void Square::update() {
    // Update square vertices
    squareVertices[0] = center.x - width; squareVertices[1] = center.y - width; // top left
    squareVertices[2] = center.x + width; squareVertices[3] = center.y - width; // top right
    squareVertices[4] = center.x + width; squareVertices[5] = center.y + width; // bottom right
    squareVertices[6] = center.x - width; squareVertices[7] = center.y + width; // bottom left
}

void Square::display() {
    /* Draw the square */
    /* Step 1: Enable the clients for the vertex arrays */
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    /* Step 2: Set up arrays and draw them */
    glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    glColorPointer(3, GL_FLOAT, 0, squareColors);
    glDrawArrays(GL_POLYGON, 0, 4);

    /* Step 3: Disable the clients */
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

//checks if mouse clicked the square
bool Square::del(float x, float y) {
    return(x > (center.x - width) && x < (center.x + width) && y >(center.y - width) && y < (center.y + width));
}


Circle::Circle() : radius(0), mass(0), Shape() {}
Circle::Circle(float r, Vec2f c, Vec2f v, Vec2f a, int i) : radius(r), mass(r * 10.0f), Shape(c, v, a, i) {}

/**
* Prints square info
*/
void Circle::printInfo() {
    std::cout << "radius: " << radius << "\n";
    std::cout << "center: " << center.x << " " << center.y << "\n";
    std::cout << "velocity: " << velocity.x << " " << velocity.y << "\n";
    std::cout << "acceleration: " << acceleration.x << " " << acceleration.y << "\n";
    std::cout << "id: " << id << "\n";
}

std::string Circle::access() {
    return std::string("Circle");
}

/**
* Updates the position of the circles each frame
*/
void Circle::update() {
    // Lamda function to decide if a circle overlaps with another
    auto DoCirclesOverlap = [](float x1, float y1, float r1, float x2, float y2, float r2) {
        return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) <= (r1 + r2) * (r1 + r2);
    };

    if (run) {
        velocity.x += acceleration.x;
        velocity.y += acceleration.y;
        center.x += velocity.x;
        center.y += velocity.y;
    }

    //Floor check
    if (center.y + radius > 600) {
        // Set the block on the floor and make sure it doesn't accelerate in the x direction or move in the y anymore
        center.y = 600 - radius;
        acceleration.x = 0.0;
        velocity.y = 0;
        // have it come to a stop (50.0 is just a seemingly decent number)
        velocity.x -= (velocity.x / 50.0);
        if (velocity.x <= 0.01 && velocity.x >= -0.01) {
            velocity.x = 0;
        }
    }

    // Stops circles if their speed is negligible
    if ((velocity.x * velocity.x) + (velocity.y * velocity.y) < 0.01) {
        velocity.y = 0;
        velocity.x = 0;
    }

    std::vector<std::pair<Circle*, Circle*>> collidingPairs;
    std::vector<Circle*> vecFakeCircles;

    // This will handle static collisions between edgees and circles
    for (auto& circle : vecCircles) {

        // Collision between edges
        for (auto& square : vecSquares) {
            float lines[16] = {
                square->squareVertices[0], square->squareVertices[1], square->squareVertices[2], square->squareVertices[3],
                square->squareVertices[2], square->squareVertices[3], square->squareVertices[4], square->squareVertices[5],
                square->squareVertices[4], square->squareVertices[5], square->squareVertices[6], square->squareVertices[7],
                square->squareVertices[6], square->squareVertices[7], square->squareVertices[0], square->squareVertices[1],
            };
            // for each line of the square
            for (int i = 0; i < 16; i += 4) {

                float sx = lines[i]; // start x
                float sy = lines[i + 1];
                float ex = lines[i + 2]; // end x
                float ey = lines[i + 3];

                // check collision against each edge

                float lineX1 = ex - sx; // line between end and start of side of square
                float lineY1 = ey - sy;

                // line from circle to start of edge
                float lineX2 = circle->center.x - sx;
                float lineY2 = circle->center.y - sy;

                float edgeLength = lineX1 * lineX1 + lineY1 * lineY1;

                // The min and max make sure to clamp the value between 0 and the length of the line segment
                // which is then normalized. 
                float t = std::max(0.0f, std::min(edgeLength, lineX1 * lineX2 + lineY1 * lineY2)) / edgeLength;

                // Find the point on the line that is closest to the circle
                float closestPointX = sx + t * lineX1;
                float closestPointY = sy + t * lineY1;

                // we can check to see if the circle collided with the line based off of how far the point is compared
                // to the radius of the circle
                float distance = sqrtf((circle->center.x - closestPointX) * (circle->center.x - closestPointX) + (circle->center.y - closestPointY) * (circle->center.y - closestPointY));
                if (distance <= circle->radius) {
                    // Collision has occured!
                    // We're gonna treat the collision as if the circle hit another stationary circle at the same spot it hit the edge
                    Circle* fakeCircle = new Circle();
                    fakeCircle->radius = 1; // this is a bit of a hack, but hopefully the circle won't poke out the edges of the square
                    fakeCircle->mass = circle->mass * bounceFactor; // The mass is also a hack, 
                    fakeCircle->center.x = closestPointX; // this should be changed later but it'll be mathy to figure out how
                    fakeCircle->center.y = closestPointY;
                    fakeCircle->velocity.x = -circle->velocity.x;
                    fakeCircle->velocity.y = -circle->velocity.y;

                    // store the fake circle
                    vecFakeCircles.push_back(fakeCircle);

                    // Add collision to vector of colliding pairs (for dynamic resolution later)
                    collidingPairs.push_back({ circle, fakeCircle });

                    // calculate displacement required so they don't overlap
                    float overlap = (distance - circle->radius - fakeCircle->radius);

                    // Displace current ball away from collision
                    circle->center.x -= overlap * (circle->center.x - fakeCircle->center.x) / distance;
                    circle->center.y -= overlap * (circle->center.y - fakeCircle->center.y) / distance;
                }
            }
        }

        for (auto& target : vecCircles) {
            // Check to see that collision is not with self
            if (circle->id != target->id) {
                float cx = circle->center.x;
                float cy = circle->center.y;
                float cr = circle->radius;
                float tx = target->center.x;
                float ty = target->center.y;
                float tr = target->radius;
                // if a collision has occured 
                if (DoCirclesOverlap(cx, cy, cr, tx, ty, tr)) {
                    //std::cout << "collision between circles\n";
                    //circle->printInfo();
                    // Add to colliding pairs vector 
                    collidingPairs.push_back({ circle, target });

                    // Find the distance between the centers of the circles
                    float distance = sqrtf((cx - tx) * (cx - tx) + (cy - ty) * (cy - ty));
                    // Calculate the displacement required for both circles
                    float overlap = 0.5f * (distance - cr - tr);

                    // Displace current circle away from collision
                    circle->center.x -= overlap * (cx - tx) / distance; // x direction
                    circle->center.y -= overlap * (cy - ty) / distance; // y direction

                    // Displace target away from collision
                    target->center.x += overlap * (cx - tx) / distance; // x direction
                    target->center.y += overlap * (cy - ty) / distance; // y direction
                }
            }
        }
    }

    // Dynamic collisions
    for (auto& c : collidingPairs) {
        Circle* c1 = c.first;
        Circle* c2 = c.second;

        // Distance between circles
        float distance = sqrtf((c1->center.x - c2->center.x) * (c1->center.x - c2->center.x) +
            (c1->center.y - c2->center.y) * (c1->center.y - c2->center.y));

        // Normal 
        float nx = (c2->center.x - c1->center.x) / distance;
        float ny = (c2->center.y - c1->center.y) / distance;

        // Tangent
        float tx = -ny;
        float ty = nx;

        // Dot product tangent
        float dpTan1 = c1->velocity.x * tx + c1->velocity.y * ty;
        float dpTan2 = c2->velocity.x * tx + c2->velocity.y * ty;

        // Dot product normal
        float dpNorm1 = c1->velocity.x * nx + c1->velocity.y * ny;
        float dpNorm2 = c2->velocity.x * nx + c2->velocity.y * ny;

        // Wikipedia conservation of momentum
        float kx = (c1->velocity.x - c2->velocity.x);
        float ky = (c1->velocity.y - c2->velocity.y);
        float p = 2.0 * (nx * kx + ny * ky) / (c1->mass + c2->mass);

        // Update circle velocities
        c1->velocity.x = c1->velocity.x - p * c2->mass * nx;
        c1->velocity.y = c1->velocity.y - p * c2->mass * ny;
        c2->velocity.x = c2->velocity.x + p * c1->mass * nx;
        c2->velocity.y = c2->velocity.y + p * c1->mass * ny;
    }

    collidingPairs.clear();
}

void Circle::display() {
    /* Draw the circle */
    const int n = 50;
    glColor3ub(255, 0, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(center.x, center.y);
    for (int i = 0; i < n + 1; i++) {
        float x = (float)center.x + (radius * cos(i * 2 * 3.1416 / n));
        float y = (float)center.y + (radius * sin(i * 2 * 3.1416 / n));
        glVertex2f(x, y);
    }
    glEnd();
}

//checks if mouse clicked the square
bool Circle::del(float x, float y) {
    double xdiff = abs(center.x - x);
    double ydiff = abs(center.y - y);
    return(sqrt(xdiff * xdiff + ydiff * ydiff) < radius);
}
