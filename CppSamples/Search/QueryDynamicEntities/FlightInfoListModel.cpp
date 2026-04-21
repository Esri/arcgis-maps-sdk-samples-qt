// [WriteFile Name=QueryDynamicEntities, Category=Search]
// [Legal]
// Copyright 2026 Esri.
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "FlightInfoListModel.h"

// Qt headers
#include <QModelIndex>
// STL headers
#include <algorithm>

FlightInfoListModel::FlightInfoListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

void FlightInfoListModel::setupRoles()
{
  m_roles[TrackIdRole] = "trackId";
  m_roles[FlightNumberRole] = "flightNumber";
  m_roles[StatusRole] = "status";
  m_roles[ArrivalAirportRole] = "arrivalAirport";
  m_roles[AircraftRole] = "aircraft";
  m_roles[AltitudeFeetRole] = "altitudeFeet";
  m_roles[SpeedRole] = "speed";
  m_roles[HeadingRole] = "heading";
}

void FlightInfoListModel::clear()
{
  beginResetModel();
  m_entries.clear();
  endResetModel();
}

int FlightInfoListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_entries.count();
}

QVariant FlightInfoListModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.count())
  {
    return {};
  }

  const FlightInfoEntry& entry = m_entries.at(index.row());
  switch (role)
  {
    case TrackIdRole:
      return entry.trackId;
    case FlightNumberRole:
      return entry.flightNumber;
    case StatusRole:
      return entry.status;
    case ArrivalAirportRole:
      return entry.arrivalAirport;
    case AircraftRole:
      return entry.aircraft;
    case AltitudeFeetRole:
      return entry.altitudeFeet;
    case SpeedRole:
      return entry.speed;
    case HeadingRole:
      return entry.heading;
    default:
      return {};
  }
}

QHash<int, QByteArray> FlightInfoListModel::roleNames() const
{
  return m_roles;
}

void FlightInfoListModel::setAttributesForTrack(const QString& trackId, const QVariantMap& attributes)
{
  const int existingIndex = indexOfTrack(trackId);
  FlightInfoEntry updated;
  updated.trackId = trackId;
  updated.flightNumber = attrToString(attributes, "flight_number");
  updated.status = attrToString(attributes, "status");
  updated.arrivalAirport = attrToString(attributes, "arrival_airport");
  updated.aircraft = attrToString(attributes, "aircraft");

  // Format numeric values cleanly for UI presentation
  const QVariant altVar = attributes.value("altitude_feet");
  if (altVar.isValid() && !altVar.isNull())
  {
    const double altVal = altVar.toDouble();
    updated.altitudeFeet = QString::number(static_cast<long long>(altVal));
  }
  else
  {
    updated.altitudeFeet = {};
  }

  const QVariant speedVar = attributes.value("speed");
  if (speedVar.isValid() && !speedVar.isNull())
  {
    const double spdVal = speedVar.toDouble();
    updated.speed = QString::number(static_cast<long long>(spdVal));
  }
  else
  {
    updated.speed = {};
  }

  const QVariant headingVar = attributes.value("heading");
  if (headingVar.isValid() && !headingVar.isNull())
  {
    const double hdgVal = headingVar.toDouble();
    updated.heading = QString::number(hdgVal, 'f', 3);
  }
  else
  {
    updated.heading = {};
  }

  if (existingIndex >= 0)
  {
    m_entries[existingIndex] = updated;
    const QModelIndex modelIndex = createIndex(existingIndex, 0);
    emit dataChanged(modelIndex, modelIndex,
                     {TrackIdRole, FlightNumberRole, StatusRole, ArrivalAirportRole, AircraftRole, AltitudeFeetRole, SpeedRole, HeadingRole});
  }
  else
  {
    const int insertAt = m_entries.count();
    beginInsertRows(QModelIndex(), insertAt, insertAt);
    m_entries.append(updated);
    endInsertRows();
  }
}

int FlightInfoListModel::indexOfTrack(const QString& trackId) const
{
  QList<FlightInfoEntry>::const_iterator it = std::find_if(m_entries.cbegin(), m_entries.cend(),
                                                           [&trackId](const FlightInfoEntry& e) { return e.trackId == trackId; });
  return it != m_entries.cend() ? static_cast<int>(std::distance(m_entries.cbegin(), it)) : -1;
}

QString FlightInfoListModel::attrToString(const QVariantMap& attrs, const char* key)
{
  QVariantMap::const_iterator it = attrs.constFind(key);
  if (it == attrs.constEnd())
  {
    return {};
  }
  const QVariant& v = *it;
  return v.toString();
}
