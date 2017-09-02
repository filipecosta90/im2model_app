#include "unit_cell_view_widget.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QQuickItem>

UnitCellViewerWindow::UnitCellViewerWindow(QWidget *parent) : QWidget(parent) {

  toolsLayout = new QBoxLayout(QBoxLayout::TopToBottom,this);
  //set margins to zero so the toolbar touches the widget's edges
  toolsLayout->setContentsMargins(0, 0, 0, 0);

  toolbar = new QToolBar;
  toolbar->addAction("ZA | UV" , this, SLOT(view_along_ZA_UV()) );
  toolbar->addAction("a" , this, SLOT(view_along_a_axis()) );
  toolbar->addAction("b" , this, SLOT(view_along_b_axis()) );
  toolbar->addAction("c" ,this, SLOT(view_along_c_axis()) );
  toolbar->addAction("Fit", this, SLOT(update_m_cameraEntity_centerDistance()) );
  toolsLayout->addWidget(toolbar);

  qt_scene_view = new Qt3DExtras::Qt3DWindow( );
  Qt3DInput::QInputAspect *input = new Qt3DInput::QInputAspect;
  qt_scene_view->registerAspect(input);
  container = QWidget::createWindowContainer( qt_scene_view );

  toolsLayout->addWidget(container);
  toolsLayout->setStretchFactor(container, 8);
  m_containerLayout = new QVBoxLayout;
  container->setLayout(m_containerLayout);
  setLayout(toolsLayout);
  init();
}

void UnitCellViewerWindow::set_super_cell( SuperCell* cell ){
  super_cell = cell;
  qt_scene_super_cell->set_super_cell( super_cell );
  QObject::connect( super_cell, SIGNAL(atom_positions_changed()), qt_scene_super_cell, SLOT(reload_data_from_super_cell()));
  QObject::connect( super_cell, SIGNAL(atom_positions_changed()), this, SLOT(update_m_cameraEntity_centerDistance()));
  QObject::connect( super_cell, SIGNAL(orientation_matrix_changed()), qt_scene_super_cell, SLOT(orientate_data_from_super_cell()));

  QObject::connect( super_cell, SIGNAL(zone_axis_vector_changed()), this, SLOT(update_cameraEntity_zone_axis()) );
  QObject::connect( super_cell, SIGNAL(upward_vector_changed()), this, SLOT(update_cameraEntity_upward_vector()) );

  _flag_super_cell = true;
}

bool UnitCellViewerWindow::add_image_layer( cv::Mat layer_image ){
  return qt_scene_super_cell->add_image_layer( layer_image );
}

void UnitCellViewerWindow::init(){

  _m_rootEntity = new Qt3DCore::QEntity();
  _m_rootEntity->setObjectName(QStringLiteral("rootEntity"));

  // Camera
  _m_cameraEntity = qt_scene_view->camera();
  int view_size = 200;
  double aspect_ratio = 16.0f/9.0f;
  _m_cameraEntity->lens()->setOrthographicProjection(-aspect_ratio*view_size/2.0f, aspect_ratio*view_size/2.0f, -view_size/2.0f, view_size/2.0f, 0.1f, 1000.0f);
  _m_cameraEntity->setProjectionType(Qt3DRender::QCameraLens::OrthographicProjection);

  _m_cameraEntity->setPosition(QVector3D(0, 0, _m_cameraEntity_centerDistance));
  _m_cameraEntity->setUpVector(QVector3D(0, 1, 0));
  _m_cameraEntity->setViewCenter(QVector3D(0, 0, 0));

//  _m_cameraEntity->lens()->setPerspectiveProjection(50.0f, 16.0f/9.0f, 0.1f, 1000.0f);

  // Scene SuperCell for TDMAP ROI
  qt_scene_super_cell = new QtSceneSuperCell( _m_rootEntity, _m_cameraEntity );

  Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity( _m_rootEntity );
  Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
  light->setColor("white");
  light->setIntensity(1);
  lightEntity->addComponent(light);

  _m_lightTransform = new Qt3DCore::QTransform( lightEntity );
  _m_lightTransform->setTranslation( _m_cameraEntity->position() );
  lightEntity->addComponent( _m_lightTransform );

  // FrameGraph
  Qt3DRender::QFrameGraphNode* activeFrameGraph = qt_scene_view->activeFrameGraph();
  QList< QObject* > childrens = activeFrameGraph->children();
  QList< Qt3DRender::QViewport* > childrens_views = activeFrameGraph->findChildren<Qt3DRender::QViewport *>();

  Qt3DRender::QViewport *mainViewport = childrens_views[0];

  Qt3DRender::QViewport *topLeftViewport = new Qt3DRender::QViewport( );
  Qt3DRender::QLayerFilter *layerFilter = new Qt3DRender::QLayerFilter( topLeftViewport );
  Qt3DRender::QLayer *xyz_scene_layer = qt_scene_super_cell->get_xyz_axis_layer();
  QList< QObject* > childrens_xyz_scene_layer = xyz_scene_layer->children();
  std::cout << "childrens_xyz_scene_layer.size() " << childrens_xyz_scene_layer.size() << std::endl;
  layerFilter->addLayer( xyz_scene_layer );

  topLeftViewport->setNormalizedRect(QRectF(0, 0.8, 0.2, 0.2));
  Qt3DRender::QCameraSelector *cameraSelector = new Qt3DRender::QCameraSelector(topLeftViewport);
  Qt3DRender::QCamera *cameraEntitytopLeft = new Qt3DRender::QCamera( _m_cameraEntity );
  cameraSelector->setCamera( cameraEntitytopLeft );

  // For camera controls
  Qt3DExtras::QTrackballCameraController *camController = new Qt3DExtras::QTrackballCameraController(mainViewport);
 camController->setCamera( _m_cameraEntity );

  // Set root object of the scene
  qt_scene_view->setRootEntity( _m_rootEntity );
}

