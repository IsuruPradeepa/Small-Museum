#include <iostream>
//#include <GL/glut.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <SOIL2.h>
#include <string>
#include <filesystem>
#include <direct.h>
#include <dirent.h>
#include <list>

using namespace std;

#define PI 3.1415927

#define GetCurrentDir _getcwd

GLUquadricObj* qobj = gluNewQuadric();

// vertices for the cube
GLfloat x = 2.0f;
GLfloat y = 2.0f;
GLfloat z = 2.0f;

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
GLfloat camY = 8.5f;
GLfloat camX = 10.0f;
GLfloat camZ = 14.0f;

int width;
int height;
unsigned char* image;
GLuint tex;

//light 1
GLfloat   lightPosition[] = { 5, 5, 0, 1.0 };
GLfloat   lightDirection[] = { 0, 0, 10, 0 };
GLfloat   diffuseLight[] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat   ambientLight[] = { 0.4, 0.4, 0.4, 1.0 };

//light 2
GLfloat L1_Ambient[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat L1_Diffuse[] = { 1, 1, 1, 5.0 };
GLfloat L1_Specular[] = { 1.0, 1.0, 0.0, 10.0 };   //Declaration of the specular component of the light_2
GLfloat L1_postion[] = { 0, -3, 0, 1.0 };
GLfloat L1_direction[] = { 0, -1, 0, 0 };

GLfloat globalAmbient[] = { 0.8, 0.8, 0.8, 1.0 };

//init light
void initLighting() {
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 128);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);

    //glLightfv(GL_LIGHT1, GL_AMBIENT, L1_Ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, L1_Diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, L1_Specular);
    glLightfv(GL_LIGHT1, GL_POSITION, L1_postion);
    //glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 90);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 128);
    glLightf(GL_LIGHT2, GL_SHININESS, 128);
    //glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, L1_direction);

    //Declaration of the ligt reflecting properties for the materials
    GLfloat specularReflectance[] = { 1.0, 1.0, 1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularReflectance);
    glMateriali(GL_FRONT, GL_SHININESS, 50);
}

std::string get_current_dir() {
    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    string current_working_dir(buff);
    return current_working_dir;
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::string get_texture_location() {
    std::string working_dir = get_current_dir();
    working_dir = ReplaceAll(working_dir, "\\", "/");
    return working_dir + "/Textures/";
}

//get a list of all the files(images) inside the 'Textures' dir
//with this, all the texture images can be loaded without having to put image names inside an array
list<string> listFiles(char* dir) {
    list<string> file_list;
    DIR* dr;
    struct dirent* en;
    dr = opendir(dir); //open all or present directory
    int i = 0;
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            string filename = en->d_name;
            if (filename.find(".jpg") != std::string::npos || filename.find(".jpeg") != std::string::npos || filename.find(".png") != std::string::npos) {
                file_list.push_back(en->d_name);
            }
        }
        closedir(dr);
    }
    return file_list;
}

//textures
GLuint texture[20];

