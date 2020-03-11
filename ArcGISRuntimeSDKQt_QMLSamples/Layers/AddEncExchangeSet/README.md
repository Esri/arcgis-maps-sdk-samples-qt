# Add ENC exchange set

Display nautical charts per the ENC specification.

![](screenshot.png)

## Use case

The ENC specification describes how hydrographic data should be displayed digitally.

An ENC exchange set is a catalog of data files which can be loaded as cells. The cells contain information on how symbols should be displayed in relation to one another, so as to represent information such as depth and obstacles accurately.

## How it works

1. Specify the path to a local CATALOG.031 file to create an `EncExchangeSet`.
2. After loading the exchange set, get the `EncDataset` objects in the exchange set.
3. Create an `EncCell` for each dataset. Then create an `EncLayer` for each cell.
4. Add the ENC layer to a map's operational layers collection to display it.

## Relevant API

* EncCell
* EncDataset
* EncExchangeSet
* EncLayer

## Offline data
Read more about how to set up the sample's offline data [here](http://links.esri.com/ArcGISRuntimeQtSamples).

Link | Local Location
---------|-------|
|[Exchange Set](https://www.arcgis.com/home/item.html?id=9d2987a825c646468b3ce7512fb76e2d)| `<userhome>`/ArcGIS/Runtime/Data/ENC/ExchangeSetwithoutUpdates |
|[Hydrography](https://www.arcgis.com/home/item.html?id=af74ccbb69c846ef97085e4bebd3e76a)| `<userhome>`/ArcGIS/Runtime/Data/ENC/hydrography |

## Tags

Data, ENC, maritime, nautical chart, layers, hydrographic