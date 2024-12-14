#include <Windows.h>

#include <iostream>
#include <iomanip>

#include <gl/GL.h>
#include <gl/GLU.h>
#include <freeglut.h>

#include <vector>
#include <cmath>

#define M_PI 3.14159265358979323846         

using namespace std;


// defining as global variable 

vector<vector<float>> spheres;  // holds coordinate fo each sphere take from calculatePacking() fucntion   

double radiusGLOBAL{};
double widthGLOB{}, heightGLOB{}, depthGLOB{};
float posX{}, posY{}, posZ{};

// FUNCTION PROTOTYPES:  

//calculates the positions   

void drawSpheres();                     //draws a sphere on the matrice

void initGL();                          //all open GL initialization functions

void display();                         //draws the spheres on the matrice scene using the positions from "spheres" vector

void reshape(int width, int height);    //reshapes 

void generateSpherePos(float radius, float width, float height, float depth, float density); //generates spheres positions

void userInputUnitTest(float& radius, float& width, float& height, float& depth); //adding uit testing fucntion as main is too crowded

void drawWireframeBox(float width, float height, float depth);


// main program, int argc, char** argv are to intialize the open gl
int main(int argc, char** argv) { 

        float radius{}, width{}, height{}, depth{}, density{}; //width - y axis, height - z axis, depth - x axis 

        int totalSpheres;  //stores total amount of spheres possible to fit in a simple grid given dimensions

        cout << fixed << setprecision(2); 
       
        //calls to get unit input and unit test it
        userInputUnitTest(radius, width, height, depth);

        //call to fill the vector "spheres" with data
        generateSpherePos(radius, width, height, depth, density);


        // display results of total spheres and density 
        cout << "Total number of spheres: " << static_cast<int>(spheres.size() / 1.8) << endl;

        density = static_cast<float>((4.0 / 3.0) * M_PI * pow(radius, 3.0) * (spheres.size() / 1.8)) / static_cast<float>(width * height * depth);

        cout << "Packing density Volume Sphere to : " << density << endl;



        // initialize open GL for visualization
        glutInit(&argc, argv);

        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

        glutInitWindowSize(1000, 1000);

        glutCreateWindow("Sphere packing display");

        // initializes open gl settings and functions
        initGL();

        // create display and make reshape the window
        glutDisplayFunc( display );
        
        glutReshapeFunc( reshape );
  
        // starts open gl visualization
        glutMainLoop();

    return 0;
}


void generateSpherePos(float radius, float width, float height, float depth, float density) {
        spheres.clear();



        // FCC unit cell positions relative to the corner (0, 0, 0)
        float a = 2.0f * radius * sqrt(2.0);  // Distance between centers of adjacent spheres

        // Generate FCC sphere positions
        for (float x = 0; x < width; x += a) {
        
            for (float y = 0; y < height; y += a) {

                for (float z = 0; z < depth; z += a) {

                    // corner positions (basic lattice points)
                    spheres.push_back({ x, y, z });

                    // face-centered positions

                    if ( (x + a / 2 < width) )
                    {
                        spheres.push_back({ x + a / 2, y, z });
                    }

                    if ( (y + a / 2 < height ))
                    {
                        spheres.push_back({ x, y + a / 2, z });
                    }

                    if ( z + a / 2 < depth)
                    {
                        spheres.push_back({ x, y, z + a / 2 });
                    }

                    // Add body-centered position
                    if ( x + a / 2 < width && y + a / 2 < height && z + a / 2 < depth)
                    {

                        spheres.push_back({ x + a / 2, y + a / 2, z + a / 2 });

                    }
                }
            }
       }
}

// function to draw spheres in OpenGL
void drawSpheres() {

    for ( vector<float> pos : spheres)            //for all number of spheres the loop will run
    {
        glPushMatrix();                           //saves the matrice and doesnt allow previous spheres to modify it
                                                  //allowing to render the spheres separately, not affecting each other 
                                                  //otherwise position and translation transformation of previous sphere
                                                  //would affect position of next one, as I am basically moving sphere from
                                                  //center of the scene to its position inside the box, so it allows to 
                                                  //render each sphere independently, and saves the previous transformation 

        glTranslatef(pos[0], pos[1], pos[2]);     // glTranslatef( x , y , z )

        glutSolidSphere(radiusGLOBAL, 20, 20);          // Draw the sphere at this position

        glPopMatrix();                            //after the new sphere is translated on the matrice, it restores 
                                                  //the saved matrice to display previous and most recent objects on the grid

    }
}

// open GL initialization of scene - color modifications 
void initGL() {

    glClearColor(1.0, 1.0, 1.0, 1.0);   //fills the background white for better visability

    glEnable(GL_DEPTH_TEST);            //makes the spheres 3D - visible

    glEnable(GL_LIGHTING);              //enabling the light on the scene to see the spheres

    glEnable(GL_LIGHT0);                //initiazlizes the default source of light

}

