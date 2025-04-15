// [Legal]
// COPYRIGHT 2025 ESRI

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#include "pch.hpp" // IWYU pragma: keep

// Qt headers
#include <QApplication>
#include <QDir>
#include <QGuiApplication>
#include <QJSEngine>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QtGlobal>

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#include <QFontDatabase>
#endif

#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
#  include <QtWebEngineQuick>
#endif // QT_WEBVIEW_WEBENGINE_BACKEND

#include "ArcGISQt_global.h" // IWYU pragma: keep - for LOCALSERVER_SUPPORTED

#include "SampleManager.h"

#include "CategoryListModel.h"
#include "DataItem.h"
#include "DataItemListModel.h"
#include "Esri/ArcGISRuntime/Toolkit/register.h"
#include "Sample.h"
#include "SampleCategory.h"
#include "SampleListModel.h"
#include "SampleSearchFilterModel.h"
#include "SearchFilterCriteria.h"
#include "SourceCode.h"
#include "SourceCodeListModel.h"
#include "SyntaxHighlighter/SyntaxHighlighter.h"
#include "../CppSamples/Layers/ManageOperationalLayers/DrawOrderLayerListModel.h"

#ifdef ESRI_BUILD
#include "buildnum.h"
#endif

// All Samples
#include "../CppSamples/Analysis/AnalyzeHotspots/AnalyzeHotspots.h"
#include "../CppSamples/Analysis/AnalyzeViewshed/AnalyzeViewshed.h"
#include "../CppSamples/Analysis/DistanceMeasurementAnalysis/DistanceMeasurementAnalysis.h"
#include "../CppSamples/Analysis/Geotriggers/Geotriggers.h"
#include "../CppSamples/Analysis/LineOfSightGeoElement/LineOfSightGeoElement.h"
#include "../CppSamples/Analysis/LineOfSightLocation/LineOfSightLocation.h"
#include "../CppSamples/Analysis/StatisticalQuery/StatisticalQuery.h"
#include "../CppSamples/Analysis/StatisticalQueryGroupSort/StatisticalQueryGroupSort.h"
#include "../CppSamples/Analysis/ViewshedCamera/ViewshedCamera.h"
#include "../CppSamples/Analysis/ViewshedGeoElement/ViewshedGeoElement.h"
#include "../CppSamples/Analysis/ViewshedLocation/ViewshedLocation.h"
#include "../CppSamples/CloudAndPortal/IntegratedWindowsAuthentication/IntegratedWindowsAuthentication.h"
#include "../CppSamples/CloudAndPortal/TokenAuthentication/TokenAuthentication.h"
#include "../CppSamples/DisplayInformation/AddGraphicsWithRenderer/AddGraphicsWithRenderer.h"
#include "../CppSamples/DisplayInformation/BuildLegend/BuildLegend.h"
#include "../CppSamples/DisplayInformation/ControlAnnotationSublayerVisibility/ControlAnnotationSublayerVisibility.h"
#include "../CppSamples/DisplayInformation/CreateSymbolStylesFromWebStyles/CreateSymbolStylesFromWebStyles.h"
#include "../CppSamples/DisplayInformation/CustomDictionaryStyle/CustomDictionaryStyle.h"
#include "../CppSamples/DisplayInformation/DisplayClusters/DisplayClusters.h"
#include "../CppSamples/DisplayInformation/GODictionaryRenderer/GODictionaryRenderer.h"
#include "../CppSamples/DisplayInformation/GODictionaryRenderer_3D/GODictionaryRenderer_3D.h"
#include "../CppSamples/DisplayInformation/GOSymbols/GOSymbols.h"
#include "../CppSamples/DisplayInformation/IdentifyGraphics/IdentifyGraphics.h"
#include "../CppSamples/DisplayInformation/Picture_Marker_Symbol/Picture_Marker_Symbol.h"
#include "../CppSamples/DisplayInformation/QueryFeaturesWithArcadeExpression/QueryFeaturesWithArcadeExpression.h"
#include "../CppSamples/DisplayInformation/ReadSymbolsFromMobileStyle/ReadSymbolsFromMobileStyle.h"
#include "../CppSamples/DisplayInformation/ShowCallout/ShowCallout.h"
#include "../CppSamples/DisplayInformation/ShowGrid/ShowGrid.h"
#include "../CppSamples/DisplayInformation/ShowLabelsOnLayers/ShowLabelsOnLayers.h"
#include "../CppSamples/DisplayInformation/ShowPopup/ShowPopup.h"
#include "../CppSamples/DisplayInformation/Simple_Marker_Symbol/Simple_Marker_Symbol.h"
#include "../CppSamples/DisplayInformation/Simple_Renderer/Simple_Renderer.h"
#include "../CppSamples/DisplayInformation/SketchOnMap/SketchOnMap.h"
#include "../CppSamples/DisplayInformation/SymbolizeShapefile/SymbolizeShapefile.h"
#include "../CppSamples/DisplayInformation/Unique_Value_Renderer/Unique_Value_Renderer.h"
#include "../CppSamples/EditData/AddFeaturesFeatureService/AddFeaturesFeatureService.h"
#include "../CppSamples/EditData/ContingentValues/ContingentValues.h"
#include "../CppSamples/EditData/CreateKmlMultiTrack/CreateKmlMultiTrack.h"
#include "../CppSamples/EditData/DeleteFeaturesFeatureService/DeleteFeaturesFeatureService.h"
#include "../CppSamples/EditData/EditAndSyncFeatures/EditAndSyncFeatures.h"
#include "../CppSamples/EditData/EditFeatureAttachments/EditFeatureAttachments.h"
#include "../CppSamples/EditData/EditFeaturesWithFeatureLinkedAnnotation/EditFeaturesWithFeatureLinkedAnnotation.h"
#include "../CppSamples/EditData/EditKmlGroundOverlay/EditKmlGroundOverlay.h"
#include "../CppSamples/EditData/EditWithBranchVersioning/EditWithBranchVersioning.h"
#include "../CppSamples/EditData/SnapGeometryEdits/SnapGeometryEdits.h"
#include "../CppSamples/EditData/SnapGeometryEditsWithRules/SnapGeometryEditsWithRules.h"
#include "../CppSamples/EditData/UpdateAttributesFeatureService/UpdateAttributesFeatureService.h"
#include "../CppSamples/EditData/UpdateGeometryFeatureService/UpdateGeometryFeatureService.h"
#include "../CppSamples/Features/ControlTimeExtentTimeSlider/ControlTimeExtentTimeSlider.h"
#include "../CppSamples/Features/CreateMobileGeodatabase/CreateMobileGeodatabase.h"
#include "../CppSamples/Features/FeatureLayerChangeRenderer/FeatureLayerChangeRenderer.h"
#include "../CppSamples/Features/FeatureLayerDictionaryRenderer/FeatureLayerDictionaryRenderer.h"
#include "../CppSamples/Features/FeatureLayerQuery/FeatureLayerQuery.h"
#include "../CppSamples/Features/FeatureLayerSelection/FeatureLayerSelection.h"
#include "../CppSamples/Features/FilterByDefinitionExpressionOrDisplayFilter/FilterByDefinitionExpressionOrDisplayFilter.h"
#include "../CppSamples/Features/GenerateGeodatabaseReplicaFromFeatureService/GenerateGeodatabaseReplicaFromFeatureService.h"
#include "../CppSamples/Features/ListRelatedFeatures/ListRelatedFeatures.h"
#include "../CppSamples/Features/ServiceFeatureTableCache/ServiceFeatureTableCache.h"
#include "../CppSamples/Features/ServiceFeatureTableManualCache/ServiceFeatureTableManualCache.h"
#include "../CppSamples/Features/ServiceFeatureTableNoCache/ServiceFeatureTableNoCache.h"
#include "../CppSamples/Geometry/Buffer/Buffer.h"
#include "../CppSamples/Geometry/ClipGeometry/ClipGeometry.h"
#include "../CppSamples/Geometry/ConvexHull/ConvexHull.h"
#include "../CppSamples/Geometry/CreateAndEditGeometries/CreateAndEditGeometries.h"
#include "../CppSamples/Geometry/CreateGeometries/CreateGeometries.h"
#include "../CppSamples/Geometry/CutGeometry/CutGeometry.h"
#include "../CppSamples/Geometry/DensifyAndGeneralize/DensifyAndGeneralize.h"
#include "../CppSamples/Geometry/FormatCoordinates/FormatCoordinates.h"
#include "../CppSamples/Geometry/GeodesicOperations/GeodesicOperations.h"
#include "../CppSamples/Geometry/ListTransformations/ListTransformations.h"
#include "../CppSamples/Geometry/NearestVertex/NearestVertex.h"
#include "../CppSamples/Geometry/ProjectGeometry/ProjectGeometry.h"
#include "../CppSamples/Geometry/SpatialOperations/SpatialOperations.h"
#include "../CppSamples/Geometry/SpatialRelationships/SpatialRelationships.h"
#include "../CppSamples/Layers/AddCustomDynamicEntityDataSource/AddCustomDynamicEntityDataSource.h"
#include "../CppSamples/Layers/AddDynamicEntityLayer/AddDynamicEntityLayer.h"
#include "../CppSamples/Layers/AddEncExchangeSet/AddEncExchangeSet.h"
#include "../CppSamples/Layers/ApplyMosaicRuleToRasters/ApplyMosaicRuleToRasters.h"
#include "../CppSamples/Layers/ApplyUniqueValuesWithAlternateSymbols/ApplyUniqueValuesWithAlternateSymbols.h"
#include "../CppSamples/Layers/ArcGISMapImageLayerUrl/ArcGISMapImageLayerUrl.h"
#include "../CppSamples/Layers/ArcGISTiledLayerUrl/ArcGISTiledLayerUrl.h"
#include "../CppSamples/Layers/BlendRasterLayer/BlendRasterLayer.h"
#include "../CppSamples/Layers/BrowseOGCAPIFeatureService/BrowseOGCAPIFeatureService.h"
#include "../CppSamples/Layers/BrowseWfsLayers/BrowseWfsLayers.h"
#include "../CppSamples/Layers/ChangeSublayerRenderer/ChangeSublayerRenderer.h"
#include "../CppSamples/Layers/ChangeSublayerVisibility/ChangeSublayerVisibility.h"
#include "../CppSamples/Layers/ConfigureClusters/ConfigureClusters.h"
#include "../CppSamples/Layers/CreateAndSaveKmlFile/CreateAndSaveKmlFile.h"
#include "../CppSamples/Layers/DisplayAnnotation/DisplayAnnotation.h"
#include "../CppSamples/Layers/DisplayDimensions/DisplayDimensions.h"
#include "../CppSamples/Layers/DisplayFeatureLayers/DisplayFeatureLayers.h"
#include "../CppSamples/Layers/DisplayKml/DisplayKml.h"
#include "../CppSamples/Layers/DisplayKmlNetworkLinks/DisplayKmlNetworkLinks.h"
#include "../CppSamples/Layers/DisplayOgcApiFeatureCollection/DisplayOgcApiFeatureCollection.h"
#include "../CppSamples/Layers/DisplaySubtypeFeatureLayer/DisplaySubtypeFeatureLayer.h"
#include "../CppSamples/Layers/DisplayWfsLayer/DisplayWfsLayer.h"
#include "../CppSamples/Layers/ExportTiles/ExportTiles.h"
#include "../CppSamples/Layers/ExportVectorTiles/ExportVectorTiles.h"
#include "../CppSamples/Layers/FeatureCollectionLayerFromPortal/FeatureCollectionLayerFromPortal.h"
#include "../CppSamples/Layers/FeatureCollectionLayerQuery/FeatureCollectionLayerQuery.h"
#include "../CppSamples/Layers/FeatureLayerRenderingModeMap/FeatureLayerRenderingModeMap.h"
#include "../CppSamples/Layers/FeatureLayerRenderingModeScene/FeatureLayerRenderingModeScene.h"
#include "../CppSamples/Layers/Feature_Collection_Layer/Feature_Collection_Layer.h"
#include "../CppSamples/Layers/GroupLayers/GroupLayers.h"
#include "../CppSamples/Layers/Hillshade_Renderer/Hillshade_Renderer.h"
#include "../CppSamples/Layers/IdentifyKmlFeatures/IdentifyKmlFeatures.h"
#include "../CppSamples/Layers/IdentifyRasterCell/IdentifyRasterCell.h"
#include "../CppSamples/Layers/ListKmlContents/ListKmlContents.h"
#include "../CppSamples/Layers/LoadWfsXmlQuery/LoadWfsXmlQuery.h"
#include "../CppSamples/Layers/ManageOperationalLayers/ManageOperationalLayers.h"
#include "../CppSamples/Layers/OSM_Layer/OSM_Layer.h"
#include "../CppSamples/Layers/PlayAKmlTour/PlayAKmlTour.h"
#include "../CppSamples/Layers/QueryMapImageSublayer/QueryMapImageSublayer.h"
#include "../CppSamples/Layers/QueryOGCAPICQLFilters/QueryOGCAPICQLFilters.h"
#include "../CppSamples/Layers/RasterColormapRenderer/RasterColormapRenderer.h"
#include "../CppSamples/Layers/RasterFunctionService/RasterFunctionService.h"
#include "../CppSamples/Layers/RasterLayerFile/RasterLayerFile.h"
#include "../CppSamples/Layers/RasterLayerGeoPackage/RasterLayerGeoPackage.h"
#include "../CppSamples/Layers/RasterLayerService/RasterLayerService.h"
#include "../CppSamples/Layers/RasterRenderingRule/RasterRenderingRule.h"
#include "../CppSamples/Layers/RasterRgbRenderer/RasterRgbRenderer.h"
#include "../CppSamples/Layers/RasterStretchRenderer/RasterStretchRenderer.h"
#include "../CppSamples/Layers/StyleWmsLayer/StyleWmsLayer.h"
#include "../CppSamples/Layers/TileCacheLayer/TileCacheLayer.h"
#include "../CppSamples/Layers/VectorTiledLayerUrl/VectorTiledLayerUrl.h"
#include "../CppSamples/Layers/WMTS_Layer/WMTS_Layer.h"
#include "../CppSamples/Layers/Web_Tiled_Layer/Web_Tiled_Layer.h"
#include "../CppSamples/Layers/WmsLayerUrl/WmsLayerUrl.h"
#include "../CppSamples/Maps/ApplyScheduledMapUpdates/ApplyScheduledMapUpdates.h"
#include "../CppSamples/Maps/BrowseBuildingFloors/BrowseBuildingFloors.h"
#include "../CppSamples/Maps/ChangeBasemap/ChangeBasemap.h"
#include "../CppSamples/Maps/ChangeViewpoint/ChangeViewpoint.h"
#include "../CppSamples/Maps/ConfigureBasemapStyleLanguage/ConfigureBasemapStyleLanguage.h"
#include "../CppSamples/Maps/CreateAndSaveMap/CreateAndSaveMap.h"
#include "../CppSamples/Maps/CreateDynamicBasemapGallery/CreateDynamicBasemapGallery.h"
#include "../CppSamples/Maps/DisplayDeviceLocation/DisplayDeviceLocation.h"
#include "../CppSamples/Maps/DisplayDeviceLocationWithNmeaDataSources/DisplayDeviceLocationWithNmeaDataSources.h"
#include "../CppSamples/Maps/DisplayDrawingStatus/DisplayDrawingStatus.h"
#include "../CppSamples/Maps/DisplayLayerViewDrawState/DisplayLayerViewDrawState.h"
#include "../CppSamples/Maps/DisplayMap/DisplayMap.h"
#include "../CppSamples/Maps/DisplayOverviewMap/DisplayOverviewMap.h"
#include "../CppSamples/Maps/DownloadPreplannedMap/DownloadPreplannedMap.h"
#include "../CppSamples/Maps/GenerateOfflineMap/GenerateOfflineMap.h"
#include "../CppSamples/Maps/GenerateOfflineMapLocalBasemap/GenerateOfflineMapLocalBasemap.h"
#include "../CppSamples/Maps/GenerateOfflineMap_Overrides/GenerateOfflineMap_Overrides.h"
#include "../CppSamples/Maps/HonorMobileMapPackageExpiration/HonorMobileMapPackageExpiration.h"
#include "../CppSamples/Maps/IdentifyLayers/IdentifyLayers.h"
#include "../CppSamples/Maps/ManageBookmarks/ManageBookmarks.h"
#include "../CppSamples/Maps/MapLoaded/MapLoaded.h"
#include "../CppSamples/Maps/MapReferenceScale/MapReferenceScale.h"
#include "../CppSamples/Maps/MapRotation/MapRotation.h"
#include "../CppSamples/Maps/MinMaxScale/MinMaxScale.h"
#include "../CppSamples/Maps/MobileMap_SearchAndRoute/MobileMap_SearchAndRoute.h"
#include "../CppSamples/Maps/OpenMapUrl/OpenMapUrl.h"
#include "../CppSamples/Maps/OpenMobileMap_MapPackage/OpenMobileMap_MapPackage.h"
#include "../CppSamples/Maps/ReadGeoPackage/ReadGeoPackage.h"
#include "../CppSamples/Maps/SetInitialMapArea/SetInitialMapArea.h"
#include "../CppSamples/Maps/SetInitialMapLocation/SetInitialMapLocation.h"
#include "../CppSamples/Maps/SetMapSpatialReference/SetMapSpatialReference.h"
#include "../CppSamples/Maps/SetMaxExtent/SetMaxExtent.h"
#include "../CppSamples/Maps/ShowDeviceLocationUsingIndoorPositioning/ShowDeviceLocationUsingIndoorPositioning.h"
#include "../CppSamples/Maps/ShowLocationHistory/ShowLocationHistory.h"
#include "../CppSamples/Maps/ShowMagnifier/ShowMagnifier.h"
#include "../CppSamples/Maps/TakeScreenshot/MapImageProvider.h"
#include "../CppSamples/Maps/TakeScreenshot/TakeScreenshot.h"
#include "../CppSamples/Routing/ClosestFacility/ClosestFacility.h"
#include "../CppSamples/Routing/DisplayRouteLayer/DisplayRouteLayer.h"
#include "../CppSamples/Routing/FindClosestFacilityToMultipleIncidentsService/FindClosestFacilityToMultipleIncidentsService.h"
#include "../CppSamples/Routing/FindRoute/FindRoute.h"
#include "../CppSamples/Routing/FindServiceAreasForMultipleFacilities/FindServiceAreasForMultipleFacilities.h"
#include "../CppSamples/Routing/NavigateARouteWithRerouting/NavigateARouteWithRerouting.h"
#include "../CppSamples/Routing/NavigateRoute/NavigateRoute.h"
#include "../CppSamples/Routing/OfflineRouting/OfflineRouting.h"
#include "../CppSamples/Routing/RouteAroundBarriers/RouteAroundBarriers.h"
#include "../CppSamples/Routing/ServiceArea/ServiceArea.h"
#include "../CppSamples/Scenes/Add3DTilesLayer/Add3DTilesLayer.h"
#include "../CppSamples/Scenes/AddAPointSceneLayer/AddAPointSceneLayer.h"
#include "../CppSamples/Scenes/AddIntegratedMeshLayer/AddIntegratedMeshLayer.h"
#include "../CppSamples/Scenes/Animate3DSymbols/Animate3DSymbols.h"
#include "../CppSamples/Scenes/AnimateImagesWithImageOverlay/AnimateImagesWithImageOverlay.h"
#include "../CppSamples/Scenes/BasicSceneView/BasicSceneView.h"
#include "../CppSamples/Scenes/ChangeAtmosphereEffect/ChangeAtmosphereEffect.h"
#include "../CppSamples/Scenes/ChooseCameraController/ChooseCameraController.h"
#include "../CppSamples/Scenes/CreateTerrainSurfaceFromLocalRaster/CreateTerrainSurfaceFromLocalRaster.h"
#include "../CppSamples/Scenes/CreateTerrainSurfaceFromLocalTilePackage/CreateTerrainSurfaceFromLocalTilePackage.h"
#include "../CppSamples/Scenes/Display3DLabelsInScene/Display3DLabelsInScene.h"
#include "../CppSamples/Scenes/DisplaySceneLayer/DisplaySceneLayer.h"
#include "../CppSamples/Scenes/DistanceCompositeSymbol/DistanceCompositeSymbol.h"
#include "../CppSamples/Scenes/ExtrudeGraphics/ExtrudeGraphics.h"
#include "../CppSamples/Scenes/FeatureLayerExtrusion/FeatureLayerExtrusion.h"
#include "../CppSamples/Scenes/FilterFeaturesInScene/FilterFeaturesInScene.h"
#include "../CppSamples/Scenes/GetElevationAtPoint/GetElevationAtPoint.h"
#include "../CppSamples/Scenes/OpenMobileScenePackage/OpenMobileScenePackage.h"
#include "../CppSamples/Scenes/OpenScene/OpenScene.h"
#include "../CppSamples/Scenes/OrbitCameraAroundObject/OrbitCameraAroundObject.h"
#include "../CppSamples/Scenes/RealisticLightingAndShadows/RealisticLightingAndShadows.h"
#include "../CppSamples/Scenes/SceneLayerSelection/SceneLayerSelection.h"
#include "../CppSamples/Scenes/ScenePropertiesExpressions/ScenePropertiesExpressions.h"
#include "../CppSamples/Scenes/SetSurfacePlacementMode/SetSurfacePlacementMode.h"
#include "../CppSamples/Scenes/Symbols/Symbols.h"
#include "../CppSamples/Scenes/SyncMapViewSceneView/SyncMapViewSceneView.h"
#include "../CppSamples/Scenes/TerrainExaggeration/TerrainExaggeration.h"
#include "../CppSamples/Scenes/ViewContentBeneathTerrainSurface/ViewContentBeneathTerrainSurface.h"
#include "../CppSamples/Scenes/ViewPointCloudDataOffline/ViewPointCloudDataOffline.h"
#include "../CppSamples/Search/FindAddress/FindAddress.h"
#include "../CppSamples/Search/FindPlace/FindPlace.h"
#include "../CppSamples/Search/OfflineGeocode/OfflineGeocode.h"
#include "../CppSamples/Search/ReverseGeocodeOnline/ReverseGeocodeOnline.h"
#include "../CppSamples/Search/SearchDictionarySymbolStyle/SearchDictionarySymbolStyle.h"
#include "../CppSamples/UtilityNetwork/ConfigureSubnetworkTrace/ConfigureSubnetworkTrace.h"
#include "../CppSamples/UtilityNetwork/CreateLoadReport/CreateLoadReport.h"
#include "../CppSamples/UtilityNetwork/DisplayContentOfUtilityNetworkContainer/DisplayContentOfUtilityNetworkContainer.h"
#include "../CppSamples/UtilityNetwork/DisplayUtilityAssociations/DisplayUtilityAssociations.h"
#include "../CppSamples/UtilityNetwork/PerformValveIsolationTrace/PerformValveIsolationTrace.h"
#include "../CppSamples/UtilityNetwork/TraceUtilityNetwork/TraceUtilityNetwork.h"
#include "../CppSamples/UtilityNetwork/ValidateUtilityNetworkTopology/ValidateUtilityNetworkTopology.h"

