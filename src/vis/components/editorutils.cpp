/****************************************************************************
 **
 ** Copyright (C) 2016 The Qt Company Ltd.
 ** Contact: https://www.qt.io/licensing/
 **
 ** This file is part of the Qt3D Editor of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:GPL-EXCEPT$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see https://www.qt.io/terms-conditions. For further
 ** information use the contact form at https://www.qt.io/contact-us.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3 as published by the Free Software
 ** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
 ** included in the packaging of this file. Please review the following
 ** information to ensure the GNU General Public License requirements will
 ** be met: https://www.gnu.org/licenses/gpl-3.0.html.
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/
#include "editorutils.h"
#include "qdummyobjectpicker.h"

void EditorUtils::removeExpandedChildEntities(Qt3DCore::QEntity *entity, const QString &childName ){
  QList<Qt3DCore::QEntity*> childs = entity->findChildren<Qt3DCore::QEntity*>( childName );
    Q_FOREACH (Qt3DCore::QEntity *childEntity, childs) {
      delete childEntity;
    }
}

Qt3DRender::QObjectPicker *EditorUtils::entityPicker(Qt3DCore::QEntity *entity){
  Qt3DCore::QComponentVector components = entity->components();
  for (int i = 0; i < components.size(); i++) {
    Qt3DRender::QObjectPicker *picker
      = qobject_cast<Qt3DRender::QObjectPicker *>(components.value(i));
    if (picker)
      return picker;
  }
  return nullptr;
}

void EditorUtils::setEnabledToSubtree(Qt3DCore::QEntity *entity, bool enable){
  entity->setEnabled(enable);
  Q_FOREACH (QObject *child, entity->children()) {
    Qt3DCore::QEntity *childEntity = qobject_cast<Qt3DCore::QEntity *>(child);
    if (childEntity)
      setEnabledToSubtree(childEntity, enable);
  }
}

void EditorUtils::setEnabledExpandedChildEntities(Qt3DCore::QEntity *entity, const QString &childName, bool enable ){
  QList<Qt3DCore::QEntity*> childs = entity->findChildren<Qt3DCore::QEntity*>( childName );
    Q_FOREACH (Qt3DCore::QEntity *childEntity, childs) {
      setEnabledToSubtree(childEntity,enable);
    }
}

Qt3DCore::QEntity *EditorUtils::findEntityByName(Qt3DCore::QEntity *entity, const QString &name){
  if (entity->objectName() == name) {
    return entity;
  } else {
    for (QObject *child : entity->children()) {
      Qt3DCore::QEntity *childEntity = qobject_cast<Qt3DCore::QEntity *>(child);
      if (childEntity) {
        Qt3DCore::QEntity *foundEntity = findEntityByName(childEntity, name);
        if (foundEntity)
          return foundEntity;
      }
    }
  }
  return nullptr;
}
