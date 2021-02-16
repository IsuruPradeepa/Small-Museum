#include <stdlib.h>
#include <GL/glut.h>  
#include <math.h>
#include "SOIL2/SOIL2.h"
#include <iostream>

#define GL_CLAMP_TO_EDGE 0x812F
#define PI = 3.14159;

// vertices for the cube
GLfloat x = 30.0f;
GLfloat y = -20.0f;
GLfloat z = 50.0f;

// vertices for the small cube
GLfloat p = 6.0f;
GLfloat q = 6.0f;
GLfloat r = 6.0f;

// variables to move outermost Object Frame (to move all the rendered environment)
GLfloat moveX = 0.0f;
GLfloat moveY = 0.0f;
GLfloat moveZ = 0.0f;

// variables to rotate outermost Object Frame (to move all the rendered environment)
GLfloat rotX = 0.0f;
GLfloat rotY = 0.0f;
GLfloat rotZ = 0.0f;


// For animating the rotation of the objects
GLfloat animateRotation = 0.0f;

//variables to move the camera
GLfloat camY = 0.0f;
GLfloat camX = 0.0f;
GLfloat camZ = 0.0f;


#define PI 3.14159265f
GLfloat rot = 0.0f;
GLfloat radius = 9.0;

//An array to store the normal vector data for the faces
GLfloat vNormal[3] = {};

GLuint tex;

void initTexture() {

    int width, height;
    unsigned char* image = SOIL_load_image("D:/CS Practicals/300 Level/CS308 Computer Graphics Programming/Individual Project/SmallMuseum/Images/brick.jpg", &width, &height, 0, SOIL_LOAD_RGB);


    if (!image) {
        std::cout << "Failed to load texture: " << sizeof(image) << std::endl;
    }
    else {
        std::cout << &image << std::endl;
    }


    unsigned char data[] =
    {
        128, 128, 128, 255,
        255, 0, 0, 255,
        0, 255, 0, 255,
        0, 0, 255, 255
    };

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
        GL_UNSIGNED_BYTE, image);

    // Use the following line in order to load the created texture map instead of the image
    // glTexImage2D( GL_TEXTURE_2D, 0,GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );    

}


//Function to initialize the lighting properties. you can add upto 8 lights in openGL (0-7)
void initLighting() {

    //Decalring the Ambient, Diffuse components of the LIght_0 and the position in the eye coordinate system
    GLfloat L0_Ambient[] = { 0.4, 0.4, 0.4, 1.0 };
    GLfloat L0_Diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat L0_postion[] = { 5, 5, 0, 1.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, L0_Ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, L0_Diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, L0_postion);

    GLfloat L1_Ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat L1_Diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat L1_Specular[] = { 0.0, 1.0, 0.0, 1.0 };   //Declaration of the specular component of the light_1
    GLfloat L1_postion[] = { -5, 5, 0, 1.0 };

    glLightfv(GL_LIGHT1, GL_AMBIENT, L1_Ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, L1_Diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, L1_Specular);
    glLightfv(GL_LIGHT1, GL_POSITION, L1_postion);

    //Declaration of the ligt reflecting properties for the materials
    GLfloat specularReflectance[] = { 1.0, 1.0, 1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularReflectance);
    glMateriali(GL_FRONT, GL_SHININESS, 50);


    GLfloat L2_Ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat L2_Diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat L2_Specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat L2_postion[] = { 0, 5, 5, 1.0 };
    GLfloat L2_SpotDirection[] = { 0.0, -1.0, -1.0 };

    glLightfv(GL_LIGHT2, GL_AMBIENT, L2_Ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, L2_Diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, L2_Specular);
    glLightfv(GL_LIGHT2, GL_POSITION, L2_postion);

    //Creating a spotlight from light_2 by declaring the direction vetor and area that the spotligt can shine(fov of the spotlight)
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, L2_SpotDirection);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0);


}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    GLfloat globalAmbient[] = { 0.4, 0.4, 0.4, 1.0 };
    glEnable(GL_DEPTH_TEST);

    //Enable the adding of lights
    glEnable(GL_LIGHTING);


    glShadeModel(GL_SMOOTH);

    // Changing the global Ambient illumination without any lights present
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    (GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    initLighting();

    // Enabling the color tracking of each faces of the materials. this enables the color visibility of the materials
    glEnable(GL_COLOR_MATERIAL);

    //Turn on lighting
    glEnable(GL_LIGHT0);
    // glEnable(GL_LIGHT1);
    // glEnable(GL_LIGHT2);

    // enable the normalization of the normal vectors (converting the vector values to the range 0-1)
    glEnable(GL_NORMALIZE);

    initTexture();
}