void loadTexture() {
    std::string texture_location = get_texture_location(); //'Texture' dir

    list<string> texture_list_from_location = listFiles(&texture_location[0]); //loading the list of texture images, inside the 'Texture' dir

    list<string>::iterator it;
    int i;
    for (it = texture_list_from_location.begin(), i = 0; it != texture_list_from_location.end(); ++it, ++i) {
        std::string location = texture_location + *it; //creating the path for each texture

        texture[i] = SOIL_load_OGL_texture(&location[0], SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, 0); //putting all the textures inside an array for further usage
        //SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT

        if (texture[i] == 0) {
            std::cout << "Failed to load texture: " << sizeof(texture) << std::endl;
        }
        else {
            std::cout << "Texture " << i << ": \'" << &texture[i] << "\' loaded > " << *it << std::endl;
        }

        glBindTexture(GL_TEXTURE_2D, texture[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    //enable lighing
    glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    initLighting();

    (GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Enabling the color tracking of each faces of the materials. this enables the color visibility of the materials
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_NORMALIZE);

    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    //Turn on lighting
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    //texture
    glEnable(GL_TEXTURE_2D);
    loadTexture();

}

void drawCube(int n) {

    glBindTexture(GL_TEXTURE_2D, texture[n]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBegin(GL_QUADS);

    // Top face
    glColor3f(1.0, 1.0f, 1.0);
    glTexCoord2f(0,0);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0, 1);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(1, 1);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1, 0);
    glVertex3f(1.0f, 1.0f, 1.0f);

    // Bottom face
    glColor3f(1.0, 1.0, 1.0);
    glTexCoord2f(0, 0);
    glVertex3f(1.0f, -0.0f, 1.0f);
    glTexCoord2f(0, 1);
    glVertex3f(-1.0f, -0.0f, 1.0f);
    glTexCoord2f(1, 1);
    glVertex3f(-1.0f, -0.0f, -1.0f);
    glTexCoord2f(1, 0);
    glVertex3f(1.0f, -0.0f, -1.0f);

    // Front face
    glColor3f(1.0, 1.0, 1.0);
    glTexCoord2f(0, 0);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0, 1);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1, 1);
    glVertex3f(-1.0f, -0.0f, 1.0f);
    glTexCoord2f(1, 0);
    glVertex3f(1.0f, -0.0f, 1.0f);

    // Back face
    glColor3f(1.0, 1.0, 1.0);
    glTexCoord2f(0, 0);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0, 1);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(1, 1);
    glVertex3f(1.0f, -0.0f, -1.0f);
    glTexCoord2f(1, 0);
    glVertex3f(-1.0f, -0.0f, -1.0f);

    // Left face
    glColor3f(1.0, 1.0, 1.0);
    glTexCoord2f(0, 0);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(0, 1);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(1, 1);
    glVertex3f(-1.0f, -0.0f, -1.0f);
    glTexCoord2f(1, 0);
    glVertex3f(-1.0f, -0.0f, 1.0f);

    // Right face
    glColor3f(1.0, 1.0, 1.0);
    glTexCoord2f(0, 0);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0, 1);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1, 1);
    glVertex3f(1.0f, -0.0f, 1.0f);
    glTexCoord2f(1, 0);
    glVertex3f(1.0f, -0.0f, -1.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

//Solid Plate
void plate() {
    glPushMatrix();
    glColor3f(0.36,0.25,0.20);
        glRotatef(-90,1,0,0);
        glTranslatef(0, 0, 3);
        glScalef(0.5, 0.5, 0.5);
        gluCylinder(qobj, 7, 7, 1, 30, 10);
        gluDisk(qobj, 0.4, 7, 30, 30);
        glTranslatef(0, 0, 1);
        gluDisk(qobj, 0.4, 7, 30, 30);
    glPopMatrix();
}


//Hlaf Sphere
void halfShpere(int scalex, int scaley, int r){
    int i, j;
    GLfloat v[50 * 50][3];

    for (i = 0; i < scalex; ++i) {
        for (j = 0; j < scaley; ++j) {
            v[i * scaley + j][0] = r * cos(j * 2 * PI / scaley) * cos(i * PI / (2 * scalex));
            v[i * scaley + j][1] = r * sin(i * PI / (2 * scalex));
            v[i * scaley + j][2] = r * sin(j * 2 * PI / scaley) * cos(i * PI / (2 * scalex));
        }
    }

    glBegin(GL_QUADS);
    for (i = 0; i < scalex - 1; ++i) {
        for (j = 0; j < scaley; ++j) {
            glVertex3fv(v[i * scaley + j]);
            glVertex3fv(v[i * scaley + (j + 1) % scaley]);
            glVertex3fv(v[(i + 1) * scaley + (j + 1) % scaley]);
            glVertex3fv(v[(i + 1) * scaley + j]);
        }
    }
    glEnd();
}


//Temple
void temple() {
    glPushMatrix();
        glTranslatef(12.0, 0.0, 12.0);
        glPushMatrix();
        glRotatef(animateRotation,0,1,0);
            plate();
            glTranslatef(0,0.5,0);
            glPushMatrix();
                glColor3f(1, 1, 1);
                glTranslatef(0.0, 3.0, 0.0);
                halfShpere(50, 50, 2);
                //glutSolidSphere(2,50,50);
            glPopMatrix();
            //poles
            glPushMatrix();
                glTranslatef(3,3,0);
                glRotatef(-70,1,0,0);
                glScalef(0.3,0.3,0.3);
                //glColor3f(0.1, 1, 1);
                gluCylinder(qobj, 0.5, 0.5, 16, 30, 30);
                glPushMatrix();
                    glTranslatef(0,0,17);
                    glRotatef(-90, 1, 0, 0);
                    glEnable(GL_TEXTURE_2D);
                    drawCube(15);
                    glDisable(GL_TEXTURE_2D);
                glPopMatrix();
            glPopMatrix();
            glPushMatrix();
                glTranslatef(-3, 3, 0);
                glRotatef(-90, 1, 0, 0);
                glScalef(0.3, 0.3, 0.3);
                gluCylinder(qobj, 0.5, 0.5, 16, 30, 30);
                glPushMatrix();
                    glTranslatef(0, 0, 17);
                    glRotatef(-90, 1, 0, 0);
                    glEnable(GL_TEXTURE_2D);
                    drawCube(15);
                    glDisable(GL_TEXTURE_2D);
                glPopMatrix();
            glPopMatrix();
            glPushMatrix();
                glTranslatef(0, 3, 3);
                glRotatef(-100, 1, 0, 0);
                glScalef(0.3, 0.3, 0.3);
                gluCylinder(qobj, 0.5, 0.5, 16, 30, 30);
                glPushMatrix();
                    glTranslatef(0, 0, 17);
                    glRotatef(-90, 1, 0, 0);
                    glEnable(GL_TEXTURE_2D);
                    drawCube(15);
                    glDisable(GL_TEXTURE_2D);
                glPopMatrix();
            glPopMatrix();
            glPushMatrix();
                glTranslatef(0, 3, -3);
                glRotatef(-90, 1, 0, 0);
                glScalef(0.3, 0.3, 0.3);
                gluCylinder(qobj, 0.5, 0.5, 16, 30, 30);
                glPushMatrix();
                    glTranslatef(0, 0, 17);
                    glRotatef(-90, 1, 0, 0);
                    glEnable(GL_TEXTURE_2D);
                    drawCube(15);
                    glDisable(GL_TEXTURE_2D);
                glPopMatrix();
            glPopMatrix();

            glPushMatrix();
                glColor3f(1.0, 1.0, 1.0);
                glTranslatef(0.0, 4.5, 0.0);
                glPushMatrix();
                    glRotatef(-90,1,0,0);
                    glutSolidCone(0.4, 2.0, 20, 10);
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
        glPushMatrix();
            glScalef(3.0, 3.0, 3.0);
            glEnable(GL_TEXTURE_2D);
            drawCube(1);
            glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    glPopMatrix();
}

//Teapot
void teaPot() {
    glPushMatrix();
    glTranslatef(-12.0, 0.0, 12.0);
    glPushMatrix();
        glColor3f(0.1f, 0.1f, 0.0f);
        glTranslatef(0.0, 4.0, 0.0);
        glutSolidTeapot(1.0);
    glPopMatrix();
    glPushMatrix();
        glScalef(3.0, 3.0, 3.0);
        glEnable(GL_TEXTURE_2D);
        drawCube(1);
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glPopMatrix();
}


//Pyramid structure
void drawPyramid() {
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[16]);
    glBegin(GL_TRIANGLES);
    glTexCoord2f(1, 0.5);
    glVertex3f(-0.5, 0.0, -0.5);
    glTexCoord2f(1,0);
    glVertex3f(-0.5, 0.0, 0.5);
    glTexCoord2f(0, 0);
    glVertex3f(0.0, 0.5, 0.0);

    glTexCoord2f(1, 0.5);
    glVertex3f(-0.5, 0.0, 0.5);
    glTexCoord2f(0, 1);
    glVertex3f(0.5, 0.0, 0.5);
    glTexCoord2f(0, 0);
    glVertex3f(0.0, 0.5, 0.0);

    glTexCoord2f(1, 0.5);
    glVertex3f(0.5, 0.0, 0.5);
    glTexCoord2f(0, 1);
    glVertex3f(0.5, 0.0, -0.5);
    glTexCoord2f(0, 0);
    glVertex3f(0.0, 0.5, 0.0);

    glTexCoord2f(1, 0.5);
    glVertex3f(0.5, 0.0, -0.5);
    glTexCoord2f(0, 1);
    glVertex3f(-0.5, 0.0, -0.5);
    glTexCoord2f(0, 0);
    glVertex3f(0.0, 0.5, 0.0);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(0.5, 0.0, -0.5);
    glVertex3f(-0.5, 0.0, -0.5);
    glVertex3f(-0.5, 0.0, 0.5);
    glVertex3f(0.5, 0.0, 0.5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}


void pyramid() {
    glPushMatrix();
        glTranslatef(-12.0, 0.0, -12.0);
        glPushMatrix();
            //glColor3f(0.1f, 0.1f, 0.0f);
            glTranslatef(0.0, 3.0, 0.0);
            glScalef(5.0, 8.0, 5.0);
            drawPyramid();
        glPopMatrix();
        glPushMatrix();
            glScalef(3.0, 3.0, 3.0);
            glEnable(GL_TEXTURE_2D);
            drawCube(1);
            glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    glPopMatrix();
}

void roomFloor() {
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glScalef(20,20,20);
        glPushMatrix();
        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);

        glTexCoord2f(0,0);
        glVertex3f(1.0f, -0.0f, 1.0f);
        glTexCoord2f(0, 1);
        glVertex3f(-1.0f, -0.0f, 1.0f);
        glTexCoord2f(1, 1);
        glVertex3f(-1.0f, -0.0f, -1.0f);
        glTexCoord2f(1, 0);
        glVertex3f(1.0f, -0.0f, -1.0f);
        glEnd();
        glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void roomWalls() {
    glPushMatrix();
    //glColor3f(0.94, 0.81, 0.99);
    glEnable(GL_TEXTURE_2D);
    glScalef(20,20,20);
        glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, texture[8]);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glBegin(GL_QUADS);
            // Front face  (z = 1.0f)
            glTexCoord3f(1.0f, 1.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, 1.0f);
            glTexCoord3f(-1.0f, 1.0f, 1.0f);
            glVertex3f(-1.0f, 1.0f, 1.0f);
            glTexCoord3f(-1.0f, -0.0f, 1.0f);
            glVertex3f(-1.0f, -0.0f, 1.0f);
            glTexCoord3f(1.0f, -0.0f, 1.0f);
            glVertex3f(1.0f, -0.0f, 1.0f);

            // Back face (z = -1.0f)
            glTexCoord3f(-1.0f, 1.0f, -1.0f);
            glVertex3f(-1.0f, 1.0f, -1.0f);
            glTexCoord3f(1.0f, 1.0f, -1.0f);
            glVertex3f(1.0f, 1.0f, -1.0f);
            glTexCoord3f(1.0f, -0.0f, -1.0f);
            glVertex3f(1.0f, -0.0f, -1.0f);
            glTexCoord3f(-1.0f, -0.0f, -1.0f);
            glVertex3f(-1.0f, -0.0f, -1.0f);

            // Left face (x = -1.0f)
            glTexCoord2f(0, 0);
            glVertex3f(-1.0f, 1.0f, 1.0f);
            glTexCoord2f(1, 0);
            glVertex3f(-1.0f, 1.0f, -1.0f);
            glTexCoord2f(1, 1);
            glVertex3f(-1.0f, -0.0f, -1.0f);
            glTexCoord2f(0, 1);
            glVertex3f(-1.0f, -0.0f, 1.0f);

            // Right face (x = 1.0f)
            glTexCoord2f(0, 0);
            glVertex3f(1.0f, 1.0f, -1.0f);
            glTexCoord2f(1, 0);
            glVertex3f(1.0f, 1.0f, 1.0f);
            glTexCoord2f(1, 1);
            glVertex3f(1.0f, -0.0f, 1.0f);
            glTexCoord2f(0, 1);
            glVertex3f(1.0f, -0.0f, -1.0f);
            glEnd();
        glPopMatrix();
        glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, texture[0]);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glBegin(GL_QUADS);
            // Top face
            glColor3f(1.0, 1.0f, 1.0);
            glTexCoord2f(0, 0);
            glVertex3f(1.0f, 1.0f, -1.0f);
            glTexCoord2f(0, 1);
            glVertex3f(-1.0f, 1.0f, -1.0f);
            glTexCoord2f(1, 1);
            glVertex3f(-1.0f, 1.0f, 1.0f);
            glTexCoord2f(1, 0);
            glVertex3f(1.0f, 1.0f, 1.0f);
            glEnd();
        glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void paintings() {
    //Border
    glPushMatrix();
        glColor3f(0.36, 0.25, 0.20);
        glTranslatef(0, 5, -22.9);
        glScalef(3.2, 3.2, 3.2);
        glPushMatrix();
            glBegin(GL_QUADS);
            // Front face  (z = 1.0f)
            glVertex3f(1.0f, 3.0f, 1.0f);
            glVertex3f(-1.0f, 3.0f, 1.0f);
            glVertex3f(-1.0f, -0.0f, 1.0f);
            glVertex3f(1.0f, -0.0f, 1.0f);
            glEnd();
        glPopMatrix();
    glPopMatrix();

    //Painting
    glPushMatrix();
        glColor3f(1, 1, 1);
        glTranslatef(0, 6.1, -22.1);
        glPushMatrix();
        glScalef(2.5, 2.5, 2.5);
            glPushMatrix();
            glEnable(GL_TEXTURE_2D);
            //glRotatef(180, 0, 0, 1);
                    glPushMatrix();
                    glBindTexture(GL_TEXTURE_2D, texture[5]);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                    glBegin(GL_QUADS);
                    // Front face
                    glTexCoord2f(0, 0);
                    glVertex3f(1.0f, 3.0f, 1.0f);
                    glTexCoord2f(1, 0);
                    glVertex3f(-1.0f, 3.0f, 1.0f);
                    glTexCoord2f(1, 1);
                    glVertex3f(-1.0f, -0.0f, 1.0f);
                    glTexCoord2f(0, 1);
                    glVertex3f(1.0f, -0.0f, 1.0f);
                    glEnd();
                    glPopMatrix();
            glDisable(GL_TEXTURE_2D);
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();
}

