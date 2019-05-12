import QtQuick 2.0

Item {
    property int value: 4
    property int minimum: 0
    property int maximum: 10
    property bool isOutsideControl: false

    property string colorBackground: "gray"
    property string colorProgress: "white"
    property string colorPanel: "white"

    id: root

    Rectangle {
        id: rectBackground
        height: parent.height/2
        width: parent.width
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
        color: colorBackground
    }

    Rectangle {
        id: rectPanel
        anchors {
            verticalCenter: root.verticalCenter
            horizontalCenter: rectProgress.right
        }

        height: parent.height
        width: height
        radius: width
        color: colorPanel
    }

    Rectangle {
        id: rectProgress
        height: rectBackground.height
        width: value*root.width/maximum
        color: colorProgress
        anchors {
            verticalCenter: rectBackground.verticalCenter
            left: root.left
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onPressed: {
            isOutsideControl = true
            setPixels(mouse.x)
        }
        onReleased: {
            setPixels(mouse.x)
            isOutsideControl = false
        }
        onPositionChanged: setPixels(mouse.x)
    }

    function setPixels(pixels) {
        var value = (maximum - minimum) / root.width * pixels + minimum
        root.value = Math.min(Math.max(minimum, value), maximum)
    }
}