#ifdef LOCALSERVER_SUPPORTED
#include "../CppSamples/LocalServer/LocalServerGeoprocessing/LocalServerGeoprocessing.h"
#include "../CppSamples/LocalServer/LocalServerFeatureLayer/LocalServerFeatureLayer.h"
#include "../CppSamples/LocalServer/LocalServerMapImageLayer/LocalServerMapImageLayer.h"
#include "../CppSamples/LocalServer/LocalServerServices/LocalServerServices.h"
#endif // LOCALSERVER_SUPPORTED

#ifdef SHOW_PORTAL_SAMPLES
#include "../CppSamples/CloudAndPortal/SearchForWebmap/SearchForWebmap.h"
#include "../CppSamples/CloudAndPortal/AddItemsToPortal/AddItemsToPortal.h"
#include "../CppSamples/CloudAndPortal/PortalUserInfo/PortalUserInfo.h"
#include "../CppSamples/CloudAndPortal/ShowOrgBasemaps/ShowOrgBasemaps.h"
#endif // HIDE_PORTAL_SAMPLES

#ifdef SHOW_RASTER_FUNCTION_SAMPLE
#include "../CppSamples/Layers/RasterFunctionFile/RasterFunctionFile.h"
#endif // SHOW_RASTER_FUNCTION_SAMPLE

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

