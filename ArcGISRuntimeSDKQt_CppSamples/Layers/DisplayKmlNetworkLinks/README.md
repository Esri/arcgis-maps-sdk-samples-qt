# Display KML network links

KML files can reference other KML files on the network and support automatically refreshing content. This sample demonstrates how to display a file with a network link, including displaying any network link control messages at launch.

![](screenshot.png)

## How to use the sample

The sample will load the KML file automatically. The data shown should refresh automatically every few seconds.

The message button in the top right hand corner of the sample will show the last message recieved from the KML network reource.

## How it works

In general, no special code is needed to support showing KML/KMZ files with network links. KML supports specifying a message to show the user when loading a network link. Apps can optionally listen to the `KmlDataset.NetworkLinkControlMessage` event to show any messages from KML network link controls.

## Relevant API

* `KmlDataset(Uri)`
* `KmlLayer(KmlDataset)`
* `KmlNetworkLinkControlMessageEventArgs`
* `KmlDataset.NetworkLinkControlMessage`

## About the data

This map shows the current air traffic in parts of Europe with heading, altitude, and ground speed. Additionally, noise levels from ground monitoring stations are shown.

## Tags

KML, KMZ, OGC, Keyhole, Network Link, Network Link Control
