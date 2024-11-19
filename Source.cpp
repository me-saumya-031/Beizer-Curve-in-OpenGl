#include <GL/glut.h>
#include <math.h>

// Screen 
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Animation  
float time = 0.0f;
float boatT = 0.0f;  //boat position
float cloudT = 0.0f;  //clouds position


// cubic Bézier curve
void drawBezierCurve(float p0[2], float p1[2], float p2[2], float p3[2]) {
    glBegin(GL_LINE_STRIP);
    for (float t = 0.0f; t <= 1.0f; t += 0.01f) {
        float u = 1.0f - t;
        float x = u * u * u * p0[0] + 3 * u * u * t * p1[0] + 3 * u * t * t * p2[0] + t * t * t * p3[0];
        float y = u * u * u * p0[1] + 3 * u * u * t * p1[1] + 3 * u * t * t * p2[1] + t * t * t * p3[1];
        glVertex2f(x, y);
    }
    glEnd();
}

// point on the Bézier curve 
void getBezierPoint(float t, float p0[2], float p1[2], float p2[2], float p3[2], float* x, float* y) {
    float u = 1.0f - t;
    *x = u * u * u * p0[0] + 3 * u * u * t * p1[0] + 3 * u * t * t * p2[0] + t * t * t * p3[0];
    *y = u * u * u * p0[1] + 3 * u * u * t * p1[1] + 3 * u * t * t * p2[1] + t * t * t * p3[1];
}

//boat 
void drawBoat(float x, float y) {
    float baseWidth = 0.10f;
    float topWidth = 0.06f;
    float height = 0.04f;

    //  trapezoid
    glColor3f(0.6f, 0.3f, 0.0f);  // Brown 
    glBegin(GL_POLYGON);
    glVertex2f(x - baseWidth / 2, y);                 // Bottom-left
    glVertex2f(x + baseWidth / 2, y);                 // Bottom-right
    glVertex2f(x + topWidth / 2, y - height);         // Top-right 
    glVertex2f(x - topWidth / 2, y - height);         // Top-left 
    glEnd();

    // Sail
    float sailHeight = 0.08f;
    glColor3f(1.0f, 0.0f, 0.0f);  //Red
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.03f, y);     // Left corner 
    glVertex2f(x + 0.03f, y);     // Right corner 
    glVertex2f(x, y + sailHeight); // Top
    glEnd();
}

