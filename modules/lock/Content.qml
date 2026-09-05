pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Caelestia.Config
import qs.components
import qs.services

Item {
    id: root

    required property var lock

    Center {
        anchors.centerIn: parent
        height: parent.height
        lock: root.lock
    }
}
