// [Legal]
// Copyright 2022 Esri.

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

#include "pch.hpp"

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

#include "GAnalytics.h"

#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
#  include <QtWebEngine>
#endif // QT_WEBVIEW_WEBENGINE_BACKEND

// Choose a manager based on the type of interface.
// Add toolkit header for Cpp only
#ifdef CPP_VIEWER
#  include "CppSampleManager.h"
#  include "ArcGISRuntimeEnvironment.h"
#else
#  include "QmlSampleManager.h"
#endif

#include "CategoryListModel.h"
#include "DataItem.h"
#include "DataItemListModel.h"
#include "Esri/ArcGISRuntime/Toolkit/register.h"
#include "PermissionsHelper.h"
#include "Sample.h"
#include "SampleCategory.h"
#include "SampleListModel.h"
#include "SampleSearchFilterModel.h"
#include "SearchFilterCriteria.h"
#include "SourceCode.h"
#include "SourceCodeListModel.h"
#include "SyntaxHighlighter/syntax_highlighter.h"
#include "DrawOrderLayerListModel.h"

#ifdef ESRI_BUILD
#include "../../../../../buildnum/buildnum.h"
#endif

// All CPP Samples
#ifdef CPP_VIEWER
#include "AddAPointSceneLayer.h"
#include "AddEncExchangeSet.h"
#include "AddFeaturesFeatureService.h"
#include "AddGraphicsWithRenderer.h"
#include "AddIntegratedMeshLayer.h"
#include "AddItemsToPortal.h"
#include "AnalyzeHotspots.h"
#include "AnalyzeViewshed.h"
#include "Animate3DSymbols.h"
#include "AnimateImagesWithImageOverlay.h"
#include "ApplyMosaicRuleToRasters.h"
#include "ApplyScheduledMapUpdates.h"
#include "ApplyUniqueValuesWithAlternateSymbols.h"
#include "ArcGISMapImageLayerUrl.h"
#include "ArcGISTiledLayerUrl.h"
#include "BasicSceneView.h"
#include "BlendRasterLayer.h"
#include "BookmarkListModel.h"
#include "BrowseBuildingFloors.h"
#include "BrowseOGCAPIFeatureService.h"
#include "BrowseWfsLayers.h"
#include "Buffer.h"
#include "BuildLegend.h"
#include "ChangeAtmosphereEffect.h"
#include "ChangeBasemap.h"
#include "ChangeSublayerRenderer.h"
#include "ChangeSublayerVisibility.h"
#include "ChangeViewpoint.h"
#include "ChooseCameraController.h"
#include "ClipGeometry.h"
#include "ClosestFacility.h"
#include "ConfigureSubnetworkTrace.h"
#include "ContingentValues.h"
#include "ControlAnnotationSublayerVisibility.h"
#include "ControlTimeExtentTimeSlider.h"
#include "ConvexHull.h"
#include "CreateAndSaveKmlFile.h"
#include "CreateAndSaveMap.h"
#include "CreateGeometries.h"
#include "CreateLoadReport.h"
#include "CreateMobileGeodatabase.h"
#include "CreateSymbolStylesFromWebStyles.h"
#include "CreateTerrainSurfaceFromLocalRaster.h"
#include "CreateTerrainSurfaceFromLocalTilePackage.h"
#include "Credential.h"
#include "CustomDictionaryStyle.h"
#include "CutGeometry.h"
#include "DeleteFeaturesFeatureService.h"
#include "DensifyAndGeneralize.h"
#include "Display3DLabelsInScene.h"
#include "DisplayAnnotation.h"
#include "DisplayDimensions.h"
#include "DisplayContentOfUtilityNetworkContainer.h"
#include "DisplayDeviceLocation.h"
#include "DisplayDeviceLocationWithNmeaDataSources.h"
#include "DisplayDrawingStatus.h"
#include "DisplayGrid.h"
#include "DisplayKml.h"
#include "DisplayKmlNetworkLinks.h"
#include "DisplayLayerViewDrawState.h"
#include "DisplayMap.h"
#include "DisplayOgcApiFeatureCollection.h"
#include "DisplayOverviewMap.h"
#include "DisplayRouteLayer.h"
#include "DisplaySceneLayer.h"
#include "DisplaySubtypeFeatureLayer.h"
#include "DisplayUtilityAssociations.h"
#include "DisplayWfsLayer.h"
#include "DistanceCompositeSymbol.h"
#include "DistanceMeasurementAnalysis.h"
#include "DownloadPreplannedMap.h"
#include "EditAndSyncFeatures.h"
#include "EditFeatureAttachments.h"
#include "EditFeaturesWithFeatureLinkedAnnotation.h"
#include "EditKmlGroundOverlay.h"
#include "EditWithBranchVersioning.h"
#include "ExportTiles.h"
#include "ExportVectorTiles.h"
#include "ExtrudeGraphics.h"
#include "Feature_Collection_Layer.h"
#include "FeatureCollectionLayerFromPortal.h"
#include "FeatureCollectionLayerQuery.h"
#include "FeatureLayer_GeoPackage.h"
#include "FeatureLayerChangeRenderer.h"
#include "FeatureLayerDictionaryRenderer.h"
#include "FeatureLayerExtrusion.h"
#include "FeatureLayerFeatureService.h"
#include "FeatureLayerGeodatabase.h"
#include "FeatureLayerQuery.h"
#include "FeatureLayerRenderingModeMap.h"
#include "FeatureLayerRenderingModeScene.h"
#include "FeatureLayerSelection.h"
#include "FeatureLayerShapefile.h"
#include "FilterByDefinitionExpressionOrDisplayFilter.h"
#include "FindAddress.h"
#include "FindClosestFacilityToMultipleIncidentsService.h"
#include "FindPlace.h"
#include "FindRoute.h"
#include "FindServiceAreasForMultipleFacilities.h"
#include "FormatCoordinates.h"
#include "GenerateGeodatabaseReplicaFromFeatureService.h"
#include "GenerateOfflineMap_Overrides.h"
#include "GenerateOfflineMap.h"
#include "GenerateOfflineMapLocalBasemap.h"
#include "GeodesicOperations.h"
#include "Geotriggers.h"
#include "GetElevationAtPoint.h"
#include "GODictionaryRenderer_3D.h"
#include "GODictionaryRenderer.h"
#include "GOSymbols.h"
#include "GroupLayers.h"
#include "Hillshade_Renderer.h"
#include "HonorMobileMapPackageExpiration.h"
#include "IdentifyGraphics.h"
#include "IdentifyKmlFeatures.h"
#include "IdentifyLayers.h"
#include "IdentifyRasterCell.h"
#include "IntegratedWindowsAuthentication.h"
#include "LineOfSightGeoElement.h"
#include "LineOfSightLocation.h"
#include "ListKmlContents.h"
#include "ListRelatedFeatures.h"
#include "ListTransformations.h"
#include "LoadWfsXmlQuery.h"
#include "ManageBookmarks.h"
#include "ManageOperationalLayers.h"
#include "MapImageProvider.h"
#include "MapLoaded.h"
#include "MapReferenceScale.h"
#include "MapRotation.h"
#include "MinMaxScale.h"
#include "MobileMap_SearchAndRoute.h"
#include "NavigateRoute.h"
#include "NavigateARouteWithRerouting.h"
#include "NearestVertex.h"
#include "OfflineGeocode.h"
#include "OfflineRouting.h"
#include "OpenMapUrl.h"
#include "OpenMobileMap_MapPackage.h"
#include "OpenMobileScenePackage.h"
#include "OpenScene.h"
#include "OrbitCameraAroundObject.h"
#include "OSM_Layer.h"
#include "PerformValveIsolationTrace.h"
#include "Picture_Marker_Symbol.h"
#include "PlayAKmlTour.h"
#include "PortalUserInfo.h"
#include "ProjectGeometry.h"
#include "QueryFeaturesWithArcadeExpression.h"
#include "QueryMapImageSublayer.h"
#include "QueryOGCAPICQLFilters.h"
#include "RasterColormapRenderer.h"
#include "RasterFunctionService.h"
#include "RasterLayerFile.h"
#include "RasterLayerGeoPackage.h"
#include "RasterLayerService.h"
#include "RasterRenderingRule.h"
#include "RasterRgbRenderer.h"
#include "RasterStretchRenderer.h"
#include "ReadGeoPackage.h"
#include "ReadSymbolsFromMobileStyle.h"
#include "RealisticLightingAndShadows.h"
#include "ReverseGeocodeOnline.h"
#include "RouteAroundBarriers.h"
#include "SceneLayerSelection.h"
#include "ScenePropertiesExpressions.h"
#include "SearchDictionarySymbolStyle.h"
#include "SearchForWebmap.h"
#include "ServiceArea.h"
#include "ServiceFeatureTableCache.h"
#include "ServiceFeatureTableManualCache.h"
#include "ServiceFeatureTableNoCache.h"
#include "SetInitialMapArea.h"
#include "SetInitialMapLocation.h"
#include "SetMapSpatialReference.h"
#include "SetMaxExtent.h"
#include "ShowCallout.h"
#include "ShowDeviceLocationUsingIndoorPositioning.h"
#include "ShowLabelsOnLayers.h"
#include "ShowLocationHistory.h"
#include "ShowMagnifier.h"
#include "ShowOrgBasemaps.h"
#include "ShowPopup.h"
#include "Simple_Marker_Symbol.h"
#include "Simple_Renderer.h"
#include "SketchOnMap.h"
#include "SpatialOperations.h"
#include "SpatialRelationships.h"
#include "StatisticalQuery.h"
#include "StatisticalQueryGroupSort.h"
#include "StyleWmsLayer.h"
#include "SuggestListModel.h"
#include "Surface_Placement.h"
#include "SymbolizeShapefile.h"
#include "Symbols.h"
#include "SyncMapViewSceneView.h"
#include "TakeScreenshot.h"
#include "TerrainExaggeration.h"
#include "TileCacheLayer.h"
#include "TokenAuthentication.h"
#include "TraceUtilityNetwork.h"
#include "Unique_Value_Renderer.h"
#include "UpdateAttributesFeatureService.h"
#include "UpdateGeometryFeatureService.h"
#include "VectorTiledLayerUrl.h"
#include "ViewContentBeneathTerrainSurface.h"
#include "ViewPointCloudDataOffline.h"
#include "ViewshedCamera.h"
#include "ViewshedGeoElement.h"
#include "ViewshedLocation.h"
#include "Web_Tiled_Layer.h"
#include "WmsLayerUrl.h"
#include "WMTS_Layer.h"

