import QtQuick 2.0

Item {
    width: 400
    height: 400

    Column {
        id: column
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }
        width: 100
        Rectangle {
            width: 100
            height: 50
            color: "cyan"

            Text {
                anchors.fill: parent
                text: "Create"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: stack.pushCommand();
            }
        }

        Rectangle {
            width: 100
            height: 50
            color: "tomato"
            Text {
                anchors.fill: parent
                text: "Undo"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: stack.undo()
            }
        }
        Rectangle {
            width: 100
            height: 50
            color: "blue"
            Text {
                anchors.fill: parent
                text: "Redo"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: stack.redo()
            }
        }
    }

    //undostack
    Text {
        id: title
        anchors {
            top: parent.top
            left: column.right
            leftMargin: 10
        }
        text: "UndoStack:"
    }

    ListView {
        anchors {
            top: title.bottom
            topMargin: 20
            bottom: parent.bottom
            left: title.left
            right: parent.right
        }

        model: stack.stack;

        delegate:Text {
            height: 25
            width: 200
            text: modelData
        }
    }
}
