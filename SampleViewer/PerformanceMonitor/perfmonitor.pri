#-------------------------------------------------

# [Legal]
# Copyright 2026 Esri.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# [Legal]

#-------------------------------------------------

# Included only from the perfmonitor block in SampleViewer.pro. Nothing here is built otherwise.

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/PerfMetricsModel.h \
    $$PWD/PerformanceMonitor.h

SOURCES += \
    $$PWD/PerfMetricsModel.cpp \
    $$PWD/PerformanceMonitor.cpp

RESOURCES += $$PWD/perf.qrc
