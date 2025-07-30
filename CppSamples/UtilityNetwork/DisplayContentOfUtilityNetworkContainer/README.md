# Display content of utility network container

A utility network container allows a dense collection of features to be represented by a single feature, which can be used to reduce map clutter.

![](screenshot.png)

## Use case

Offering a container view for features aids in the review for valid structural attachment and containment relationships and helps determine if a dataset has an association role set. Container views often model a cluster of electrical devices on a pole top or inside a cabinet or vault.

## How to use the sample

Select a container feature to show all features inside the container. The container is shown as a polygon graphic with the content features contained within. The viewpoint and scale of the map are also changed to the container's extent. Connectivity and attachment associations inside the container are shown as red and blue dotted lines respectively.

## How it works

1. Load a web map that includes ArcGIS Pro [Subtype Group Layers](https://pro.arcgis.com/en/pro-app/latest/help/mapping/layer-properties/subtype-layers.htm) with only container features visible (i.e. fuse bank, switch bank, transformer bank, hand hole and junction box).
2. Get and load the first `UtilityNetwork` from the web map.
3. Add a `GraphicsOverlay` for displaying a container view.
4. Create a connection to `MapQuickView::mouseClicked`.
5. Identify a feature with `MapView::identifyLayersAsync` and create a `UtilityElement` from it.
6. Get the associations for this element using `UtilityNetwork::associationsAsync(UtilityElement *element, UtilityAssociationType::Containment)`.
7. Turn-off the visibility of all `OperationalLayers`.
8. Get the features for the `UtilityElement`(s) from the associations using `UtilityNetwork::featuresForElementsAsync(const QList<UtilityElement *> &elements)`
9. Add a `Graphic` with the geometry and symbol of each feature to the `GraphicsOverlay`.
10. Get associations for the extent of the `GraphicsOverlay` using `UtilityNetwork::associationsAsync(const Envelope &extent)`
11. Add a `Graphic` to represent the association geometry between container features using a symbol that distinguishes between `Attachment` and `Connectivity` association type.
12. Add another `Graphic` that represents this extent and zoom to this extent with some buffer.

## Relevant API

* SubtypeFeatureLayer
* UtilityAssociation
* UtilityAssociationType
* UtilityElement
* UtilityNetwork

## About the data

The [Naperville electric](https://sampleserver7.arcgisonline.com/server/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer) network feature service, hosted on ArcGIS Online (authentication required: this is handled within the sample code), contains a utility network used to find associations shown in this sample and a web map portal item, [Naperville electric containers](https://sampleserver7.arcgisonline.com/portal/home/item.html?id=813eda749a9444e4a9d833a4db19e1c8), that use the same feature service endpoint and displays only container features.

## Additional information

Using utility network on ArcGIS Enterprise 10.8 requires an ArcGIS Enterprise member account licensed with the [Utility Network user type extension](https://enterprise.arcgis.com/en/portal/latest/administer/windows/license-user-type-extensions.htm#ESRI_SECTION1_41D78AD9691B42E0A8C227C113C0C0BF). Please refer to the [utility network services documentation](https://enterprise.arcgis.com/en/server/latest/publish-services/windows/utility-network-services.htm).

## Tags

associations, connectivity association, containment association, structural attachment associations, utility network
