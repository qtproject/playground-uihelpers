import QtQuick 2.0

Rectangle {
    id: root
    property alias text: label.text
    signal clicked()


    width: 50
    height: 50
    Text {
        id: label
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