#ifdef LOCALSERVER_SUPPORTED
#include "LocalServerFeatureLayer.h"
#include "LocalServerGeoprocessing.h"
#include "LocalServerMapImageLayer.h"
#include "LocalServerServices.h"
#endif // LOCALSERVER_SUPPORTED

#ifdef SHOW_PORTAL_SAMPLES
#include "AddItemsToPortal.h"
#include "PortalUserInfo.h"
#include "SearchForWebmap.h"
#include "ShowOrgBasemaps.h"
#endif // HIDE_PORTAL_SAMPLES

#ifdef SHOW_RASTER_FUNCTION_SAMPLE
#include "RasterFunctionFile.h"
#endif // SHOW_RASTER_FUNCTION_SAMPLE

#else
#include "NavigateRouteSpeaker.h"
#endif // CPP_VIEWER

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

QObject* esriSampleManagerProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
QObject* gAnalyticsProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
QObject* syntaxHighlighterProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
void registerClasses();
void registerCppSampleClasses();

int main(int argc, char *argv[])
{
  // add support for feature toggles on mobile, which is disabled by default
  qputenv("QT_ENABLE_FEATURE_TOGGLE_MOBILE", "true");
  qputenv("QSG_RHI_BACKEND", "opengl");

#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
  QtWebEngine::initialize();
#endif // QT_WEBVIEW_WEBENGINE_BACKEND

#ifdef CPP_VIEWER
  QGuiApplication::setApplicationName("ArcGIS Runtime Qt Cpp Samples");
#else
  QGuiApplication::setApplicationName("ArcGIS Runtime Qt Qml Samples");
#endif

  QGuiApplication::setOrganizationName("Esri");
  QApplication app(argc, argv);

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

#ifdef CPP_VIEWER
  // add image provider
  const QString name = MapImageProvider::imageProviderId();
  engine.addImageProvider(name, new MapImageProvider);
#endif // CPP_VIEWER

  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

  return app.exec();
}