QObject* esriSampleManagerProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
QObject* syntaxHighlighterProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
void registerClasses();
void registerCppSampleClasses();

int main(int argc, char *argv[])
{
#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
  QtWebEngineQuick::initialize();
#endif // QT_WEBVIEW_WEBENGINE_BACKEND

  QGuiApplication::setApplicationName("ArcGIS Maps Qt Samples");
  QGuiApplication::setOrganizationName("Esri");
  QApplication app(argc, argv);

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
  if (qEnvironmentVariableIsSet("ESRI_AUTOMATION_TEST_SUSE_FONT_PATH"))
  {
    const auto fontPath = qEnvironmentVariable("ESRI_AUTOMATION_TEST_SUSE_FONT_PATH");
    // There is a known issue on certain Linux distros with newer versions of Qt
    // where no fonts are loaded by default. Work around it by loading a specific font
    QFontDatabase::addApplicationFont(fontPath);
  }
#endif
  
  // register sample viewer classes
  registerClasses();

  QQmlApplicationEngine engine;
  engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));
#ifdef ESRI_BUILD
  engine.rootContext()->setContextProperty("buildNum", BUILD_NUM);
#else
  engine.rootContext()->setContextProperty("buildNum", QUOTE(ArcGIS_Runtime_Version));
