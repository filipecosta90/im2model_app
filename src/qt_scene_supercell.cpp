#include "qt_scene_supercell.h"

static const QString cameraVisibleEntityName = QStringLiteral("__internal camera visible entity");
static const QString lightVisibleEntityName = QStringLiteral("__internal light visible entity");
static const QString sceneLoaderSubEntityName = QStringLiteral("__internal sceneloader sub entity");
static const QString helperArrowName = QStringLiteral("__internal helper arrow");
static const QVector3D defaultLightDirection(0.0f, -1.0f, 0.0f);
static const float freeViewCameraNearPlane = 0.1f;
static const float freeViewCameraFarPlane = 10000.0f;
static const float freeViewCameraFov = 45.0f;
static const int dragCornerHandleCount = 8; // One handle for each selection box corner
static const QColor selectionBoxColor("#43adee");
static const QColor cameraFrustumColor("#c22555");
static const QColor helperPlaneColor("#585a5c");
static const QColor helperArrowColorX("red");
static const QColor helperArrowColorY("green");
static const QColor helperArrowColorZ("blue");

QtSceneSuperCell::QtSceneSuperCell(Qt3DCore::QEntity *rootEntity, Qt3DRender::QCamera *camEntity ) : m_rootEntity(rootEntity) {
  cameraEntity = camEntity;

  // Axis
  m_axisEntity = new Qt3DCore::QEntity(m_rootEntity);
  Qt3DRender::QGeometryRenderer *geometryRenderer = new Qt3DRender::QGeometryRenderer(m_axisEntity);
  xyz_axis_layer = new Qt3DRender::QLayer(m_axisEntity);
  m_axisEntity->addComponent( geometryRenderer );
  m_axisEntity->addComponent( xyz_axis_layer );

  sphere_layer = new Qt3DRender::QLayer(m_rootEntity);

  m_helperArrows = new Qt3DCore::QEntity( m_axisEntity );
  m_helperArrows->setEnabled(true);
  m_helperArrows->setObjectName(QStringLiteral("__internal helper arrows"));

  QMatrix4x4 matrix;
  Qt3DCore::QEntity *arrow_y = createArrowEntity(helperArrowColorY, m_helperArrows, matrix, helperArrowName);
  arrow_y->setEnabled(true);

  matrix.rotate(90.0f, QVector3D(1.0f, 0.0f, 0.0f));
  Qt3DCore::QEntity *arrow_z = createArrowEntity(helperArrowColorZ, m_helperArrows, matrix, helperArrowName);
  arrow_z->setEnabled(true);

  matrix = QMatrix();
  matrix.rotate(-90.0f, QVector3D(0.0f, 0.0f, 1.0f));
  Qt3DCore::QEntity *arrow_x = createArrowEntity(helperArrowColorX, m_helperArrows, matrix, helperArrowName);
  arrow_x->setEnabled(true);


  //createObjectPickerForEntity(arrow);
  m_helperArrowsTransform = new Qt3DCore::QTransform();
  m_helperArrows->addComponent(m_helperArrowsTransform);
  m_helperArrows->setParent(m_axisEntity);

}

bool QtSceneSuperCell::update_image_layer( cv::Mat layer_image , double width_nm, double height_nm , Qt3DCore::QTransform* transform, std::string layer_name, int layer_number){
  bool result = false;
  if( m_plane_entity_vector.size() == 0 ){
    result = add_image_layer( layer_image, width_nm, height_nm, transform, layer_name );
    reload_data_from_super_cell();
  }
  else{
    Qt3DExtras::QPlaneMesh* planeMesh = planeMesh_vector[layer_number-1];
    planeMesh->setWidth( width_nm );
    planeMesh->setHeight( height_nm );
    Qt3DExtras::QDiffuseMapMaterial *material = planeMaterial_vector[layer_number-1];
    QColor color;
    color.setRgbF(1.0,1.0,1.0,1.0);
    material->setAmbient(color);
    TextureImage* image = image_vector[layer_number-1];
    image->setImage( layer_image );
    image->update();
    material->diffuse()->addTextureImage(image);
    reload_data_from_super_cell();
    result = true;
  }
  return result;
}

bool QtSceneSuperCell::contains_image_layer( std::string layer_name, int layer_number ){
  bool result = false;
  if( m_plane_entity_vector.size() > (layer_number-1) ){
    result = true;
  }
  return result;
}

