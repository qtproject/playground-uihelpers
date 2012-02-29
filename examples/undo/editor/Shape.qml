import QtQuick 2.0

Rectangle {
    width: parent.width
    height: 50
    MouseArea {
        anchors.fill: parent
        onClicked: {
            canvas.insertObject(parent.color)
        }
    }
}