#endif
  engine.rootContext()->setContextProperty("qtVersion", QUOTE(Qt_Version));
  engine.addImportPath(QString("qrc:/qml"));
  engine.addImportPath(QString("qrc:/"));

// in macOS, applicationDirPath() returns a path within the app bundle. bin folder must be explicitly added to import path
#ifdef ESRI_IMPORT_PATH
  QString EsriQmlImportPath = QUOTE(ESRI_IMPORT_PATH);
  engine.addImportPath(EsriQmlImportPath);
#endif

#ifdef BUILD_FROM_SETUP
  QString arcGISRuntimeImportPath = QUOTE(ARCGIS_RUNTIME_IMPORT_PATH);

#if defined(LINUX_PLATFORM_REPLACEMENT)
  // on some linux platforms the string 'linux' is replaced with 1
  // fix the replacement paths which were created
  QString replaceString = QUOTE(LINUX_PLATFORM_REPLACEMENT);
  arcGISRuntimeImportPath = arcGISRuntimeImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
#endif // LINUX_PLATFORM_REPLACEMENT

  engine.addImportPath(arcGISRuntimeImportPath);
#endif // BUILD_FROM_SETUP

  // register toolkit components
  Esri::ArcGISRuntime::Toolkit::registerComponents(engine);

  // add image provider
  const QString name = MapImageProvider::imageProviderId();
  engine.addImageProvider(name, new MapImageProvider);

  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

  return app.exec();
}

