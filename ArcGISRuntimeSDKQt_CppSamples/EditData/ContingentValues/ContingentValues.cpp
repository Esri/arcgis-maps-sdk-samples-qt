// [WriteFile Name=ContingentValues, Category=EditData]
// [Legal]
// Copyright 2021 Esri.

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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "ContingentValues.h"

#include "ArcGISFeature.h"
#include "CodedValueDomain.h"
#include "ContingentValue.h"
#include "ContingentCodedValue.h"
#include "ContingentValuesDefinition.h"
#include "ContingentValuesResult.h"
#include "Domain.h"
#include "FeatureLayer.h"
#include "Field.h"
#include "FieldGroup.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "GeometryEngine.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MobileMapPackage.h"
#include "RangeDomain.h"
#include "ServiceFeatureTable.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"

#include <QDir>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_ANDROID
  dataPath = "/sdcard";
#elif defined Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QDir::homePath();
#endif

  return dataPath;
}
}

ContingentValues::ContingentValues(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTerrain, this))
{
  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/geodatabase/ContingentValuesBirdNests.geodatabase";
  m_geodatabase = new Geodatabase(dataPath, this);
}

ContingentValues::~ContingentValues() = default;

void ContingentValues::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ContingentValues>("Esri.Samples", 1, 0, "ContingentValuesSample");
}

MapQuickView* ContingentValues::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ContingentValues::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;

  m_mapView->setMap(m_map);

  m_graphicsOverlay = new GraphicsOverlay(this);
  Symbol* bufferSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::ForwardDiagonal, Qt::red, new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::black, 2, this), this);
  m_graphicsOverlay->setRenderer(new SimpleRenderer(bufferSymbol, this));

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  m_mapView->setViewpoint(Viewpoint(Point(-13236000, 4081200), 8822));

  createConnections();
  m_geodatabase->load();

  emit mapViewChanged();
}

void ContingentValues::createNewNest(const QVariantMap attributes)
{
  if (m_gdbFeatureTable->loadStatus() != LoadStatus::Loaded)
  {
    qDebug() << "Geodatabase feature table not loaded";
    return;
  }

  const Point nestPoint = m_mapView->screenToLocation(m_featureAttributesPaneX, m_featureAttributesPaneY);

  ArcGISFeature* nestFeature = static_cast<ArcGISFeature*>(m_gdbFeatureTable->createFeature(attributes, nestPoint, this));
  m_gdbFeatureTable->addFeature(nestFeature);
}

