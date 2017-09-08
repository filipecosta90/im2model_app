#ifndef SCENEMODIFIER_H
#define SCENEMODIFIER_H

#include <Qt3DRender/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qcameralens.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtGui/QScreen>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <Qt3DInput/QInputAspect>
#include <QFrameGraphNode>
#include <Qt3DCore/QEntity>
 #include <Qt3DRender/QGeometryRenderer>
 #include <Qt3DRender/QLayer>
 #include <Qt3DRender/QLayerFilter>
 #include <Qt3DRender/QViewport>

#include <Qt3DExtras/qtorusmesh.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/qpointlight.h>

#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qaspectengine.h>
#include <QSceneLoader>
#include <QTechniqueFilter>
#include <QViewport>
#include <QClearBuffers>
#include <QCameraSelector>
#include <QRenderPassFilter>
#include <QSceneLoader>

#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DExtras/qforwardrenderer.h>

#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QPhongMaterial>

#include <QComponent>
#include <QMetaObject>
#include <QMetaProperty>

#include <QtCore/QObject>
#include <QtCore/QDebug>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QViewport>
#include <QGoochMaterial>
#include <QColor>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QViewport>

#include <boost/shared_array.hpp>

#include "super_cell.hpp"
#include "vis/components/editorutils.h"
#include "vis/texture_image.h"

static const QString atomEntityName = QStringLiteral("__internal atom");

class QtSceneSuperCell : public QObject
{
  Q_OBJECT
  public:
    explicit QtSceneSuperCell( Qt3DCore::QEntity *rootEntity, Qt3DRender::QCamera *cameraEntity );
    ~QtSceneSuperCell();
    void set_super_cell( SuperCell* cell );
    bool add_image_layer( cv::Mat layer_image , double width_nm, double height_nm, Qt3DCore::QTransform* transform = nullptr );

    Qt3DRender::QLayer* get_xyz_axis_layer(){ return xyz_axis_layer; }
    Qt3DRender::QLayer* get_sphere_layer(){ return sphere_layer; }

	static Qt3DRender::QMesh *createArrowMesh();
	static Qt3DCore::QEntity *createArrowEntity(const QColor &color, Qt3DCore::QEntity *parent, const QMatrix4x4 &matrix, const QString &name);

    public slots:
    void reload_data_from_super_cell();

  private:
    Qt3DRender::QCamera *cameraEntity;
    Qt3DCore::QEntity *m_rootEntity;
    // helper arrows
    Qt3DCore::QEntity *m_helperArrows;
    Qt3DCore::QTransform *m_helperArrowsTransform;

    Qt3DCore::QEntity *m_plane_entity;
    Qt3DExtras::QPlaneMesh* planeMesh;
    TextureImage* image;
    Qt3DExtras::QDiffuseMapMaterial* planeMaterial;
    Qt3DCore::QTransform *planeTransform;


    Qt3DCore::QEntity *m_cylinderEntity_x;
    Qt3DCore::QEntity *m_cylinderEntity_y;
    Qt3DCore::QEntity *m_cylinderEntity_z;
    Qt3DCore::QEntity *m_axisEntity;
    Qt3DRender::QLayer *xyz_axis_layer;
    Qt3DCore::QEntity *m_modelEntity;
    std::vector<Qt3DCore::QEntity*> sphere_entities;
    Qt3DRender::QLayer *sphere_layer;
    std::vector<Qt3DExtras::QSphereMesh*> sphere_meshes;
    SuperCell* super_cell = nullptr;
    bool _flag_super_cell = false;
};

#endif // SCENEMODIFIER_H
