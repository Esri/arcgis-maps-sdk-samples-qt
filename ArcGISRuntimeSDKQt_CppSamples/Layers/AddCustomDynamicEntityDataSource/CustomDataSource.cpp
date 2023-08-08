#include "Domain.h"
#include "DynamicEntityDataSourceInfo.h"
#include "Field.h"
#include "ServiceTypes.h"
#include "CustomDataSource.h"
#include "DynamicEntityDataSource.h"
#include "SpatialReference.h"
#include "Point.h"

#include <QtConcurrent/QtConcurrent>
#include <QFile>
#include <QFuture>
#include <QTextStream>

using namespace Esri::ArcGISRuntime;

CustomDataSource::CustomDataSource(QObject* parent) : DynamicEntityDataSource(parent)
{

}

CustomDataSource::CustomDataSource(const QString& fileName, const QString& entityIdField, const int msDelay, QObject* parent) :
  DynamicEntityDataSource(parent), m_fileName(fileName), m_entityIdField(entityIdField), m_msDelay(msDelay)
{
}

QFuture<DynamicEntityDataSourceInfo*> CustomDataSource::onLoadAsync()
{
  m_fields = getSchema();
  m_watcher.setFuture(QtConcurrent::run([this](){observationProcessLoopAsync();}));
  //return QtFuture::makeReadyFuture<DynamicEntityDataSourceInfo*>(new DynamicEntityDataSourceInfo("m_entityIdField", m_fields, this));

  DynamicEntityDataSourceInfo* dynamicEntityDataSourceInfo = new DynamicEntityDataSourceInfo("m_entityIdField", m_fields, this);
  dynamicEntityDataSourceInfo->setSpatialReference(SpatialReference::wgs84());
  return QtFuture::makeReadyFuture(dynamicEntityDataSourceInfo);
}

QFuture<void> CustomDataSource::onConnectAsync()
{
  m_isConnected = true;
  return QtFuture::makeReadyFuture();
}

QFuture<void> CustomDataSource::onDisconnectAsync()
{
  m_isConnected = false;
  return QtFuture::makeReadyFuture();
}

void CustomDataSource::observationProcessLoopAsync()
{
  // Open the file for processing.
  QFile file(m_fileName);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream stream(&file);

    while (!stream.atEnd() && !m_watcher.isCanceled())
    {
      QString line = stream.readLine();

      if (m_isConnected)
      {
        QJsonDocument json = QJsonDocument::fromJson(line.toUtf8());
        auto jsonObject = json.object();
        auto geometryObject = jsonObject.value("geometry").toObject();

        const Point point(geometryObject.value("x").toDouble(),
                          geometryObject.value("y").toDouble(),
                          SpatialReference::wgs84());

        auto attributesObject = jsonObject.value("attributes").toObject();
        QVariantMap attributes = convertJsonObjectToVariantMap(attributesObject);

        addObservation(point, attributes);
      }

      QThread::msleep(m_msDelay);
    }

    if (!m_watcher.isCanceled())
      observationProcessLoopAsync();
  }
}

QVariantMap CustomDataSource::convertJsonObjectToVariantMap(const QJsonObject& jsonObject)
{
  QVariantMap variantMap;

  for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
    const QString key = it.key();
    const QJsonValue value = it.value();

    variantMap[key] = value.toVariant();
  }

  return variantMap;
}

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
