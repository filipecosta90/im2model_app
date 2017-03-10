// standard C++ libraries
#include <cassert>
#include <stdexcept>
#include <cmath>
#include <list>
#include <cctype>
#include <fstream>
#include <cassert>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstdio>
#include <sstream>      // std::stringstream
#include <string>       // std::string
#include <iostream>     // std::cout
#include <cstdlib>
#include <iomanip>
#include <algorithm>    // std::min_element, std::max_element
#include <stdio.h>
// Include std::exception so we can handling any argument errors
// emitted by the command line parser
#include <exception>

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

// Include the headers relevant to the boost::program_options
// library
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

// Visualization
// third-party libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "opengl.hpp"

// project classes
#include "celslc_prm.hpp"
#include "msa_prm.hpp"
#include "wavimg_prm.hpp"
#include "simgrid_steplength.hpp"
#include "mc_driver.hpp"
#include "unit_cell.hpp"
#include "super_cell.hpp"
#include "chem_database.hpp"

// vis classes
#include "program.hpp"
#include "camera.hpp"

using namespace cv;

// Global Variables

#ifdef _WIN32
std::string celslc_bin_string =  "../bin/drprobe_clt_bin_winx64/celslc";
std::string msa_bin_string = "../bin/drprobe_clt_bin_winx64/msa";
std::string wavimg_bin_string = "../bin/drprobe_clt_bin_winx64/wavimg";
#elif defined __unix__
////do something for unix like #include <unistd.h>
#elif defined __APPLE__
std::string celslc_bin_string =  "../bin/drprobe_clt_bin_osx/celslc";
std::string msa_bin_string = "../bin/drprobe_clt_bin_osx/msa";
std::string wavimg_bin_string = "../bin/drprobe_clt_bin_osx/wavimg";
#endif

// Experimental Image info
cv::Mat experimental_image; 
cv::Mat experimental_image_roi;
cv::Mat experimental_working; 

Rect roi_rectangle;
int roi_x_size;
int roi_y_size;
int roi_center_x;
int roi_center_y;
int supercell_min_width;
int supercell_min_height;

double sampling_rate_experimental_x_nm_per_pixel;
double  sampling_rate_experimental_y_nm_per_pixel;

int edge_detection_threshold = 100;
int max_thresh = 255;
RNG rng(12345);
int max_contour_distance_px = 19;
int max_contour_distance_thresh_px = 255;

Unit_Cell unit_cell;
Super_Cell super_cell;
cv::Point3d  zone_axis_vector_uvw;
cv::Point3d  upward_vector_hkl;

/*
   Represents a textured geometry asset
   Contains everything necessary to draw arbitrary geometry with a single texture:
   - shaders
   - a texture
   - a VBO
   - a IBO 
   - a VAO
   - the parameters to glDrawArrays (drawType, drawStart, drawCount)
   */

struct ModelAsset {
  vis::Program* shaders;
  GLuint vbo;
  GLuint ibo;
  GLuint vao;
  GLenum drawType;
  GLint drawStart;
  GLint drawCount;
  GLint numIndices;

  ModelAsset():
    shaders(NULL),
    vbo(0),
    ibo(0),
    vao(0),
    drawType(GL_TRIANGLES),
    drawStart(0),
    drawCount(0),
    numIndices(0)
  {}
};

/*
   Represents an instance of an `ModelAsset`
   Contains a pointer to the asset, and a model transformation matrix to be used when drawing.
   */
struct ModelInstance {
  ModelAsset* asset;
  glm::mat4 transform;
  glm::vec4 rgba;
  GLenum polygon_mode; 
  GLenum polygon_face;


  ModelInstance() :
    asset(NULL),
    transform(),
    rgba(1.0f,0.0f,0.0f,1.0f),
    polygon_mode(GL_FILL),
    polygon_face(GL_FRONT_AND_BACK)
  {}
};

/* test opengl */

// constants
const glm::vec2 SCREEN_SIZE(1024 , 760);

// globals
GLFWwindow* gWindow = NULL;
double gScrollY = 0.0;
vis::Camera gCamera;
ModelAsset vis_atom_asset;
ModelAsset vis_unit_cell_asset;
std::list<ModelInstance> gInstances;
GLfloat gDegreesRotated = 0.0f;

double DEGS_TO_RAD = M_PI/180.0f;

// returns a new vis::Program created from the given vertex and fragment shader filenames
static vis::Program* LoadShaders(const char* vertFilename, const char* fragFilename) {
  std::vector<vis::Shader> shaders;
  shaders.push_back(vis::Shader::shaderFromFile(vertFilename, GL_VERTEX_SHADER));
  shaders.push_back(vis::Shader::shaderFromFile(fragFilename, GL_FRAGMENT_SHADER));
  return new vis::Program(shaders);
}

