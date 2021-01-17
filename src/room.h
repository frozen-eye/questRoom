#pragma once

#include <vector>

using namespace std;

// settings
#define DEFAULT_FILENAME "in.txt"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

struct Point
{
    float x;
    float y;

    unsigned int owner;
    bool flag;
};

struct Line
{
    Point start;
    Point end;
    unsigned int id;
    unsigned int owner;

    // line formula kx+b
    float k;
    float b;
};

struct Dimensions
{
    int width;
    int height;
};

struct Phigure
{
    vector<Point> points;
    vector<Line> lines;

    unsigned int id;
};