void registerCppSampleClasses()
{
  // register the draw order model
  qmlRegisterType<DrawOrderLayerListModel>("Esri.Samples", 1, 0, "DrawOrderListModel");

#ifdef CPP_VIEWER
  // Register the C++ Samples under the Esri.Samples namespace
  AddAPointSceneLayer::init();
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
  ConfigureSubnetworkTrace::init();
  ContingentValues::init();
  ControlAnnotationSublayerVisibility::init();
  ControlTimeExtentTimeSlider::init();
  ConvexHull::init();
  CreateAndSaveKmlFile::init();
  CreateAndSaveMap::init();
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
  DisplayDimensions::init();
  DisplayContentOfUtilityNetworkContainer::init();
  DisplayDeviceLocation::init();
  DisplayDeviceLocationWithNmeaDataSources::init();
  DisplayDrawingStatus::init();
  DisplayGrid::init();
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
  Feature_Collection_Layer::init();
  FeatureCollectionLayerFromPortal::init();
  FeatureCollectionLayerQuery::init();
  FeatureLayer_GeoPackage::init();
  FeatureLayerChangeRenderer::init();
  FeatureLayerDictionaryRenderer::init();
  FeatureLayerExtrusion::init();
  FeatureLayerFeatureService::init();
  FeatureLayerGeodatabase::init();
  FeatureLayerQuery::init();
  FeatureLayerRenderingModeMap::init();
  FeatureLayerRenderingModeScene::init();
  FeatureLayerSelection::init();
  FeatureLayerShapefile::init();
  FilterByDefinitionExpressionOrDisplayFilter::init();
  FindAddress::init();
  FindClosestFacilityToMultipleIncidentsService::init();
  FindPlace::init();
  FindRoute::init();
  FindServiceAreasForMultipleFacilities::init();
  FormatCoordinates::init();
  GenerateGeodatabaseReplicaFromFeatureService::init();
  GenerateOfflineMap_Overrides::init();
  GenerateOfflineMap::init();
  GenerateOfflineMapLocalBasemap::init();
  GeodesicOperations::init();
  Geotriggers::init();
  GetElevationAtPoint::init();
  GODictionaryRenderer_3D::init();
  GODictionaryRenderer::init();
  GOSymbols::init();
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
  NavigateRoute::init();
  NavigateARouteWithRerouting::init();
  NearestVertex::init();
  OfflineGeocode::init();
  OfflineRouting::init();
  OpenMapUrl::init();
  OpenMobileMap_MapPackage::init();
  OpenMobileScenePackage::init();
  OpenScene::init();
  OrbitCameraAroundObject::init();
  OSM_Layer::init();
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
  ShowCallout::init();
  ShowDeviceLocationUsingIndoorPositioning::init();
  ShowLabelsOnLayers::init();
  ShowLocationHistory::init();
  ShowMagnifier::init();
  ShowPopup::init();
  Simple_Marker_Symbol::init();
  Simple_Renderer::init();
  SketchOnMap::init();
  SpatialOperations::init();
  SpatialRelationships::init();
  StatisticalQuery::init();
  StatisticalQueryGroupSort::init();
  StyleWmsLayer::init();
  Surface_Placement::init();
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
  VectorTiledLayerUrl::init();
  ViewContentBeneathTerrainSurface::init();
  ViewPointCloudDataOffline::init();
  ViewshedCamera::init();
  ViewshedGeoElement::init();
  ViewshedLocation::init();
  Web_Tiled_Layer::init();
  WmsLayerUrl::init();
  WMTS_Layer::init();

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

#endif // CPP_VIEWER
}

