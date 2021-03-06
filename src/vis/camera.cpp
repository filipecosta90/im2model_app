#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace vis;

static const float MaxVerticalAngle = 85.0f; //must be less than 90 to avoid gimbal lock

Camera::Camera() :
  _eye(0,0,3),
  _center(0,0,0),
  _vis_up(0,0,1),
  _position(0.0f, 0.0f, 4.0f),
  _horizontalAngle(0.0f),
  _verticalAngle(0.0f),
  _fieldOfView(50.0f),
  _nearPlane(0.01f),
  _farPlane(100.0f),
  _viewportAspectRatio(4.0f/3.0f),
  _window_width(1024),
  _window_height(720)
{
}

const glm::vec3& Camera::position() const {
  return _position;
}

void Camera::set_vis_up( const glm::vec3& upward_vector ){
  _vis_up = upward_vector;
}

void Camera::set_u( const glm::vec3& u ){
  _u = u;
}

void Camera::set_v( const glm::vec3& v ){
  _v = v;
}

void Camera::set_n( const glm::vec3& n ){
  _n = n;
}

void Camera::set_eye(const glm::vec3& eye_vector ){
  _eye = eye_vector; 
}

void Camera::set_center(const glm::vec3& center_vector ){
  _center = center_vector;
}

void Camera::set_window_width( int width ){
  _window_width = width;
}

void Camera::set_window_height( int height ){
  _window_height = height;
}

void Camera::setPosition(const glm::vec3& position) {
  _position = position;
}

void Camera::offsetPosition(const glm::vec3& offset) {
  _position += offset;
}

float Camera::fieldOfView() const {
  return _fieldOfView;
}

void Camera::setFieldOfView(float fieldOfView) {
  assert(fieldOfView > 0.0f && fieldOfView < 180.0f);
  _fieldOfView = fieldOfView;
}

float Camera::nearPlane() const {
  return _nearPlane;
}

float Camera::farPlane() const {
  return _farPlane;
}

void Camera::setNearAndFarPlanes(float nearPlane, float farPlane) {
  assert(nearPlane > 0.0f);
  assert(farPlane > nearPlane);
  _nearPlane = nearPlane;
  _farPlane = farPlane;
}

glm::mat4 Camera::orientation() const {
  glm::mat4 orientation;
  orientation = glm::rotate(orientation, glm::radians(_verticalAngle), glm::vec3(1,0,0));
  orientation = glm::rotate(orientation, glm::radians(_horizontalAngle), glm::vec3(0,1,0));
  return orientation;
}

void Camera::offsetOrientation(float upAngle, float rightAngle) {
  _horizontalAngle += rightAngle;
  _verticalAngle += upAngle;
  normalizeAngles();
}

void Camera::lookAt(glm::vec3 position) {
  assert(position != _position);
  glm::vec3 direction = glm::normalize(position - _position);
  _verticalAngle = glm::radians(asinf(-direction.y));
  _horizontalAngle = -glm::radians(atan2f(-direction.x, -direction.z));
  normalizeAngles();
}

float Camera::viewportAspectRatio() const {
  return _viewportAspectRatio;
}

void Camera::setViewportAspectRatio(float viewportAspectRatio) {
  assert(viewportAspectRatio > 0.0);
  _viewportAspectRatio = viewportAspectRatio;
}

glm::vec3 Camera::forward() const {
  glm::vec4 forward = glm::inverse(orientation()) * glm::vec4(0,0,-1,1);
  return glm::vec3(forward);
}

glm::vec3 Camera::right() const {
  glm::vec4 right = glm::inverse(orientation()) * glm::vec4(1,0,0,1);
  return glm::vec3(right);
}

glm::vec3 Camera::up() const {
  glm::vec4 up = glm::inverse(orientation()) * glm::vec4(0,1,0,1);
  return glm::vec3(up);
}

glm::mat4 Camera::matrix() const {
  return projection() * view();
}

glm::mat4 Camera::projection() const {
  //ortho  
  //return glm::ortho(0.0f, (float) _window_width, 0.0f,(float) _window_height , _nearPlane, _farPlane );
  return glm::perspective(glm::radians(_fieldOfView), _viewportAspectRatio, _nearPlane, _farPlane);
}

glm::mat4 Camera::view() const {
  //glm::mat4 view = glm::lookAt( _eye, _center, _vis_up ) * glm::translate(glm::mat4(), -_position); 
  //  std::cout << glm::to_string( view ) << std::endl;// return glm::lookAt( _eye, _center, _vis_up );
  glm::mat4 view = glm::lookAt( _position, _center + _n, _vis_up ); //orientation() * glm::translate(glm::mat4(), -_position);

  //glm::lookAt( _eye, _center, _vis_up );
  return view; 
}

void Camera::normalizeAngles() {
  _horizontalAngle = fmodf(_horizontalAngle, 360.0f);
  //fmodf can return negative values, but this will make them all positive
  if(_horizontalAngle < 0.0f)
    _horizontalAngle += 360.0f;
  if(_verticalAngle > MaxVerticalAngle)
    _verticalAngle = MaxVerticalAngle;
  else if(_verticalAngle < -MaxVerticalAngle)
    _verticalAngle = -MaxVerticalAngle;
}

