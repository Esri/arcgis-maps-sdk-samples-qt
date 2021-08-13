import QtQuick 2.6
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.12

Item {

    property var rtCreate: ArcGISRuntimeEnvironment.createObject

    function createHeartGeometry(centerX, centerY, sideLength, sr) {
        const minX = centerX - 0.5 * sideLength;
        const minY = centerY - 0.5 * sideLength;
        const arcRadius = sideLength * 0.25;

        // Variables used for arcs
        const rotationAngle = 0;
        const isMinor = false;
        const isCounterClockwise = false;
        const semiMajorAxis = sideLength * 0.25;
        const minorMajorRatio = 1;

        // Bottom left curve
        const leftCurveStart = rtCreate("Point", {x: centerX, y: minY, spatialReference: sr});
        const leftControlPoint1 = rtCreate("Point", {x: centerX, y: minY + 0.25 * sideLength, spatialReference: sr});
        const leftControlPoint2 = rtCreate("Point", {x: minX, y: centerY, spatialReference: sr});
        const leftCurveEnd = rtCreate("Point", {x: minX, y: minY + 0.75 * sideLength, spatialReference: sr});

        const leftCurveSegment = Factory.CubicBezierSegment.createWithPoints(leftCurveStart, leftControlPoint1, leftControlPoint2, leftCurveEnd, sr);

        // Top left arc
        const leftArcStart = leftCurveEnd;
        const leftArcEnd = rtCreate("Point", {x: centerX, y: minY + 0.75 * sideLength, spatialReference: sr});

        const leftArc = Factory.EllipticArcSegment.createEllipticArcWithStartAndEndpoints(leftArcStart, leftArcEnd, rotationAngle, isMinor, isCounterClockwise, semiMajorAxis, minorMajorRatio, sr)

        // Top right arc
        const rightArcStart = leftArcEnd;
        const rightArcEnd = rtCreate("Point", {x: minX + sideLength, y: minY + 0.75 * sideLength, spatialReference: sr});

        const rightArc = Factory.EllipticArcSegment.createEllipticArcWithStartAndEndpoints(rightArcStart, rightArcEnd, rotationAngle, isMinor, isCounterClockwise, semiMajorAxis, minorMajorRatio, sr)

        // Bottom right curve
        const rightCurveStart = rightArcEnd;
        const rightControlPoint1 = rtCreate("Point", {x: minX + sideLength, y: centerY, spatialReference: sr});
        const rightControlPoint2 = leftControlPoint1;
        const rightCurveEnd = leftCurveStart;

        const rightCurveSegment = Factory.CubicBezierSegment.createWithPoints(rightCurveStart, rightControlPoint1, rightControlPoint2, rightCurveEnd, sr);

        // Create a part with each of the segments
        const part = rtCreate("Part", {spatialReference: sr});
        part.addSegment(leftCurveSegment);
        part.addSegment(leftArc);
        part.addSegment(rightArc);
        part.addSegment(rightCurveSegment);

        const polygonBuilder = rtCreate("PolygonBuilder", {spatialReference: sr});
        polygonBuilder.parts.addPart(part);

        return polygonBuilder.geometry;

    }
}