void canon() {
    glPushMatrix();
    glTranslatef(12.0, 0.0, -12.0);
    glRotatef(-90,0,1,0);
        glPushMatrix();
        glTranslatef(0,6.5,-3);
        glRotatef(25,1,0,0);
        glScalef(0.2, 0.2, 0.2);
            glPushMatrix();

                //canon tube
                glColor3f(0, 0.6, 1);
                glTranslatef(0, 2, 0);
                gluCylinder(qobj, 7, 5, 4, 30, 10);
                gluCylinder(qobj, 5, 5, 35, 30, 10);
                glTranslatef(0,0,34);
                gluSphere(qobj,5,30,30);

                //Tires
                glPushMatrix();
                    glRotatef(90,0,1,0);
                    glPushMatrix();
                        glTranslatef(8,0,5);
                        gluCylinder(qobj, 7, 7, 5, 30, 10);
                        gluDisk(qobj, 0.4, 7, 30, 30);
                        glTranslatef(0, 0, 5);
                        gluDisk(qobj, 0.4, 7, 30, 30);
                    glPopMatrix();
                
                    glPushMatrix();
                        glTranslatef(8, 0, -10);
                        gluCylinder(qobj, 7, 7, 5, 30, 10);
                        gluDisk(qobj, 0.4, 7, 30, 30);
                        glTranslatef(0, 0, 5);
                        gluDisk(qobj, 0.4, 7, 30, 30);
                    glPopMatrix();
                glPopMatrix();

                //canon balls
                glPushMatrix();
                    glPushMatrix();
                    glTranslatef(10,-12,-20);
                    gluSphere(qobj, 2, 30, 30);
                    glPopMatrix();

                    glPushMatrix();
                    glTranslatef(7, -12, -20);
                    gluSphere(qobj, 2, 30, 30);
                    glPopMatrix();

                    glPushMatrix();
                    glTranslatef(14, -12, -20);
                    gluSphere(qobj, 2, 30, 30);
                    glPopMatrix();

                    glPushMatrix();
                    glTranslatef(10, -12, -20);
                    gluSphere(qobj, 2, 30, 30);
                    glPopMatrix();
                glPopMatrix();

            glPopMatrix();
        glPopMatrix();

        glPushMatrix();
        glScalef(3.0, 3.0, 3.0);
        glEnable(GL_TEXTURE_2D);
        drawCube(1);
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

    glPopMatrix();
}