// initialises the vis_atom_asset global
static void LoadVisUnitCellAsset( double side ) {
  // set all the elements of vis_atom_asset
  vis_unit_cell_asset.shaders = LoadShaders("shaders/cube.v.glsl", "shaders/cube.f.glsl");
  vis_unit_cell_asset.drawType = GL_TRIANGLES;
  glGenBuffers(1, &vis_unit_cell_asset.vbo);
  glGenVertexArrays(1, &vis_unit_cell_asset.vao);
  glGenBuffers(1, &vis_unit_cell_asset.ibo);

  // bind the VAO
  glBindVertexArray(vis_unit_cell_asset.vao);

  std::vector<GLfloat> vertices;
  std::vector<GLint> indices;

  //------------------------
  //-- creates a sphere as a "standard sphere" triangular mesh with the specified
  //-- number of latitude (nLatitude) and longitude (nLongitude) lines
  cv::Point3d pt(0.0f,0.0f,0.0f);
  int initial_vertices_size = vertices.size() / 3;    // the needed padding

  // bottom vertex 1 - A
  vertices.push_back( pt.x );
  vertices.push_back( pt.y );
  vertices.push_back( pt.z );
  // bottom vertex 2 - B
  vertices.push_back( pt.x+side );
  vertices.push_back( pt.y );
  vertices.push_back( pt.z );
  // bottom vertex 3 - C
  vertices.push_back( pt.x+side );
  vertices.push_back( pt.y+side );
  vertices.push_back( pt.z );
  // bottom vertex 4 - D
  vertices.push_back( pt.x );
  vertices.push_back( pt.y+side );
  vertices.push_back( pt.z );
  // top vertex 5 - E
  vertices.push_back( pt.x );
  vertices.push_back( pt.y );
  vertices.push_back( pt.z+side );
  // top vertex 6 - F
  vertices.push_back( pt.x+side );
  vertices.push_back( pt.y );
  vertices.push_back( pt.z+side );
  // top vertex 7 - G
  vertices.push_back( pt.x+side );
  vertices.push_back( pt.y+side );
  vertices.push_back( pt.z+side );
  // top vertex 8 - H
  vertices.push_back( pt.x );
  vertices.push_back( pt.y+side );
  vertices.push_back( pt.z+side );

  int a,b,c,d,e,f,g,h;
  a = initial_vertices_size;
  b = a + 1;
  c = b + 1;
  d = c + 1;
  e = d + 1;
  f = e + 1;
  g = f + 1;
  h = g + 1;

  //front 1
  indices.push_back( b );
  indices.push_back( c );
  indices.push_back( g );
  // front 2
  indices.push_back( b );
  indices.push_back( g );
  indices.push_back( f );
  // right 1
  indices.push_back( g );
  indices.push_back( h );
  indices.push_back( d );
  // right 2
  indices.push_back( g );
  indices.push_back( d );
  indices.push_back( c );
  // back 1
  indices.push_back( a );
  indices.push_back( d );
  indices.push_back( h );
  // back 2
  indices.push_back( a );
  indices.push_back( h );
  indices.push_back( e );
  // left 1
  indices.push_back( a );
  indices.push_back( b );
  indices.push_back( f );
  // left 2
  indices.push_back( a );
  indices.push_back( f );
  indices.push_back( e );
  // top 1
  indices.push_back( g );
  indices.push_back( f );
  indices.push_back( e );
  // top 2
  indices.push_back( g );
  indices.push_back( e );
  indices.push_back( h );
  // bottom 1
  indices.push_back( a );
  indices.push_back( d );
  indices.push_back( c );
  // bottom 2
  indices.push_back( a );
  indices.push_back( c );
  indices.push_back( b );

  vis_unit_cell_asset.drawStart = 0;
  vis_unit_cell_asset.drawCount = vertices.size();
  vis_unit_cell_asset.numIndices = indices.size();

  // bind the VBO
  glBindBuffer(GL_ARRAY_BUFFER, vis_unit_cell_asset.vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

  // bind the IBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vis_unit_cell_asset.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), indices.data(), GL_STATIC_DRAW);

  // connect the xyz to the "vert" attribute of the vertex shader
  glEnableVertexAttribArray(vis_unit_cell_asset.shaders->attrib("vert"));
  glVertexAttribPointer(vis_unit_cell_asset.shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL); //3*sizeof(GLfloat), NULL);

  // unbind buffers 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // unbind the VAO
  glBindVertexArray(0);
}

// initialises the vis_atom_asset global
static void LoadVisAtomAsset() {
  // set all the elements of vis_atom_asset
  vis_atom_asset.shaders = LoadShaders("shaders/cube.v.glsl", "shaders/cube.f.glsl");
  vis_atom_asset.drawType = GL_TRIANGLES;
  glGenBuffers(1, &vis_atom_asset.vbo);
  glGenVertexArrays(1, &vis_atom_asset.vao);
  glGenBuffers(1, &vis_atom_asset.ibo);

  // bind the VAO
  glBindVertexArray(vis_atom_asset.vao);

  std::vector<GLfloat> vertices;
  std::vector<GLint> indices;

  //------------------------
  //-- creates a sphere as a "standard sphere" triangular mesh with the specified
  //-- number of latitude (nLatitude) and longitude (nLongitude) lines
  cv::Point3d pt(0.0f,0.0f,0.0f);
  float radius = 0.5f;
  int nLatitude = 10;
  int nLongitude = 10;
  int p, s, i;
  float x, y, z, out;
  int nPitch = nLongitude + 1;
  int initial_vertices_size = vertices.size() / 3;    // the needed padding
  std::cout << "initial_vertices_size " << initial_vertices_size << std::endl;

  int numVertices = 0;    // the number of vertex points added.
  float pitchInc = (180. / (float)nPitch) * DEGS_TO_RAD;
  float rotInc   = (360. / (float)nLatitude) * DEGS_TO_RAD;

  // Top vertex.
  vertices.push_back( pt.x );
  vertices.push_back( pt.y );
  vertices.push_back( pt.z+radius );
  // Bottom vertex.
  vertices.push_back( pt.x );
  vertices.push_back( pt.y );
  vertices.push_back( pt.z-radius );

  numVertices = numVertices+2;

  int fVert = initial_vertices_size + numVertices;    // Record the first vertex index for intermediate vertices.
  for(p=1; p<nPitch; p++)     // Generate all "intermediate vertices":
  {
    out = radius * sin((float)p * pitchInc);
    if(out < 0) out = -out;    // abs() command won't work with all compilers
    z   = radius * cos(p * pitchInc);
    for(s=0; s<=nLatitude; s++)
    {
      x = out * cos(s * rotInc);
      y = out * sin(s * rotInc);
      vertices.push_back( x+pt.x );
      vertices.push_back( y+pt.y );
      vertices.push_back( z+pt.z );
      numVertices++;
    }
  }

  //## create triangles between intermediate points
  for(p=1; p<(nPitch-1); p++) // starts from lower part of sphere up
  {
    for(s=0; s<(nLatitude); s++) {
      i = p*(nLatitude+1) + s;
      // a - d //
      // | \ | //
      // b - c //
      const int upper_pos_lat = i - (nLatitude+1);
      float a = upper_pos_lat+fVert;
      float b = i+fVert;
      float c = i+1+fVert;
      float d = upper_pos_lat+1+fVert;
      // 1st triangle abc
      indices.push_back( a );
      indices.push_back( b );
      indices.push_back( c );
      // 2nd triangle acd
      indices.push_back( a );
      indices.push_back( c );
      indices.push_back( d );
    }
  }

  //## create triangles connecting to top and bottom vertices
  int offLastVerts  = initial_vertices_size + numVertices - (nLatitude+1);
  for(s=0; s<(nLatitude-1); s++)
  {
    //top
    indices.push_back( initial_vertices_size   );
    indices.push_back( (s+2)+fVert );
    indices.push_back( (s+1)+fVert );
    //bottom
    indices.push_back( initial_vertices_size + 1);
    indices.push_back( (s+1)+offLastVerts );
    indices.push_back( (s+2)+offLastVerts );
  }

  vis_atom_asset.drawStart = 0;
  vis_atom_asset.drawCount = vertices.size();
  vis_atom_asset.numIndices = indices.size();

  // bind the VBO
  glBindBuffer(GL_ARRAY_BUFFER, vis_atom_asset.vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

  // bind the IBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vis_atom_asset.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), indices.data(), GL_STATIC_DRAW);

  // connect the xyz to the "vert" attribute of the vertex shader
  glEnableVertexAttribArray(vis_atom_asset.shaders->attrib("vert"));
  glVertexAttribPointer(vis_atom_asset.shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL); //3*sizeof(GLfloat), NULL);

  // unbind buffers 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // unbind the VAO
  glBindVertexArray(0);

}

// convenience function that returns a translation matrix
glm::mat4 translate(GLfloat x, GLfloat y, GLfloat z) {
  return glm::translate(glm::mat4(), glm::vec3(x,y,z));
}

// convenience function that returns a scaling matrix
glm::mat4 scale(GLfloat x, GLfloat y, GLfloat z) {
  return glm::scale(glm::mat4(), glm::vec3(x,y,z));
}

//create all the `instance` structs for the 3D scene, and add them to `gInstances`
static void CreateInstances() {

  ModelInstance unit_cell_asset;
  unit_cell_asset.asset = &vis_unit_cell_asset;
  unit_cell_asset.polygon_mode = GL_LINE;
  gInstances.push_back( unit_cell_asset );

  std::vector<glm::vec3> atom_positions = super_cell.get_atom_positions_vec();
  std::vector<glm::vec4> atom_cpk_rgba_colors = super_cell.get_atom_cpk_rgba_colors_vec();
  std::vector<double> atom_empirical_radii = super_cell.get_atom_empirical_radii_vec();
  std::vector<glm::vec3>::iterator pos_itt;

  std::cout << "going to create  " << atom_positions.size() << " instances" << std::endl;
  for ( size_t vec_pos = 0; vec_pos < atom_positions.size(); vec_pos++){
    const glm::vec3 pos = atom_positions.at(vec_pos);
    ModelInstance atom;
    atom.asset = &vis_atom_asset;
    atom.rgba = atom_cpk_rgba_colors.at(vec_pos);
    const double radii = atom_empirical_radii.at(vec_pos);
    atom.transform = translate(pos.x, pos.y, pos.z) * scale(radii, radii, radii );
    gInstances.push_back(atom);
  }
}

//renders a single `ModelInstance`
static void RenderInstance(const ModelInstance& inst) {
  ModelAsset* asset = inst.asset;
  vis::Program* shaders = asset->shaders;

  //bind the shaders
  shaders->use();

  //set the shader uniforms
  shaders->setUniform("camera", gCamera.matrix());
  shaders->setUniform("model", inst.transform);
  shaders->setUniform("model_color", inst.rgba);

  //bind VAO and draw
  glBindVertexArray(asset->vao);
  glBindBuffer(GL_ARRAY_BUFFER, asset->vbo);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, asset->ibo);

  glPolygonMode( inst.polygon_face , inst.polygon_mode );

  glDrawElements( asset->drawType , asset->numIndices, GL_UNSIGNED_INT , 0);

  // unbind the element render buffer 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  // unbind the vertex array buffer 
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  //unbind everything
  glBindVertexArray(0);
  shaders->stopUsing();

}

// draws a single frame
static void Render() {
  // clear everything
  glClearColor(1, 1, 1, 1); // white bg 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // render all the instances
  std::list<ModelInstance>::const_iterator it;

  for(it = gInstances.begin(); it != gInstances.end(); ++it){
    RenderInstance(*it);
  }
  // swap the display buffers (displays what was just drawn)
  glfwSwapBuffers(gWindow);
}


// update the scene based on the time elapsed since last update
static void Update(float secondsElapsed) {
  //rotate the first instance in `gInstances`
  const GLfloat degreesPerSecond = 180.0f;
  gDegreesRotated += secondsElapsed * degreesPerSecond;

  //move position of camera based on WASD keys, and XZ keys for up and down
  const float moveSpeed = 2.0; //units per second
  if(glfwGetKey(gWindow, 'S')){
    gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.forward());
  } else if(glfwGetKey(gWindow, 'W')){
    gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.forward());
  }
  if(glfwGetKey(gWindow, 'A')){
    gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.right());
  } else if(glfwGetKey(gWindow, 'D')){
    gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.right());
  }
  if(glfwGetKey(gWindow, 'Z')){
    gCamera.offsetPosition(secondsElapsed * moveSpeed * -glm::vec3(0,1,0));
  } else if(glfwGetKey(gWindow, 'X')){
    gCamera.offsetPosition(secondsElapsed * moveSpeed * glm::vec3(0,1,0));
  }

  //rotate camera based on mouse movement
  const float mouseSensitivity = 0.1f;
  double mouseX, mouseY;
  glfwGetCursorPos(gWindow, &mouseX, &mouseY);
  gCamera.offsetOrientation(mouseSensitivity * (float)mouseY, mouseSensitivity * (float)mouseX);
  glfwSetCursorPos(gWindow, 1, 1); //reset the mouse, so it doesn't go out of the window

  //increase or decrease field of view based on mouse wheel
  const float zoomSensitivity = -0.2f;
  float fieldOfView = gCamera.fieldOfView() + zoomSensitivity * (float)gScrollY;
  if(fieldOfView < 5.0f) fieldOfView = 5.0f;
  if(fieldOfView > 130.0f) fieldOfView = 130.0f;
  gCamera.setFieldOfView(fieldOfView);
  gScrollY = 0;
}