void ContingentValues::createConnections()
{
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent mouseEvent)
  {
    if (!m_nestsLayer)
      return;

    m_selectedFeature = nullptr;
    m_nestsLayer->clearSelection();
    m_mapView->identifyLayer(m_nestsLayer, mouseEvent.x(), mouseEvent.y(), 5, false, 1);
    m_featureAttributesPaneX = mouseEvent.x();
    m_featureAttributesPaneY = mouseEvent.y();
    //const Point mousePoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
    //createNewNest(mousePoint);
  });

  connect(m_mapView, &MapQuickView::identifyLayerCompleted, this, [this](QUuid, IdentifyLayerResult* identifyResult)
  {
    if (!identifyResult)
      return;

    if (identifyResult->geoElements().size() > 0)
    {
      m_selectedFeature = static_cast<ArcGISFeature*>(identifyResult->geoElements().at(0));
      m_nestsLayer->selectFeature(m_selectedFeature);
      setFeatureAttributes(m_selectedFeature->attributes()->attributesMap());
      setFeatureAttributesPaneVisibe(true);
    }
    else
    {
      setFeatureAttributesPaneVisibe(false);
    }
  });

  connect(m_geodatabase, &Geodatabase::doneLoading, this, [this]()
  {
    qDebug() << m_geodatabase->geodatabaseFeatureTables().size();
    m_gdbFeatureTable = m_geodatabase->geodatabaseFeatureTable("BirdNests");
    if (!m_gdbFeatureTable)
      return;

    connect(m_gdbFeatureTable, &GeodatabaseFeatureTable::doneLoading, this, [this]
    {
      m_nestsLayer = new FeatureLayer(m_gdbFeatureTable, this);
      m_map->operationalLayers()->append(m_nestsLayer);

      bufferFeatures();

      //      for (Field field : m_gdbFeatureTable->fields())
      //      {
      //        qDebug() << field.name();
      //        if (field.domain().domainType() == DomainType::CodedValueDomain)
      //        {
      //          CodedValueDomain codedValueDomain = static_cast<CodedValueDomain>(field.domain());
      //          QList<QString> values = {""};
      //          for (CodedValue codedValue : codedValueDomain.codedValues())
      //          {
      //            values.append(codedValue.name());
      //          }
      //          m_codedValueDomains.insert(field.name(), (QVariant)values);
      //        }
      //        else if (field.domain().domainType() == DomainType::RangeDomain)
      //        {
      //          RangeDomain rangeDomain = static_cast<RangeDomain>(field.domain());
      //          m_rangeDomains.insert(field.name(), (QVariant)(QList<QString>{rangeDomain.minValue().toString(), rangeDomain.maxValue().toString()}));
      //        }
      //      }

      if (!m_codedValueDomains.isEmpty() || !m_rangeDomains.isEmpty())
        emit domainsChanged();

      //      qDebug() << m_codedValueDomains;
      //      qDebug() << m_rangeDomains;

      m_contingentValuesDefinition = m_gdbFeatureTable->contingentValuesDefinition();

      connect(m_contingentValuesDefinition, &ContingentValuesDefinition::doneLoading, this, [this]()
      {
//        for (FieldGroup* fieldGroup : m_contingentValuesDefinition->fieldGroups())
//        {
//          qDebug() << "field group name" << fieldGroup->name();
//        }

        //        if (!m_contingentValuesDefinition->loadError().isEmpty())
        //        {
        //          Error e = m_contingentValuesDefinition->loadError();
        //          qDebug() << e.message() << e.additionalMessage();
        //        }

        //        if (m_contingentValuesDefinition->fieldGroups().size() == 0)
        //          return;

        ArcGISFeature* feature = static_cast<ArcGISFeature*>(m_gdbFeatureTable->createFeature(this));

        QMap<QString, QVariant> featureAttributes;


        //        ContingentValuesResult* cvr = m_gdbFeatureTable->contingentValues(feature, "Activity");

        //        for (ContingentValue* contingentValue : cvr->contingentValuesByFieldGroup(this).value("NestBuffer"))
        //        {
        //          ContingentCodedValue* contingentCodedValue = static_cast<ContingentCodedValue*>(contingentValue);
        //          if (contingentCodedValue)
        //          {
        //            CodedValue cd = contingentCodedValue->codedValue();
        //            qDebug() << "coded value name" << cd.name();
        //          }
        //        }

        featureAttributes.insert("Activity", "OCCUPIED");

        //        cvr = m_gdbFeatureTable->contingentValues(feature, "Protection");

        //        for (ContingentValue* contingentValue : cvr->contingentValuesByFieldGroup(this).value("NestBuffer"))
        //        {
        //          ContingentCodedValue* contingentCodedValue = static_cast<ContingentCodedValue*>(contingentValue);
        //          if (contingentCodedValue)
        //          {
        //            CodedValue cd = contingentCodedValue->codedValue();
        //            qDebug() << "coded value name" << cd.name();
        //          }
        //        }

        featureAttributes.insert("Protection", "NOT_ENDANGERED");

        feature->attributes()->setAttributesMap(featureAttributes);

        ContingentValuesResult* cvr = m_gdbFeatureTable->contingentValues(feature, "BufferSize");

        for (ContingentValue* contingentValue : cvr->contingentValuesByFieldGroup(this).value("BufferSizeRangeFieldGroup"))
        {
          ContingentCodedValue* contingentCodedValue = static_cast<ContingentCodedValue*>(contingentValue);
          if (contingentCodedValue)
          {
            CodedValue cd = contingentCodedValue->codedValue();
            qDebug() << "coded value name" << cd.name();
          }
        }
      });

      m_contingentValuesDefinition->load();
    });

    connect(m_gdbFeatureTable, &GeodatabaseFeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* results)
    {
      qDebug() << "queryFeaturesCompleted";
      if (!results)
      {
        qDebug() << "FeatureQueryResult error";
        return;
      }
      FeatureIterator iterator = results->iterator();

      while (iterator.hasNext())
      {

        Feature* feature = iterator.next(this);
        const double bufferDistance = feature->attributes()->attributeValue("BufferSize").toDouble();
        Polygon buffer = GeometryEngine::buffer(feature->geometry(), bufferDistance);
        m_mapView->graphicsOverlays()->at(0)->graphics()->append(new Graphic(buffer, this));
      }
      qDebug() << "m_graphicsOverlay->graphics()->size():" << m_graphicsOverlay->graphics()->size();
    });

    connect(m_gdbFeatureTable, &GeodatabaseFeatureTable::addFeatureCompleted, this, [this](QUuid, bool success)
    {
      qDebug() << "Added feature:" << success;
      if (success)
      {
        bufferFeatures();
      }
    });

    m_gdbFeatureTable->load();
  });
}