bool QtSceneSuperCell::add_image_layer(  cv::Mat layer_image , double width_nm, double height_nm, Qt3DCore::QTransform* transform1, std::string layer_name ){

  // add the plane that will contain the image
  Qt3DCore::QEntity* planeEntity = new Qt3DCore::QEntity(m_rootEntity);
  QString layerTypeEntityName = imageEntityName + QString::fromStdString( layer_name );
  planeEntity->setObjectName( layerTypeEntityName );

  Qt3DExtras::QPlaneMesh* planeMesh = new Qt3DExtras::QPlaneMesh(planeEntity);
  planeMesh->setWidth( width_nm );
  planeMesh->setHeight( height_nm );
  planeEntity->addComponent(planeMesh);

  Qt3DCore::QTransform* transform = new Qt3DCore::QTransform( planeEntity );
  transform->setRotation(QQuaternion::fromAxisAndAngle(1,0,0,90));
  planeEntity->addComponent(transform);

  Qt3DExtras::QDiffuseMapMaterial *material = new Qt3DExtras::QDiffuseMapMaterial( planeEntity );
  QColor color;
  color.setRgbF(1.0,1.0,1.0,1.0);
  material->setAmbient(color);
  material->setShininess(0.0f);
  TextureImage* image = new TextureImage;
  image->setImage( layer_image );
  image->update();

  material->diffuse()->addTextureImage(image);
  planeEntity->addComponent(material);

  //save for later
  m_plane_entity_vector.push_back( planeEntity );
  planeMesh_vector.push_back( planeMesh );
  image_vector.push_back( image );
  planeMaterial_vector.push_back( material );
  planeTransform_vector.push_back( transform );

  return true;
}

QtSceneSuperCell::~QtSceneSuperCell(){
}

// Creates a single arrow
Qt3DRender::QMesh *QtSceneSuperCell::createArrowMesh() {
  Qt3DRender::QMesh *customMesh = new Qt3DRender::QMesh();
  QUrl source(QStringLiteral("qrc:///Icons/MeshArrow3d"));
  if (!source.isValid()) {
    std::cout << "Invalid URL: " << source.toString().toStdString() << std::endl;
  }
  customMesh->setSource(source);
  std::cout << " load mesh from " << source.toString().toStdString() << " with " << customMesh->vertexCount() << "vertices. " << std::endl;
  return customMesh;
}

Qt3DCore::QEntity *QtSceneSuperCell::createArrowEntity(const QColor &color, Qt3DCore::QEntity *parent, const QMatrix4x4 &matrix, const QString &name) {

  Qt3DCore::QEntity *arrow = new Qt3DCore::QEntity(parent);
  arrow->setObjectName(name);

  Qt3DRender::QGeometryRenderer *mesh = createArrowMesh();
  Qt3DExtras::QGoochMaterial *material = new Qt3DExtras::QGoochMaterial();
  material->setDiffuse(color);

  Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
  transform->setMatrix(matrix);
  transform->setScale(3.0f);
  arrow->addComponent(mesh);
  arrow->addComponent(material);
  arrow->addComponent(transform);
  arrow->setEnabled(true);
  return arrow;
}

void QtSceneSuperCell::set_super_cell( SuperCell* cell ){
  super_cell = cell;
  _flag_super_cell = true;
}

std::vector<std::string> QtSceneSuperCell::get_atom_symbols_vec(){
  std::vector<std::string> atom_symbols;
  if( _flag_super_cell ){
atom_symbols = super_cell->get_atom_symbols_vec();
}
return atom_symbols;
}

bool QtSceneSuperCell::enable_helper_arrows( bool enabled ){
  EditorUtils::setEnabledExpandedChildEntities(m_rootEntity, QStringLiteral("__internal helper arrows"), enabled );
  return true;
}

bool QtSceneSuperCell::enable_image_layer( std::string layer_name, bool enabled ){
  QString layerTypeEntityName = imageEntityName + QString::fromStdString( layer_name );
  EditorUtils::setEnabledExpandedChildEntities(m_rootEntity, layerTypeEntityName, enabled );
  return true;
}

bool QtSceneSuperCell::enable_atom_type( int distinct_atom_pos, bool enabled ){
  bool result = false;
  if( _flag_super_cell ){
    std::vector<std::string> atom_symbols = super_cell->get_atom_symbols_vec();
    if( atom_symbols.size() > distinct_atom_pos ){
      const std::string atom_symbol = atom_symbols[distinct_atom_pos];
      QString atomTypeEntityName = atomEntityName + QString::fromStdString( atom_symbol );
      EditorUtils::setEnabledExpandedChildEntities(m_rootEntity, atomTypeEntityName, enabled );
      result = true;
    }
  }
  return result;
}

