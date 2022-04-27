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

#include "SampleSearchFilterModel.h"

#include <QDebug>

#include "SearchFilterCriteria.h"
#include "SearchFilterSimpleKeywordCriteria.h"

SampleSearchFilterModel::SampleSearchFilterModel(QObject* parent) :
  SampleSearchFilterModel(nullptr, parent)
{
  setSearchFilterCriteria(new SearchFilterSimpleKeywordCriteria(this));
  sort(0, Qt::DescendingOrder);
}

SampleSearchFilterModel::~SampleSearchFilterModel() = default;

SampleSearchFilterModel::SampleSearchFilterModel(SearchFilterCriteria* searchCriteria, QObject* parent) :
  QSortFilterProxyModel(parent),
  m_searchCriteria(searchCriteria)
{
  connect(this, &QAbstractProxyModel::sourceModelChanged,
          this, &SampleSearchFilterModel::discoverUniqueRoleIdForRoles);
}

QVariant SampleSearchFilterModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  auto source = mapToSource(index);

  if (role == blurbRole())
  {
    return m_filterResults[source.row()].second;
  }
  else if (role == scoreRole())
  {
    return m_filterResults[source.row()].first;
  }
  else
  {
    return QSortFilterProxyModel::data(index, role);
  }
}

QString SampleSearchFilterModel::filterString() const
{
  return m_filterString;
}

void SampleSearchFilterModel::setFilterString(QString filterString)
{
  if (m_filterString != filterString)
  {
    m_filterString = std::move(filterString);
    emit filterStringChanged();
    updateFilterResults();
  }
}

int SampleSearchFilterModel::scoreRole() const
{
  return m_scoreRole;
}

int SampleSearchFilterModel::blurbRole() const
{
  return m_blurbRole;
}

bool SampleSearchFilterModel::lessThan(const QModelIndex& left,
                                       const QModelIndex& right) const
{
  if (m_searchCriteria)
  {
    const auto indexL = m_filterResults.constFind(left.row());
    if (indexL == m_filterResults.cend())
    {
      return false;
    }
    else
    {
      const auto indexR = m_filterResults.constFind(right.row());
      return indexR == m_filterResults.cend() ? true
                                              : indexL->first < indexR->first;
    }
  }
  else
  {
    // No search criteria provided so we will not perform any appropriate sorting.
    return left < right;
  }
}

bool SampleSearchFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& /*sourceParent*/) const
{
  if (m_searchCriteria && sourceModel()) // Perform no filtering if this is not properly set.
  {
    return m_filterResults[sourceRow].first > 0;
  }
  return true; // If there is no searchCriteria object or all cells in the row pass.
}

QHash<int, QByteArray> SampleSearchFilterModel::roleNames() const
{
  auto names = QSortFilterProxyModel::roleNames();
  names[m_blurbRole] = "blurb";
  names[m_scoreRole] = "score";
  return names;
}

void SampleSearchFilterModel::discoverUniqueRoleIdForRoles()
{
  const auto ids = unusedRoleIds(2);
  if (ids.length() == 2)
  {
    m_blurbRole = ids[0];
    m_scoreRole = ids[1];
    setSortRole(scoreRole());
    emit roleIdsChanged();
  }
}

QList<int> SampleSearchFilterModel::unusedRoleIds(int count) const
{
  QList<int> ids;
  ids.reserve(count);

  auto roleNames = QSortFilterProxyModel::roleNames();

  // Have we requested more free roles that there are available to us? If so
  // then we break out early.
  if (count > std::numeric_limits<int>::max() - roleNames.count())
  {
    return ids;
  }

  // Blurb roles needs an id slotted where the source model does
  // not have an ID reserved.
  for (int role = Qt::UserRole; ; ++role)
  {
    // Find first unique UserRole in the collection of roleNames.
    // End the unique ID discoverey.
    if (roleNames.count(role) < 1)
    {
      ids << role;
      if (ids.size() >= count)
        break;
    }
  }
  return ids;
}

void SampleSearchFilterModel::updateFilterResults()
{
  // Go through each sample and calculate the filter results.
  m_filterResults.clear();
  const auto srcModel = sourceModel();
  if (srcModel)
  {
    const auto totalRows = srcModel->rowCount();
    for (int i = 0; i < totalRows; ++i)
    {
      const auto indx = srcModel->index(i, 0);
      m_filterResults[i] = m_searchCriteria->scoreValue(indx, m_filterString);
    }

    // All across the model the blurb and score roles have changed.
    // Note that even though the source model does _not_ contain the given
    // roles, we are emitting the signal from the source model so that the
    // QSortFilterProxyModel will do the right thing.
    emit sourceModel()->dataChanged(index(0, 0),
                                    index(rowCount()-1, columnCount()-1),
                                    { blurbRole(), scoreRole() });
  }
  // Update the filter
  invalidateFilter();
}

SearchFilterCriteria* SampleSearchFilterModel::searchFilterCriteria() const
{
  return m_searchCriteria;
}

void SampleSearchFilterModel::setSearchFilterCriteria(SearchFilterCriteria* searchCriteria)
{
  m_searchCriteria = searchCriteria;
  updateFilterResults();
}