void checkers() {
    glPushMatrix();
        //Checkers Board
        glPushMatrix();
            glColor3f(1.0, 1.0, 1.0);
            glRotatef(-90,1,0,0);
            glPushMatrix();
                glTranslatef(0,-2.5,1);
                glScalef(2.5, 2.5, 2.5);
                glPushMatrix();
                //glRotatef(180, 0, 0, 1);
                    glPushMatrix();
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, texture[2]);
                        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                        glBegin(GL_QUADS);
                        // Front face  (z = 1.0f)
                        glTexCoord3f(1.0f, 2.0f, 1.0f);
                        glVertex3f(1.0f, 2.0f, 1.0f);
                        glTexCoord3f(-1.0f, 2.0f, 1.0f);
                        glVertex3f(-1.0f, 2.0f, 1.0f);
                        glTexCoord3f(-1.0f, -0.0f, 1.0f);
                        glVertex3f(-1.0f, -0.0f, 1.0f);
                        glTexCoord3f(1.0f, -0.0f, 1.0f);
                        glVertex3f(1.0f, -0.0f, 1.0f);
                        glEnd();
                        glDisable(GL_TEXTURE_2D);
                    glPopMatrix();
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
        glPopMatrix();

        //Checkers disks
        
        glPushMatrix();

        glPushMatrix();
            glColor3f(0.0,1.0,1.0);
                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(0.1, 2.2, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(1.4, 2.2, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(-1.2, 2.2, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(-2.4, 2.2, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(0.8, 1.6, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(-0.5, 1.6, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(-1.7, 1.6, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(-3.0, 1.6, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

            glPopMatrix();

            //Other side
            glPushMatrix();
                
            glColor3f(1.0, 1.0, 0.0);

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(0.1, -2.2, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(1.4, -2.2, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(-1.2, -2.2, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(-2.4, -2.2, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(0.8, -1.6, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(-0.5, -1.6, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(-1.7, -1.6, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

                glPushMatrix();
                glRotatef(-90, 1, 0, 0);
                glTranslatef(-3.0, -1.6, 3);
                glScalef(0.1, 0.1, 0.1);
                glPushMatrix();
                glTranslatef(8, 0, 5);
                gluCylinder(qobj, 2, 2, 2, 30, 10);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glTranslatef(0, 0, 2);
                gluDisk(qobj, 0.4, 2, 30, 30);
                glPopMatrix();
                glPopMatrix();

            glPopMatrix();

        glPopMatrix();

        glPushMatrix();
        glColor3f(0.0, 1.0, 1.0);
            glScalef(3.0, 3.0, 3.0);
            glEnable(GL_TEXTURE_2D);
            drawCube(1);
            glDisable(GL_TEXTURE_2D);
        glPopMatrix();

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


    //Objects

    roomFloor();
    roomWalls();
    temple();
    teaPot();
    pyramid();
    canon();
    paintings();
    checkers();
    
    glPopMatrix();

    glutSwapBuffers();
}

void keyboardSpecial(int key, int x, int y) {
    if (key == GLUT_KEY_UP)
        moveZ += 1;

    if (key == GLUT_KEY_DOWN)
        moveZ -= 1;

    if (key == GLUT_KEY_LEFT)
        rotY -= 1;

    if (key == GLUT_KEY_RIGHT)
        rotY += 1;

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'w')
        camY += 0.5;
    if (key == 's')
        camY -= 0.5;
    if (key == 'd')
        camX += 0.5;
    if (key == 'a')
        camX -= 0.5;
    if (key == 'c')
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (key == 'C')
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (key == 'l')
        glDisable(GL_LIGHT0);
    if (key == 'L')
        glEnable(GL_LIGHT0);

    if (key == 'k')
        glDisable(GL_LIGHT1);
    if (key == 'K')
        glEnable(GL_LIGHT1);

    glutPostRedisplay();

}

void Timer(int x) {
    animateRotation += animateRotation >= 360.0 ? -animateRotation : 2;
    glutPostRedisplay();

    //printf("%f %f %f\n", camX, camY, camZ);

    glutTimerFunc(150, Timer, 1);
}

void changeSize(GLsizei w, GLsizei h) {
    glViewport(0, 0, w, h);
    GLfloat aspect_ratio = h == 0 ? w / 1 : (GLfloat)w / (GLfloat)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //Define the Perspective projection frustum 
    // (FOV_in_vertical, aspect_ratio, z-distance to the near plane from the camera position, z-distance to far plane from the camera position)
    gluPerspective(90.0, aspect_ratio, 1.0, 300.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("Museum");
    glutDisplayFunc(display);
    glutReshapeFunc(changeSize);

    // keyboard function activation
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboardSpecial);

    glutTimerFunc(60.0, Timer, 1);
    init();
    glutMainLoop();

    return 0;
}