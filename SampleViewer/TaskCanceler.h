// [Legal]
// Copyright 2024 Esri.

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

#ifndef TaskCanceler_H
#define TaskCanceler_H

// Qt headers
#include <QFutureWatcher>

#include <memory>
#include <mutex>
#include <unordered_set>

// This class is a helper to ensure any QFuture-based async tasks are
// canceled upon destruction. This is useful to ensure QFuture
// continuations aren't being executed at the same time as samples are being
// destructed

class TaskCanceler
{
public:
  TaskCanceler() = default;
  ~TaskCanceler()
  {
    cancelAllTasks_();
  }

  template <typename T>
  void addTask(const QFuture<T>& future)
  {
    auto watcher = std::make_shared<QFutureWatcher<T>>();

    QObject::connect(watcher.get(), &QFutureWatcher<T>::finished, &m_connectionContext, [this, watcher]()
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_watchers.erase(watcher);
    });
    watcher->setFuture(future);

    std::lock_guard<std::mutex> lock(m_mutex);
    m_watchers.insert(watcher);
  }

private:
  void cancelAllTasks_()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& watcher : m_watchers)
    {
      if (watcher->isRunning())
      {
        watcher->cancel();
      }
    }
  }

  std::unordered_set<std::shared_ptr<QFutureWatcherBase>> m_watchers;
  std::mutex m_mutex;
  QObject m_connectionContext;
};

#endif // TaskCanceler_H