void UnitCellViewerWindow::update_m_cameraEntity_centerDistance(){
  if( _flag_super_cell ){
    _m_cameraEntity_centerDistance = 30 * super_cell->get_max_length_abc_Nanometers();
    QVector3D at_view = _m_cameraEntity->viewVector();
    at_view.normalize();
    at_view*= _m_cameraEntity_centerDistance;
    _m_lightTransform->setTranslation(-at_view);
    _m_cameraEntity->setPosition(-at_view);
  }
}

void UnitCellViewerWindow::update_cameraEntity_zone_axis(){
  if( _flag_super_cell ){
    if( super_cell->get_flag_zone_axis()){
      cv::Point3d zone_axis = super_cell->get_zone_axis() ;
      q_zone_axis_vector = QVector3D( zone_axis.x, zone_axis.y, zone_axis.z  );
      q_zone_axis_vector.normalize();
      q_zone_axis_vector*= _m_cameraEntity_centerDistance;
      _m_lightTransform->setTranslation(-q_zone_axis_vector);
      _m_cameraEntity->setPosition(-q_zone_axis_vector);
    }
  }
}

void UnitCellViewerWindow::update_cameraEntity_upward_vector(){
  if( _flag_super_cell ){
    if( super_cell->get_flag_upward_vector() ){
      cv::Point3d upward_vector = super_cell->get_upward_vector();
      QVector3D q_upward_vector = QVector3D( upward_vector.x, upward_vector.y, upward_vector.z  );
      q_upward_vector.normalize();
      _m_cameraEntity->setUpVector( q_upward_vector );
    }
  }
}

void UnitCellViewerWindow::view_along_ZA_UV(){
  update_cameraEntity_zone_axis();
  update_cameraEntity_upward_vector();
}

void UnitCellViewerWindow::view_along_a_axis(){
  q_zone_axis_vector = QVector3D( 1 , 0 , 0 );
  q_zone_axis_vector.normalize();
  q_zone_axis_vector*= _m_cameraEntity_centerDistance;
  _m_cameraEntity->setPosition(q_zone_axis_vector);
  _m_lightTransform->setTranslation(q_zone_axis_vector);
  _m_cameraEntity->setUpVector(QVector3D(0, 0, 1));
}

void UnitCellViewerWindow::view_along_b_axis(){
  q_zone_axis_vector = QVector3D( 0 , 1 , 0 );
  q_zone_axis_vector.normalize();
  q_zone_axis_vector*= _m_cameraEntity_centerDistance;
  _m_cameraEntity->setPosition(q_zone_axis_vector);
  _m_lightTransform->setTranslation(q_zone_axis_vector);
    _m_cameraEntity->setUpVector(QVector3D(1, 0, 0));
}

void UnitCellViewerWindow::view_along_c_axis(){
  q_zone_axis_vector = QVector3D( 0 , 0 , 1 );
  q_zone_axis_vector.normalize();
  q_zone_axis_vector*= _m_cameraEntity_centerDistance;
  _m_cameraEntity->setPosition(q_zone_axis_vector);
  _m_lightTransform->setTranslation(q_zone_axis_vector);
  _m_cameraEntity->setUpVector(QVector3D(0, 1, 0));
}
