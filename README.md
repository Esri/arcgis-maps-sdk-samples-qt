# Overview
ArcGIS Runtime SDK 100.9 for Qt sample applications.  The repo contains [Qt](http://qt.io) projects for each sample that can be run from within the Qt Creator IDE.

# Prerequisites
* Qt SDK 5.15 or higher (Open source or commercial)
* Qt Creator
* For more information, please visit the [System Requirements](https://developers.arcgis.com/qt/latest/qml/guide/arcgis-runtime-sdk-for-qt-system-requirements.htm) page.

## Fork the repo
If you haven't already, fork [the repo](https://github.com/Esri/arcgis-runtime-samples-qt/fork).

## Clone the repo

### Command line Git
[Clone the ArcGIS Qt SDK Samples](https://help.github.com/articles/fork-a-repo#Step-2-clone-your-fork)

Open your terminal, navigate to your working directory, use ```git clone``` to get a copy of the repo.

```
# Clone your fork of the repository into the current directory in terminal
$ git clone https://github.com/YOUR-USERNAME/arcgis-runtime-samples-qt.git
```

## Configure remote upstream for your fork
To sync changes you make in a fork with this repository, you must configure a remote that points to the upstream repository in Git.

- Open a terminal (Mac users) or command prompt (Windows & Linux users)
- List the current configured remote repository for your fork

```
$ git remote -v
origin	https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git (fetch)
origin	https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git (push)
```

- Specify a new remote upstream repository

```
$ git remote add upstream https://github.com/Esri/arcgis-runtime-samples-qt.git
```

- Verify the new upstream repository

```
$ git remote -v

origin	https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git (fetch)
origin	https://github.com/YOUR_USERNAME/arcgis-runtime-samples-qt.git (push)
upstream https://github.com/Esri/arcgis-runtime-samples-qt.git (fetch)
upstream https://github.com/Esri/arcgis-runtime-samples-qt.git (push)
```

### Sync your fork
Once you have set up a remote upstream you can keep your fork up to date with our samples repository by syncing your fork.

- Open a terminal (Mac users) or command prompt (Windows & Linux users)
- Change to the current working directory of your local repository
- Fetch the branches and commits from the upstream repository.  Commits to ```master``` will be stored in a local branch, ```upstream/master```.

```
$ git fetch upstream
```

- Check out your forks local ```master``` branch

```
$ git checkout master
```

- Merge changes from ```upstream/master``` into  your local ```master``` branch which syncs your forks ```master``` branch with our samples repository.

```
$ git merge upstream/master
```
## Open a sample project file in Qt Creator
Start Qt Creator. When the IDE opens to the Welcome screen, click on the **Open Project** button and browse to a project file (.pro) within your forked repo location.
Configure the project and run the sample.

## Use a previous version
The `master` branch contains samples for the current, most up-to-date version of ArcGIS Runtime. To access previous versions, you can checkout by a tag or commit, or alternativley you can download the source zip:

- [100.0](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/1529)
- [100.1](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/1744)
- [100.2.1](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/100.2.1)
- [100.3](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/2102)
- [100.4](https://github.com/Esri/arcgis-runtime-samples-qt/releases/tag/100.4.0.2207)

## Use offline data in the samples
Some of the samples consume offline data, including TPKs, VTPKs, mobile geodatabases, and mobile map packages, among other data formats. If the sample does consume offline data, the sample's associated readme.md will note where you can download the data (from ArcGIS Online), and where the data can be placed once downloaded. For example, it may say something like:

Link | Local Location
---------|-------|
|[Los Angeles Vector Tile Package](https://www.arcgis.com/home/item.html?id=d9f8ce6f6ac84b90a665a861d71a5d0a)| `<userhome>`/ArcGIS/Runtime/Data/vtpk/LosAngeles.vtpk |

In this case, you would download the VTPK from the ArcGIS Online Location, and place in specified location. `<userhome>` is used throughout to denote a generic location on the different platforms. The below table specifies where this is on all of the supported platforms:

Platform |  Userhome Location | Example Location
---------|--------------------|------------------|
Windows  | `%userprofile%`    | `C:\Users\Bob\ArcGIS\Runtime\Data` |
Linux    | `~`                | `/users/bob/ArcGIS/Runtime/Data`   |
Mac OS X | `~`                | `/Users/Bob/ArcGIS/Runtime/Data`   |
Android  | `/sdcard`          | `/sdcard/ArcGIS/Runtime/Data`      |
iOS      | `<your app bundle>`| `<appbundle>/ArcGIS/Runtime/Data`  |

## Resources

* [ArcGIS Runtime QtSDK Resource Center](https://developers.arcgis.com/qt/latest/)
* [Qt and QML](http://www.qt.io/)
* [ArcGIS Blog](http://blogs.esri.com/esri/arcgis/)
* [twitter@esri](http://twitter.com/esri)

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


A copy of the license is available in the repository's [license.txt]( https://github.com/Esri/arcgis-runtime-samples-qt/blob/master/license.txt) file
