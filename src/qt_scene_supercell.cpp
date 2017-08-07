/****************************************************************************
 **
 ** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
 ** Contact: https://www.qt.io/licensing/
 **
 ** This file is part of the Qt3D module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see https://www.qt.io/terms-conditions. For further
 ** information use the contact form at https://www.qt.io/contact-us.
 **
 ** BSD License Usage
 ** Alternatively, you may use this file under the terms of the BSD license
 ** as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of The Qt Company Ltd nor the names of its
 **     contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include "qt_scene_supercell.h"

#include <QtCore/QDebug>

QtSceneSuperCell::QtSceneSuperCell(Qt3DCore::QEntity *rootEntity) : m_rootEntity(rootEntity) {

  // Plane shape data
  Qt3DExtras::QPlaneMesh *planeMesh = new Qt3DExtras::QPlaneMesh();
  planeMesh->setWidth(2);
  planeMesh->setHeight(2);

  // Plane mesh transform
  Qt3DCore::QTransform *planeTransform = new Qt3DCore::QTransform();
  planeTransform->setScale(1.3f);
  planeTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 45.0f));
  planeTransform->setTranslation(QVector3D(0.0f, -4.0f, 0.0f));

  Qt3DExtras::QPhongMaterial *planeMaterial = new Qt3DExtras::QPhongMaterial();
  planeMaterial->setDiffuse(QColor(QRgb(0xa69929)));

  // Plane
  m_planeEntity = new Qt3DCore::QEntity(m_rootEntity);
  m_planeEntity->addComponent(planeMesh);
  m_planeEntity->addComponent(planeMaterial);
  m_planeEntity->addComponent(planeTransform);
  m_planeEntity->setEnabled(true);
}

QtSceneSuperCell::~QtSceneSuperCell(){
}

void QtSceneSuperCell::set_super_cell( SuperCell* cell ){
  super_cell = cell;
   _flag_super_cell = true;
}


void QtSceneSuperCell::reload_data_from_super_cell(){
if( _flag_super_cell ){
  std::cout << "########\nreloading data from super cell\n########\#" << std::endl;
  const std::vector< std::vector<cv::Point3d> > atom_positions_vec = super_cell->get_atom_positions_vec();
  const std::vector<std::string> atom_symbols = super_cell->get_atom_symbols_vec();
  const std::vector<cv::Vec4d> atom_cpk_rgba_colors = super_cell->get_atom_cpk_rgba_colors_vec();
  std::vector<double> atom_empirical_radiis = super_cell->get_atom_empirical_radiis_vec();
  for( int distinct_atom_pos = 0; distinct_atom_pos < atom_positions_vec.size(); distinct_atom_pos++ ){

    // Sphere shape data
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRings(20);
    sphereMesh->setSlices(20);
    Qt3DExtras::QPhongMaterial *sphereMaterial = new Qt3DExtras::QPhongMaterial();

    const cv::Vec4d atom_cpk_rgba_color = atom_cpk_rgba_colors[distinct_atom_pos];
    const double atom_empirical_radii = ( atom_empirical_radiis[distinct_atom_pos] ) / 10.f;

    sphereMaterial->setDiffuse(QColor(QRgb(0xa69929)));
    sphereMesh->setRadius( atom_empirical_radii );
    sphere_meshes.push_back( sphereMesh );

    const std::vector<cv::Point3d> same_type_atoms = atom_positions_vec[distinct_atom_pos];
    for( int same_type_pos = 0; same_type_pos < atom_positions_vec[distinct_atom_pos].size(); same_type_pos++ ){
      const cv::Point3d atom_pos = atom_positions_vec[distinct_atom_pos][same_type_pos];
      // Sphere mesh transform
      Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform();
      sphereTransform->setTranslation(QVector3D( atom_pos.x, atom_pos.y, atom_pos.z ));
      // Sphere
      Qt3DCore::QEntity* sphereEntity = new Qt3DCore::QEntity(m_rootEntity);
      sphereEntity->addComponent( sphereMesh );
      sphereEntity->addComponent( sphereMaterial );
      sphereEntity->addComponent( sphereTransform );
      sphereEntity->setEnabled( true );
      sphere_entities.push_back( sphereEntity );
    }
  }
  }
}

void QtSceneSuperCell::enablePlane(bool enabled){
  //m_planeEntity->setEnabled(enabled);
}

void QtSceneSuperCell::enableSphere(bool enabled){
  //m_sphereEntity->setEnabled(enabled);
}