// records how far the y axis has been scrolled
void OnScroll(GLFWwindow* window, double deltaX, double deltaY) {
  gScrollY += deltaY;
}

void OnError(int errorCode, const char* msg) {
  throw std::runtime_error(msg);
}

// the program starts here
void AppVis() {
  // initialise GLFW
  glfwSetErrorCallback(OnError);
  if(!glfwInit())
    throw std::runtime_error("glfwInit failed");

  // open a window with GLFW
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  gWindow = glfwCreateWindow((int)SCREEN_SIZE.x, (int)SCREEN_SIZE.y, "Im2Model Vis", NULL, NULL);
  if(!gWindow)
    throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");

  // GLFW settings
  glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPos(gWindow, 1, 1);
  glfwSetScrollCallback(gWindow, OnScroll);
  glfwMakeContextCurrent(gWindow);

  // initialise GLEW
  glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
  if(glewInit() != GLEW_OK)
    throw std::runtime_error("glewInit failed");

  // GLEW throws some errors, so discard all the errors so far
  while(glGetError() != GL_NO_ERROR) {}

  // print out some info about the graphics drivers
  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

  // make sure OpenGL version 3.2 API is available
  if(!GLEW_VERSION_3_2)
    throw std::runtime_error("OpenGL 3.2 API is not available.");

  // OpenGL settings
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  // initialise the vis_unit_cell asset
  LoadVisUnitCellAsset( unit_cell.get_cell_length_a_Nanometers() );
  // initialise the vis_atom_asset asset
  LoadVisAtomAsset();

  // create all the instances in the 3D scene based on the vis_atom_asset asset
  CreateInstances();

  // setup gCamera

  glm::vec3 vec_n = glm::normalize( glm::vec3( zone_axis_vector_uvw.x, zone_axis_vector_uvw.y, zone_axis_vector_uvw.z ) ); 
  glm::vec3 vec_v = glm::normalize( glm::vec3( upward_vector_hkl.x, upward_vector_hkl.y, upward_vector_hkl.z ) );
  cv::Point3d cv_t = unit_cell.get_vector_t();
  glm::vec3 vec_up = glm::normalize( glm::vec3( cv_t.x, cv_t.y, cv_t.z ) );

  glm::vec3 center =  glm::vec3(0.0f,0.0f,0.0f);

  glm::vec3 eye = glm::normalize( glm::vec3( zone_axis_vector_uvw.x, zone_axis_vector_uvw.y, zone_axis_vector_uvw.z ) );
  glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f);
  position.x = super_cell.get_super_cell_length_a_Nanometers()*2.0f; 
  position.y = super_cell.get_super_cell_length_b_Nanometers()*2.0f; 
  position.z = super_cell.get_super_cell_length_c_Nanometers()*2.0f; 

  gCamera.set_center( center );
  gCamera.setPosition( position ); // position );
  gCamera.set_n( vec_n );
  gCamera.lookAt( -eye );
  gCamera.set_vis_up( vec_up );

  std::cout << "Visualization Parameters" << std::endl;
  std::cout << "Center: " << glm::to_string( center ) <<  std::endl;   
  std::cout << "CAM Looking in direction: " << glm::to_string( eye ) <<  std::endl; 
  std::cout << "CAM Up: " << glm::to_string( vec_up ) <<  std::endl;
  std::cout << "CAM Position: " << glm::to_string( position ) <<  std::endl;  
  std::cout << "View Matrix:" << std::endl;
  std::cout << glm::to_string( gCamera.view() ) << std::endl;

  // gCamera.setPosition(glm::vec3(upward_vector_hkl.x, upward_vector_hkl.y, upward_vector_hkl.z));
  // gCamera.set_eye((super_cell.get_super_cell_length_a_Nanometers()*2.0f,super_cell.get_super_cell_length_b_Nanometers()*2.0f,super_cell.get_super_cell_length_c_Nanometers() * 2.0f));
  //gCamera.set_center( glm::vec3(0.0f,0.0f,0.0f) );
  //gCamera.set_vis_up( glm::normalize (glm::vec3(zone_axis_vector_uvw.x, zone_axis_vector_uvw.y, zone_axis_vector_uvw.z) ) );
  // gCamera.lookAt(glm::vec3(zone_axis_vector_uvw.x, zone_axis_vector_uvw.y, zone_axis_vector_uvw.z));
  gCamera.set_window_width( SCREEN_SIZE.x );
  gCamera.set_window_height( SCREEN_SIZE.y );
  gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

  // run while the window is open
  double lastTime = glfwGetTime();
  while(!glfwWindowShouldClose(gWindow)){
    // process pending events
    glfwPollEvents();

    // update the scene based on the time elapsed since last update
    double thisTime = glfwGetTime();
    Update((float)(thisTime - lastTime));
    lastTime = thisTime;

    // draw one frame
    Render();

    // check for errors
    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
      std::cerr << "OpenGL Error " << error << std::endl;

    //exit program if escape key is pressed
    if(glfwGetKey(gWindow, GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(gWindow, GL_TRUE);
  }

  // clean up and exit
  glfwTerminate();
}

/// Function Headers
void thresh_callback(int, void* );

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
  if  ( event == EVENT_LBUTTONDOWN )
  {
    std::cout << "New ROI center - position (" << x << ", " << y << ")" << std::endl;
    roi_rectangle.x = x - ( roi_x_size / 2 );
    roi_rectangle.y = y - ( roi_y_size / 2) ;
    roi_rectangle.width = roi_x_size;
    roi_rectangle.height = roi_y_size;
    experimental_working = experimental_image.clone();
    // copy the roi
    cv::Mat roi = experimental_image(roi_rectangle);
    experimental_image_roi.create( roi_x_size, roi_y_size , CV_8UC1 );
    roi.copyTo(experimental_image_roi);
    //draw the roi
    rectangle(experimental_working, roi_rectangle, Scalar(0,0,255), 5);
    imshow("Experimental Window", experimental_working);
  }
  else if  ( event == EVENT_LBUTTONUP )
  {
    //  std::cout << "Left button released" << std::endl;
  }
  else if ( event == EVENT_MOUSEMOVE )
  {
    //  std::cout << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;
  }
}

