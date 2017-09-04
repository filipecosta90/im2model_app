
#ifndef SRC_VIS_UNITCELLVIEWERWINDOW_H__
#define SRC_VIS_UNITCELLVIEWERWINDOW_H__


#include <QMainWindow>
#include <QtWidgets>

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

/** END BOOST **/
#include "inputcontrols/qtrackballcameracontroller.h"
#include "../qt_scene_supercell.h"

#include <QWidget>
#include <QQuickWidget>
#include <QQuickView>

QT_FORWARD_DECLARE_CLASS(QRadioButton)
QT_FORWARD_DECLARE_CLASS(QCheckBox)
QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QLayout)

  class UnitCellViewerWindow : public QWidget {
    Q_OBJECT
    public:
      UnitCellViewerWindow(QWidget *parent = 0);
      void set_super_cell( SuperCell* );
      bool add_image_layer( cv::Mat layer_image, int width, int height, Qt3DCore::QTransform* transform = nullptr );

      public slots:
      void init();
      void	update_lightEntity_view_vector(const QVector3D &viewVector);
      void update_cameraEntity_zone_axis();
      void update_cameraEntity_upward_vector();
      void update_m_cameraEntity_centerDistance();
      void view_along_ZA_UV();
      void view_along_a_axis();
      void view_along_b_axis();
      void view_along_c_axis();

    protected:


    private:
      //Layout
      QLayout *m_containerLayout;
      QWidget *container;
      QBoxLayout *toolsLayout;
      QToolBar *toolbar;
      //Core vis
      Qt3DCore::QEntity *_m_rootEntity;
      Qt3DExtras::Qt3DWindow *qt_scene_view;
      QtSceneSuperCell* qt_scene_super_cell = nullptr;
      SuperCell* super_cell = nullptr;
      bool _flag_super_cell = false;
      Qt3DRender::QCamera *_m_cameraEntity;
      Qt3DRender::QDirectionalLight* _m_cameraLight;

      Qt3DCore::QTransform *_m_lightTransform;
      double _m_cameraEntity_centerDistance = 10.0f;
      QVector3D q_zone_axis_vector;
  };

#endif
