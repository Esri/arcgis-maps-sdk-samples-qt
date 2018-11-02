// [WriteFile Name=BuildLegend, Category=DisplayInformation]
// [Legal]
// Copyright 2018 Esri.

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

#include "BuildLegend.h"

#include "ui_BuildLegend.h"

#include "Map.h"
#include "ArcGISTiledLayer.h"
#include "ArcGISMapImageLayer.h"
#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"

#include <QUrl>
#include <QIdentityProxyModel>

using namespace Esri::ArcGISRuntime;

namespace
{

/*
 * This is nothing more than a helper class that shadows the
 * LegendInfoSymbolUrlRole as a Qt::DecorationRole, since the
 * LegendInfoListModel does not give us this functionality for free.
 */
class RoleProxyModel: public QIdentityProxyModel
{
public:
  using QIdentityProxyModel::QIdentityProxyModel;

  QVariant data(const QModelIndex& index, int role) const override;
};

QVariant RoleProxyModel::data(const QModelIndex& index, int role) const
{
  if (role == Qt::DecorationRole)
  {
    const QUrl iconRole =
        index.data(LegendInfoListModel::LegendInfoSymbolUrlRole).toUrl();
    return QIcon(iconRole.toLocalFile());
  }
  else
  {
    return QIdentityProxyModel::data(index, role);
  }
}

}

BuildLegend::BuildLegend(QWidget* parent /* = nullptr */):
  QWidget(parent),
  m_ui(new Ui::BuildLegend)
{
  m_ui->setupUi(this);

  // Create a map using the topo basemap
  m_map = new Map(Basemap::topographic(this), this);
  m_map->setAutoFetchLegendInfos(true);
  // set initial viewpoint
  m_map->setInitialViewpoint(
        Viewpoint(Point(-11e6, 6e6, SpatialReference(3857)), 9e7));

  m_ui->mapView->setMap(m_map);

  connect(m_map->legendInfos(), &LegendInfoListModel::fetchLegendInfosCompleted,
          this, [this]()
  {
    // set the legend info list model
    RoleProxyModel* roleModel = new RoleProxyModel(this);
    roleModel->setSourceModel(m_map->legendInfos());
    m_ui->legendView->setModel(roleModel);
  });

  addLayers();
}

BuildLegend::~BuildLegend()
{
  delete m_ui;
}

void BuildLegend::addLayers()
{
  ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(QUrl("https://services.arcgisonline.com/ArcGIS/rest/services/Specialty/Soil_Survey_Map/MapServer"), this);
  m_map->operationalLayers()->append(tiledLayer);

  ArcGISMapImageLayer* mapImageLayer = new ArcGISMapImageLayer(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer"), this);
  m_map->operationalLayers()->append(mapImageLayer);

  ServiceFeatureTable* featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Recreation/FeatureServer/0"), this);
  FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
  m_map->operationalLayers()->append(featureLayer);
}