void thresh_callback(int, void* )
{
  cv::Mat experimental_working_canny, canny_output;
  std::vector< std::vector<Point> > contours;
  std::vector< std::vector<Point> > contours_2nd_itt;

  std::vector<Vec4i> hierarchy;

  //// contours = get_experimental_image_contours(experimental_image, edge_detection_threshold );

  std::cout << "Detected " << contours.size() << " countours" << std::endl;
  Mat1f dist(contours.size(), contours.size(), 0.f);
  Mat1i in_range(contours.size(), contours.size(), 0);

  for( size_t i = 0; i< contours.size(); i++ ){
    for( size_t j = (i+1); j< contours.size(); j++ ){
      const double raw_distance = fabs(cv::pointPolygonTest( contours[i], contours[j][0] , true ));
      dist[i][j]=raw_distance;
      dist[j][i]=raw_distance;
    }
  }

  cv::Mat raw_edge_drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  cv::Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );

  int contours_in_range = 0;
  for( size_t i = 0; i< contours.size(); i++ ){
    const double raw_distance = fabs(cv::pointPolygonTest( contours[i], Point2f(roi_center_x,roi_center_y), true ));
    if ( raw_distance < max_contour_distance_px ){
      in_range[i][1]=1;
      contours_in_range++;
    }
  }

  std::cout << "There are " << contours_in_range << " contours in range("<< max_contour_distance_px <<"px) of the ROI" << std::endl;
  int itt_num = 1;
  int added_itt = 1;

  while ( added_itt > 0 ){
    added_itt = 0;
    for( size_t i = 0; i< contours.size(); i++ ){
      if ( in_range[i][1] == 1 ){
        for( size_t j = 0; j < contours.size(); j++ ){
          if ( in_range[j][1] == 0 ){
            if ( ( dist[i][j] < max_contour_distance_px )|| (dist[j][i] < max_contour_distance_px ) ){
              in_range[j][1]=1;
              added_itt++;
            }
          }
        }
      }
    }
    std::cout << "Added  " << added_itt << " contours in iteration # "<< itt_num << std::endl;
    itt_num++;
  }

  /// Find the convex hull object for each contour
  std::vector<std::vector<cv::Point> > roi_contours;

  for( size_t i = 0; i< contours.size(); i++ ){
    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    if ( in_range[i][1] == 1 ){
      roi_contours.push_back(contours[i]);
      drawContours( drawing, contours, i, color, 2, 8, NULL , 0, Point() );
    }
    drawContours( raw_edge_drawing, contours, i, color, 2, 8, NULL , 0, Point() );
  }

  std::vector<cv::Point> contours_merged;

  for( size_t i = 0; i < roi_contours.size(); i++ ){
    for ( size_t j = 0; j< roi_contours[i].size(); j++  ){
      contours_merged.push_back(roi_contours[i][j]);
    }
  }

  std::vector<std::vector<cv::Point>> hull( 1 );
  convexHull( Mat(contours_merged), hull[0], false );

  std::cout << " HULL SIZE: " << hull[0].size() << std::endl;
  std::vector<cv::Point>::iterator contour_it; 

  for ( contour_it = hull[0].begin(); contour_it != hull[0].end(); contour_it++){
    cv::Point p1 = *contour_it;
    std::cout << p1 << std::endl;  
  }

  cv::Rect supercell_boundaries_rect = boundingRect(contours_merged);
  Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
  drawContours( drawing, hull, 0, color, 3, 8, std::vector<Vec4i>(), 0, Point() );
  double roi_area_px = cv::contourArea(hull[0],false);
  double roi_area_nm = roi_area_px * sampling_rate_experimental_x_nm_per_pixel * sampling_rate_experimental_y_nm_per_pixel;
  std::cout << "ROI area " << roi_area_px << " px, " << roi_area_nm << " nm^2" << std::endl;

  namedWindow( "Contours", WINDOW_AUTOSIZE );

  // line 1

  std::stringstream output_legend_line1;
  std::stringstream output_legend_line2;
  std::stringstream output_legend_line3;
  std::stringstream output_legend_line4;

  output_legend_line1 <<  "ROI area: " << roi_area_px << " sq. px, " << roi_area_nm << "sq. nm";
  output_legend_line2 <<  "Canny thresh: " << edge_detection_threshold;
  output_legend_line3 <<  "Max distance between countours: " << max_contour_distance_px<< " px";
  output_legend_line4 <<  "# Contours selected: " << roi_contours.size() << " from "<< contours.size() << ", calculated in " << itt_num << " iterations";

  std::string line1_experimental_roi_info = output_legend_line1.str();
  std::string line2_experimental_roi_info = output_legend_line2.str();
  std::string line3_experimental_roi_info = output_legend_line3.str();
  std::string line4_experimental_roi_info = output_legend_line4.str();

  int legend_position_x = 10;
  int legent_position_y_bottom_left_line_1 = 25;
  int legent_position_y_bottom_left_line_2 = 50;
  int legent_position_y_bottom_left_line_3 = 75;
  int legent_position_y_bottom_left_line_4 = 100;

  putText(drawing, line1_experimental_roi_info , cvPoint(legend_position_x , legent_position_y_bottom_left_line_1), FONT_HERSHEY_PLAIN, 1.5, cvScalar(255,255,255), 1, CV_AA);
  putText(drawing, line2_experimental_roi_info , cvPoint(legend_position_x , legent_position_y_bottom_left_line_2), FONT_HERSHEY_PLAIN, 1.5, cvScalar(255,255,255), 1, CV_AA);
  putText(drawing, line3_experimental_roi_info , cvPoint(legend_position_x , legent_position_y_bottom_left_line_3), FONT_HERSHEY_PLAIN, 1.5, cvScalar(255,255,255), 1, CV_AA);
  putText(drawing, line4_experimental_roi_info , cvPoint(legend_position_x , legent_position_y_bottom_left_line_4), FONT_HERSHEY_PLAIN, 1.5, cvScalar(255,255,255), 1, CV_AA);

  cv::rectangle(drawing, supercell_boundaries_rect, Scalar(0,0,255), 2);
  std::cout << "Boundaries rectangle: " << supercell_boundaries_rect << std::endl; 
  std::cout << "Boundaries rectangle size: " << supercell_boundaries_rect.size() << std::endl; 
  supercell_min_width = supercell_boundaries_rect.width;
  supercell_min_height = supercell_boundaries_rect.height;
  imshow( "Contours", drawing );

  // save the experimental calculated roi
  imwrite( "exp_roi_auto.png", drawing );
  imwrite( "exp_roi_raw_edge_detected.png", raw_edge_drawing );
  imwrite( "exp_raw.png", experimental_image );

}