void registerClasses()
{
  qmlRegisterType<PermissionsHelper>("Esri.ArcGISRuntimeSamples", 1, 0, "PermissionsHelper");

  qmlRegisterSingletonType<DownloadSampleManager>("Esri.ArcGISRuntimeSamples", 1, 0, "SampleManager",
                                                  &esriSampleManagerProvider);
  // Register the Google Analytics class
  qmlRegisterSingletonType<GAnalytics>("Telemetry", 1, 0, "GAnalytics", &gAnalyticsProvider);

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

#ifndef CPP_VIEWER
  qmlRegisterType<NavigateRouteSpeaker>("Esri.samples", 1, 0, "NavigateRouteSpeaker");
#endif

  // register the C++ Sample Classes if building for C++ API
  registerCppSampleClasses();
}

QObject* esriSampleManagerProvider(QQmlEngine* engine, QJSEngine*)
{
#ifdef CPP_VIEWER
  static QObject* sampleManager = new CppSampleManager(engine);
#else
  static QObject* sampleManager = new QmlSampleManager(engine, engine);
#endif
  return sampleManager;
}

QObject* gAnalyticsProvider(QQmlEngine* engine, QJSEngine*)
{
  static QObject* gAnalytics = new GAnalytics(engine);
  return gAnalytics;
}

QObject* syntaxHighlighterProvider(QQmlEngine* engine, QJSEngine*)
{
  static SyntaxHighlighter* syntaxHighlighter = new SyntaxHighlighter(engine);
  return syntaxHighlighter;
}
