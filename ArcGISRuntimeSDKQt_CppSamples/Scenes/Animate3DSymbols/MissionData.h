// [WriteFile Name=Animate3DSymbols, Category=Scenes]
// [Legal]
// Copyright 2016 Esri.

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

#ifndef MISSIONDATA_H
#define MISSIONDATA_H

#include <vector>

class QString;

class MissionData
{
public:

  struct DataPoint
  {
    DataPoint(double lon, double lat, double elevation, double heading, double pitch, double roll):
      m_lon(lon),
      m_lat(lat),
      m_elevation(elevation),
      m_heading(heading),
      m_pitch(pitch),
      m_roll(roll){}

    double m_lon;
    double m_lat;
    double m_elevation;
    double m_heading;
    double m_pitch;
    double m_roll;
  };

  typedef std::vector< DataPoint > DataPointList;

  MissionData();

  bool parse(const QString& dataPath);

  bool isEmpty() const {return m_data.empty();}
  size_t size() const {return m_data.size();}
  const DataPoint& dataAt(size_t i) const;

private:

  DataPointList m_data;
  static DataPoint m_nullData;
};

#endif // MISSIONDATA_H
