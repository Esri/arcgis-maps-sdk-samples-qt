# Update labels and symbols to scale for visual accessibility

Scale feature labels and symbols according to the system text-size setting.

![](screenshot.png)

## Use case

Use this pattern to improve map readability for users who increase text size in their operating system’s accessibility settings. Enable `MapView.useSystemTextScale` to scale feature labels automatically.

## How to use the sample

Change the text size in the operating system's accessibility settings to see the restaurant labels resize. Restart the sample to see the restaurant symbols resize. On iOS, open **Settings** > **Accessibility** > **Display & Text Size** > **Larger Text**.

Clear **Apply system text size to labels** to stop labels from following the system text size, then select it again to restore system text scaling. Restaurant symbols continue to follow the system text size independently of this setting. The legend and current values show how labels and symbols are scaled.

Select a restaurant to show its name and WGS 84 coordinates in a callout. Select elsewhere to clear the callout.

## How it works

1. Create a `Map` and add a `FeatureLayer`.
2. Load the `restaurant` symbol from `Esri2DPointSymbolsStyle` and apply it with a `SimpleRenderer`.
3. Load the feature layer, then add a `LabelDefinition` with a `TextSymbol`.
4. To scale labels, enable `MapView.useSystemTextScale`.
5. Use the checkbox to toggle system text scaling for labels without changing symbol scaling.
6. Identify a restaurant with `GeoView::identifyLayerAsync` and show its name and WGS 84 coordinates in a callout.

## Relevant API

- MapView.useSystemTextScale

## About the data

This sample uses a [Redlands restaurants](https://www.arcgis.com/home/item.html?id=46119989eccd46a58b8f3d7aedadeb90) feature layer covering food establishments in Redlands, California. Each feature represents a single restaurant.

The restaurant symbol comes from [Esri's 2D point symbol web style](https://www.arcgis.com/home/item.html?id=220936cc6ed342c9937abd8f180e7d1e).

## Additional information

`MapView.useSystemTextScale` is supported on iOS, Android, Windows, and Linux (GNOME desktop environments only). It is not supported on macOS.

The symbol size and callout text are based on the application font, which Qt reads from the system text size when the app starts. Restart the sample after changing the system text size to update them.

## Tags

accessibility, label, readability, scale, symbol, text, visual impairment
