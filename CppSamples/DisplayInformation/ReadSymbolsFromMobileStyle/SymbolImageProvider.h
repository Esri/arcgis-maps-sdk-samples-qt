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

#ifndef SYMBOLIMAGEPROVIDER_H
#define SYMBOLIMAGEPROVIDER_H

// Qt headers
#include <QHash>
#include <QImage>
#include <QQuickImageProvider>

class SymbolImageProvider : public QQuickImageProvider
{

public:
  SymbolImageProvider();
  ~SymbolImageProvider() override = default;

public:
  QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;
  void addImage(const QString& id, const QImage& img);
  static QString imageProviderId();

private:
  QHash<QString, QImage> m_images;
};

#endif // SYMBOLIMAGEPROVIDER_H
