import QtQuick 2.0
import Playground.UiHelpers.UndoFramework 1.0

Rectangle {
    id: root

    width: 800
    height: 600

    UndoStack {
        id: stack
    }

    UndoPropertyCommand {
        id: moveCommand
        properties: ["x", "y"]
    }

    UndoCommand {
        id: colorCommand
        onUndo: target.color = Qt.rgba(0, 0, 0, 1);
        onRedo: target.color = Qt.rgba(0.5, 0.2, 0.1, 1);
        onCommandDestroyed: console.log("Command destroyed!");
    }

    Row {
        anchors {
            right: parent.right
            top: parent.top
            margins: 20
        }
        spacing: 20

        Button {
            color: "red"
            width: 100
            text: "Change color"
            onClicked: stack.push(colorCommand, rec);
        }
        Button {
            color: "blue"
            text: "Undo"
            onClicked: stack.undo();
        }
        Button {
            color: "green"
            text: "Redo"
            onClicked: stack.redo();
        }
    }

    Rectangle {
        id: rec

        x: 20
        y: 50
        width: 50
        height: 50
        color: "black"

        Drag.active: dragArea.drag.active
        Drag.hotSpot.x: 10
        Drag.hotSpot.y: 10

        MouseArea {
            id: dragArea

            anchors.fill: parent
            drag.target: parent

            onPressed: stack.push(moveCommand, parent);
        }
    }
}
