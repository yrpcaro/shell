pragma ComponentBehavior: Bound

import QtQuick
import Quickshell
import Caelestia.Components

Item {
    id: root

    property var source: ""
    property bool playing: true
    property real speed: 1.0
    property int fillMode: Image.PreserveAspectFit
    property bool asynchronous: true
    property bool mipmap: true

    readonly property bool isSvg: {
        const s = ("" + (root.source ?? "")).toLowerCase();
        return s.endsWith(".svg") || s.endsWith(".svgz");
    }

    Loader {
        id: loader

        anchors.fill: parent
        sourceComponent: root.isSvg ? svgComp : imgComp
    }

    Component {
        id: svgComp

        AnimatedSvg {
            anchors.fill: parent
            source: root.source
            playing: root.playing
            speed: root.speed
            fillMode: root.fillMode === Image.Stretch ? AnimatedSvg.Stretch
                    : root.fillMode === Image.PreserveAspectCrop ? AnimatedSvg.PreserveAspectCrop
                    : AnimatedSvg.PreserveAspectFit
        }
    }

    Component {
        id: imgComp

        AnimatedImage {
            anchors.fill: parent
            source: root.source
            playing: root.playing
            speed: root.speed
            fillMode: root.fillMode === Image.Stretch ? AnimatedImage.Stretch
                    : root.fillMode === Image.PreserveAspectCrop ? AnimatedImage.PreserveAspectCrop
                    : AnimatedImage.PreserveAspectFit
            asynchronous: root.asynchronous
            mipmap: root.mipmap
            sourceSize.width: root.width * ((QsWindow.window as QsWindow)?.devicePixelRatio ?? 1)
        }
    }
}
