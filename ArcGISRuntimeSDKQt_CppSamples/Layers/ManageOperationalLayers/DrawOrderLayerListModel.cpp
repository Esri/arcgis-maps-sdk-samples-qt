// Copyright 2019 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "DrawOrderLayerListModel.h"

DrawOrderLayerListModel::DrawOrderLayerListModel(QObject* parent):
  QSortFilterProxyModel(parent)
{
  sort(0);
}

DrawOrderLayerListModel::~DrawOrderLayerListModel()
{
}

void DrawOrderLayerListModel::setLayerListModel(QAbstractItemModel* layerListModel)
{
  setSourceModel(layerListModel);
}

int DrawOrderLayerListModel::mappedIndex(int index) const
{
  const QModelIndex sourceIndex = mapToSource(this->index(index, 0));
  return sourceIndex.row();
}

bool DrawOrderLayerListModel::lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const
{
  return sourceLeft.row() >= sourceRight.row();
}
