// [Legal]
// Copyright 2022 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#include "pch.hpp"

#include "QmlSampleManager.h"

#include <QMetaObject>
#include <QQmlComponent>
#include <QQmlProperty>
#include <QDebug>

QmlSampleManager::QmlSampleManager(QQmlEngine* engine, QObject* parent):
  DownloadSampleManager(parent)
{
  QQmlComponent portalItemComponent(
        engine,
        QStringLiteral("qrc:/qmlsamples/QmlSampleManagerPortalItem.qml"),
        this);

  QQmlComponent runtimeEnvironmentComponent(
        engine,
        QStringLiteral("qrc:/qmlsamples/QmlArcGISRuntimeEnvironment.qml"),
        this);

  m_portal = portalItemComponent.create();
  m_arcgisRuntimeEnvironment = runtimeEnvironmentComponent.create();
}

QmlSampleManager::~QmlSampleManager()
{
  delete m_portal;
}

void QmlSampleManager::clearCredentialCache()
{
  QMetaObject::invokeMethod(m_arcgisRuntimeEnvironment, "clearCredentialCache");
}

void QmlSampleManager::loadPortal()
{
  QMetaObject::invokeMethod(m_portal, "load");
}

bool QmlSampleManager::isPortalLoaded() const
{
  return qvariant_cast<bool>(QQmlProperty::read(m_portal, "portalIsLoaded"));
}

void QmlSampleManager::createPortalItem(const QString& itemId)
{
  QMetaObject::invokeMethod(m_portal, "createPortalItem",
                            Q_ARG(QVariant, itemId));
}

void QmlSampleManager::fetchData(const QString& outputPath)
{
  const auto url = QUrl::fromLocalFile(outputPath);
  QMetaObject::invokeMethod(m_portal, "fetch",
                            Q_ARG(QVariant, QVariant::fromValue(url)));
}

QString QmlSampleManager::api() const
{
  return QStringLiteral("QML");
}