bool QtSceneSuperCell::updateAtomMeshRadius( int distinct_atom_pos, double radius ){
  bool result = false;
  if( _flag_super_cell && ( sphere_meshes.size() > distinct_atom_pos ) ){
    std::cout << "updateAtomMeshRadius " << radius << std::endl;
    std::vector<Qt3DExtras::QSphereMesh*> sphere_meshes_inner = sphere_meshes[distinct_atom_pos];
    for( int same_type_pos = 0; same_type_pos < sphere_meshes_inner.size(); same_type_pos++ ){
    Qt3DExtras::QSphereMesh *sphereMesh = sphere_meshes_inner[same_type_pos];
    if( sphereMesh ){
      std::cout << "updateAtomMeshRadius to " << (float) radius << " from: "<< sphereMesh->radius() <<  std::endl;
      sphereMesh->setRadius( (float) radius );
    }
  }
    result = true;
  }
  return result;
}

void QtSceneSuperCell::reload_data_from_super_cell(){
  if( _flag_super_cell ){

    const std::vector< std::vector<cv::Point3d> > atom_positions_vec = super_cell->get_atom_positions_vec();
    const std::vector<cv::Vec4d> atom_cpk_rgba_colors = super_cell->get_atom_cpk_rgba_colors_vec();
    std::vector<double> atom_empirical_radiis = super_cell->get_atom_empirical_radiis_vec();
    std::vector<std::string> atom_symbols = super_cell->get_atom_symbols_vec();

    //reserve space on vec
    sphere_meshes.reserve( atom_positions_vec.size() );

    for( int distinct_atom_pos = 0; distinct_atom_pos < atom_positions_vec.size(); distinct_atom_pos++ ){

      const cv::Vec4d atom_cpk_rgba_color = atom_cpk_rgba_colors[distinct_atom_pos];
      const double atom_empirical_radii = atom_empirical_radiis[distinct_atom_pos];

      const std::vector<cv::Point3d> same_type_atoms = atom_positions_vec[distinct_atom_pos];
      const std::string atom_symbol = atom_symbols[distinct_atom_pos];
      QString atomTypeEntityName = atomEntityName + QString::fromStdString( atom_symbol );
      QString atomTypeMeshName = atomMeshName + QString::fromStdString( atom_symbol );

      EditorUtils::removeExpandedChildEntities(m_rootEntity,atomTypeEntityName);

      std::vector<Qt3DExtras::QSphereMesh*> sphere_meshes_inner;

      //reserve space on vec
      sphere_meshes_inner.reserve( atom_positions_vec[distinct_atom_pos].size() );

      for( int same_type_pos = 0; same_type_pos < atom_positions_vec[distinct_atom_pos].size(); same_type_pos++ ){
        const cv::Point3d atom_pos = atom_positions_vec[distinct_atom_pos][same_type_pos];

        Qt3DCore::QEntity* sphereEntity = new Qt3DCore::QEntity( m_rootEntity );
sphereEntity->setParent( m_rootEntity );

        // Mesh
        Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh( sphereEntity );
        sphereMesh->setParent(sphereEntity);
        sphereMesh->setRings(10);
        sphereMesh->setSlices(10);
        sphereMesh->setRadius( atom_empirical_radii );
        sphereMesh->setObjectName( atomTypeMeshName );
        sphere_meshes_inner.push_back( sphereMesh );

        //Layer
        Qt3DRender::QGeometryRenderer *geometryRenderer = new Qt3DRender::QGeometryRenderer(sphereEntity);
        geometryRenderer->setParent(sphereEntity);
        
        sphereEntity->addComponent( geometryRenderer );
        sphereEntity->addComponent( sphere_layer );
        sphereEntity->addComponent( sphereMesh );

        // Material
        Qt3DExtras::QGoochMaterial *sphereMaterial = new Qt3DExtras::QGoochMaterial( sphereEntity );
        sphereMaterial->setDiffuse(QColor::fromRgbF( atom_cpk_rgba_color[0], atom_cpk_rgba_color[1], atom_cpk_rgba_color[2] ));
/*
TO DO
        Qt3DExtras::QPhongAlphaMaterial *sphereMaterial = new Qt3DExtras::QPhongAlphaMaterial( sphereEntity );
        sphereMaterial->setAlpha( 1.0f );
        sphereMaterial->setAmbient( QColor::fromRgbF( atom_cpk_rgba_color[0], atom_cpk_rgba_color[1], atom_cpk_rgba_color[2], 0.3f ) );
*/
        sphereEntity->addComponent( sphereMaterial );

        // Transform
        Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform( sphereEntity );
        sphereTransform->setTranslation(QVector3D( atom_pos.x, atom_pos.y, atom_pos.z ));
        sphereEntity->addComponent( sphereTransform );
        //create different entity names for each atom type
        //Save entity
        sphereEntity->setEnabled( true );
        sphereEntity->setObjectName( atomTypeEntityName );
        sphere_entities.push_back( sphereEntity );
      }
      sphere_meshes.push_back( sphere_meshes_inner );
    }
  }
}
