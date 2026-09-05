pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import M3Shapes
import Caelestia.Config
import qs.components
import qs.components.effects
import qs.components.images
import qs.services
import qs.utils

ColumnLayout {
    id: root

    readonly property real avatarSize: 96
    readonly property color bgColour: Colours.tPalette.m3surfaceContainerHighest

    spacing: Tokens.spacing.small

    Item {
        Layout.alignment: Qt.AlignHCenter
        implicitWidth: root.avatarSize
        implicitHeight: root.avatarSize

        MaterialShape {
            id: shape

            anchors.fill: parent
            implicitSize: root.avatarSize
            shape: MaterialShape.Circle
            color: Qt.alpha(root.bgColour, 1)
            opacity: root.bgColour.a
            layer.enabled: true
        }

        MaterialIcon {
            anchors.centerIn: parent
            text: "person"
            color: Colours.palette.m3primary
            fontStyle: Tokens.font.icon.size(root.avatarSize * 0.5).build()
            visible: pfp.status !== Image.Ready
        }

        CachingImage {
            id: pfp

            anchors.fill: shape
            path: `${Paths.home}/.face`

            layer.enabled: true
            layer.effect: Mask {
                maskSource: shape
            }
        }

        Rectangle {
            anchors.fill: parent
            radius: width / 2
            color: "transparent"
            border.color: Qt.alpha(Colours.palette.m3primary, 0.35)
            border.width: 2
        }
    }

    StyledText {
        Layout.alignment: Qt.AlignHCenter
        text: SysInfo.user
        color: Colours.palette.m3onSurface
        font: Tokens.font.title.builders.medium.weight(Font.DemiBold).build()
    }
}
