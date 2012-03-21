import QtQuick 2.0

Rectangle {
    width: 300
    height: 400
    color: "blue"

    Rectangle {
        y: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        height: 20
        color: "white"

        TextInput {
            anchors.fill: parent
            clip: true
            onTextChanged: completionModel.setCompletionPrefix(text)
        }
    }

    Rectangle {
        anchors.top: parent.top
        anchors.topMargin: 40
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        color: "white"

        ListView {
            anchors.fill: parent
            model: completionModel
            clip: true
            delegate: Text {
                text: display
            }
        }
    }
}
