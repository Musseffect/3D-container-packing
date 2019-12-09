#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>
#include <QVector2D>

class Camera
{
protected:
    float width;
    float height;
public:
    virtual QMatrix4x4 getView() = 0;
    virtual QMatrix4x4 getProjection() = 0;
    void setWidth(float w);
    void setHeight(float h);
};

class SphericalCamera:public Camera
{
    float fov;
    float nearPlane;
    float farPlane;

    float minDistance;
    float maxDistance;

    QVector3D center;
    float distance;
    float pitch;
    float yaw;
public:
    SphericalCamera();
    QMatrix4x4 getView();
    QMatrix4x4 getProjection();
    void setFov(float fov);
    void setYaw(float angle);
    void setPitch(float angle);
    void changeDistance(float change);
    void move(QVector2D delta);
    void setCenter(QVector3D c);
    void setDistance(float d);
    float getDistance() const;
    float getMinDistance() const;
    float getMaxDistance() const;
    float getDistanceNormalized() const;
};

class OrthoCamera:public Camera
{
    float nearPlane;
    float farPlane;
    float zoom;
    QVector3D position;
    QVector3D direction;
    QVector3D up;
public:
    OrthoCamera();
    void setDirection(QVector3D value);
    void setPosition(QVector3D value);
    void setUp(QVector3D value);
    void translate(QVector3D vec);
    void setZoom(float value);
    void setNearPlane(float value);
    void setFarPlane(float value);
    QMatrix4x4 getView();
    QMatrix4x4 getProjection();
};


#endif // CAMERA_H
