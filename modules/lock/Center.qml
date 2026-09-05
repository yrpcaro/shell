pragma ComponentBehavior: Bound

import "center"
import QtQuick
import QtQuick.Layouts
import Caelestia.Config
import qs.components
import qs.services

ColumnLayout {
    id: root

    required property var lock
    readonly property real centerScale: Math.min(1, Math.max(0.7, (lock.screen?.height ?? 1080) / 1080))
    readonly property int centerWidth: Math.min(420, (lock.screen?.width ?? 1920) * 0.8)

    Layout.preferredWidth: centerWidth
    Layout.fillWidth: false
    Layout.fillHeight: true

    spacing: Tokens.spacing.medium

    Item {
        Layout.fillHeight: true
    }

    Clock {
        Layout.alignment: Qt.AlignHCenter
        centerScale: root.centerScale * 0.85
    }

    StyledText {
        Layout.alignment: Qt.AlignHCenter

        text: Time.format("dddd • d MMMM").toUpperCase()
        color: Colours.palette.m3onSurfaceVariant
        font: Tokens.font.title.builders.small.weight(Font.DemiBold).build()
    }

    Item {
        Layout.preferredHeight: Tokens.spacing.medium
    }

    ProfilePic {
        Layout.alignment: Qt.AlignHCenter
    }

    Item {
        Layout.preferredHeight: Tokens.spacing.medium
    }

    PasswordInput {
        Layout.alignment: Qt.AlignHCenter
        centerScale: root.centerScale
        centerWidth: root.centerWidth
        lock: root.lock
    }

    StateMessage {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true
        pam: root.lock.pam
    }

    Item {
        Layout.fillHeight: true
    }
}
