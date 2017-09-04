#include "NGLScene.h"
#include <QGuiApplication>
#include <QMouseEvent>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include "SceneData.h"

NGLScene::NGLScene()
{
  setTitle( "Qt5 Simple NGLGLM Demo" );
}


NGLScene::~NGLScene()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL( int _w, int _h )
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
}


void NGLScene::initializeGL()
{
  // we must call that first before any other GL commands to load and link the
  // gl commands from the lib, if that is not done program will crash
  nglglm::NGLInit::instance();
  glClearColor( 0.4f, 0.4f, 0.4f, 1.0f ); // Grey Background
  // enable depth testing for drawing
  glEnable( GL_DEPTH_TEST );
// enable multisampling for smoother drawing
#ifndef USINGIOS_
  glEnable( GL_MULTISAMPLE );
#endif
  // now to load the shader and set the values
  // grab an instance of shader manager
  nglglm::ShaderLib* shader = nglglm::ShaderLib::instance();
  // we are creating a shader called Phong to save typos
  // in the code create some constexpr
  constexpr auto shaderProgram = "Phong";
  constexpr auto vertexShader  = "PhongVertex";
  constexpr auto fragShader    = "PhongFragment";
  // create the shader program
  shader->createShaderProgram( shaderProgram );
  // now we are going to create empty shaders for Frag and Vert
  shader->attachShader( vertexShader, nglglm::ShaderType::VERTEX );
  shader->attachShader( fragShader, nglglm::ShaderType::FRAGMENT );
  // attach the source
  shader->loadShaderSource( vertexShader, "shaders/PhongVertex.glsl" );
  shader->loadShaderSource( fragShader, "shaders/PhongFragment.glsl" );
  // compile the shaders
  shader->compileShader( vertexShader );
  shader->compileShader( fragShader );
  // add them to the program
  shader->attachShaderToProgram( shaderProgram, vertexShader );
  shader->attachShaderToProgram( shaderProgram, fragShader );


  // now we have associated that data we can link the shader
  shader->linkProgramObject( shaderProgram );
  // and make it active ready to load values
  ( *shader )[ shaderProgram ]->use();

  shader->setUniform("light.position",glm::vec4(0.0f,1.0f,0.0f,0.0f));
  shader->setUniform("light.ambient",glm::vec4(0.1f,0.1f,0.1f,1.0f));
  shader->setUniform("light.diffuse",glm::vec4(1.0f,1.0f,1.0f,1.0f));
  shader->setUniform("light.specular",glm::vec4(1.0f,1.0f,1.0f,1.0f));

  shader->setUniform("material.ambient",glm::vec4(0.274725f,0.1995f,0.0745f,1.0f));
  shader->setUniform("material.diffuse",glm::vec4(0.75164f,0.60648f,0.22648f,1.0f));
  shader->setUniform("material.specular",glm::vec4(0.628281f,0.555802f,0.3666065f,1.0f));
  shader->setUniform("material.shininess",51.2f);

  shader->setUniform("viewerPos",glm::vec3(2,2,2));
}


void NGLScene::loadMatricesToShader()
{
  nglglm::ShaderLib* shader = nglglm::ShaderLib::instance();

  glm::mat4 MV;
  glm::mat4 MVP;
  glm::mat3 normalMatrix;
  glm::mat4 M;
  M            = m_mouseGlobalTX;
  MV           = glm::lookAt(glm::vec3(2,2,2),glm::vec3(0,0,0),glm::vec3(0,1,0)) * M;
  MVP          = glm::perspective(45.0f,(float)m_win.width/m_win.height,0.1f,100.0f) * MV;
  normalMatrix=glm::inverseTranspose(glm::mat3(MV));

  shader->setUniform( "MV", MV );
  shader->setUniform( "MVP", MVP );
  shader->setUniform( "normalMatrix", normalMatrix );
  shader->setUniform( "M", M );
}

void NGLScene::paintGL()
{
  glViewport( 0, 0, m_win.width, m_win.height );
  // clear the screen and depth buffer
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // grab an instance of the shader manager
  nglglm::ShaderLib* shader = nglglm::ShaderLib::instance();
  ( *shader )[ "Phong" ]->use();

  // Rotation based on the mouse position for our global transform
  glm::mat4 rotX=nglglm::rotateX(m_win.spinXFace);
  glm::mat4 rotY=nglglm::rotateY(m_win.spinYFace);

  // create the rotation matrices
  // multiply the rotations
  m_mouseGlobalTX =  rotX * rotY;
  // add the translations
  m_mouseGlobalTX[ 3 ][ 0 ] = m_modelPos.x;
  m_mouseGlobalTX[ 3 ][ 1 ] = m_modelPos.y;
  m_mouseGlobalTX[ 3 ][ 2 ] = m_modelPos.z;

  // get the VBO instance and draw the built in teapot
  nglglm::VAOPrimitives* prim = nglglm::VAOPrimitives::instance();
  // draw
  loadMatricesToShader();
  prim->draw( "teapot" );
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent( QKeyEvent* _event )
{
  // that method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch ( _event->key() )
  {
    // escape key to quit
    case Qt::Key_Escape:
      QGuiApplication::exit( EXIT_SUCCESS );
      break;
// turn on wirframe rendering
#ifndef USINGIOS_
    case Qt::Key_W:
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      break;
    // turn off wire frame
    case Qt::Key_S:
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
      break;
#endif
    // show full screen
    case Qt::Key_F:
      showFullScreen();
      break;
    // show windowed
    case Qt::Key_N:
      showNormal();
      break;
    case Qt::Key_Space :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      //m_modelPos.set(nglglm::Vec3::zero());
    break;
    default:
      break;
  }
  update();
}
