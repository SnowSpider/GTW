#include <GL/gl.h>                                                // Header File For The OpenGL32 Library
#include <GL/glu.h>                                                // Header File For The GLu32 Library
#include <stdio.h>

#include "math.h"                                               // Needed for sqrt

#include "arcball.h"                                            // ArcBall header

# define Epsilon 1.0e-5

//Arcball sphere constants:
//Diameter is       2.0f
//Radius is         1.0f
//Radius squared is 1.0f

void ArcBall::mapToSphere(const Vec2& newPt, Vec3& newVec) const {
    Vec2 tempPt;
    GLfloat length;

    //Copy paramter into temp point
    tempPt = newPt;

    //Adjust point coords and scale down to range of [-1 ... 1]
    tempPt.x  = (tempPt.x * adjustWidth)  - 1.0f;
    tempPt.y  = 1.0f - (tempPt.y * adjustHeight);

    //Compute the square of the length of the vector to the point from the center
    length = tempPt.length2();

    //If the point is mapped outside of the sphere... (length > radius squared)
    if (length > 1.0f){
        GLfloat norm;

        //Compute a normalizing factor (radius / sqrt(length))
        norm  = 1.0f / sqrt(length);

        //Return the "normalized" vector, a point on the sphere
        newVec.x = tempPt.x * norm;
        newVec.y = tempPt.y * norm;
        newVec.z = 0.0f;
    }
    else{    //Else it's on the inside
        //Return a vector to a point mapped inside the sphere sqrt(radius squared - length)
        newVec.x = tempPt.x;
        newVec.y = tempPt.y;
        newVec.z = sqrt(1.0f - length);
    }
}

//Create/Destroy
ArcBall::ArcBall(GLfloat newWidth, GLfloat newHeight){
    //Clear initial values
    stVec.clear();
    enVec.clear();

    //Set initial bounds
    setBounds(newWidth, newHeight);
}

//Mouse down
void ArcBall::click(const Vec2& newPt){
    //Map the point to the sphere
    mapToSphere(newPt, stVec);
}

//Mouse drag, calculate rotation
void ArcBall::drag(const Vec2& newPt, Quat& newRot){
    //Map the point to the sphere
    mapToSphere(newPt, enVec);

    //Return the quaternion equivalent to the rotation
    Vec3 perp;

    //Compute the vector perpendicular to the begin and end vectors
    perp = stVec ^ enVec;

    //Compute the length of the perpendicular vector
    if (perp.length() > Epsilon){    //if its non-zero
        //We're ok, so return the perpendicular vector as the transform after all
        newRot.x = perp.x;
        newRot.y = perp.y;
        newRot.z = perp.z;
        //In the quaternion values, w is cosine (theta / 2), where theta is rotation angle
        newRot.w = stVec * enVec;
    }
    else{                                    //if its zero
        //The begin and end vectors coincide, so return an identity transform
        newRot.x = 
        newRot.y = 
        newRot.z = 
        newRot.w = 0.0f;
    }
}