void registerCppSampleClasses()
{
  // register the draw order model
  qmlRegisterType<DrawOrderLayerListModel>("Esri.Samples", 1, 0, "DrawOrderListModel");

  // Register the Samples under the Esri.Samples namespace
  Add3DTilesLayer::init();
  AddAPointSceneLayer::init();
  AddCustomDynamicEntityDataSource::init();
  AddDynamicEntityLayer::init();
  AddEncExchangeSet::init();
  AddFeaturesFeatureService::init();
  AddGraphicsWithRenderer::init();
  AddIntegratedMeshLayer::init();
  AnalyzeHotspots::init();
  AnalyzeViewshed::init();
  Animate3DSymbols::init();
  AnimateImagesWithImageOverlay::init();
  ApplyMosaicRuleToRasters::init();
  ApplyScheduledMapUpdates::init();
  ApplyUniqueValuesWithAlternateSymbols::init();
  ArcGISMapImageLayerUrl::init();
  ArcGISTiledLayerUrl::init();
  BasicSceneView::init();
  BlendRasterLayer::init();
  BrowseBuildingFloors::init();
  BrowseOGCAPIFeatureService::init();
  BrowseWfsLayers::init();
  Buffer::init();
  BuildLegend::init();
  ChangeAtmosphereEffect::init();
  ChangeBasemap::init();
  ChangeSublayerRenderer::init();
  ChangeSublayerVisibility::init();
  ChangeViewpoint::init();
  ChooseCameraController::init();
  ClipGeometry::init();
  ClosestFacility::init();
  ConfigureBasemapStyleLanguage::init();
  ConfigureClusters::init();
  ConfigureSubnetworkTrace::init();
  ContingentValues::init();
  CreateKmlMultiTrack::init();
  ControlAnnotationSublayerVisibility::init();
  ControlTimeExtentTimeSlider::init();
  ConvexHull::init();
  CreateAndEditGeometries::init();
  CreateAndSaveKmlFile::init();
  CreateAndSaveMap::init();
  CreateDynamicBasemapGallery::init();
  CreateGeometries::init();
  CreateLoadReport::init();
  CreateMobileGeodatabase::init();
  CreateSymbolStylesFromWebStyles::init();
  CreateTerrainSurfaceFromLocalRaster::init();
  CreateTerrainSurfaceFromLocalTilePackage::init();
  CustomDictionaryStyle::init();
  CutGeometry::init();
  DeleteFeaturesFeatureService::init();
  DensifyAndGeneralize::init();
  Display3DLabelsInScene::init();
  DisplayAnnotation::init();
  DisplayClusters::init();
  DisplayContentOfUtilityNetworkContainer::init();
  DisplayDeviceLocation::init();
  DisplayDeviceLocationWithNmeaDataSources::init();
  DisplayDimensions::init();
  DisplayDrawingStatus::init();
  DisplayFeatureLayers::init();
  DisplayKml::init();
  DisplayKmlNetworkLinks::init();
  DisplayLayerViewDrawState::init();
  DisplayMap::init();
  DisplayOgcApiFeatureCollection::init();
  DisplayOverviewMap::init();
  DisplayRouteLayer::init();
  DisplaySceneLayer::init();
  DisplaySubtypeFeatureLayer::init();
  DisplayUtilityAssociations::init();
  DisplayWfsLayer::init();
  DistanceCompositeSymbol::init();
  DistanceMeasurementAnalysis::init();
  DownloadPreplannedMap::init();
  EditAndSyncFeatures::init();
  EditFeatureAttachments::init();
  EditFeaturesWithFeatureLinkedAnnotation::init();
  EditKmlGroundOverlay::init();
  EditWithBranchVersioning::init();
  ExportTiles::init();
  ExportVectorTiles::init();
  ExtrudeGraphics::init();
  FeatureCollectionLayerFromPortal::init();
  FeatureCollectionLayerQuery::init();
  FeatureLayerChangeRenderer::init();
  FeatureLayerDictionaryRenderer::init();
  FeatureLayerExtrusion::init();
  FeatureLayerQuery::init();
  FeatureLayerRenderingModeMap::init();
  FeatureLayerRenderingModeScene::init();
  FeatureLayerSelection::init();
  Feature_Collection_Layer::init();
  FilterByDefinitionExpressionOrDisplayFilter::init();
  FilterFeaturesInScene::init();
  FindAddress::init();
  FindClosestFacilityToMultipleIncidentsService::init();
  FindPlace::init();
  FindRoute::init();
  FindServiceAreasForMultipleFacilities::init();
  FormatCoordinates::init();
  GODictionaryRenderer::init();
  GODictionaryRenderer_3D::init();
  GOSymbols::init();
  GenerateGeodatabaseReplicaFromFeatureService::init();
  GenerateOfflineMap::init();
  GenerateOfflineMapLocalBasemap::init();
  GenerateOfflineMap_Overrides::init();
  GeodesicOperations::init();
  Geotriggers::init();
  GetElevationAtPoint::init();
  GroupLayers::init();
  Hillshade_Renderer::init();
  HonorMobileMapPackageExpiration::init();
  IdentifyGraphics::init();
  IdentifyKmlFeatures::init();
  IdentifyLayers::init();
  IdentifyRasterCell::init();
  IntegratedWindowsAuthentication::init();
  LineOfSightGeoElement::init();
  LineOfSightLocation::init();
  ListKmlContents::init();
  ListRelatedFeatures::init();
  ListTransformations::init();
  LoadWfsXmlQuery::init();
  ManageBookmarks::init();
  ManageOperationalLayers::init();
  MapLoaded::init();
  MapReferenceScale::init();
  MapRotation::init();
  MinMaxScale::init();
  MobileMap_SearchAndRoute::init();
  NavigateARouteWithRerouting::init();
  NavigateRoute::init();
  NearestVertex::init();
  OSM_Layer::init();
  OfflineGeocode::init();
  OfflineRouting::init();
  OpenMapUrl::init();
  OpenMobileMap_MapPackage::init();
  OpenMobileScenePackage::init();
  OpenScene::init();
  OrbitCameraAroundObject::init();
  PerformValveIsolationTrace::init();
  Picture_Marker_Symbol::init();
  PlayAKmlTour::init();
  ProjectGeometry::init();
  QueryFeaturesWithArcadeExpression::init();
  QueryMapImageSublayer::init();
  QueryOGCAPICQLFilters::init();
  RasterColormapRenderer::init();
  RasterFunctionService::init();
  RasterLayerFile::init();
  RasterLayerGeoPackage::init();
  RasterLayerService::init();
  RasterRenderingRule::init();
  RasterRgbRenderer::init();
  RasterStretchRenderer::init();
  ReadGeoPackage::init();
  ReadSymbolsFromMobileStyle::init();
  RealisticLightingAndShadows::init();
  ReverseGeocodeOnline::init();
  RouteAroundBarriers::init();
  SceneLayerSelection::init();
  ScenePropertiesExpressions::init();
  SearchDictionarySymbolStyle::init();
  ServiceArea::init();
  ServiceFeatureTableCache::init();
  ServiceFeatureTableManualCache::init();
  ServiceFeatureTableNoCache::init();
  SetInitialMapArea::init();
  SetInitialMapLocation::init();
  SetMapSpatialReference::init();
  SetMaxExtent::init();
  SetSurfacePlacementMode::init();
  ShowCallout::init();
  ShowDeviceLocationUsingIndoorPositioning::init();
  ShowGrid::init();
  ShowLabelsOnLayers::init();
  ShowLocationHistory::init();
  ShowMagnifier::init();
  ShowPopup::init();
  Simple_Marker_Symbol::init();
  Simple_Renderer::init();
  SketchOnMap::init();
  SnapGeometryEdits::init();
  SnapGeometryEditsWithRules::init();
  SpatialOperations::init();
  SpatialRelationships::init();
  StatisticalQuery::init();
  StatisticalQueryGroupSort::init();
  StyleWmsLayer::init();
  SymbolizeShapefile::init();
  Symbols::init();
  SyncMapViewSceneView::init();
  TakeScreenshot::init();
  TerrainExaggeration::init();
  TileCacheLayer::init();
  TokenAuthentication::init();
  TraceUtilityNetwork::init();
  Unique_Value_Renderer::init();
  UpdateAttributesFeatureService::init();
  UpdateGeometryFeatureService::init();
  ValidateUtilityNetworkTopology::init();
  VectorTiledLayerUrl::init();
  ViewContentBeneathTerrainSurface::init();
  ViewPointCloudDataOffline::init();
  ViewshedCamera::init();
  ViewshedGeoElement::init();
  ViewshedLocation::init();
  WMTS_Layer::init();
  Web_Tiled_Layer::init();
  WmsLayerUrl::init();

#ifdef SHOW_PORTAL_SAMPLES
  PortalUserInfo::init();
  SearchForWebmap::init();
  AddItemsToPortal::init();
  ShowOrgBasemaps::init();
#endif // HIDE_PORTAL_SAMPLES

#ifdef SHOW_RASTER_FUNCTION_SAMPLE
  RasterFunctionFile::init();
#endif // SHOW_RASTER_FUNCTION_SAMPLE

#ifdef LOCALSERVER_SUPPORTED
  LocalServerServices::init();
  LocalServerMapImageLayer::init();
  LocalServerFeatureLayer::init();
  LocalServerGeoprocessing::init();
#endif // LOCALSERVER_SUPPORTED

}

