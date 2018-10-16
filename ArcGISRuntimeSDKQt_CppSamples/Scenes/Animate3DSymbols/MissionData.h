#ifndef MISSIONDATA_H
#define MISSIONDATA_H

#include "Point.h"
#include <cmath>

class MissionData
{
public:

  struct DataPoint
  {
    DataPoint(){}
    DataPoint(double lon, double lat, double elevation, double heading, double pitch, double roll):
      m_pos(Esri::ArcGISRuntime::Point(lon, lat, elevation, Esri::ArcGISRuntime::SpatialReference::wgs84())),
      m_heading(heading),
      m_pitch(pitch),
      m_roll(roll){}

    Esri::ArcGISRuntime::Point m_pos;
    double m_heading = NAN;
    double m_pitch = NAN;
    double m_roll = NAN;
  };

  typedef std::vector<DataPoint> DataPointList;

  MissionData();
  ~MissionData();

  bool parse(const QString& dataPath);
  bool isEmpty() const {return m_data.empty();}
  size_t size() const {return m_data.size();}
  const DataPoint& dataAt(size_t i) const;
  bool ready() const {return m_ready;}

private:
  DataPointList m_data;
  bool m_ready;
};

#endif // MISSIONDATA_H
