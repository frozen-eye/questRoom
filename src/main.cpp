#define _USE_MATH_DEFINES

#include <GLFW/glfw3.h>
#include <assert.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <cmath>

#include "room.h"

// window
GLFWwindow *window;

// mouse and observer positions
Point mouse;
Point observer_position;

// room dimensions
Dimensions room{0, 0};

// storage for phigures, lines, points
vector<Phigure> phigures;
map<unsigned int, Line> lines;
vector<Point> points;

// visible elements
set<unsigned int> visible_line_ids;

// number of elements in file
unsigned int number_of_elements = 0;

static void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
    mouse.x = xpos;
    mouse.y = ypos;
}

static bool read_configuration(const char *filename)
{
    // read the text file here
    ifstream in_file{filename};
    string str;
    char ch;

    //check if the file is open
    if (in_file.is_open())
    {
        in_file >> room.width;
        in_file >> room.height;

        cout << "Room: " << room.width << 'x' << room.height << endl;

        in_file >> observer_position.x;
        in_file >> observer_position.y;

        cout << "Observer position: " << observer_position.x << ',' << observer_position.y << endl;

        in_file >> number_of_elements;
        assert(number_of_elements > 0);

        cout << "Number of elements: " << number_of_elements << endl
             << endl
             << "Cartesian coordinates:" << endl;

        // skip new line
        in_file.ignore(1, '\n');

        for (unsigned int idx = 0; idx < number_of_elements; idx++)
        {
            Phigure phigure;
            phigure.id = idx;

            getline(in_file, str);
            istringstream istr(str);

            while (!istr.eof())
            {
                Point p;

                istr >> p.x;
                istr >> ch;
                assert(ch == ',');

                istr >> p.y;
                cout << p.x << ',' << p.y << ' ';

                // adjust according to observer position
                p.x -= observer_position.x;
                p.y -= observer_position.y;

                // save the point
                phigure.points.push_back(p);
            }
            cout << endl;
            phigures.push_back(phigure);
        }
    }
    cout << endl;

    // close file
    in_file.close();
    return true;
}

bool cartesian_to_polar()
{
    cout << "Polar coordinates:" << endl;
    for (auto ph = phigures.begin(); ph != phigures.end(); ph++)
    {
        for (auto p = ph->points.begin(); p != ph->points.end(); p++)
        {
            // we can skip here SQRT() calcucation as we need only relative values, not absolute ones
            double fi = sqrt(p->x * p->x + p->y * p->y);

            // make the screen more user-friendly
            p->x = (atan2(p->y, p->x) + M_PI);
            p->y = fi;

            cout << p->x << "," << p->y << " ";
        }
        cout << endl;
    }
    cout << endl;

    return true;
}

void build_sort_lines()
{
    cout << "Lines calculated based on points: " << endl;
    for (auto it = phigures.begin(); it != phigures.end(); it++)
    {
        Point last_point = *it->points.begin();
        for (auto point = it->points.rbegin(); point != it->points.rend(); point++)
        {
            Line line;
            line.owner = it->id;

            // need to sort lines
            if (point->x > last_point.x)
            {
                line.start = last_point;
                line.end = *point;
            }
            else
            {
                line.start = *point;
                line.end = last_point;
            }

            // push lines into phigures
            it->lines.push_back(line);

            cout << line.start.x << ',' << line.start.y << " .. " << line.end.x << ',' << line.end.y << "; ";
            last_point = *point;
        }
        cout << endl;
    }
}

void mark_lines()
{
    unsigned int index = 1;
    for (auto it = phigures.begin(); it != phigures.end(); it++)
    {
        for (auto line = it->lines.begin(); line != it->lines.end(); line++)
        {
            line->id = index;
            line->k = (line->end.y - line->start.y) / ((line->end.x - line->start.x));
            line->b = (line->start.y - line->k * line->start.x);
            lines[index++] = *line;
        }
    }
}

void mark_sort_points()
{
    for (auto it = phigures.begin(); it != phigures.end(); it++)
    {
        for (auto line = it->lines.begin(); line != it->lines.end(); line++)
        {
            // set owner to line id
            line->start.owner = line->id;
            line->end.owner = line->id;

            // update flags
            line->start.flag = true;
            line->end.flag = false;

            // push to vector
            points.push_back(line->start);
            points.push_back(line->end);
        }
    }

    // sort points
    sort(points.begin(), points.end(), [](Point a, Point b) { return a.x < b.x; });
}

void find_visible_lines()
{
    set<unsigned int> ids;

    Point x = *points.begin();
    bool not_first = false;
    for (auto point = points.begin(); point != points.end(); point++)
    {
        if (not_first && x.x != point->x)
        {
            float min_y = x.y;
            int id = x.owner;

            for (auto it = ids.begin(); it != ids.end(); it++)
            {
                Line line = lines[*it];
                float y = x.x * line.k + line.b;

                if (y < min_y)
                {
                    min_y = y;
                    id = line.id;
                }
            }
            visible_line_ids.insert(id);
        }

        if (point->flag)
        {
            ids.insert(point->owner);
        }
        else
        {
            ids.erase(point->owner);
        }
        x = *point;
        not_first = true;
    }
}

void display_visible_lines_and_phigures()
{
    set<unsigned int> visible_phigures;

    for (auto it = visible_line_ids.begin(); it != visible_line_ids.end(); it++)
    {
        unsigned int owner = lines[*it].owner;
        visible_phigures.insert(owner);
    }

    cout << endl << "Visible phigures: ";
    for (auto idx = visible_phigures.begin(); idx != visible_phigures.end(); idx++)
    {
        cout << *idx << ' ';
    }

    cout << endl;
    cout << "Total number of visible phigures: " << visible_phigures.size() << endl;
}

void drawScene()
{
    GLfloat red = 0.0f, green = 1.0f, blue = 0.0f;

    glLoadIdentity();                           // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glOrtho(0, 2 * M_PI, 0, room.height, 0, 1); // essentially set coordinate system

    // set color for all phigures here
    glColor3f(red, green, blue);

    // draw all phigures
    for (auto phigure = phigures.begin(); phigure != phigures.end(); phigure++)
    {
        glBegin(GL_LINE_LOOP);
        for (auto point = phigure->points.begin(); point != phigure->points.end(); point++)
        {
            glVertex2f(point->x, point->y);
        }
        glEnd();
    }
}

void drawMouseXY()
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);

    glBegin(GL_LINE_LOOP);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(mouse.x, 0.0f);
    glVertex2f(mouse.x, height);

    glEnd();

    glPopMatrix();
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *win, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

int main(void)
{
    // read a configuration file
    read_configuration(DEFAULT_FILENAME);

    // transform cartesian to polar coordinate system
    cartesian_to_polar();

    // build lines
    build_sort_lines();

    // mark lines with numbers
    mark_lines();

    // mark and sort points
    mark_sort_points();

    // find visible lines
    find_visible_lines();

    // display visible lines and phigures
    display_visible_lines_and_phigures();

    // glfw: initialize and configure
    // ------------------------------
    if (!glfwInit())
    {
        cerr << "Failed to initialize GLFW" << endl;
        return 1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(room.width, room.height, "QuestRoom", NULL, NULL);

    if (!window)
    {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return 1;
    }

    // attach mouse
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    // resize handler
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set vieport
    glViewport(0.0f, 0.0f, room.width, room.height); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels

    // Get info of GPU and supported OpenGL version
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "OpenGL version supported " << glGetString(GL_VERSION) << endl;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw main scene
        // ---------------
        drawScene();

        // draw mouse-axis
        // ---------------
        drawMouseXY();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}