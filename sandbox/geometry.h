#include <cstdlib> 
#include <cstdio> 
#include <iostream> 
#include <vector>

template<typename T>
class Vec2
{
public:
    Vec2() : x(0), y(0) {}
    Vec2(T xx) : x(xx), y(xx) {}
    Vec2(T xx, T yy) : x(xx), y(yy) {}
    Vec2 operator + (const Vec2& v) const
    {
        return Vec2(x + v.x, y + v.y);
    }
    Vec2 operator / (const T& r) const
    {
        return Vec2(x / r, y / r);
    }
    Vec2 operator * (const T& r) const
    {
        return Vec2(x * r, y * r);
    }
    Vec2& operator /= (const T& r)
    {
        x /= r, y /= r; return *this;
    }
    Vec2& operator *= (const T& r)
    {
        x *= r, y *= r; return *this;
    }
    friend Vec2 operator * (const T& r, const Vec2<T>& v)
    {
        return Vec2(v.x * r, v.y * r);
    }
    T x, y;
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;

class Shape
{
public:
    Vec2f center;
    Vec2f velocity;
    Vec2f acceleration;
    int id;

    virtual ~Shape() {}
    Shape();
    Shape(Vec2f c, Vec2f v, Vec2f a, int i);

    // could change or get rid of later, just using this for print statements
    virtual void printInfo() = 0;

    // in charge of drawing the shape
    virtual void display() = 0;

    //if mouse clicked the square
    virtual bool del(float, float) = 0;

    // updates center, velocity, acceleration 
    virtual void update() = 0;

    virtual std::string access() = 0;
};

/**
* Square class
* Notes: Possibly change to rect class in future
*/
class Square : public Shape
{
public:
    float width;
    float squareVertices[8];
    float squareColors[12];

    Square(float w, Vec2f c, Vec2f v, Vec2f a, int i);
    void printInfo();
    void display();
    void update();
    bool del(float, float);
    std::string access();
};

class Circle : public Shape {
public:
    float radius;
    float mass;

    Circle();
    Circle(float r, Vec2f c, Vec2f v, Vec2f a, int i);
    void printInfo();
    void display();//nanogui::GLShader);
    void update();
    bool del(float, float);
    std::string access();
};