// open Gl display function
void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         //allows to render color on the scene 

    gluLookAt(posX, posY, posZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);        //setting camera position to look at scene 

    drawSpheres();                                              //draws spheres on the scene

    drawWireframeBox(widthGLOB + radiusGLOBAL, heightGLOB + radiusGLOBAL, depthGLOB + radiusGLOBAL);

    glutSwapBuffers();                                          //allows for rendering the scene, othwerise screen 
                                                                //is black as it doesnt display other frames in the buffer
}

// open GL reshape function to maintain screen aspect ratios and display matrice correctly, rendering settings
void reshape(int width, int height) {

    float aspect{};

    aspect = static_cast<float>(width) / height;    //value that allowes to maintain the correct proportions
                                                    //scaling of 3d options

    glViewport(0, 0, width, height);                //area of rendering 

    glMatrixMode(GL_PROJECTION);                    //2d to 3d rendering 

    gluPerspective(45.0, aspect, 0.01, 1000.0);       //fov, aspect, near render, far render

}

void userInputUnitTest(float &radius, float &width, float &height, float &depth) {
    // sphere radius and box dimensions input from user

    int suggested{};

    cout << "______________________________________________________________________________________________\n";
    cout << "Enter the whole radius of spheres that you want to pack in a square or rectangle of any dimensions: \n";
   
    
    

    //unit testing for radius 
 
    while (true) {
        cin >> radiusGLOBAL;

        // Validate input
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
            cout << "Invalid input. Please enter a whole number: ";
            continue;
        }
        else if ((radiusGLOBAL <= 0) || (radiusGLOBAL > 30)) {

            cout << "\nRadius must be positive, whole number less than 30: ";
            continue;
        }
        else {
            break;
        }
    }

    radiusGLOBAL = static_cast<int>(radiusGLOBAL);

    radius = static_cast<double>(radiusGLOBAL);

   
    suggested = radius * 5 * 2;

    cout << "\nPlease type choice number.\n";
    
    int choice{}, multi{};
    float dimension{};

    cout << "1. " << "Suggested dimensions of the box: " << suggested << " x " << suggested << " x " << suggested << endl;
    cout << "2. " << "Enter custom height, width and depth. " << endl;

    cin >> choice; 

    //Choice of box dimensions and has unit testing;
    switch (choice) {
        
    case 1: cout << "You chose suggested dimensions of " << suggested << " x " << suggested << " x " << suggested << endl;

        posX = suggested * 2;
        posY = suggested * 2;
        posZ = suggested * 2;

        widthGLOB = suggested;
        heightGLOB = suggested;
        depthGLOB = suggested; 

        break; 


    case 2: cout << "Enter the width, height, and depth of the rectangular box: ";
            cin >> widthGLOB >> heightGLOB >> depthGLOB;

            while
                (
                    cin.fail() ||
                    widthGLOB <= 0 || widthGLOB > 300
                    || heightGLOB <= 0 || heightGLOB > 300
                    || depthGLOB <= 0 || depthGLOB > 300

                )
            {
                cin.clear();
                cin.ignore();

                cout << "All input dimensions and radius must be positive values less than 300." << endl;

                cout << "Enter the width, height, and depth of the rectangular box: " << endl;

                cin >> widthGLOB >> heightGLOB >> depthGLOB;

            }

            posX = widthGLOB * 2;
            posY = heightGLOB * 2;
            posZ = depthGLOB * 2;

            break; 
    }

    width = widthGLOB;
    height = heightGLOB;
    depth = depthGLOB;
    

    //checks if spheres can even fit into the box 
    while (radius * 2 > min(width, height) || radius * 2 > min(width, depth) || radius * 2 > min(depth, height)) {

        cout << "Sphere radius is too large for the given dimensions." << endl;

        cout << "Enter the radius of spheres: " << endl;

        cin >> radius;

    }
   
}

void drawWireframeBox(float width, float height, float depth) {
    
    // Draw a wireframe box around the sphere positions
    glBegin(GL_LINES);
    
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(width, 0.0f, 0.0f);
    
    glVertex3f(width, 0.0f, 0.0f);
    glVertex3f(width, height, 0.0f);

    glVertex3f(width, height, 0.0f);
    glVertex3f(0.0f, height, 0.0f);

    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
   
    glVertex3f(0.0f, 0.0f, depth);
    glVertex3f(width, 0.0f, depth);

    glVertex3f(width, 0.0f, depth);
    glVertex3f(width, height, depth);

    glVertex3f(width, height, depth);
    glVertex3f(0.0f, height, depth);

    glVertex3f(0.0f, height, depth);
    glVertex3f(0.0f, 0.0f, depth);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, depth);

    glVertex3f(width, 0.0f, 0.0f);
    glVertex3f(width, 0.0f, depth);

    glVertex3f(width, height, 0.0f);
    glVertex3f(width, height, depth);

    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(0.0f, height, depth);

    glEnd();
}
