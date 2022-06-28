#ifndef INDOORSLOCATIONDATASOURCECREATOR_H
#define INDOORSLOCATIONDATASOURCECREATOR_H

#include <QUuid>
#include <QObject>

namespace Esri
{
namespace ArcGISRuntime
{
class ArcGISFeatureTable;
class FeatureTable;
class IndoorsLocationDataSource;
class Map;
}
}


class IndoorsLocationDataSourceCreator : public QObject
{
  Q_OBJECT

public:
  IndoorsLocationDataSourceCreator(QObject* parent = nullptr);
  ~IndoorsLocationDataSourceCreator();

public slots:
  void createIndoorsLocationDataSource(Esri::ArcGISRuntime::Map* map, const QString &positioningTableName, const QString &pathwaysTableName, const QStringList &globalIdSortNames);

signals:
  void createIndoorsLocationDataSourceCompleted(Esri::ArcGISRuntime::IndoorsLocationDataSource* IndoorsLocationDataSource);

private:
  void returnIndoorsLocationDataSource();
  void findPositioningTable();
  void findPathwaysTable();
  void findGlobalId();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::FeatureTable* m_positioningTable = nullptr;
  Esri::ArcGISRuntime::ArcGISFeatureTable* m_pathwaysTable = nullptr;
  QUuid m_globalId;
  QString m_positioningTableName;
  QString m_pathwaysTableName;
  QStringList m_globalIdSortNames;
};

#endif // INDOORSLOCATIONDATASOURCECREATOR_H
