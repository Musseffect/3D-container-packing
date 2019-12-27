#include "camera.h"
#include <QtMath>
#include <QtGlobal>


SphericalCamera::SphericalCamera():center(0.0,0.0,0.0),distance(10.0f),
    fov(75.0f),Camera(),nearPlane(0.01f),farPlane(300.0f),
    pitch(0.0f),yaw(0.0f),maxDistance(200.0f),minDistance(0.5f)
{
}

QMatrix4x4 SphericalCamera::getView()
{
    QMatrix4x4 view;
    view.lookAt(center+QVector3D(cos(pitch)*sin(yaw),sin(pitch),cos(pitch)*cos(yaw))*distance,center,QVector3D(0.0,1.0,0.0));
    return view;
}

QMatrix4x4 SphericalCamera::getProjection()
{
    QMatrix4x4 proj;
    proj.perspective(fov,width/height,nearPlane,farPlane);
    return proj;
}

void Camera::setWidth(float w)
{
    width=w;
}

void Camera::setHeight(float h)
{
    height=h;
}

void SphericalCamera::setFov(float fov)
{
    this->fov=fov;
}

void SphericalCamera::setYaw(float angle)
{
    yaw=angle-qFloor(angle/qDegreesToRadians(360.0f))*qDegreesToRadians(360.0f);
}

void SphericalCamera::setPitch(float angle)
{
    pitch=qMax(qMin(angle,
                  qDegreesToRadians(80.0f)),
              -qDegreesToRadians(80.0f));
}

void SphericalCamera::move(QVector2D delta)
{
    setPitch(pitch+delta.y()*qDegreesToRadians(180.0f));
    setYaw(yaw+delta.x()*qDegreesToRadians(180.0f));
}
void SphericalCamera::changeDistance(float change)
{
    setDistance(distance+qLn(qAbs(change)+1.0)*(change>0.0?1.0:-1.0));
}
void SphericalCamera::setCenter(QVector3D c)
{
    center=c;
}

void SphericalCamera::setDistance(float d)
{
    distance=qMin(maxDistance,qMax(minDistance,d));
}

float SphericalCamera::getDistance() const
{
   return distance;
}

float SphericalCamera::getMinDistance() const
{
    return minDistance;
}

float SphericalCamera::getMaxDistance() const
{
    return maxDistance;
}

float SphericalCamera::getDistanceNormalized() const
{
    return distance-minDistance/
            (maxDistance-minDistance);
}

OrthoCamera::OrthoCamera():zoom(1.0f),nearPlane(0.01f),farPlane(1000.0f)
{
}

void OrthoCamera::setDirection(QVector3D value)
{
    direction=value;
}

void OrthoCamera::setPosition(QVector3D value)
{
    position=value;
}

void OrthoCamera::setUp(QVector3D value)
{
    up=value;
}

void OrthoCamera::translate(QVector3D vec)
{
    position+=vec;
}

void OrthoCamera::setZoom(float value)
{
    zoom=value;
}

QMatrix4x4 OrthoCamera::getView()
{
    QMatrix4x4 view;
    view.lookAt(position,position+direction,up);
    return view;
}

QMatrix4x4 OrthoCamera::getProjection()
{
    QMatrix4x4 proj;
    proj.ortho(-zoom,zoom,-zoom,zoom,nearPlane,farPlane);
    return proj;
}
