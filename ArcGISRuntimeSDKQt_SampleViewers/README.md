# Overview

ArcGIS Runtime SDK 100.14 for Qt sample viewer applications.

## Prerequisites

* Qt SDK 5.15.2 or higher (Open source or commercial)
* Qt Creator
* [ArcGIS Runtime Samples - Qt](https://github.com/Esri/arcgis-runtime-samples-qt)
* [ArcGIS Runtime Toolkit - Qt](https://github.com/Esri/arcgis-runtime-toolkit-qt) is required as some samples utilize toolkit components.
* For more information, please visit the [System Requirements](https://developers.arcgis.com/qt/reference/system-requirements/) page.

## Fork the repo

If you haven't already, fork [the repo](https://github.com/Esri/arcgis-runtime-sample-viewer-qt/fork).

## Clone the repo

### Command line Git

[Clone the ArcGIS Qt SDK Samples](https://help.github.com/articles/fork-a-repo#Step-2-clone-your-fork)

Open your terminal, navigate to your ArcGIS Runtime Samples Qt directory, use ```git clone``` to get a copy of the repo.

```shell
# Clone your fork of the repository into the current directory in terminal
$ git clone https://github.com/YOUR_USERNAME/arcgis-runtime-sample-viewer-qt.git
```

## Configure remote upstream for your fork

To sync changes you make in a fork with this repository, you must configure a remote that points to the upstream repository in Git.

* Open a terminal (Mac users) or command prompt (Windows & Linux users)
* List the current configured remote repository for your fork

```shell
$ git remote -v
origin https://github.com/YOUR_USERNAME/arcgis-runtime-sample-viewer-qt.git (fetch)
origin https://github.com/YOUR_USERNAME/arcgis-runtime-sample-viewer-qt.git (push)
```

* Specify a new remote upstream repository

```shell
$ git remote add upstream https://github.com/Esri/arcgis-runtime-sample-viewer-qt.git
```

* Verify the new upstream repository

```shell
$ git remote -v

origin https://github.com/YOUR_USERNAME/arcgis-runtime-sample-viewer-qt.git (fetch)
origin https://github.com/YOUR_USERNAME/arcgis-runtime-sample-viewer-qt.git (push)
upstream https://github.com/Esri/arcgis-runtime-sample-viewer-qt.git (fetch)
upstream https://github.com/Esri/arcgis-runtime-sample-viewer-qt.git (push)
```

### Sync your fork

Once you have set up a remote upstream you can keep your fork up to date with our samples repository by syncing your fork.

* Open a terminal (Mac users) or command prompt (Windows & Linux users)
* Change to the current working directory of your local repository
* Fetch the branches and commits from the upstream repository.  Commits to ```main``` will be stored in a local branch, ```upstream/main```.

```shell
$ git fetch upstream
```

* Check out your forks local ```main``` branch

```shell
$ git checkout main
```

* Merge changes from ```upstream/main``` into  your local ```main``` branch which syncs your forks ```main``` branch with our samples repository.

```shell
$ git merge upstream/main
```

## Build a sample viewer locally

Start Qt Creator. When the IDE opens to the Welcome screen, click on the **Open Project** button and browse to a project file (.pro) within your forked repo location.
Configure the project and run the sample.

## Issues

Find a bug or want to request a new feature?  Please let us know by submitting an issue.

## Contributing

Esri welcomes contributions from anyone and everyone. Please see our [guidelines for contributing](https://github.com/esri/contributing).

## Licensing

Copyright 2020 Esri

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

A copy of the license is available in the repository's [license.txt]( https://github.com/Esri/arcgis-runtime-samples-qt/blob/main/license.txt) file
