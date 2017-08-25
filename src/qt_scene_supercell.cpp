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

   Qt3DCore::QEntity *arrow_x = createArrowEntity(helperArrowColorY, m_helperArrows, matrix, helperArrowName);
   arrow_x->setEnabled(true);

   matrix.rotate(90.0f, QVector3D(1.0f, 0.0f, 0.0f));
   Qt3DCore::QEntity *arrow_y = createArrowEntity(helperArrowColorZ, m_helperArrows, matrix, helperArrowName);

   matrix = QMatrix();
   matrix.rotate(-90.0f, QVector3D(0.0f, 0.0f, 1.0f));
   Qt3DCore::QEntity *arrow_z = createArrowEntity(helperArrowColorX, m_helperArrows, matrix, helperArrowName);
   //createObjectPickerForEntity(arrow);
   m_helperArrowsTransform = new Qt3DCore::QTransform();
   m_helperArrows->addComponent(m_helperArrowsTransform);
   m_helperArrows->setParent(m_axisEntity);
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
	Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
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

void QtSceneSuperCell::reload_data_from_super_cell(){
  if( _flag_super_cell ){

    const std::vector< std::vector<cv::Point3d> > atom_positions_vec = super_cell->get_atom_positions_vec();
    const std::vector<cv::Vec4d> atom_cpk_rgba_colors = super_cell->get_atom_cpk_rgba_colors_vec();
    std::vector<double> atom_empirical_radiis = super_cell->get_atom_empirical_radiis_vec();

    EditorUtils::removeExpandedChildEntities(m_rootEntity,atomEntityName);

    for( int distinct_atom_pos = 0; distinct_atom_pos < atom_positions_vec.size(); distinct_atom_pos++ ){

      const cv::Vec4d atom_cpk_rgba_color = atom_cpk_rgba_colors[distinct_atom_pos];
      const double atom_empirical_radii = atom_empirical_radiis[distinct_atom_pos];

      const std::vector<cv::Point3d> same_type_atoms = atom_positions_vec[distinct_atom_pos];
      for( int same_type_pos = 0; same_type_pos < atom_positions_vec[distinct_atom_pos].size(); same_type_pos++ ){
        const cv::Point3d atom_pos = atom_positions_vec[distinct_atom_pos][same_type_pos];

        Qt3DCore::QEntity* sphereEntity = new Qt3DCore::QEntity(m_rootEntity);
        //Layer
        Qt3DRender::QGeometryRenderer *geometryRenderer = new Qt3DRender::QGeometryRenderer(sphereEntity);
        sphereEntity->addComponent( geometryRenderer );
        sphereEntity->addComponent( sphere_layer );

        // Mesh
        Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh( sphereEntity );
        sphereMesh->setRings(5);
        sphereMesh->setSlices(5);
        sphereMesh->setRadius( atom_empirical_radii );
        sphere_meshes.push_back( sphereMesh );
        sphereEntity->addComponent( sphereMesh );

        // Material
        Qt3DExtras::QGoochMaterial *sphereMaterial = new Qt3DExtras::QGoochMaterial( sphereEntity );
        sphereMaterial->setDiffuse(QColor::fromRgbF( atom_cpk_rgba_color[0], atom_cpk_rgba_color[1], atom_cpk_rgba_color[2] ) );
        sphereEntity->addComponent( sphereMaterial );

        // Transform
        Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform( sphereEntity );
        sphereTransform->setTranslation(QVector3D( atom_pos.x, atom_pos.y, atom_pos.z ));
        sphereEntity->addComponent( sphereTransform );

        //Save entity
        sphereEntity->setEnabled( true );
        sphereEntity->setObjectName( atomEntityName );
        sphere_entities.push_back( sphereEntity );
      }
    }
  }
}

void QtSceneSuperCell::orientate_data_from_super_cell(){
  std::cout << " orientate_data_from_super_cell" << std::endl;
}
