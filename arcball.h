#ifndef _ARCBALL_H
#define _ARCBALL_H

//Only support assertions in debug builds
#ifdef _DEBUG
# include "assert.h"
#else
# define assert(x) { }
#endif

#include "vec.h"
#include "mat.h"
#include "quat.h"

//utility macros
//assuming IEEE-754(GLfloat), which i believe has max precision of 7 bits
# define Epsilon 1.0e-5

    typedef class ArcBall_t
    {
        protected:
            inline
            void _mapToSphere(const Vec2& NewPt, Vec3& NewVec) const;

        public:
            //Create/Destroy
                    ArcBall_t(GLfloat NewWidth, GLfloat NewHeight);
                   ~ArcBall_t() { /* nothing to do */ };

            //Set new bounds
            inline
            void    setBounds(GLfloat NewWidth, GLfloat NewHeight)
            {
                assert((NewWidth > 1.0f) && (NewHeight > 1.0f));

                //Set adjustment factor for width/height
                this->adjustWidth  = 1.0f / ((NewWidth  - 1.0f) * 0.5f);
                this->adjustHeight = 1.0f / ((NewHeight - 1.0f) * 0.5f);
            }

            //Mouse down
            void    click(const Vec2& NewPt);

            //Mouse drag, calculate rotation
            void    drag(const Vec2& NewPt, Quat& NewRot);

        protected:
            Vec3   StVec;          //Saved click vector
            Vec3   EnVec;          //Saved drag vector
            GLfloat     adjustWidth;    //Mouse bounds width
            GLfloat     adjustHeight;   //Mouse bounds height

    } ArcBallT;

#endif

