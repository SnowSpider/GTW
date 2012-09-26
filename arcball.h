#ifndef _ARCBALL_H
#define _ARCBALL_H

//Only support assertions in debug builds
#ifdef _DEBUG
# include "assert.h"
#else
# define assert(x) { }
#endif

#include "vec.h"
#include "quat.h"

class ArcBall{
    protected:
        Vec3 stVec;           //Saved click vector
        Vec3 enVec;           //Saved drag vector
        GLfloat adjustWidth;  //Mouse bounds width
        GLfloat adjustHeight; //Mouse bounds height
        inline void mapToSphere(const Vec2& newPt, Vec3& newVec) const;
    public:
        ArcBall(GLfloat newWidth, GLfloat newHeight);
        ~ArcBall() { /* nothing to do */ };

        inline void setBounds(GLfloat newWidth, GLfloat newHeight){ //Set new bounds
            assert((newWidth > 1.0f) && (newHeight > 1.0f));

            //Set adjustment factor for width/height
            adjustWidth  = 1.0f / ((newWidth  - 1.0f) * 0.5f);
            adjustHeight = 1.0f / ((newHeight - 1.0f) * 0.5f);
        }
        void click(const Vec2& newPt); //Mouse down
        void drag(const Vec2& newPt, Quat& newRot); //Mouse drag, calculate rotation
};

#endif

