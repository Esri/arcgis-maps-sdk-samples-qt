#-------------------------------------------------
#  Copyright 2017 ESRI
#
#  All rights reserved under the copyright laws of the United States
#  and applicable international laws, treaties, and conventions.
#
#  You may freely redistribute and use this sample code, with or
#  without modification, provided you include the original copyright
#  notice and use restrictions.
#
#  See the Sample code usage restrictions document for further information.
#-------------------------------------------------
HEADERS += \
    "$$SAMPLEPATHQML/Routing/NavigateRoute/NavigateRouteSpeaker.h"

INCLUDEPATH += \
    "$$SAMPLEPATHQML/Routing/NavigateRoute"

SOURCES += \
    "$$SAMPLEPATHQML/Routing/NavigateRoute/NavigateRouteSpeaker.cpp"

RESOURCES += \
    "$$SAMPLEPATHQML/Analysis/AnalyzeHotspots/AnalyzeHotspots.qrc" \
    "$$SAMPLEPATHQML/Analysis/DistanceMeasurementAnalysis/DistanceMeasurementAnalysis.qrc" \
    "$$SAMPLEPATHQML/Analysis/AnalyzeViewshed/AnalyzeViewshed.qrc" \
    "$$SAMPLEPATHQML/Analysis/Geotriggers/Geotriggers.qrc" \
    "$$SAMPLEPATHQML/Analysis/LineOfSightGeoElement/LineOfSightGeoElement.qrc" \
    "$$SAMPLEPATHQML/Analysis/LineOfSightLocation/LineOfSightLocation.qrc" \
    "$$SAMPLEPATHQML/Analysis/StatisticalQuery/StatisticalQuery.qrc" \
    "$$SAMPLEPATHQML/Analysis/StatisticalQueryGroupSort/StatisticalQueryGroupSort.qrc" \
    "$$SAMPLEPATHQML/Analysis/ViewshedCamera/ViewshedCamera.qrc" \
    "$$SAMPLEPATHQML/Analysis/ViewshedGeoElement/ViewshedGeoElement.qrc" \
    "$$SAMPLEPATHQML/Analysis/ViewshedLocation/ViewshedLocation.qrc" \
    "$$SAMPLEPATHQML/CloudAndPortal/IntegratedWindowsAuthentication/IntegratedWindowsAuthentication.qrc" \
    "$$SAMPLEPATHQML/CloudAndPortal/TokenAuthentication/TokenAuthentication.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/AddGraphicsWithRenderer/AddGraphicsWithRenderer.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/BuildLegend/BuildLegend.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/ControlAnnotationSublayerVisibility/ControlAnnotationSublayerVisibility.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/CreateSymbolStylesFromWebStyles/CreateSymbolStylesFromWebStyles.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/CustomDictionaryStyle/CustomDictionaryStyle.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/DisplayGrid/DisplayGrid.qrc" \
    # "$$SAMPLEPATHQML/DisplayInformation/GODictionaryRenderer_3D/GODictionaryRenderer_3D.qrc" \ # Qt 6.2 does not support getting entries from an xml list
    # "$$SAMPLEPATHQML/DisplayInformation/GODictionaryRenderer/GODictionaryRenderer.qrc" \ # Qt 6.2 does not support getting entries from an xml list
    "$$SAMPLEPATHQML/DisplayInformation/GOSymbols/GOSymbols.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/IdentifyGraphics/IdentifyGraphics.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/Picture_Marker_Symbol/Picture_Marker_Symbol.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/QueryFeaturesWithArcadeExpression/QueryFeaturesWithArcadeExpression.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/ReadSymbolsFromMobileStyle/ReadSymbolsFromMobileStyle.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/ShowCallout/ShowCallout.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/ShowLabelsOnLayers/ShowLabelsOnLayers.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/ShowPopup/ShowPopup.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/Simple_Marker_Symbol/Simple_Marker_Symbol.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/Simple_Renderer/Simple_Renderer.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/SketchOnMap/SketchOnMap.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/SymbolizeShapefile/SymbolizeShapefile.qrc" \
    "$$SAMPLEPATHQML/DisplayInformation/Unique_Value_Renderer/Unique_Value_Renderer.qrc" \
    "$$SAMPLEPATHQML/EditData/AddFeaturesFeatureService/AddFeaturesFeatureService.qrc" \
    "$$SAMPLEPATHQML/EditData/ContingentValues/ContingentValues.qrc" \
    "$$SAMPLEPATHQML/EditData/DeleteFeaturesFeatureService/DeleteFeaturesFeatureService.qrc" \
    "$$SAMPLEPATHQML/EditData/EditAndSyncFeatures/EditAndSyncFeatures.qrc" \
    "$$SAMPLEPATHQML/EditData/EditFeatureAttachments/EditFeatureAttachments.qrc" \
    "$$SAMPLEPATHQML/EditData/EditFeaturesWithFeatureLinkedAnnotation/EditFeaturesWithFeatureLinkedAnnotation.qrc" \
    "$$SAMPLEPATHQML/EditData/EditKmlGroundOverlay/EditKmlGroundOverlay.qrc" \
    "$$SAMPLEPATHQML/EditData/EditWithBranchVersioning/EditWithBranchVersioning.qrc" \
    "$$SAMPLEPATHQML/EditData/UpdateAttributesFeatureService/UpdateAttributesFeatureService.qrc" \
    "$$SAMPLEPATHQML/EditData/UpdateGeometryFeatureService/UpdateGeometryFeatureService.qrc" \
    "$$SAMPLEPATHQML/Features/ControlTimeExtentTimeSlider/ControlTimeExtentTimeSlider.qrc" \
    "$$SAMPLEPATHQML/Features/CreateMobileGeodatabase/CreateMobileGeodatabase.qrc" \
    "$$SAMPLEPATHQML/Features/FeatureLayer_ChangeRenderer/FeatureLayer_ChangeRenderer.qrc" \
    "$$SAMPLEPATHQML/Features/FeatureLayer_DictionaryRenderer/FeatureLayer_DictionaryRenderer.qrc" \
    "$$SAMPLEPATHQML/Features/FeatureLayer_Query/FeatureLayer_Query.qrc" \
    "$$SAMPLEPATHQML/Features/FeatureLayer_Selection/FeatureLayer_Selection.qrc" \
    "$$SAMPLEPATHQML/Features/FilterByDefinitionExpressionOrDisplayFilter/FilterByDefinitionExpressionOrDisplayFilter.qrc" \
    "$$SAMPLEPATHQML/Features/GenerateGeodatabaseReplicaFromFeatureService/GenerateGeodatabaseReplicaFromFeatureService.qrc" \
    "$$SAMPLEPATHQML/Features/ListRelatedFeatures/ListRelatedFeatures.qrc" \
    "$$SAMPLEPATHQML/Features/ServiceFeatureTable_Cache/ServiceFeatureTable_Cache.qrc" \
    "$$SAMPLEPATHQML/Features/ServiceFeatureTable_ManualCache/ServiceFeatureTable_ManualCache.qrc" \
    "$$SAMPLEPATHQML/Features/ServiceFeatureTable_NoCache/ServiceFeatureTable_NoCache.qrc" \
    "$$SAMPLEPATHQML/Geometry/Buffer/Buffer.qrc" \
    "$$SAMPLEPATHQML/Geometry/ClipGeometry/ClipGeometry.qrc" \
    "$$SAMPLEPATHQML/Geometry/ConvexHull/ConvexHull.qrc" \
    "$$SAMPLEPATHQML/Geometry/CreateAndEditGeometries/CreateAndEditGeometries.qrc" \
    "$$SAMPLEPATHQML/Geometry/CreateGeometries/CreateGeometries.qrc" \
    "$$SAMPLEPATHQML/Geometry/CutGeometry/CutGeometry.qrc" \
    "$$SAMPLEPATHQML/Geometry/DensifyAndGeneralize/DensifyAndGeneralize.qrc" \
    "$$SAMPLEPATHQML/Geometry/FormatCoordinates/FormatCoordinates.qrc" \
    "$$SAMPLEPATHQML/Geometry/GeodesicOperations/GeodesicOperations.qrc" \
    "$$SAMPLEPATHQML/Geometry/ListTransformations/ListTransformations.qrc" \
    "$$SAMPLEPATHQML/Geometry/NearestVertex/NearestVertex.qrc" \
    "$$SAMPLEPATHQML/Geometry/ProjectGeometry/ProjectGeometry.qrc" \
    "$$SAMPLEPATHQML/Geometry/SpatialOperations/SpatialOperations.qrc" \
    "$$SAMPLEPATHQML/Geometry/SpatialRelationships/SpatialRelationships.qrc" \
    "$$SAMPLEPATHQML/Layers/AddDynamicEntityLayer/AddDynamicEntityLayer.qrc" \
    "$$SAMPLEPATHQML/Layers/AddEncExchangeSet/AddEncExchangeSet.qrc" \
    "$$SAMPLEPATHQML/Layers/ApplyMosaicRuleToRasters/ApplyMosaicRuleToRasters.qrc" \
    "$$SAMPLEPATHQML/Layers/ApplyUniqueValuesWithAlternateSymbols/ApplyUniqueValuesWithAlternateSymbols.qrc" \
    "$$SAMPLEPATHQML/Layers/ArcGISMapImageLayerUrl/ArcGISMapImageLayerUrl.qrc" \
    "$$SAMPLEPATHQML/Layers/ArcGISTiledLayerUrl/ArcGISTiledLayerUrl.qrc" \
    "$$SAMPLEPATHQML/Layers/BlendRasterLayer/BlendRasterLayer.qrc" \
    "$$SAMPLEPATHQML/Layers/BrowseOGCAPIFeatureService/BrowseOGCAPIFeatureService.qrc" \
    "$$SAMPLEPATHQML/Layers/BrowseWfsLayers/BrowseWfsLayers.qrc" \
    "$$SAMPLEPATHQML/Layers/ChangeSublayerRenderer/ChangeSublayerRenderer.qrc" \
    "$$SAMPLEPATHQML/Layers/ChangeSublayerVisibility/ChangeSublayerVisibility.qrc" \
    "$$SAMPLEPATHQML/Layers/CreateAndSaveKmlFile/CreateAndSaveKmlFile.qrc" \
    "$$SAMPLEPATHQML/Layers/DisplayAnnotation/DisplayAnnotation.qrc" \
    "$$SAMPLEPATHQML/Layers/DisplayDimensions/DisplayDimensions.qrc" \
    "$$SAMPLEPATHQML/Layers/DisplayFeatureLayers/DisplayFeatureLayers.qrc" \
    "$$SAMPLEPATHQML/Layers/DisplayKml/DisplayKml.qrc" \
    "$$SAMPLEPATHQML/Layers/DisplayKmlNetworkLinks/DisplayKmlNetworkLinks.qrc" \
    "$$SAMPLEPATHQML/Layers/DisplayOgcApiFeatureCollection/DisplayOgcApiFeatureCollection.qrc" \
    "$$SAMPLEPATHQML/Layers/DisplaySubtypeFeatureLayer/DisplaySubtypeFeatureLayer.qrc" \
    "$$SAMPLEPATHQML/Layers/DisplayWfsLayer/DisplayWfsLayer.qrc" \
    "$$SAMPLEPATHQML/Layers/ExportTiles/ExportTiles.qrc" \
    "$$SAMPLEPATHQML/Layers/ExportVectorTiles/ExportVectorTiles.qrc" \
    "$$SAMPLEPATHQML/Layers/Feature_Collection_Layer/Feature_Collection_Layer.qrc" \
    "$$SAMPLEPATHQML/Layers/FeatureCollectionLayerFromPortal/FeatureCollectionLayerFromPortal.qrc" \
    "$$SAMPLEPATHQML/Layers/FeatureCollectionLayerQuery/FeatureCollectionLayerQuery.qrc" \
    "$$SAMPLEPATHQML/Layers/FeatureLayerRenderingModeMap/FeatureLayerRenderingModeMap.qrc" \
    "$$SAMPLEPATHQML/Layers/FeatureLayerRenderingModeScene/FeatureLayerRenderingModeScene.qrc" \
    "$$SAMPLEPATHQML/Layers/GroupLayers/GroupLayers.qrc" \
    "$$SAMPLEPATHQML/Layers/Hillshade_Renderer/Hillshade_Renderer.qrc" \
    "$$SAMPLEPATHQML/Layers/IdentifyKmlFeatures/IdentifyKmlFeatures.qrc" \
    "$$SAMPLEPATHQML/Layers/IdentifyRasterCell/IdentifyRasterCell.qrc" \
    "$$SAMPLEPATHQML/Layers/ListKmlContents/ListKmlContents.qrc" \
    "$$SAMPLEPATHQML/Layers/LoadWfsXmlQuery/LoadWfsXmlQuery.qrc" \
    "$$SAMPLEPATHQML/Layers/ManageOperationalLayers/ManageOperationalLayers.qrc" \
    "$$SAMPLEPATHQML/Layers/OSM_Layer/OSM_Layer.qrc" \
    "$$SAMPLEPATHQML/Layers/PlayAKmlTour/PlayAKmlTour.qrc" \
    "$$SAMPLEPATHQML/Layers/QueryMapImageSublayer/QueryMapImageSublayer.qrc" \
    "$$SAMPLEPATHQML/Layers/QueryOGCAPICQLFilters/QueryOGCAPICQLFilters.qrc" \
    "$$SAMPLEPATHQML/Layers/RasterColormapRenderer/RasterColormapRenderer.qrc" \
    "$$SAMPLEPATHQML/Layers/RasterFunctionService/RasterFunctionService.qrc" \
    "$$SAMPLEPATHQML/Layers/RasterLayerFile/RasterLayerFile.qrc" \
    "$$SAMPLEPATHQML/Layers/RasterLayerGeoPackage/RasterLayerGeoPackage.qrc" \
    "$$SAMPLEPATHQML/Layers/RasterLayerService/RasterLayerService.qrc" \
    "$$SAMPLEPATHQML/Layers/RasterRenderingRule/RasterRenderingRule.qrc" \
    "$$SAMPLEPATHQML/Layers/RasterRgbRenderer/RasterRgbRenderer.qrc" \
    "$$SAMPLEPATHQML/Layers/RasterStretchRenderer/RasterStretchRenderer.qrc" \
    "$$SAMPLEPATHQML/Layers/StyleWmsLayer/StyleWmsLayer.qrc" \
    "$$SAMPLEPATHQML/Layers/TileCacheLayer/TileCacheLayer.qrc" \
    "$$SAMPLEPATHQML/Layers/VectorTiledLayerUrl/VectorTiledLayerUrl.qrc" \
    "$$SAMPLEPATHQML/Layers/Web_Tiled_Layer/Web_Tiled_Layer.qrc" \
    "$$SAMPLEPATHQML/Layers/WmsLayerUrl/WmsLayerUrl.qrc" \
    "$$SAMPLEPATHQML/Layers/WMTS_Layer/WMTS_Layer.qrc" \
    "$$SAMPLEPATHQML/Maps/ApplyScheduledMapUpdates/ApplyScheduledMapUpdates.qrc" \
    "$$SAMPLEPATHQML/Maps/BrowseBuildingFloors/BrowseBuildingFloors.qrc" \
    "$$SAMPLEPATHQML/Maps/ChangeBasemap/ChangeBasemap.qrc" \
    "$$SAMPLEPATHQML/Maps/ChangeViewpoint/ChangeViewpoint.qrc" \
    "$$SAMPLEPATHQML/Maps/CreateAndSaveMap/CreateAndSaveMap.qrc" \
    "$$SAMPLEPATHQML/Maps/DisplayDeviceLocation/DisplayDeviceLocation.qrc" \
    "$$SAMPLEPATHQML/Maps/DisplayDeviceLocationWithNmeaDataSources/DisplayDeviceLocationWithNmeaDataSources.qrc" \
    "$$SAMPLEPATHQML/Maps/DisplayDrawingStatus/DisplayDrawingStatus.qrc" \
    "$$SAMPLEPATHQML/Maps/DisplayLayerViewDrawState/DisplayLayerViewDrawState.qrc" \
    "$$SAMPLEPATHQML/Maps/DisplayMap/DisplayMap.qrc" \
    "$$SAMPLEPATHQML/Maps/DisplayOverviewMap/DisplayOverviewMap.qrc" \
    "$$SAMPLEPATHQML/Maps/DownloadPreplannedMap/DownloadPreplannedMap.qrc" \
    "$$SAMPLEPATHQML/Maps/GenerateOfflineMap_Overrides/GenerateOfflineMap_Overrides.qrc" \
    "$$SAMPLEPATHQML/Maps/GenerateOfflineMap/GenerateOfflineMap.qrc" \
    "$$SAMPLEPATHQML/Maps/GenerateOfflineMapLocalBasemap/GenerateOfflineMapLocalBasemap.qrc" \
    "$$SAMPLEPATHQML/Maps/HonorMobileMapPackageExpiration/HonorMobileMapPackageExpiration.qrc" \
    "$$SAMPLEPATHQML/Maps/IdentifyLayers/IdentifyLayers.qrc" \
    "$$SAMPLEPATHQML/Maps/ManageBookmarks/ManageBookmarks.qrc" \
    "$$SAMPLEPATHQML/Maps/MapLoaded/MapLoaded.qrc" \
    "$$SAMPLEPATHQML/Maps/MapReferenceScale/MapReferenceScale.qrc" \
    "$$SAMPLEPATHQML/Maps/MapRotation/MapRotation.qrc" \
    "$$SAMPLEPATHQML/Maps/MinMaxScale/MinMaxScale.qrc" \
    "$$SAMPLEPATHQML/Maps/MobileMap_SearchAndRoute/MobileMap_SearchAndRoute.qrc" \
    "$$SAMPLEPATHQML/Maps/OpenMapUrl/OpenMapUrl.qrc" \
    "$$SAMPLEPATHQML/Maps/OpenMobileMap_MapPackage/OpenMobileMap_MapPackage.qrc" \
    "$$SAMPLEPATHQML/Maps/ReadGeoPackage/ReadGeoPackage.qrc" \
    "$$SAMPLEPATHQML/Maps/SetInitialMapArea/SetInitialMapArea.qrc" \
    "$$SAMPLEPATHQML/Maps/SetInitialMapLocation/SetInitialMapLocation.qrc" \
    "$$SAMPLEPATHQML/Maps/SetMapSpatialReference/SetMapSpatialReference.qrc" \
    "$$SAMPLEPATHQML/Maps/SetMaxExtent/SetMaxExtent.qrc" \
    "$$SAMPLEPATHQML/Maps/ShowDeviceLocationUsingIndoorPositioning/ShowDeviceLocationUsingIndoorPositioning.qrc" \
    "$$SAMPLEPATHQML/Maps/ShowLocationHistory/ShowLocationHistory.qrc" \
    "$$SAMPLEPATHQML/Maps/ShowMagnifier/ShowMagnifier.qrc" \
    "$$SAMPLEPATHQML/Maps/TakeScreenshot/TakeScreenshot.qrc" \
    "$$SAMPLEPATHQML/Routing/ClosestFacility/ClosestFacility.qrc" \
    "$$SAMPLEPATHQML/Routing/DisplayRouteLayer/DisplayRouteLayer.qrc" \
    "$$SAMPLEPATHQML/Routing/FindClosestFacilityToMultipleIncidentsService/FindClosestFacilityToMultipleIncidentsService.qrc" \
    "$$SAMPLEPATHQML/Routing/FindRoute/FindRoute.qrc" \
    "$$SAMPLEPATHQML/Routing/FindServiceAreasForMultipleFacilities/FindServiceAreasForMultipleFacilities.qrc" \
    "$$SAMPLEPATHQML/Routing/NavigateRoute/NavigateRoute.qrc" \
    "$$SAMPLEPATHQML/Routing/NavigateARouteWithRerouting/NavigateARouteWithRerouting.qrc" \
    "$$SAMPLEPATHQML/Routing/OfflineRouting/OfflineRouting.qrc" \
    "$$SAMPLEPATHQML/Routing/RouteAroundBarriers/RouteAroundBarriers.qrc" \
    "$$SAMPLEPATHQML/Routing/ServiceArea/ServiceArea.qrc" \
    "$$SAMPLEPATHQML/Scenes/AddAPointSceneLayer/AddAPointSceneLayer.qrc" \
    "$$SAMPLEPATHQML/Scenes/AddIntegratedMeshLayer/AddIntegratedMeshLayer.qrc" \
    "$$SAMPLEPATHQML/Scenes/Animate3DSymbols/Animate3DSymbols.qrc" \
    "$$SAMPLEPATHQML/Scenes/AnimateImagesWithImageOverlay/AnimateImagesWithImageOverlay.qrc" \
    "$$SAMPLEPATHQML/Scenes/BasicSceneView/BasicSceneView.qrc" \
    "$$SAMPLEPATHQML/Scenes/ChangeAtmosphereEffect/ChangeAtmosphereEffect.qrc" \
    "$$SAMPLEPATHQML/Scenes/ChooseCameraController/ChooseCameraController.qrc" \
    "$$SAMPLEPATHQML/Scenes/CreateTerrainSurfaceFromLocalRaster/CreateTerrainSurfaceFromLocalRaster.qrc" \
    "$$SAMPLEPATHQML/Scenes/CreateTerrainSurfaceFromLocalTilePackage/CreateTerrainSurfaceFromLocalTilePackage.qrc" \
    "$$SAMPLEPATHQML/Scenes/Display3DLabelsInScene/Display3DLabelsInScene.qrc" \
    "$$SAMPLEPATHQML/Scenes/DisplaySceneLayer/DisplaySceneLayer.qrc" \
    "$$SAMPLEPATHQML/Scenes/DistanceCompositeSymbol/DistanceCompositeSymbol.qrc" \
    "$$SAMPLEPATHQML/Scenes/ExtrudeGraphics/ExtrudeGraphics.qrc" \
    "$$SAMPLEPATHQML/Scenes/FeatureLayerExtrusion/FeatureLayerExtrusion.qrc" \
    "$$SAMPLEPATHQML/Scenes/GetElevationAtPoint/GetElevationAtPoint.qrc" \
    "$$SAMPLEPATHQML/Scenes/OpenMobileScenePackage/OpenMobileScenePackage.qrc" \
    "$$SAMPLEPATHQML/Scenes/OpenScene/OpenScene.qrc" \
    "$$SAMPLEPATHQML/Scenes/OrbitCameraAroundObject/OrbitCameraAroundObject.qrc" \
    "$$SAMPLEPATHQML/Scenes/RealisticLightingAndShadows/RealisticLightingAndShadows.qrc" \
    "$$SAMPLEPATHQML/Scenes/SceneLayerSelection/SceneLayerSelection.qrc" \
    "$$SAMPLEPATHQML/Scenes/ScenePropertiesExpressions/ScenePropertiesExpressions.qrc" \
    "$$SAMPLEPATHQML/Scenes/SurfacePlacement/Surface_Placement.qrc" \
    "$$SAMPLEPATHQML/Scenes/Symbols/Symbols.qrc" \
    "$$SAMPLEPATHQML/Scenes/SyncMapViewSceneView/SyncMapViewSceneView.qrc" \
    "$$SAMPLEPATHQML/Scenes/TerrainExaggeration/TerrainExaggeration.qrc" \
    "$$SAMPLEPATHQML/Scenes/ViewContentBeneathTerrainSurface/ViewContentBeneathTerrainSurface.qrc" \
    "$$SAMPLEPATHQML/Scenes/ViewPointCloudDataOffline/ViewPointCloudDataOffline.qrc" \
    "$$SAMPLEPATHQML/Search/FindAddress/FindAddress.qrc" \
    "$$SAMPLEPATHQML/Search/FindPlace/FindPlace.qrc" \
    "$$SAMPLEPATHQML/Search/OfflineGeocode/OfflineGeocode.qrc" \
    "$$SAMPLEPATHQML/Search/ReverseGeocodeOnline/ReverseGeocodeOnline.qrc" \
    "$$SAMPLEPATHQML/Search/SearchDictionarySymbolStyle/SearchDictionarySymbolStyle.qrc" \
    "$$SAMPLEPATHQML/UtilityNetwork/ConfigureSubnetworkTrace/ConfigureSubnetworkTrace.qrc" \
    "$$SAMPLEPATHQML/UtilityNetwork/CreateLoadReport/CreateLoadReport.qrc" \
    "$$SAMPLEPATHQML/UtilityNetwork/DisplayContentOfUtilityNetworkContainer/DisplayContentOfUtilityNetworkContainer.qrc" \
    "$$SAMPLEPATHQML/UtilityNetwork/DisplayUtilityAssociations/DisplayUtilityAssociations.qrc" \
    "$$SAMPLEPATHQML/UtilityNetwork/PerformValveIsolationTrace/PerformValveIsolationTrace.qrc" \
    "$$SAMPLEPATHQML/UtilityNetwork/TraceUtilityNetwork/TraceUtilityNetwork.qrc"

# Exclude samples with WebView from macOS and Linux
win32|android|ios {
  DEFINES += SHOW_PORTAL_SAMPLES
  RESOURCES += \
    "$$SAMPLEPATHQML/CloudAndPortal/AddItemsToPortal/AddItemsToPortal.qrc" \
    "$$SAMPLEPATHQML/CloudAndPortal/PortalUserInfo/PortalUserInfo.qrc" \
    "$$SAMPLEPATHQML/CloudAndPortal/SearchForWebmap/SearchForWebmap.qrc" \
    "$$SAMPLEPATHQML/CloudAndPortal/ShowOrgBasemaps/ShowOrgBasemaps.qrc"
}

INCLUDEPATH += \
    "$$SAMPLEPATHQML/Layers/ManageOperationalLayers"

HEADERS += \
    "$$SAMPLEPATHQML/Layers/ManageOperationalLayers/DrawOrderLayerListModel.h"

SOURCES += \
    "$$SAMPLEPATHQML/Layers/ManageOperationalLayers/DrawOrderLayerListModel.cpp"

# Exclude Raster Function File from macOS
!macx {
  RESOURCES += \
    "$$SAMPLEPATHQML/Layers/RasterFunctionFile/RasterFunctionFile.qrc"
}
