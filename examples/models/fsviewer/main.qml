import QtQuick 2.0

Item {
    id: root
    anchors.fill: parent

    ListView {
        anchors.fill: parent
        model: fsmodel
        delegate: Rectangle {
            height: 20
            color: "green"
            width: parent.width
            Text {
                anchors.fill: parent
                text: model.fileName
            }
        }
    }
}
