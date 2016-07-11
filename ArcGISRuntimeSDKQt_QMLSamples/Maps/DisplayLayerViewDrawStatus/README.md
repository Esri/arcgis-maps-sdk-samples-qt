#DisplayLayerViewDrawStatus

This sample demonstrates how to get view status for layers in a map.

![](screenshot.png)

##How it works
MapView has a signal handler called onLayerViewStateChanged that get emitted every time a layer's view status changes.  The signal contains two arguments, namely, layer and layerViewState. The former being the Layer whose state got changed and the latter is a LayerViewState object defining the new state.

##Tips

##Features
