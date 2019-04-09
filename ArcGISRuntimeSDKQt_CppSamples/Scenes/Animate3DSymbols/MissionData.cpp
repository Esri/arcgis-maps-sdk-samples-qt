// [WriteFile Name=Animate3DSymbols, Category=Scenes]
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

#include "MissionData.h"

#include <QFile>

// MissionData

MissionData::MissionData():
  m_ready(false)
{
}

MissionData::~MissionData() = default;

bool MissionData::parse(const QString& dataPath)
{
  m_data.clear();
  m_ready = false;

  QFile file(dataPath);
  if(!file.exists())
    return false;

  if (!file.open(QIODevice::ReadOnly))
    return false;

  while (!file.atEnd())
  {
    QByteArray line = file.readLine();
    QList<QByteArray> parts = line.split(',');
    if(parts.size() < 6)
      continue;

    bool ok = false;
    double lon = parts.at(0).toDouble(&ok);
    if(!ok)
      continue;

    double lat = parts.at(1).toDouble(&ok);
    if(!ok)
      continue;

    double elevation = parts.at(2).toDouble(&ok);
    if(!ok)
      continue;

    double heading = parts.at(3).toDouble(&ok);
    if(!ok)
      continue;

    double pitch = parts.at(4).toDouble(&ok);
    if(!ok)
      continue;

    double roll = parts.at(5).simplified().toDouble(&ok);
    if(!ok)
      continue;

    m_data.emplace_back((double)lon, (double)lat, (double)elevation, (double)heading, (double)pitch, (double)roll);
  }

  m_ready = m_data.size() > 0;
  return m_ready;
}

const MissionData::DataPoint& MissionData::dataAt(size_t i) const
{
  if(i < m_data.size())
    return m_data[i];

  static MissionData::DataPoint dataPoint;
  return dataPoint;
}
