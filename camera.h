#ifndef CAMERA_H_
#define CAMERA_H_

#include "vec.h"
#include "quaternion.h"

class Camera {
    public:
    Camera(float rotspeed, float movespeed);

    void init();
    void resize(int width, int height);

    const Vec3& getPosition() const;
    const Quaternion& getRotation() const;
    Mat4 getViewMatrix() const;

    void movex(float xmod);
    void movey(float ymod);
    void movez(float zmod);
    void rotatex(float xmod);
    void rotatey(float ymod);
    void update(float xrot, float yrot, float xmove, float ymove, float zmove, float delta=1.f);
    void render();

    private:
    Vec3 position;
    Quaternion rotation;

    float rotspeed;
    float movespeed;
};

#endif