void drawCloud(float x, float y) {
    glColor3f(0.1f, 0.3f, 0.7f);  // Dark blue 

    float radius = 0.08f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (3.14 / 180.0f);
        float cloudX = x + radius * cos(angle);
        float cloudY = y + radius * sin(angle);
        glVertex2f(cloudX, cloudY);
    }
    glEnd();

    radius = 0.07f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + 0.1f, y);  // Right puff
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (3.14 / 180.0f);
        float cloudX = x + 0.1f + radius * cos(angle);
        float cloudY = y + radius * sin(angle);
        glVertex2f(cloudX, cloudY);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x - 0.1f, y);  // Left puff
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (3.14 / 180.0f);
        float cloudX = x - 0.1f + radius * cos(angle);
        float cloudY = y + radius * sin(angle);
        glVertex2f(cloudX, cloudY);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Sun
    float sunCenterX = 0.8f;
    float sunCenterY = 0.8f;
    float sunRadius = 0.1f;

    glColor3f(1.0f, 1.0f, 0.0f);  // Yellow 
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(sunCenterX, sunCenterY);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * (3.14 / 180.0f);
        float x = sunCenterX + sunRadius * cos(angle);
        float y = sunCenterY + sunRadius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

    int numCurves = 20;
    float curveSpacing = 2.0f / numCurves;  

    //control points
    float boatCurve[4][2];
    float boat2Curve[4][2];  
    float cloudCurve[4][2];  
    float cloud2Curve[4][2]; 
    float cloud3Curve[4][2];

    glLineWidth(3.0f);  

    //  Bézier curves
    for (int i = 0; i < numCurves; i++) {
        float offset = -1.0f + i * curveSpacing;  // Vertical offset for each wave

        // control points
        float p0[2] = { -1.0f, offset };
        float p1[2] = { -0.5f, offset + 0.2f * sin(time + i) };
        float p2[2] = { 0.5f, offset - 0.2f * sin(time + i) };
        float p3[2] = { 1.0f, offset };

        //vertical position of curve ... width ... color
        if (i < numCurves / 2) {
            // Top 50% curves - Dark Blue
            glColor3f(0.1f, 0.3f, 0.7f);
            glLineWidth(3.0f);  
        }
        else {
            // Bottom 50% curves - Light Blue
            glColor3f(0.5f, 0.8f, 1.0f);
            glLineWidth(1.0f); 
        }

        drawBezierCurve(p0, p1, p2, p3);

        // Control points first cloud
        if (i == 17) {
            cloudCurve[0][0] = p0[0] - 0.2f; cloudCurve[0][1] = p0[1]; 
            cloudCurve[1][0] = p1[0] - 0.2f; cloudCurve[1][1] = p1[1];
            cloudCurve[2][0] = p2[0] - 0.2f; cloudCurve[2][1] = p2[1];
            cloudCurve[3][0] = p3[0] - 0.2f; cloudCurve[3][1] = p3[1];
        }

        // Control points second cloud
        if (i == 18) {
            cloud2Curve[0][0] = p0[0] + 0.2f; cloud2Curve[0][1] = p0[1];
            cloud2Curve[1][0] = p1[0] + 0.2f; cloud2Curve[1][1] = p1[1];
            cloud2Curve[2][0] = p2[0] + 0.2f; cloud2Curve[2][1] = p2[1];
            cloud2Curve[3][0] = p3[0] + 0.2f; cloud2Curve[3][1] = p3[1];
        }

        // Control points second cloud
        if (i == 19) {
            cloud3Curve[0][0] = p0[0] + 0.4f; cloud3Curve[0][1] = p0[1];  
            cloud3Curve[1][0] = p1[0] + 0.4f; cloud3Curve[1][1] = p1[1];
            cloud3Curve[2][0] = p2[0] + 0.4f; cloud3Curve[2][1] = p2[1];
            cloud3Curve[3][0] = p3[0] + 0.4f; cloud3Curve[3][1] = p3[1];
        }

        // Control points first boat
        if (i == 10) {
            boatCurve[0][0] = p0[0]; boatCurve[0][1] = p0[1];
            boatCurve[1][0] = p1[0]; boatCurve[1][1] = p1[1];
            boatCurve[2][0] = p2[0]; boatCurve[2][1] = p2[1];
            boatCurve[3][0] = p3[0]; boatCurve[3][1] = p3[1];
        }

        // Control points 2nd boat
        if (i == 5) {
            boat2Curve[0][0] = p0[0]; boat2Curve[0][1] = p0[1];
            boat2Curve[1][0] = p1[0]; boat2Curve[1][1] = p1[1];
            boat2Curve[2][0] = p2[0]; boat2Curve[2][1] = p2[1];
            boat2Curve[3][0] = p3[0]; boat2Curve[3][1] = p3[1];
        }
    }

    // boat on 11th curve
    float boatX, boatY;
    getBezierPoint(boatT, boatCurve[0], boatCurve[1], boatCurve[2], boatCurve[3], &boatX, &boatY);

    // 2nd boat on 6th curve 
    float boat2X, boat2Y;
    getBezierPoint(boatT, boat2Curve[0], boat2Curve[1], boat2Curve[2], boat2Curve[3], &boat2X, &boat2Y);

    // cloud on 18th curve 
    float cloudX, cloudY;
    getBezierPoint(cloudT, cloudCurve[0], cloudCurve[1], cloudCurve[2], cloudCurve[3], &cloudX, &cloudY);

    // cloud on 19th curve=
    float cloud2X, cloud2Y;
    getBezierPoint(cloudT, cloud2Curve[0], cloud2Curve[1], cloud2Curve[2], cloud2Curve[3], &cloud2X, &cloud2Y);

    // cloud on 20th curve 
    float cloud3X, cloud3Y;
    getBezierPoint(cloudT, cloud3Curve[0], cloud3Curve[1], cloud3Curve[2], cloud3Curve[3], &cloud3X, &cloud3Y);

    drawBoat(boatX, boatY);      
    drawBoat(boat2X, boat2Y);    
    drawCloud(cloudX, cloudY);   
    drawCloud(cloud2X, cloud2Y);
    drawCloud(cloud3X, cloud3Y); 

    glutSwapBuffers();
}



// animation
void timer(int value) {
    time += 0.02f;  // Increment time of curve
    boatT += 0.0031f;  // Speed of boat
    cloudT += 0.002f; // Slow down the cloud's movement

    // Reset 
    if (boatT > 1.0f) boatT = 0.0f;
    if (cloudT > 1.0f) cloudT = 0.0f;

    glutPostRedisplay();  // Redisplay
    glutTimerFunc(16, timer, 0);
}


void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("Boat and Waves with Clouds");

    init();

    glutDisplayFunc(display);
    glutTimerFunc(25, timer, 0);

    glutMainLoop();
    return 0;
}