//Calculate the normals for two vectors va, vb ( normal for a plane/face )
GLfloat* calNormal(GLfloat* va, GLfloat* vb) {
    vNormal[0] = va[1] * vb[2] - va[2] * vb[1];
    vNormal[1] = va[0] * vb[2] - va[2] * vb[0];
    vNormal[2] = va[0] * vb[1] - va[1] * vb[0];

    return vNormal;
}

// Find the Normal from three points in 3d space.
// BE AWARE THAT THE POINTS SHOULD BE DECLARED IN ANTI-CLOCKWISE ORDER FOR THE TWO SIDES(FORMING THE TWO VECTORS)
// HAVING ONE VERTEX COMMON TO BOTH SIDES (vertices abc form two sides as 'ab' and 'ac')
// Otherwise you'll get the wrong/ reverse vector
GLfloat* findNormal(GLfloat* a, GLfloat* b, GLfloat* c) {

    GLfloat va[] = { b[0] - a[0], b[1] - a[1], b[2] - a[2] };
    GLfloat vb[] = { c[0] - a[0], c[1] - a[1], c[2] - a[2] };

    return calNormal(va, vb);
}

void DrawGrid() {
    GLfloat ext = 20.0f;
    GLfloat step = 1.0f;
    GLfloat yGrid = -0.45f;
    GLint line, line_x;

    glPushMatrix();
    glColor3f(0.3, 0.3, 0.3);
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0.0, 0.0, yGrid);
    for (line = -ext; line <= ext; line += step) {
        for (line_x = -ext; line_x <= ext; line_x += step) {
            glRectf(line_x + 0.02, line + 0.02, line_x + step - 0.02, line + step - 0.02);
        }

    }

    glPopMatrix();

}

void drawPyramid() {
    glBegin(GL_TRIANGLES);
    {
        GLfloat front[3][3] = {
            {-2.5, 0, 2.5},
            {2.5, 0, 2.5},
            {0, 4.0, 0}
        };

        glNormal3fv(findNormal(front[0], front[1], front[2]));
        glTexCoord2f(0, 0);
        glVertex3fv(front[0]);
        glTexCoord2f(1, 0);
        glVertex3fv(front[1]);
        glTexCoord2f(0.5, 1);
        glVertex3fv(front[2]);
    }

    {
        GLfloat back[3][3] = {
            {2.5, 0, -2.5},
            {-2.5, 0, -2.5},
            {0, 4.0, 0}
        };

        glNormal3fv(findNormal(back[0], back[1], back[2]));
        glTexCoord2f(0, 0);
        glVertex3fv(back[0]);
        glTexCoord2f(1, 0);
        glVertex3fv(back[1]);
        glTexCoord2f(0.5, 1);
        glVertex3fv(back[2]);
    }

    {
        GLfloat left[3][3] = {
            {-2.5, 0, -2.5},
            {-2.5, 0, 2.5},
            {0, 4.0, 0}
        };

        glNormal3fv(findNormal(left[0], left[1], left[2]));
        glTexCoord2f(0, 0);
        glVertex3fv(left[0]);
        glTexCoord2f(1, 0);
        glVertex3fv(left[1]);
        glTexCoord2f(0.3, 1);
        glVertex3fv(left[2]);
    }

    {
        GLfloat right[3][3] = {
            {2.5, 0, 2.5},
            {2.5, 0, -2.5},
            {0, 4.0, 0}
        };

        glNormal3fv(findNormal(right[0], right[1], right[2]));
        glTexCoord2f(0, 0);
        glVertex3fv(right[0]);
        glTexCoord2f(1, 0);
        glVertex3fv(right[1]);
        glTexCoord2f(0.5, 1);
        glVertex3fv(right[2]);
    }


    glEnd();
}


/*
Use this function to identify the transformations(translation/ rotation/ scaling) if needed
(call this function at the place you need to determine the orientation of the axes)
*/
void drawAxes() {

    glBegin(GL_LINES);

    glLineWidth(1.5);

    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-20, 0, 0);
    glVertex3f(20, 0, 0);

    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, -20, 0);
    glVertex3f(0, 20, 0);

    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0, 0, -20);
    glVertex3f(0, 0, 20);

    glEnd();
}

