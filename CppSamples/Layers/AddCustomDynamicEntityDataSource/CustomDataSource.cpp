// [Legal]
// Copyright 2024 Esri.
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

// sample headers
#include "CustomDataSource.h"

// ArcGIS Maps SDK headers
#include "Domain.h"
#include "DynamicEntityDataSource.h"
#include "DynamicEntityDataSourceInfo.h"
#include "Field.h"
#include "Point.h"
#include "ServiceTypes.h"
#include "SpatialReference.h"

// Qt headers
#include <QFile>
#include <QFuture>
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>

using namespace Esri::ArcGISRuntime;

CustomDataSource::CustomDataSource(QObject* parent) : DynamicEntityDataSource(parent)
{
}

CustomDataSource::CustomDataSource(const QString& fileName, const QString& entityIdField, const int msDelay, QObject* parent) :
  DynamicEntityDataSource(parent), m_fileName(fileName), m_entityIdField(entityIdField), m_msDelay(msDelay)
{
}

CustomDataSource::~CustomDataSource()
{
  // The app will crash upon destruction if the asynchronous method is still running upon destruction
  m_watcher.cancel();
  m_watcher.waitForFinished();
}

// Override the virtual onLoadAsync method to define what the DynamicEntityDataSource will do upon load
QFuture<DynamicEntityDataSourceInfo*> CustomDataSource::onLoadAsync()
{
  m_fields = getSchema();

  m_file.setFileName(m_fileName);

  if (m_file.open(QIODevice::ReadOnly | QIODevice::Text))
    m_textStream.setDevice(&m_file);

  // Create a DynamicEntityDataSourceInfo object to return
  DynamicEntityDataSourceInfo* dynamicEntityDataSourceInfo = new DynamicEntityDataSourceInfo(m_entityIdField, m_fields, this);

  // Your data may not display correctly if you do not have a spatial reference set
  dynamicEntityDataSourceInfo->setSpatialReference(SpatialReference::wgs84());

  // Return the QFuture<DynamicEntityDataSourceInfo*>
  return QtFuture::makeReadyFuture(dynamicEntityDataSourceInfo);
}

// Override the virtual onConnectAsync method to define what the DynamicEntityDataSource will do when the data source is connected
QFuture<void> CustomDataSource::onConnectAsync()
{
  m_watcher.setFuture(QtConcurrent::run([this](){observationProcessLoopAsync();}));
  return QtFuture::makeReadyFuture();
}

// Override the virtual onDisconnectAsync method to define what the DynamicEntityDataSource will do when the data source is disconnected
QFuture<void> CustomDataSource::onDisconnectAsync()
{
  m_watcher.cancel();
  m_watcher.waitForFinished();
  return QtFuture::makeReadyFuture();
}

// This method runs asynchronously to step through the accompanying .json file and call addObservation(geometry, attributes) with each line
void CustomDataSource::observationProcessLoopAsync()
{
  while (!m_textStream.atEnd() && !m_watcher.isCanceled())
  {   
    const QString line = m_textStream.readLine();
    const QJsonObject jsonObject = QJsonDocument::fromJson(line.toUtf8()).object();

    // Get the observation geometry from the line
    const QJsonObject geometryObject = jsonObject.value("geometry").toObject();
    const Point point(geometryObject.value("x").toDouble(),
                      geometryObject.value("y").toDouble(),
                      SpatialReference::wgs84());

    // Get the observation attributes from the line
    const QVariantMap attributes = jsonObject.value("attributes").toObject().toVariantMap();

    addObservation(point, attributes);

    QThread::msleep(m_msDelay);

    if (m_textStream.atEnd())
      m_textStream.seek(0);
  }
}

// Schema fields that are hardcoded to match the accompanying .json data
QList<Field> CustomDataSource::getSchema()
{
  return QList<Field>
  {
    Field(FieldType::Text, "MMSI", "", 256, Domain(), false, false),
    Field(FieldType::Float64, "BaseDateTime", "", 8, Domain(), false, false),
    Field(FieldType::Float64, "LAT", "", 8, Domain(), false, false),
    Field(FieldType::Float64, "LONG", "", 8, Domain(), false, false),
    Field(FieldType::Float64, "SOG", "", 8, Domain(), false, false),
    Field(FieldType::Float64, "COG", "", 8, Domain(), false, false),
    Field(FieldType::Float64, "Heading", "", 8, Domain(), false, false),
    Field(FieldType::Text, "VesselName", "", 256, Domain(), false, false),
    Field(FieldType::Text, "IMO", "", 256, Domain(), false, false),
    Field(FieldType::Text, "CallSign", "", 256, Domain(), false, false),
    Field(FieldType::Text, "VesselType", "", 256, Domain(), false, false),
    Field(FieldType::Text, "Status", "", 256, Domain(), false, false),
    Field(FieldType::Float64, "Length", "", 8, Domain(), false, false),
    Field(FieldType::Float64, "Width", "", 8, Domain(), false, false),
    Field(FieldType::Text, "Cargo", "", 256, Domain(), false, false),
    Field(FieldType::Text, "globalid", "", 256, Domain(), false, false)
  };
}
