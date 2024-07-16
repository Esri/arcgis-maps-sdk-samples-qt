// [WriteFile Name=EditWithBranchVersioning, Category=EditData]
// [Legal]
// Copyright 2020 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifndef EDITWITHBRANCHVERSIONING_H
#define EDITWITHBRANCHVERSIONING_H

// C++ API headers
#include "Error.h"

// Qt headers
#include <QObject>
#include <QUrl>
#include <QUuid>

namespace Esri::ArcGISRuntime
{
class AuthenticationManager;
class ServiceVersionParameters;
class Map;
class MapQuickView;
class ServiceGeodatabase;
class FeatureLayer;
class ServiceFeatureTable;
class ServiceVersionInfo;
class ArcGISFeature;
class Credential;
class Point;
class IdentifyLayerResult;
class ErrorException;
class FeatureTableEditResult;
}

Q_MOC_INCLUDE("AuthenticationManager.h")
Q_MOC_INCLUDE("MapQuickView.h")

class EditWithBranchVersioning : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::AuthenticationManager* authManager READ authManager NOTIFY authManagerChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString sgdbCurrentVersion MEMBER m_sgdbCurrentVersionName NOTIFY sgdbCurrentVersionChanged)
  Q_PROPERTY(QString currentTypeDamage MEMBER m_currentTypeDamage NOTIFY currentTypeDamageChanged)
  Q_PROPERTY(QString errorMessage MEMBER m_errorMessage NOTIFY errorMessageChanged)
  Q_PROPERTY(bool busy MEMBER m_busy NOTIFY busyChanged)
  Q_PROPERTY(bool sgdbVersionIsDefault MEMBER m_sgdbVersionIsDefault NOTIFY sgdbVersionIsDefaultChanged)

public:
  explicit EditWithBranchVersioning(QObject* parent = nullptr);
  ~EditWithBranchVersioning();

  static void init();

  Esri::ArcGISRuntime::AuthenticationManager* authManager() const;
  Esri::ArcGISRuntime::ServiceVersionParameters* createParams();
  void clearSelection();
  void moveFeature(const Esri::ArcGISRuntime::Point& mapPoint);
  Q_INVOKABLE void applyEdits();
  Q_INVOKABLE void clearSelectedFeature();
  Q_INVOKABLE void createVersion(const QString& versionName, const QString& versionAccess, const QString& description);
  Q_INVOKABLE void switchVersion();
  Q_INVOKABLE void updateAttribute(const QString& attributeValue);

signals:
  void applyingEdits();
  void applyingEditsCompleted();
  void authManagerChanged();
  void busyChanged();
  void createVersionSuccess();
  void currentTypeDamageChanged();
  void errorMessageChanged();
  void featureSelected();
  void hideWindow();
  void mapViewChanged();
  void sgdbCurrentVersionChanged();
  void sgdbVersionIsDefaultChanged();

private:
  void onCreateVersionCompleted_(Esri::ArcGISRuntime::ServiceVersionInfo* serviceVersionInfo);
  void onMapDoneLoading_(const Esri::ArcGISRuntime::Error& error);
  void onSgdbDoneLoadingCompleted_(const Esri::ArcGISRuntime::Error& error);
  void onIdentifyLayerCompleted_(Esri::ArcGISRuntime::IdentifyLayerResult* identifyResult);
  void onSwitchVersionCompleted_();
  void onApplyEditsCompleted_(const QList<Esri::ArcGISRuntime::FeatureTableEditResult*>& featureTableEditResults);
  void onTaskFailed_(const Esri::ArcGISRuntime::ErrorException& taskException);

  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void connectSgdbSignals();
  bool sgdbVerionIsDefault() const;

  Esri::ArcGISRuntime::ArcGISFeature* m_selectedFeature = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
  Esri::ArcGISRuntime::ServiceGeodatabase* m_serviceGeodatabase = nullptr;
  Esri::ArcGISRuntime::Credential* m_cred = nullptr;

  bool m_busy = false;
  bool m_sgdbVersionIsDefault = true;
  QString m_createdVersionName;
  QString m_currentTypeDamage;
  QString m_errorMessage;
  QString m_sgdbCurrentVersionName;
  QString m_sgdbVersionAccess;
  QString m_sgdbVersionDescription;
};

#endif // EDITWITHBRANCHVERSIONING_H