void drawCube() {
    // glTranslatef(0.0, 1.6, 0.0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // BACK
    glBegin(GL_QUADS);
    glColor3f(0.4f, 0.3f, 0.5f);
    glVertex3f(x, y, z);
    glVertex3f(x, -y, z);
    glVertex3f(-x, -y, z);
    glVertex3f(-x, y, z);
    glEnd();

    // FRONT
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(x, y, -z);
    glVertex3f(-x, y, -z);
    glVertex3f(-x, -y, -z);
    glVertex3f(x, -y, -z);
    glEnd();

    // LEFT
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(-x, -y, -z);
    glVertex3f(-x, y, -z);
    glVertex3f(-x, y, z);
    glVertex3f(-x, -y, z);
    glEnd();


    //Right
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(x, y, z);
    glVertex3f(x, y, -z);
    glVertex3f(x, -y, -z);
    glVertex3f(x, -y, z);
    glEnd();


    //Bottom
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(x, y, -z);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(x, y, z);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(-x, y, z);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-x, y, -z);
    glEnd();

    //Top
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(x, -y, z);
    glVertex3f(x, -y, -z);
    glVertex3f(-x, -y, -z);
    glVertex3f(-x, -y, z);
    glEnd();
}

void drawSmallCube() {
    // glTranslatef(0.0, 1.6, 0.0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // BACK
    glBegin(GL_QUADS);
    glColor3f(0.4f, 0.3f, 0.5f);
    glVertex3f(p, q, r);
    glVertex3f(p, -q, r);
    glVertex3f(-p, -q, r);
    glVertex3f(-p, q, r);
    glEnd();

    // FRONT
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(p, q, -r);
    glVertex3f(-p, q, -r);
    glVertex3f(-p, -q, -r);
    glVertex3f(p, -q, -r);
    glEnd();

    // LEFT
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(-p, -q, -r);
    glVertex3f(-p, q, -r);
    glVertex3f(-p, q, r);
    glVertex3f(-p, -q, r);
    glEnd();


    //Right
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(p, q, r);
    glVertex3f(p, q, -r);
    glVertex3f(p, -q, -r);
    glVertex3f(p, -q, r);
    glEnd();


    //Bottom
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(p, q, -r);
    glVertex3f(p, q, r);
    glVertex3f(-p, q, r);
    glVertex3f(-p, q, -r);
    glEnd();

    //Top
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(p, -q, r);
    glVertex3f(p, -q, -r);
    glVertex3f(-p, -q, -r);
    glVertex3f(-p, -q, r);
    glEnd();
}

void whiteAtom() {
    glPushMatrix();
    glRotatef(rot, 0.0f, 1.0f, 1.0f);
    glColor3f(0.6f, 0.6f, 0.6f);

    glBegin(GL_LINES);
    for (GLfloat i = 0; i <= PI * radius; i += 0.0001) {
        glVertex3f(radius * cos(i), radius * sin(i), 0);
    }
    glEnd();

    glTranslatef(0.0, 9.0, 0.0);
    glRotatef(rot, 0.0f, 1.0f, 1.0f);
    glutSolidSphere(0.4, 100, 100);
    glPopMatrix();
}

void pinkAtom() {
    glPushMatrix();
    glRotatef(rot + 170, 0.0f, 1.0f, 1.0f);
    glColor3f(0.6f, 0.0f, 0.6f);

    glBegin(GL_LINES);
    for (GLfloat i = 0; i <= PI * radius; i += 0.0001) {
        glVertex3f(radius * cos(i), radius * sin(i), 0);
    }
    glEnd();

    glTranslatef(0.0, 9.0, 0.0);
    glRotatef(rot + 170, 0.0f, 1.0f, 1.0f);
    glutSolidSphere(0.4, 100, 100);
    glPopMatrix();
}

void yellowAtom() {
    glPushMatrix();
    glRotatef(rot + 90, 0.0f, 1.0f, 1.0f);
    glColor3f(0.6f, 0.6f, 0.0f);

    glBegin(GL_LINES);
    for (GLfloat i = 0; i <= PI * radius; i += 0.0001) {
        glVertex3f(radius * cos(i), radius * sin(i), 0);
    }
    glEnd();

    glTranslatef(0.0, 9.0, 0.0);
    glRotatef(rot + 90, 0.0f, 1.0f, 1.0f);
    glutSolidSphere(0.4, 100, 100);
    glPopMatrix();
}

void blueAtom() {
    glPushMatrix();
    glRotatef(rot + 220, 0.0f, 1.0f, 1.0f);
    glColor3f(0.0f, 0.6f, 0.6f);

    glBegin(GL_LINES);
    for (GLfloat i = 0; i <= PI * radius; i += 0.0001) {
        glVertex3f(radius * cos(i), radius * sin(i), 0);
    }
    glEnd();

    glTranslatef(0.0, 9.0, 0.0);
    glRotatef(rot + 220, 0.0f, 1.0f, 1.0f);
    glutSolidSphere(0.4, 100, 100);
    glPopMatrix();
}