bool ContingentValues::validateContingentValues(QVariantMap attributes)
{
  if (m_geodatabase->loadStatus() != LoadStatus::Loaded ||
      m_gdbFeatureTable->loadStatus() != LoadStatus::Loaded ||
      m_contingentValuesDefinition->loadStatus() != LoadStatus::Loaded)
    return false;

  ArcGISFeature* feature = static_cast<ArcGISFeature*>(m_gdbFeatureTable->createFeature(this));
  feature->attributes()->setAttributesMap(attributes);

  ContingentValuesResult* cvr = m_gdbFeatureTable->contingentValues(feature, "Activity");

  for (ContingentValue* contingentValue : cvr->contingentValuesByFieldGroup(this).value("ProtectionFieldGroup"))
  {
    ContingentCodedValue* contingentCodedValue = static_cast<ContingentCodedValue*>(contingentValue);
    if (contingentCodedValue)
    {
      CodedValue cd = contingentCodedValue->codedValue();
      qDebug() << "coded value name" << cd.name();
    }
  }

  return false;
}

void ContingentValues::setFeatureAttributes(QVariantMap featureAttributes)
{
  m_featureAttributes = featureAttributes;
  emit featureAttributesChanged();
}

QVariantMap ContingentValues::featureAttributes() const
{
  return m_featureAttributes;
}

void ContingentValues::bufferFeatures()
{
  if (!m_gdbFeatureTable || m_gdbFeatureTable->loadStatus() != LoadStatus::Loaded)
    return;

  m_graphicsOverlay->graphics()->clear();

  QueryParameters params;
  params.setWhereClause("BufferSize > 0");
  m_gdbFeatureTable->queryFeatures(params);
}

bool ContingentValues::featureAttributesPaneVisibe() const
{
  return m_featureAttributesPaneVisible;
}
void ContingentValues::setFeatureAttributesPaneVisibe(bool showFeatureAttributesPane)
{
  m_featureAttributesPaneVisible = showFeatureAttributesPane;
  emit featureAttributesPaneVisibeChanged();
}

double ContingentValues::featureAttributesPaneX() const
{
  return m_featureAttributesPaneX;
}
double ContingentValues::featureAttributesPaneY() const
{
  return m_featureAttributesPaneY;
}

QVariantMap ContingentValues::codedValueDomains() const
{
  return m_codedValueDomains;
}

QVariantMap ContingentValues::rangeDomains() const
{
  return m_rangeDomains;
}

void ContingentValues::modifyFeatures(QVariantMap attributes, QString modificationType)
{
  if (modificationType == "CREATE")
  {
    createNewNest(attributes);
  }
  else if (modificationType == "UPDATE")
  {
    m_selectedFeature->attributes()->setAttributesMap(attributes);
  }
  else if (modificationType == "DELETE")
  {
    m_gdbFeatureTable->deleteFeature(m_selectedFeature);
  }
  else
  {
    qWarning() << modificationType << "is not in CREATE UPDATE or DELETE";
  }
  bufferFeatures();
}
