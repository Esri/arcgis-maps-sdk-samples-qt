#include "Domain.h"
#include "DynamicEntityDataSourceInfo.h"
#include "Field.h"
#include "ServiceTypes.h"
#include "SimulatedDataSource.h"
#include "DynamicEntityDataSource.h"

#include <QtConcurrent/QtConcurrent>
#include <QFile>
#include <QFuture>
#include <QTextStream>

using namespace Esri::ArcGISRuntime;

SimulatedDataSource::SimulatedDataSource(QObject* parent) :
  DynamicEntityDataSource(parent)
{

}

Esri::ArcGISRuntime::SimulatedDataSource::SimulatedDataSource(const QString& fileName, const QString& entityIdField, const int msDelay, QObject* parent) :
  DynamicEntityDataSource(parent), m_fileName(fileName), m_entityIdField(entityIdField), m_msDelay(msDelay)
{
  // Open the file for processing.
  QFile file(m_fileName);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    QTextStream m_stream(&file);
}

Esri::ArcGISRuntime::SimulatedDataSource::~SimulatedDataSource()
{

}

QFuture<Esri::ArcGISRuntime::DynamicEntityDataSourceInfo*> Esri::ArcGISRuntime::SimulatedDataSource::onLoadAsync()
{
  m_fields = getSchema();
  m_watcher.setFuture(QtConcurrent::run([this](){observationProcessLoopAsync();}));
  return QtFuture::makeReadyFuture<DynamicEntityDataSourceInfo*>(new DynamicEntityDataSourceInfo(m_entityIdField, m_fields, this));
}


QFuture<void> Esri::ArcGISRuntime::SimulatedDataSource::onConnectAsync()
{
  if (m_watcher.isRunning())
    m_isConnected = true;

  return QtFuture::makeReadyFuture();
}

QFuture<void> Esri::ArcGISRuntime::SimulatedDataSource::onDisconnectAsync()
{
  if (m_watcher.isRunning())
    m_isConnected = false;

  return QtFuture::makeReadyFuture();
}

void SimulatedDataSource::addObservation(const Geometry& geometry, const QVariantMap& attributes)
{

}

QFuture<void> SimulatedDataSource::deleteEntityAsync(const QString& entityId)
{
return QtFuture::makeReadyFuture();
}

void SimulatedDataSource::setConnectionFailed(const QString& userDefinedError, bool reconnect)
{

}

void SimulatedDataSource::setConnectionStatusAndError(ConnectionStatus status, const QString& userDefinedError, bool reconnect)
{

}

void SimulatedDataSource::setLoadInfoAndError(DynamicEntityDataSourceInfo* dynamicEntityDataSourceInfo, const QString& userDefinedError)
{

}


void Esri::ArcGISRuntime::SimulatedDataSource::observationProcessLoopAsync()
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
        qDebug() << line;

      QThread::msleep(m_msDelay);
    }

    if (!m_watcher.isCanceled())
      observationProcessLoopAsync();
  }
}

QList<Field> Esri::ArcGISRuntime::SimulatedDataSource::getSchema()
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
