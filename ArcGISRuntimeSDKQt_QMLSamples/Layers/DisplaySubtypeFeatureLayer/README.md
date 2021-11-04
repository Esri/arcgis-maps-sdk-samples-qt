# Display subtype feature layer

Displays a composite layer of all the subtype values in a feature class.

![](screenshot.png)

## Use case

This is useful for controlling labeling, visibility and symbology of a given subtype as though they are distinct layers on the map.

## How to use the sample

The sample loads with the sublayer visible on the map. Toggle its visibility with the "Show sublayer" checkbox. Change the sublayer's renderer with the radio buttons, using "Show original renderer" or "Show alternative renderer", and set its minimum scale using the "Set sublayer minimum scale" button. This will set the sublayer's minimum scale to that of the current map scale. Zoom in and out to see the sublayer become visible based on its new scale range.

## How it works

1. Create a `SubtypeFeatureLayer` from a `ServiceFeatureTable` that defines a subtype, and add it to the `Map`.
2. Get a `SubtypeSublayer` from the subtype feature using its name.
3. Enable the sublayer's labels and define them with `LabelDefinitions`.
4. Set the visibility status by setting this sublayer's `visible` property.
5. Change the sublayer's symbology with `SubtypeSublayer.renderer`.
6. Update the sublayer's minimum scale value with `SubtypeSublayer.minScale`.

## Relevant API

* LabelDefinition
* ServiceFeatureTable
* SimpleLabelExpression
* SubtypeFeatureLayer
* SubtypeSublayer
* TextSymbol

## About the data

The [Naperville electrical](https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer/100) network feature service, hosted on ArcGIS Online (authentication required: this is handled within the sample code), contains a utility network with asset classification for different devices.

## Additional information

Using utility network on ArcGIS Enterprise 10.8 requires an ArcGIS Enterprise member account licensed with the [Utility Network user type extension](https://enterprise.arcgis.com/en/portal/latest/administer/windows/license-user-type-extensions.htm#ESRI_SECTION1_41D78AD9691B42E0A8C227C113C0C0BF). Please refer to the [utility network services documentation](https://enterprise.arcgis.com/en/server/latest/publish-services/windows/utility-network-services.htm).

## Additional information

Credentials:
* Username: viewer01
* Password: I68VGU^nMurF

## Tags

asset group, feature layer, labeling, sublayer, subtype, symbology, utility network, visible scale range
