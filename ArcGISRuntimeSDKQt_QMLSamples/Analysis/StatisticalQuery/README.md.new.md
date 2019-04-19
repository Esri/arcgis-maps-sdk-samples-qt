# Statistical query

This sample demonstrates how to query a table to get aggregated
statistics back for a specific field.

![](screenshot.png)

## How it works

A `ServiceFeatureTable` is created with a URL to the REST endpoint of a
feature service. `StatisticsQueryParameters` are created, and several
`StatisticDefinition` objects are created and added to the parameters.
These definitions define the various statistics that we would like to
compute, such as average, minimum, and maximum of a given field. When
the button is clicked, `queryStatistics` is executed on the
`ServiceFeatureTable`. Depending on the state of two checkboxes,
additional paramters are set. This process runs asynchronously, and once
complete, this gives access to the `QueryStatisticsResult`, which
contains key/value pairs as the result.