int main(int argc, char** argv )
{
  /////////////////////////
  // Simulation info
  /////////////////////////

  //std::cout << System::Environment::GetEnvironmentVariable("CELSLC_BIN_PATH");
  //std::cout << std::getenv("CELSLC_BIN_PATH");
  // Specifies the input super-cell file containing the atomic structure data in CIF file format.
  std::string super_cell_cif_file;
  // Specifies the output slice file name prefix.
  std::string slc_file_name_prefix;
  double projection_dir_h;
  double projection_dir_k;
  double projection_dir_l;
  double perpendicular_dir_u;
  double perpendicular_dir_v;
  double perpendicular_dir_w;
  double super_cell_size_x;
  double super_cell_size_y;
  double super_cell_size_z;
  int nx_simulated_horizontal_samples;
  int ny_simulated_vertical_samples;
  int nz_simulated_partitions;
  double ht_accelaration_voltage;

  // based on super_cell_size_x and nx_simulated_horizontal_samples
  double sampling_rate_super_cell_x_nm_pixel;
  // based on super_cell_size_y and ny_simulated_vertical_samples
  double sampling_rate_super_cell_y_nm_pixel;
  // based on super_cell_size_z and nz_simulated_partitions;
  double super_cell_z_nm_slice;

  // wavimg defocus aberration coefficient
  double coefficient_aberration_defocus;
  // wavimg spherical aberration coefficient
  double coefficient_aberration_spherical;

  int number_image_aberrations = 0;
  bool cd_switch = false;
  bool cs_switch = false;

  // Switch for applying Debye-Waller factors which effectively dampen the atomic scattering potentials.
  // Use this option for conventional HR-TEM, STEM bright-field, or STEM annular bright-field image simulations only.
  bool dwf_switch=false;

  // Switch for applying absorption potentials (imaginary part) according to Weickenmeier and Kohl [Acta Cryst. A47 (1991) p. 590-597].
  // This absorption calculation considers the loss of intensity in the elastic channel due to thermal diffuse scattering.
  bool abs_switch=false;

  bool celslc_switch = true;
  bool msa_switch = true;
  bool wavimg_switch = true;
  bool im2model_switch = true;
  bool debug_switch = false;
  bool roi_gui_switch = false;
  bool vis_gui_switch = false;
  bool sim_cmp_gui_switch = false;
  bool sim_grid_switch = false;
  bool phase_comparation_switch = false;
  bool user_estimated_defocus_nm_switch = false;
  bool user_estimated_thickness_nm_switch = false;
  bool user_estimated_thickness_slice_switch = false;

  /////////////////////////
  // Simulated Thickness info
  /////////////////////////
  int slices_load;
  int slice_samples;
  int slices_lower_bound;
  int slices_upper_bound;
  int number_slices_to_max_thickness;
  double slice_period;
  double user_estimated_thickness_nm;
  int user_estimated_thickness_slice;

  /////////////////////////
  // Simulated Defocus info
  /////////////////////////
  int defocus_samples;
  int defocus_lower_bound;
  int defocus_upper_bound;
  double defocus_period;
  int user_estimated_defocus_nm;

  /////////////////////////
  // Experimental Image info
  /////////////////////////
  int roi_pixel_size;
  int ignore_edge_pixels;
  std::string experimental_image_path;

  /////////////////////////
  // Simulated vs Experimental Image info
  /////////////////////////
  bool simulated_image_needs_reshape = false;
  double reshape_factor_from_supper_cell_to_experimental_x = 1.0f;
  double reshape_factor_from_supper_cell_to_experimental_y = 1.0f;
  double max_scale_diff = 0.0005f;
  double diff_super_cell_and_simulated_x;
  double diff_super_cell_and_simulated_y;
  // rectangle without the ignored edge pixels of the simulated image
  Rect ignore_edge_pixels_rectangle;

  int initial_simulated_image_width;
  int initial_simulated_image_height;
  int reshaped_simulated_image_width;
  int reshaped_simulated_image_height;

  MC::MC_Driver driver;

  try{
    /** Define and parse the program options
    */
    boost::program_options::options_description desc("Options");
    desc.add_options()
      ("help,h", "Print help message")
      ("no_celslc", "switch for skipping celslc execution.")
      ("no_msa", "switch for skipping msa execution.")
      ("no_wavimg", "switch for skipping wavimg execution.")
      ("no_im2model", "switch for skipping im2model execution.")
      ("debug,g", "switch for enabling debug info for celslc, msa, and wavimg execution.")

      ("cif", boost::program_options::value<std::string>(&super_cell_cif_file)->required(), "specifies the input super-cell file containing the atomic structure data in CIF file format.")
      ("slc", boost::program_options::value<std::string>(&slc_file_name_prefix)->required(), "specifies the output slice file name prefix. Absolute or relative path names can be used. Enclose the file name string using quotation marks if the file name prefix or the disk path contains space characters. The slice file names will be suffixed by '_###.sli', where ### is a 3 digit number denoting the sequence of slices generated from the supercell.")
      ("prj_h",  boost::program_options::value<double>(&projection_dir_h)->required(), "projection direction h of [hkl].")
      ("prj_k",  boost::program_options::value<double>(&projection_dir_k)->required(), "projection direction k of [hkl].")
      ("prj_l",  boost::program_options::value<double>(&projection_dir_l)->required(), "projection direction l of [hkl].")
      ("prp_u",  boost::program_options::value<double>(&perpendicular_dir_u)->required(), "perpendicular direction u for the new y-axis of the projection [uvw].")
      ("prp_v",  boost::program_options::value<double>(&perpendicular_dir_v)->required(), "perpendicular direction v for the new y-axis of the projection [uvw].")
      ("prp_w",  boost::program_options::value<double>(&perpendicular_dir_w)->required(), "perpendicular direction w for the new y-axis of the projection [uvw].")
      ("super_a",  boost::program_options::value<double>(&super_cell_size_x)->required(), "the size(in nanometers) of the new orthorhombic super-cell along the axis x.")
      ("super_b",  boost::program_options::value<double>(&super_cell_size_y)->required(), "the size(in nanometers) of the new orthorhombic super-cell along the axis y.")
      ("super_c",  boost::program_options::value<double>(&super_cell_size_z)->required(), "the size(in nanometers) of the new orthorhombic super-cell along the axis z, where z is the projection direction of the similation.")
      ("nx", boost::program_options::value<int>(&nx_simulated_horizontal_samples)->required(), "number of horizontal samples for the phase grating. The same number of pixels is used to sample the wave function in multislice calculations based on the calculated phase gratings.")
      ("ny", boost::program_options::value<int>(&ny_simulated_vertical_samples)->required(), "number of vertical samples for the phase grating. The same number of pixels is used to sample the wave function in multislice calculations based on the calculated phase gratings.")
      ("nz", boost::program_options::value<int>(&nz_simulated_partitions)->required(), "simulated partitions")
      ("ht", boost::program_options::value<double>(&ht_accelaration_voltage)->required(), "accelerating voltage defining the kinetic energy of the incident electron beam in kV.")
      ("cd", boost::program_options::value<double>(&coefficient_aberration_defocus), "Defocus Aberration definition by two coefficient values given in [nm]. This is the first coefficient value. The second coefficient value is considered to be 0.")
      ("cs", boost::program_options::value<double>(&coefficient_aberration_spherical), "Spherical Aberration definition by two coefficient values given in [nm]. This is the first coefficient value. The second coefficient value is considered to be 0.")
      ("dwf", "switch for applying Debye-Waller factors which effectively dampen the atomic scattering potentials. Use this option for conventional HR-TEM, STEM bright-field, or STEM annular bright-field image simulations only.")
      ("abs", "switch for applying absorption potentials (imaginary part) according to Weickenmeier and Kohl [Acta Cryst. A47 (1991) p. 590-597]. This absorption calculation considers the loss of intensity in the elastic channel due to thermal diffuse scattering.")
      ("slices_load", boost::program_options::value<int>(&slices_load), "number of slice files to be loaded.")
      ("slices_samples", boost::program_options::value<int>(&slice_samples)->required(), "slices samples")
      ("slices_lower_bound", boost::program_options::value<int>(&slices_lower_bound)->required(), "slices lower bound")
      ("slices_upper_bound", boost::program_options::value<int>(&slices_upper_bound)->required(), "slices Upper Bound")
      ("slices_max", boost::program_options::value<int>(&number_slices_to_max_thickness), "number of slices used to describe the full object structure up to its maximum thickness.")
      ("defocus_samples", boost::program_options::value<int>(&defocus_samples)->required(), "defocus samples")
      ("defocus_lower_bound", boost::program_options::value<int>(&defocus_lower_bound)->required(), "defocus lower bound")
      ("defocus_upper_bound", boost::program_options::value<int>(&defocus_upper_bound)->required(), "defocus upper bound")
      ("exp_image_path,i", boost::program_options::value<std::string>(&experimental_image_path)->required(), "experimental image path")
      ("estimated_defocus_nm", boost::program_options::value<int>(&user_estimated_defocus_nm), "user prediction of estimated defocus value in nanometers for the experimental image. If an user estimation is given the search algorithm will consider the nearest best match as an interest point even if there are larger match values on other defocus values. The other larger defocus value points will also be considered points of interest for the following iterations (as usual).")
      ("estimated_thickness_nm", boost::program_options::value<double>(&user_estimated_thickness_nm), "user prediction of estimated thickness value in nanometers for the experimental image. An aproximation to the nearest slice number is computed. If an user estimation is given the search algorithm will consider the nearest best match as an interest point even if there are larger match values on other thickness values. The other larger thickness points will also be considered points of interest for the following iterations (as usual).")
      ("estimated_thickness_slice", boost::program_options::value<int>(&user_estimated_thickness_slice), "user prediction of estimated thickness value in slice number for the experimental image. If an user estimation is given the search algorithm will consider the nearest best match as an interest point even if there are larger match values on other thickness values. The other larger thickness points will also be considered points of interest for the following iterations (as usual).")
      ("exp_nx", boost::program_options::value<double>(&sampling_rate_experimental_x_nm_per_pixel)->required(), "number of horizontal samples for the loaded experimental image (nm/pixel).")
      ("exp_ny", boost::program_options::value<double>(&sampling_rate_experimental_y_nm_per_pixel)->required(), "number of vertical samples for the loaded experimental image (nm/pixel).")
      ("roi_size", boost::program_options::value<int>(&roi_pixel_size)->required(), "region of interest size in pixels.")
      ("roi_x", boost::program_options::value<int>(&roi_center_x)->required(), "region center in x axis.")
      ("roi_y", boost::program_options::value<int>(&roi_center_y)->required(), "region center in y axis.")
      ("roi_gui", "switch for enabling gui region of interest selection prior to im2model execution.")
      ("vis_gui", "switch for enabling supercell visualization.")
      ("sim_cmp_gui", "switch for enabling gui im2model simullated and experimental comparation visualization.")
      ("sim_grid", "switch for enable simmulated image grid generation.")

      ("ignore_edge_pixels", boost::program_options::value<int>(&ignore_edge_pixels)->default_value(0), "number of pixels to ignore from the outter limit of the simulated image.")
      ;

    boost::program_options::variables_map vm;
    try
    {
      boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc),vm); // can throw

      /** --help option
      */
      if ( vm.count("help")  )
      {
        std::cout << "\n\n********************************************************************************\n\n" <<
          "im2model -- Atomic models for TEM image simulation and matching\n"
          <<

          "Command Line Parameters:" << std::endl
          << desc << std::endl;
        return 0;
      }

      if ( vm.count("cd") ){
        cd_switch = true;
        number_image_aberrations++;
      }

      if ( vm.count("cs") ){
        number_image_aberrations++;
        cs_switch = true;
      }
      if ( vm.count("dwf")  ){
        dwf_switch=true;
      }
      if ( vm.count("abs")  ){
        abs_switch=true;
      }
      if ( vm.count("no_celslc")  ){
        celslc_switch=false;
      }
      if ( vm.count("no_msa")  ){
        msa_switch=false;
      }
      if ( vm.count("no_wavimg")  ){
        wavimg_switch=false;
      }
      if ( vm.count("no_im2model")  ){
        im2model_switch=false;
      }
      if ( vm.count("debug")  ){
        debug_switch=true;
      }
      if ( vm.count("roi_gui")  ){
        roi_gui_switch=true;
      }
      if ( vm.count("vis_gui")  ){
        vis_gui_switch=true;
      }
      if ( vm.count("sim_cmp_gui")  ){
        sim_cmp_gui_switch=true;
      }
      if ( vm.count("sim_grid")  ){
        sim_grid_switch=true;
      }
      if ( vm.count("estimated_defocus_nm")  ){
        user_estimated_defocus_nm_switch=true;
      }
      if ( vm.count("estimated_thickness_nm")  ){
        user_estimated_thickness_nm_switch=true;
      }
      if ( vm.count("estimated_thickness_slice")  ){
        user_estimated_thickness_slice_switch=true;
      }

      boost::program_options::notify(vm); // throws on error, so do after help in case
      // there are any problems
    }
    catch(boost::program_options::error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      std::cerr << desc << std::endl;
      return -1;
    }

    /* CIF file parser */
    driver.parse( super_cell_cif_file.c_str() );
    driver.populate_unit_cell();
    driver.populate_atom_site_unit_cell();
    driver.populate_symetry_equiv_pos_as_xyz_unit_cell();
    unit_cell = driver.get_unit_cell();

    zone_axis_vector_uvw = cv::Point3d( perpendicular_dir_u, perpendicular_dir_v , perpendicular_dir_w );
    upward_vector_hkl = cv::Point3d( projection_dir_h, projection_dir_k , projection_dir_l );
    unit_cell.set_zone_axis_vector( zone_axis_vector_uvw );
    unit_cell.set_upward_vector( upward_vector_hkl );
    unit_cell.form_matrix_from_miller_indices(); 

    // Simulated image sampling rate
    sampling_rate_super_cell_x_nm_pixel = super_cell_size_x / nx_simulated_horizontal_samples;
    sampling_rate_super_cell_y_nm_pixel = super_cell_size_y / ny_simulated_vertical_samples;
    super_cell_z_nm_slice = super_cell_size_z / nz_simulated_partitions;
    diff_super_cell_and_simulated_x = fabs(sampling_rate_super_cell_x_nm_pixel - sampling_rate_experimental_x_nm_per_pixel);
    diff_super_cell_and_simulated_y = fabs(sampling_rate_super_cell_y_nm_pixel - sampling_rate_experimental_x_nm_per_pixel);

    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.

    std::cout << "sampling rate simulated [ " << sampling_rate_super_cell_x_nm_pixel << " , " << sampling_rate_super_cell_y_nm_pixel << " ]" << std::endl;

    std::cout << "sampling rate experimental [ " << sampling_rate_experimental_x_nm_per_pixel << " , " << sampling_rate_experimental_y_nm_per_pixel << " ]" << std::endl;

    initial_simulated_image_width = nx_simulated_horizontal_samples - ( 2 * ignore_edge_pixels );
    initial_simulated_image_height = nx_simulated_horizontal_samples - ( 2 * ignore_edge_pixels );
    reshaped_simulated_image_width = initial_simulated_image_width;
    reshaped_simulated_image_height = initial_simulated_image_height;

    if (( diff_super_cell_and_simulated_x >= max_scale_diff ) ||  (diff_super_cell_and_simulated_y >= max_scale_diff )){
      simulated_image_needs_reshape = true;
      reshape_factor_from_supper_cell_to_experimental_x = fabs(sampling_rate_super_cell_x_nm_pixel) / fabs(sampling_rate_experimental_x_nm_per_pixel);
      reshape_factor_from_supper_cell_to_experimental_y = fabs(sampling_rate_super_cell_y_nm_pixel) / fabs(sampling_rate_experimental_y_nm_per_pixel);

      reshaped_simulated_image_width = (int) round ( reshape_factor_from_supper_cell_to_experimental_x * initial_simulated_image_width );
      reshaped_simulated_image_height = (int) round ( reshape_factor_from_supper_cell_to_experimental_y * initial_simulated_image_height );

      std::cout << "WARNING : simulated and experimental images have different sampling rates"
        << ". Reshaping simulated images by a factor of [ "
        << reshape_factor_from_supper_cell_to_experimental_x << " , " << reshape_factor_from_supper_cell_to_experimental_y
        << " ]" << "\n\tThe simulated image portion of size [" << initial_simulated_image_width << "," << initial_simulated_image_height
        << "]" << " will be resized to ["<< reshaped_simulated_image_width << "," << reshaped_simulated_image_height << "]"
        << std::endl;
    }

    ignore_edge_pixels_rectangle.x = ignore_edge_pixels;
    ignore_edge_pixels_rectangle.y = ignore_edge_pixels;
    ignore_edge_pixels_rectangle.width = initial_simulated_image_width;
    ignore_edge_pixels_rectangle.height = initial_simulated_image_height;

    std::cout << "Defined nz: " << nz_simulated_partitions << " supercell size y " << super_cell_size_y << " sampling_rate_super_cell_y_nm_pixel: " << sampling_rate_super_cell_y_nm_pixel << std::endl;


    // Simulation Thickness Period (in slices)
    slice_period =  ( ( ((double)slices_upper_bound - (double)slices_lower_bound) ) / ((double)slice_samples -1.0f ) );
    std::cout << "Calculated slice period of " << slice_period << std::endl;

    if (slices_lower_bound == 0){
      std::cout << "WARNING: Defined slice lower bound as 0. Going to define slice lower bound as: " << slice_period << std::endl;
      slices_lower_bound = (int) slice_period;
    }

    // Simulation Defocus Period (in nm)
    defocus_period = ( defocus_upper_bound - defocus_lower_bound) / ( defocus_samples - 1 );
    std::cout << "defocus period " <<  defocus_period << std::endl;

    if (celslc_switch == true ){
      CELSLC_prm::CELSLC_prm celslc_parameters;
      celslc_parameters.set_prj_dir_h(projection_dir_h);
      celslc_parameters.set_prj_dir_k(projection_dir_k);
      celslc_parameters.set_prj_dir_l(projection_dir_l);
      celslc_parameters.set_prp_dir_u(perpendicular_dir_u);
      celslc_parameters.set_prp_dir_v(perpendicular_dir_v);
      celslc_parameters.set_prp_dir_w(perpendicular_dir_w);
      celslc_parameters.set_super_cell_size_x(super_cell_size_x);
      celslc_parameters.set_super_cell_size_y(super_cell_size_y);
      celslc_parameters.set_super_cell_size_z(super_cell_size_z);
      celslc_parameters.set_cif_file(super_cell_cif_file.c_str());
      celslc_parameters.set_slc_filename_prefix (slc_file_name_prefix.c_str());
      celslc_parameters.set_nx_simulated_horizontal_samples(nx_simulated_horizontal_samples);
      celslc_parameters.set_ny_simulated_vertical_samples(ny_simulated_vertical_samples);
      celslc_parameters.set_nz_simulated_partitions(nz_simulated_partitions);
      celslc_parameters.set_ht_accelaration_voltage(ht_accelaration_voltage);
      celslc_parameters.set_dwf_switch(dwf_switch);
      celslc_parameters.set_abs_switch(abs_switch);
      celslc_parameters.set_bin_path( celslc_bin_string );
      celslc_parameters.call_bin();
    }

    if( msa_switch == true ){
      MSA_prm::MSA_prm msa_parameters;
      // Since the release of MSA version 0.64 you may alternatively specify the electron energy in keV in line 6
      msa_parameters.set_electron_wavelength( ht_accelaration_voltage ); //0.00196875 );
      msa_parameters.set_internal_repeat_factor_of_super_cell_along_x ( 1 );
      msa_parameters.set_internal_repeat_factor_of_super_cell_along_y ( 1 );
      std::stringstream input_prefix_stream;
      input_prefix_stream << "'" << slc_file_name_prefix << "'";
      std::string input_prefix_string = input_prefix_stream.str();
      msa_parameters.set_slice_filename_prefix ( input_prefix_string );
      msa_parameters.set_number_slices_to_load ( slices_load );
      msa_parameters.set_number_frozen_lattice_variants_considered_per_slice( 1 );
      msa_parameters.set_minimum_number_frozen_phonon_configurations_used_generate_wave_functions ( 1 );
      msa_parameters.set_period_readout_or_detection_in_units_of_slices ( 1 ); // bug
      msa_parameters.set_number_slices_used_describe_full_object_structure_up_to_its_maximum_thickness ( number_slices_to_max_thickness );
      msa_parameters.set_linear_slices_for_full_object_structure();
      msa_parameters.set_prm_file_name("temporary_msa_im2model.prm");
      msa_parameters.produce_prm();
      msa_parameters.set_wave_function_name ("wave.wav");
      msa_parameters.set_bin_path( msa_bin_string );
      msa_parameters.set_debug_switch(debug_switch);
      msa_parameters.call_bin();
    }

    if(wavimg_switch == true ){
      WAVIMG_prm::WAVIMG_prm wavimg_parameters;

      std::string wave_function_name =  "'wave_sl.wav'";
      std::string wavimg_prm_name = "temporary_wavimg_im2model.prm";
      std::string file_name_output_image_wave_function = "'image.dat'";
      // setters line 1
      wavimg_parameters.set_file_name_input_wave_function( wave_function_name );
      // setters line 2
      wavimg_parameters.set_n_columns_samples_input_wave_function_pixels( ny_simulated_vertical_samples );
      wavimg_parameters.set_n_rows_samples_input_wave_function_pixels( nx_simulated_horizontal_samples );
      // setters line 3
      wavimg_parameters.set_physical_columns_sampling_rate_input_wave_function_nm_pixels( sampling_rate_super_cell_x_nm_pixel );
      wavimg_parameters.set_physical_rows_sampling_rate_input_wave_function_nm_pixels( sampling_rate_super_cell_y_nm_pixel );
      // setters line 4
      wavimg_parameters.set_primary_electron_energy( ht_accelaration_voltage );
      // setters line 5
      wavimg_parameters.set_type_of_output( 0 );
      // setters line 6
      wavimg_parameters.set_file_name_output_image_wave_function( file_name_output_image_wave_function );
      // setters line 7
      wavimg_parameters.set_n_columns_samples_output_image( ny_simulated_vertical_samples );
      wavimg_parameters.set_n_rows_samples_output_image( nx_simulated_horizontal_samples );
      // setters line 8
      wavimg_parameters.set_image_data_type( 0 );
      wavimg_parameters.set_image_vacuum_mean_intensity( 3000.0f );
      wavimg_parameters.set_conversion_rate( 1.0f );
      wavimg_parameters.set_readout_noise_rms_amplitude( 0.0f ); // colocar a zero
      // setters line 9
      wavimg_parameters.set_switch_option_extract_particular_image_frame( 1 );
      // setters line 10
      wavimg_parameters.set_image_sampling_rate_nm_pixel( sampling_rate_super_cell_x_nm_pixel );
      // setters line 11
      wavimg_parameters.set_image_frame_offset_x_pixels_input_wave_function( 0.0f );
      wavimg_parameters.set_image_frame_offset_y_pixels_input_wave_function( 0.0f );
      // setters line 12
      wavimg_parameters.set_image_frame_rotation( 0.0f );
      // setters line 13
      wavimg_parameters.set_switch_coherence_model( 1 ); // colocar a zero
      // setters line 14
      wavimg_parameters.set_partial_temporal_coherence_switch( 1 );
      wavimg_parameters.set_partial_temporal_coherence_focus_spread( 4.0f );
      // setters line 15
      wavimg_parameters.set_partial_spacial_coherence_switch( 1 ); // colocar a zero
      wavimg_parameters.set_partial_spacial_coherence_semi_convergence_angle( 0.2f );
      // setters line 16
      wavimg_parameters.set_mtf_simulation_switch( 1 ); // alterar aqui para 0
      wavimg_parameters.set_k_space_scaling( 1.0f );
      wavimg_parameters.set_file_name_simulation_frequency_modulated_detector_transfer_function( "'../simulation/mtf/MTF-US2k-300.mtf'" );
      // setters line 17
      wavimg_parameters.set_simulation_image_spread_envelope_switch( 0 );
      wavimg_parameters.set_isotropic_one_rms_amplitude( 0.03 ); // colocar a zero
      //  wavimg_parameters.set_anisotropic_second_rms_amplitude( 0.0f );
      // wavimg_parameters.set_azimuth_orientation_angle( 0.0f );
      // setters line 18
      wavimg_parameters.set_number_image_aberrations_set( number_image_aberrations );
      // setters line 19
      // check for wavimg defocus aberration coefficient
      if( cd_switch == true ){
        //Defocus (a20, C1,0, C1)
        wavimg_parameters.add_aberration_definition ( 1, coefficient_aberration_defocus, 0.0f );
      }
      // check for wavimg spherical aberration coefficient
      if( cs_switch == true ){
        //Spherical aberration (a40, C3,0, C3)
        wavimg_parameters.add_aberration_definition ( 5, coefficient_aberration_spherical, 0.0f );
      }
      // setters line 19 + aberration_definition_index_number
      wavimg_parameters.set_objective_aperture_radius( 5500.0f );
      // setters line 20 + aberration_definition_index_number
      wavimg_parameters.set_center_x_of_objective_aperture( 0.0f );
      wavimg_parameters.set_center_y_of_objective_aperture( 0.0f );
      // setters line 21 + aberration_definition_index_number
      wavimg_parameters.set_number_parameter_loops( 2 );
      wavimg_parameters.add_parameter_loop ( 1 , 1 , 1, defocus_lower_bound, defocus_upper_bound, defocus_samples, "'foc'" );
      wavimg_parameters.add_parameter_loop ( 3 , 1 , 1, slices_lower_bound, slices_upper_bound, slice_samples, "'_sl'" );
      wavimg_parameters.set_prm_file_name("temporary_wavimg_im2model.prm");
      wavimg_parameters.produce_prm();
      wavimg_parameters.set_bin_path( wavimg_bin_string );
      wavimg_parameters.set_debug_switch(debug_switch);
      wavimg_parameters.call_bin();
    }

    if (im2model_switch == true ){

      // Read the experimental image from file
      experimental_image = imread( experimental_image_path , CV_LOAD_IMAGE_GRAYSCALE );

      // initialize your temp images
      experimental_working = experimental_image.clone();

      //if fail to read the image
      if ( experimental_image.empty() ){
        std::cout << "Error loading the experimental image from: " << experimental_image_path << std::endl;
        return -1;
      }

      // Experimental image info
      // experimental image roi
      roi_x_size = roi_pixel_size;
      roi_y_size = roi_pixel_size;

      std::cout << "Experimental image width : " << experimental_image.cols << "px , heigth : " << experimental_image.rows << "px "<<  std::endl;
      roi_rectangle.x = roi_center_x  - ( roi_x_size / 2 );
      roi_rectangle.y = roi_center_y - ( roi_y_size / 2) ;
      roi_rectangle.width = roi_x_size;
      roi_rectangle.height = roi_y_size;
      experimental_working = experimental_image.clone();
      //copy the roi
      cv::Mat roi = experimental_image(roi_rectangle);
      experimental_image_roi.create( roi_x_size, roi_y_size , CV_8UC1 );
      roi.copyTo(experimental_image_roi);

      if ( roi_gui_switch == true ){
        //Create a window
        namedWindow("Experimental Window", WINDOW_AUTOSIZE );
        setMouseCallback("Experimental Window", CallBackFunc, NULL);
        rectangle(experimental_working, roi_rectangle, Scalar(0,0,255), 5);
        imshow("Experimental Window", experimental_working);
        createTrackbar( " Canny thresh:", "Experimental Window", &edge_detection_threshold, max_thresh, thresh_callback );
        createTrackbar( "Max Countour px distance: ", "Experimental Window", &max_contour_distance_px, max_contour_distance_thresh_px, thresh_callback );
        waitKey(0);
      }

      /////////////////////////////////////////////////
      // STEP LENGTH SIMULATION PROCESS STARTS HERE  //
      /////////////////////////////////////////////////

      std::cout << "Starting step length" << std::endl;
      SIMGRID_wavimg_steplength::SIMGRID_wavimg_steplength wavimg_simgrid_steps;

      wavimg_simgrid_steps.set_sampling_rate_super_cell_x_nm_pixel( sampling_rate_super_cell_x_nm_pixel );
      wavimg_simgrid_steps.set_sampling_rate_super_cell_y_nm_pixel( sampling_rate_super_cell_y_nm_pixel );
      wavimg_simgrid_steps.set_experimental_image_roi( experimental_image_roi );

      // defocus setters
      wavimg_simgrid_steps.set_defocus_lower_bound( defocus_lower_bound );
      wavimg_simgrid_steps.set_defocus_upper_bound( defocus_upper_bound );
      wavimg_simgrid_steps.set_defocus_samples(defocus_samples);
      wavimg_simgrid_steps.set_defocus_period( (int) defocus_period );

      // thicknes/slice setters
      wavimg_simgrid_steps.set_super_cell_z_nm_slice( super_cell_z_nm_slice );
      wavimg_simgrid_steps.set_slice_samples(slice_samples);
      wavimg_simgrid_steps.set_slice_period( (int) slice_period );
      wavimg_simgrid_steps.set_slices_lower_bound( slices_lower_bound );
      wavimg_simgrid_steps.set_slices_upper_bound( slices_upper_bound );

      wavimg_simgrid_steps.set_n_rows_simulated_image(nx_simulated_horizontal_samples);
      wavimg_simgrid_steps.set_n_cols_simulated_image(ny_simulated_vertical_samples);

      wavimg_simgrid_steps.set_reshaped_simulated_image_width(reshaped_simulated_image_width);
      wavimg_simgrid_steps.set_reshaped_simulated_image_height(reshaped_simulated_image_height);

      wavimg_simgrid_steps.set_ignore_edge_pixels_rectangle( ignore_edge_pixels_rectangle );
      wavimg_simgrid_steps.set_simulated_image_needs_reshape( simulated_image_needs_reshape );
      wavimg_simgrid_steps.set_reshape_factor_from_supper_cell_to_experimental_x (reshape_factor_from_supper_cell_to_experimental_x);
      wavimg_simgrid_steps.set_reshape_factor_from_supper_cell_to_experimental_y (reshape_factor_from_supper_cell_to_experimental_y);
      wavimg_simgrid_steps.set_sim_grid_switch(sim_grid_switch);
      wavimg_simgrid_steps.set_debug_switch(debug_switch);

      // user estimation of defocus and thickness
      if ( user_estimated_defocus_nm_switch ){
        wavimg_simgrid_steps.set_user_estimated_defocus_nm_switch( user_estimated_defocus_nm_switch );
        wavimg_simgrid_steps.set_user_estimated_defocus_nm( user_estimated_defocus_nm );
      }

      if ( user_estimated_thickness_nm_switch ){
        wavimg_simgrid_steps.set_user_estimated_thickness_nm_switch( user_estimated_thickness_nm_switch );
        wavimg_simgrid_steps.set_user_estimated_thickness_nm( user_estimated_thickness_nm );
      }

      if ( user_estimated_thickness_slice_switch ){
        wavimg_simgrid_steps.set_user_estimated_thickness_slice_switch( user_estimated_thickness_slice_switch );
        wavimg_simgrid_steps.set_user_estimated_thickness_slice( user_estimated_thickness_slice );
      }

      wavimg_simgrid_steps.set_iteration_number(1);
      wavimg_simgrid_steps.set_step_length_minimum_threshold ( 87.5f );
      wavimg_simgrid_steps.set_step_size( (int) defocus_period, (int) slice_period );
      std::cout << "Simulating from dat " << std::endl;
      wavimg_simgrid_steps.simulate_from_dat_file();
      std::cout << "Thresh " << std::endl;

      //thresh_callback( 1 , nullptr );

      /*      std::cout << "Super cell min width (pixels) " << supercell_min_width << std::endl;
              std::cout << "Super cell min height (pixels) " << supercell_min_height << std::endl;

              const double x_supercell_min_size_nm = supercell_min_width * sampling_rate_super_cell_x_nm_pixel;
              const double y_supercell_min_size_nm = supercell_min_height * sampling_rate_super_cell_y_nm_pixel;
              const double z_supercell_min_size_nm = wavimg_simgrid_steps.get_simgrid_best_match_thickness_nm();
              */
      unit_cell.create_atoms_from_site_and_symetry();
      super_cell = Super_Cell::Super_Cell( &unit_cell ); 
      super_cell.set_experimental_image( experimental_image );
      super_cell.calculate_supercell_boundaries_from_experimental_image( cv::Point2f(roi_center_x, roi_center_y), edge_detection_threshold , max_contour_distance_px );
      super_cell.set_sampling_rate_super_cell_x_nm_pixel( sampling_rate_super_cell_x_nm_pixel );
      super_cell.set_sampling_rate_super_cell_y_nm_pixel( sampling_rate_super_cell_y_nm_pixel );
      super_cell.set_simgrid_best_match_thickness_nm( wavimg_simgrid_steps.get_simgrid_best_match_thickness_nm() );
      super_cell.calculate_experimental_min_size_nm(); 
      super_cell.calculate_expand_factor();
      super_cell.create_atoms_from_unit_cell();
      super_cell.orientate_atoms_from_matrix();
      std::cout << "Supercell created" << std::endl;
      //wavimg_simgrid_steps.export_sim_grid();
    }

    if( vis_gui_switch ){
      /* VIS */
      AppVis();
    }

  }
  catch(std::exception& e){
    std::cerr << "Unhandled Exception reached the top of main: "
      << e.what() << ", application will now exit" << std::endl; 
    return -1;
  }
  return 0;
}