void registerClasses()
{
  qmlRegisterSingletonType<SampleManager>("Esri.ArcGISRuntimeSamples", 1, 0, "SampleManager",
                                                  &esriSampleManagerProvider);

  qmlRegisterSingletonType<SyntaxHighlighter>("Esri.ArcGISRuntimeSamples", 1, 0, "SyntaxHighlighter",
                                              &syntaxHighlighterProvider);
  qmlRegisterUncreatableType<DataItem>("Esri.ArcGISRuntimeSamples", 1, 0,
                                       "DataItem", "DataItem is an uncreatable type");
  qmlRegisterUncreatableType<DataItemListModel>("Esri.ArcGISRuntimeSamples", 1, 0,
                                                "DataItemListModel", "DataItemListModel is an uncreatable type");
  qmlRegisterUncreatableType<CategoryListModel>("Esri.ArcGISRuntimeSamples", 1, 0,
                                                "CategoryListModel", "CategoryListModel is an uncreatable type");
  qmlRegisterUncreatableType<SampleListModel>("Esri.ArcGISRuntimeSamples", 1, 0,
                                              "SampleListModel", "SampleListModel is an uncreatable type");
  qmlRegisterUncreatableType<SampleCategory>("Esri.ArcGISRuntimeSamples", 1, 0,
                                             "SampleCategory", "SampleCategory is an uncreatable type");
  qmlRegisterUncreatableType<Sample>("Esri.ArcGISRuntimeSamples", 1, 0,
                                     "Sample", "Sample is an uncreatable type");
  qmlRegisterUncreatableType<SourceCodeListModel>("Esri.ArcGISRuntimeSamples", 1, 0,
                                                  "SourceCodeListModel", "SourceCodeListModel is an uncreatable type");
  qmlRegisterUncreatableType<SourceCode>("Esri.ArcGISRuntimeSamples", 1, 0,
                                         "SourceCode", "SourceCode is an uncreatable type");

  qmlRegisterType<SampleSearchFilterModel>("Esri.ArcGISRuntimeSamples", 1, 0, "SampleSearchFilterModel");
  qmlRegisterUncreatableType<SearchFilterCriteria>("Esri.ArcGISRuntimeSamples", 1, 0, "SearchFilterCriteria", "Abstract base class");

  // register the Sample Classes
  registerCppSampleClasses();
}

QObject* esriSampleManagerProvider(QQmlEngine* engine, QJSEngine*)
{
  static QObject* sampleManager = new SampleManager(engine);
  return sampleManager;
}

QObject* syntaxHighlighterProvider(QQmlEngine* engine, QJSEngine*)
{
  static SyntaxHighlighter* syntaxHighlighter = new SyntaxHighlighter(engine);
  return syntaxHighlighter;
}
