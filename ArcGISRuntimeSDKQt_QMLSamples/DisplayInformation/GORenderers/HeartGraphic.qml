import QtQuick 2.6
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.12

Item {

    property var centerX: 10
    property var centerY: 10
    property var sideLength: 10
    property var minX: centerX - 0.5 * sideLength;
    property var minY: centerY - 0.5 * sideLength;
    property var arcRadius: sideLength * 0.25;
    property SpatialReference spatialReference: SpatialReference { wkid: 4326 }

    // Bottom left PathCurve
    Point {
        id: leftCurveStart
        x: centerX
        y: minY
        spatialReference: spatialReference
    }

    Point {
        id: leftCurveEnd
        x: minX
        y: minY + 0.75 * sideLength
        spatialReference: spatialReference
    }

    Point {
        id: leftControlPoint1
        x: centerX
        y: minY + 0.25 * sideLength
        spatialReference: spatialReference
    }

    Point {
        id: leftControlPoint2
        x: minX
        y: centerY
        spatialReference: spatialReference
    }


    // Bottom right PathCurve
    Point {
        id: rightCurveStart
        x: minX + sideLength
        y: minY + 0.75 * sideLength
        spatialReference: spatialReference
    }

    Point {
        id: rightCurveEnd
        x: centerX // leftCurveStart.x
        y: minY // leftCurveStart.y
        spatialReference: leftCurveStart.spatialReference
    }

    Point {
        id: rightControlPoint1
        x: minX + sideLength
        y: centerY
        spatialReference: spatialReference
    }

    Point {
        id: rightControlPoint2
        x: centerX // leftControlPoint1.x
        y: minY + 0.25 * sideLength // leftControlPoint1.y
        spatialReference: leftControlPoint1.spatialReference
    }

    // Top left arc
    Point {
        id: leftArcCenter
        x: minX + 0.25 * sideLength
        y: minY + 0.75 * sideLength
        spatialReference: spatialReference
    }

    // Top right arc
    Point {
        id: rightArcCenter
        x: minX + 0.75 * sideLength
        y: minY + 0.75 * sideLength
        spatialReference: spatialReference
    }

    PolygonBuilder {
        id: polygonBuilder
        spatialReference: spatialReference
    }

    function createHeartGeometry() {
        let leftCurve = Factory.CubicBezierSegment.createWithPoints(leftCurveStart, leftControlPoint1, leftControlPoint2, leftCurveEnd, spatialReference);
        const rightCurve = Factory.CubicBezierSegment.createWithPoints(rightCurveStart, rightControlPoint1, rightControlPoint2, rightCurveEnd, spatialReference);

        const topLeftArc = Factory.EllipticArcSegment.createEllipticArcWithCenter(leftArcCenter, arcRadius, Math.PI, -Math.PI, spatialReference);
        const topRightArc = Factory.EllipticArcSegment.createEllipticArcWithCenter(rightArcCenter, arcRadius, Math.PI, -Math.PI, spatialReference);

        polygonBuilder.parts.addParts([leftCurve, topLeftArc, topRightArc, rightCurve]);

        return polygonBuilder.geometry;
    }
}
