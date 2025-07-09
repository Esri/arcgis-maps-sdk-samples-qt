# Trace utility network

Discover connected features in a utility network using connected, subnetwork, upstream, and downstream traces.

![](screenshot.png)

## Use case

You can use a trace to visualize and validate the network topology of a utility network for quality assurance. Subnetwork traces are used for validating whether subnetworks, such as circuits or zones, are defined or edited appropriately.

## How to use the sample

Tap on one or more features while 'Add starting locations' or 'Add barriers' is selected. When a junction feature is identified, you may be prompted to select a terminal. When an edge feature is identified, the distance from the tapped location to the beginning of the edge feature will be computed. Select the type of trace using the drop down menu. Click 'Trace' to initiate a trace on the network. Click 'Reset' to clear the trace parameters and start over.

## How it works

1. Create a `MapView` and connect to its `mouseClicked` signal.
2. Create and load a `Map` with a web map item URL that contains a `UtilityNetwork`.
3. Get and load the first `UtilityNetwork` from the web map.
4. Get the `ServiceGeodatabase` from the utility network and fetch the line `FeatureLayer` from the `ServiceGeodatabase`'s tables.
5. Add a `GraphicsOverlay` with symbology that distinguishes starting locations from barriers.
6. Identify features on the map and add a `Graphic` that represents its purpose (starting point or barrier) at the location of each identified feature.
7.  Create a `UtilityElement` for the identified feature.
8.  Determine the type of this element using its `NetworkSource::SourceType` property.
9.  If the element is a junction with more than one terminal, display a terminal picker. Then set the junction's `terminal` property with the selected terminal.
10.  If an edge, set its `fractionAlongEdge` property using `GeometryEngine::fractionAlong`.
11. Add this `UtilityElement` to a collection of starting locations or barriers.
12. Create `UtilityTraceParameters` with the selected trace type along with the collected starting locations and barriers (if applicable).
13. Set the `UtilityTraceParameters::traceConfiguration` with the utility tier's `traceConfiguration` property.
14. Run a `UtilityNetwork::traceAsync` with the specified parameters.
15. For every `FeatureLayer` in the map, select the features using the `UtilityElement::objectId` from the filtered list of `UtilityElementTraceResult::elements`.

## Relevant API

* GeometryEngine::fractionAlong
* ServiceGeodatabase
* UtilityAssetType
* UtilityDomainNetwork
* UtilityElement
* UtilityElementTraceResult
* UtilityNetwork
* UtilityNetworkDefinition
* UtilityNetworkSource
* UtilityTerminal
* UtilityTier
* UtilityTraceConfiguration
* UtilityTraceParameters
* UtilityTraceResult
* UtilityTraceType
* UtilityTraversability

## About the data

The [Naperville Electric Web Map](https://sampleserver7.arcgisonline.com/portal/home/item.html?id=be0e4637620a453584118107931f718b), hosted on ArcGIS Online (authentication required: this is handled within the sample code), contains a utility network used to run the subnetwork-based trace shown in this sample.

## Additional information

Using utility network on ArcGIS Enterprise 10.8 requires an ArcGIS Enterprise member account licensed with the [Utility Network user type extension](https://enterprise.arcgis.com/en/portal/latest/administer/windows/license-user-type-extensions.htm#ESRI_SECTION1_41D78AD9691B42E0A8C227C113C0C0BF). Please refer to the [utility network services documentation](https://enterprise.arcgis.com/en/server/latest/publish-services/windows/utility-network-services.htm).

Credentials:
* Username: viewer01
* Password: I68VGU^nMurF

## Tags

condition barriers, downstream trace, network analysis, subnetwork trace, trace configuration, traversability, upstream trace, utility network, validate consistency
