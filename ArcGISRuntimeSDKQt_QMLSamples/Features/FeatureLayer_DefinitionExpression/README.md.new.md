# Feature layer definition expression

This sample demonstrates how to limit the features to display on the map
using a definition expression. Press the Apply Expression button to
select only features requested using a definition expression.

![](screenshot.png)

## How it works

Clicking the button will set the `definitionExpression` property to be a
where clause string on the `FeatureLayer`. This filters the view based
on features that satisfy the specified condition in the where clause.