void display() {


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glPushMatrix();

    // camera orientation (eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ)
    gluLookAt(0.0 + camX, 2.0 + camY, 5.0 + camZ, 0, 0, 0, 0, 1.0, 0);

    // move the object frame using keyboard keys
    glTranslatef(moveX, moveY, moveZ);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);


    glColor3f(1.0, 1.0, 1.0);

    //DrawGrid();

    //Cube
    glPushMatrix();
    //glRotatef(animateRotation, 0.0, 1.0, 0.0);
    glTranslatef(0.0, 1.6, 0.0);


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    drawCube();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

    //Torus(Doughnut)
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);
    glTranslatef(10.0, 3.0, 10.0);
    glRotatef(-animateRotation, 0.0, 1.0, 0.0);
    glutSolidTorus(1, 2, 50, 50);

    glPopMatrix();

    // Cone
    glPushMatrix();
    glColor3f(0.5, 0.7, 0.3);
    glTranslatef(-10.0, 3.0, 10.0);
    glRotatef(-animateRotation, 1.0, 0.0, 0.0);
    glutSolidCone(1, 3, 80, 100);

    glColor3f(0.7, 0.3, 0.5);
    glRotatef(180.0, 1.0, 0.0, 0.0);
    glutSolidCone(1, 3, 80, 100);

    glPopMatrix();

    //Teapot
    glPushMatrix();
    glColor3f(0.9, 0.1, 0.5);
    glTranslatef(10.0, 1.0, -10.0);
    glRotatef(-animateRotation, 0.0, 1.0, 0.0);

    glTranslatef(0.0, 0.0, -1.0);
    glutSolidTeapot(1);


    glPopMatrix();

    glPushMatrix();
    glTranslatef(10.0, 1.0, -10.0);
    glTranslatef(0.0, 0.0, -1.0);
    drawSmallCube();
    glPopMatrix();

    drawAxes();


    glPushMatrix();
    glColor3f(0.9, 0.1, 0.5);
    glTranslatef(-15.0, 1.0, -10.0);
    glRotatef(-animateRotation, 0.0, 1.0, 0.0);

    glTranslatef(0.0, 0.0, -1.0);
    whiteAtom();
    blueAtom();
    pinkAtom();
    yellowAtom();

    glPopMatrix();


    glPopMatrix();

    glutSwapBuffers();

}

void keyboardSpecial(int key, int x, int y) {
    if (key == GLUT_KEY_UP)
        moveZ += 1;

    if (key == GLUT_KEY_DOWN)
        moveZ -= 1;

    if (key == GLUT_KEY_LEFT)
        rotY += 5.0;

    if (key == GLUT_KEY_RIGHT)
        rotY -= 5.0;

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'w')
        camY += 0.5;
    if (key == 's')
        camY -= 0.5;

    if (key == 'c')
        glPolygonMode(GL_BACK, GL_LINE);
    // glPolygonMode(GL_FRONT, GL_FILL);
    if (key == 'C')
        glPolygonMode(GL_BACK, GL_FILL);

    if (key == 'k')
        glDisable(GL_LIGHT0);
    if (key == 'K')
        glEnable(GL_LIGHT0);
    if (key == 'l')
        glDisable(GL_LIGHT1);
    if (key == 'L')
        glEnable(GL_LIGHT1);
    if (key == 'j')
        glDisable(GL_LIGHT2);
    if (key == 'J')
        glEnable(GL_LIGHT2);

    glutPostRedisplay();

}

void Timer(int x) {
    animateRotation += animateRotation >= 360.0 ? -animateRotation : 5;
    glutPostRedisplay();

    glutTimerFunc(60, Timer, 1);
}

void changeSize(GLsizei w, GLsizei h) {
    glViewport(0, 0, w, h);
    GLfloat aspect_ratio = h == 0 ? w / 1 : (GLfloat)w / (GLfloat)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //Define the Perspective projection frustum 
    // (FOV_in_vertical, aspect_ratio, z-distance to the near plane from the camera position, z-distance to far plane from the camera position)
    gluPerspective(120.0, aspect_ratio, 1.0, 100.0);

    // if(w <= h)
    //     glOrtho(-20.0, 20.0, -20/aspect_ratio, 20/aspect_ratio, 20.0, -20.0);

    // else
    //     glOrtho(-20.0* aspect_ratio, 20.0* aspect_ratio, -20.0, 20.0, 20.0, -20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


}

int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutCreateWindow("Introduction to Lighting");
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(150, 150);



    glutDisplayFunc(display);
    glutReshapeFunc(changeSize);
    // keyboard function activation
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboardSpecial);
    init();

    glutTimerFunc(60.0, Timer, 1);
    glutMainLoop();


    return 0;
}