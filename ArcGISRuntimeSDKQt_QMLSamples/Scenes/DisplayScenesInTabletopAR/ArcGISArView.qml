/*******************************************************************************
 *  Copyright 2012-2019 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

import QtQuick 2.12
import QtQuick.Window 2.12
import Esri.ArcGISRuntime 100.6
import Esri.ArcGISArToolkit 1.0

ArcGISArViewInternal {
    id: root

    // This QML component is used to create the QML's TMCC object from the C++.
    transformationMatrixCameraController: TransformationMatrixCameraController {
        id: tmcc
    }

    // Connect the sceneView.cameraController to transformationMatrixCameraController
    Binding { target: sceneView; property: "cameraController"; value: transformationMatrixCameraController }

    // Update the translation factor.
    onTranslationFactorChanged: tmcc.translationFactor = translationFactor;

    // Render the scene.
    onRenderFrame: {
        if (sceneView)
            sceneView.renderFrame();
    }

    // Update the initial transformation, using the hit matrix.
    property TransformationMatrix initialTransformationMatrix: null

    onInitialTransformationChanged: {
        // Set the `initialTransformationMatrix` as the TransformationMatrix.identity - hit test matrix.
        const hitMatrix = TransformationMatrix.createWithQuaternionAndTranslation(
                    quaternionX, quaternionY, quaternionZ, quaternionW,
                    translationX, translationY, translationZ);

        const identityTransformationMatrix = TransformationMatrix.createIdentityMatrix();
        initialTransformationMatrix = identityTransformationMatrix.subtractTransformation(hitMatrix);
    }

    // It's not possible to create the TransformationMatrix object directly in C++. This function
    // is used to create the TM object and assign it to the TMCC.
    onTransformationMatrixChanged: {
        const matrix = TransformationMatrix.createWithQuaternionAndTranslation(
                    quaternionX, quaternionY, quaternionZ, quaternionW,
                    translationX, translationY, translationZ);

        if (!initialTransformationMatrix)
            initialTransformationMatrix = TransformationMatrix.createIdentityMatrix();

        const finalMatrix = initialTransformationMatrix.addTransformation(matrix);
        tmcc.transformationMatrix = finalMatrix;
    }

    // It's not possible to call setFieldOfViewFromLensIntrinsics directly from the C++ code, due to
    // the orientation device enumeration. This function is used to converts the orientation (int) to
    // deviceOrientation (enum).
    onFieldOfViewChanged: {
        if (!sceneView)
            return;

        const deviceOrientation = toDeviceOrientation(Screen.orientation);
        sceneView.setFieldOfViewFromLensIntrinsics(xFocalLength, yFocalLength,
                                                   xPrincipal, yPrincipal,
                                                   xImageSize, yImageSize,
                                                   deviceOrientation);
    }

    // Update the origin camera
    onOriginCameraChanged: {
        originCameraInternal = originCamera;

        // Update TMCC origin camera.
        updateTmccOriginCamera();
    }

    // Update the location.
    onLocationChanged: {
        const location = ArcGISRuntimeEnvironment.createObject("Point", { y: latitude, x: longitude, z: altitude });

        // Save location camera parameters.
        if (!locationCameraInternal) {
            locationCameraInternal = ArcGISRuntimeEnvironment.createObject("Camera", {
                location: location, heading: 0.0, pitch: 90.0, roll: 0.0 });

        }
        else {
            locationCameraInternal = ArcGISRuntimeEnvironment.createObject("Camera", {
                location: location,
                heading: locationCameraInternal.heading,
                pitch: locationCameraInternal.pitch,
                roll: locationCameraInternal.roll });
        }

        // Update TMCC origin camera.
        updateTmccOriginCamera();
    }

    // Update the heading.
    onHeadingChanged: {
        // Save location camera parameters.
        if (!locationCameraInternal) {
            const location = ArcGISRuntimeEnvironment.createObject("Point", { y: 0.0, x: 0.0, z: 0.0 });
            locationCameraInternal= ArcGISRuntimeEnvironment.createObject("Camera", {
                location: location, heading: heading, pitch: 90.0, roll: 0.0 });
        }
        else {
            locationCameraInternal= ArcGISRuntimeEnvironment.createObject("Camera", {
                location: locationCameraInternal.location,
                heading: heading,
                pitch: locationCameraInternal.pitch,
                roll: locationCameraInternal.roll });
        }

        // Update TMCC origin camera.
        updateTmccOriginCamera();
    }

    // Resets the device tracking and related properties.
    onResetTrackingChanged: {
        const camera = ArcGISRuntimeEnvironment.createObject("Camera");
        tmcc.originCamera = camera;

        initialTransformationMatrix = TransformationMatrix.createIdentityMatrix();
        tmcc.transformationMatrix = TransformationMatrix.createIdentityMatrix();
    }

    /*!
        \brief Gets the location in the real world space corresponding to the screen point \a screenPoint.
     */
    function screenToLocation(x, y) {
        const hitTest = root.hitTest(x, y);

        const hitMatrix = TransformationMatrix.createWithQuaternionAndTranslation(
                            hitTest[0], hitTest[1], hitTest[2], hitTest[3], // quaternionX, quaternionY, quaternionZ, quaternionW
                            hitTest[4], hitTest[5], hitTest[6]); // translationX, translationY, translationZ

        const origin = tmcc.originCamera.transformationMatrix;
        const intermediateMatrix = origin.addTransformation(initialTransformationMatrix);
        const finalMatrix = intermediateMatrix.addTransformation(hitMatrix);
        const camera = ArcGISRuntimeEnvironment.createObject("Camera", { transformationMatrix: finalMatrix });
        const location = ArcGISRuntimeEnvironment.createObject("Point", {
            x: camera.location.x * translationFactor,
            y: camera.location.y * translationFactor,
            z: camera.location.z * translationFactor });
        return location;
    }

    /*!
        \internal
        Cast from Qt's screen orientation to ArcGIS Runtime's screen orientation.
     */
    function toDeviceOrientation(orientation) {
        switch(Screen.orientation) {
        case Qt.PortraitOrientation:
            return Enums.DeviceOrientationPortrait;
        case Qt.LandscapeOrientation:
            return Enums.DeviceOrientationLandscapeLeft;
        case Qt.InvertedPortraitOrientation:
            return Enums.DeviceOrientationReversePortrait;
        case Qt.InvertedLandscapeOrientation:
            return Enums.DeviceOrientationPortrait;
        default:
            return Enums.DeviceOrientationPortrait;
        }
    }

    /*!
        \internal
        The viewpoint camera used to set the initial view of the scene view. This camera can be
        modified by the calibration.
     */
    property Camera originCameraInternal: null;

    /*!
        \internal
        The camera corresponding to the GPS coordinates.
     */
    property Camera locationCameraInternal: null;

    /*!
        \internal

        Calculate the origin camera to use in TMCC, by addition of the origin camera (wich contains
        the initial view of the scene view and calibration) and the location camera (which contains
        the GPS data).
        originCameraInternal and locationCameraInternal can be invalid if not set previously.
     */
    function updateTmccOriginCamera() {
        if (!originCameraInternal) {
            if (locationCameraInternal) {
                tmcc.originCamera = locationCameraInternal;
            }
        }
        else
        {
            if (!locationCameraInternal) {
                tmcc.originCamera = originCameraInternal;
            }
            else {
                const oldLocation = locationCameraInternal.location;
                const oldOriginLocation = originCameraInternal.location;

                // create a new origin camera
                const newLocation = ArcGISRuntimeEnvironment.createObject("Point", {
                    x: oldLocation.x + oldOriginLocation.x,
                    y: oldLocation.y + oldOriginLocation.y,
                    z: oldLocation.z + oldOriginLocation.z });

                const newCamera = ArcGISRuntimeEnvironment.createObject("Camera", {
                    location: newLocation,
                    heading: originCameraInternal.heading + locationCameraInternal.heading,
                    pitch: originCameraInternal.pitch + locationCameraInternal.pitch,
                    roll: originCameraInternal.roll + locationCameraInternal.roll, });

                tmcc.originCamera = newCamera;
            }
        }
    }
}
