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

#ifndef MAPIMAGEPROVIDER_H
#define MAPIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QMap>
#include <QImage>

class MapImageProvider : public QQuickImageProvider
{

public:
  MapImageProvider();
  ~MapImageProvider() override = default;

public:
  QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;
  void addImage(const QString& id, const QImage& img);
  static QString imageProviderId();

private:
  QMap<QString, QImage> m_images;
};

#endif // MAPIMAGEPROVIDER_H
