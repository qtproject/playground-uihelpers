import QtQuick 2.0
import Playground.UiHelpers.UndoFramework 1.0

Rectangle {
    id: root

    width: 800
    height: 600

    UndoStack {
        id: stack
        undoLimit: 5
    }

    UndoPropertyCommand {
        id: moveCommand
        properties: ["x", "y"]
    }

    UndoCommand {
        id: colorCommand

        property color black: Qt.rgba(0, 0, 0, 1)
        property color lightRed: Qt.rgba(0.5, 0.2, 0.1, 1)

        function swapColors(target) {
            if (target.color == lightRed)
                target.color = black;
            else
                target.color = lightRed;
        }

        onUndo: swapColors(target);
        onRedo: swapColors(target);
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
            color: stack.count ? "yellow" : "gray"
            text: "Clear"
            onClicked: stack.clear();
        }
        Button {
            color: "red"
            width: 100
            text: "Change color"
            onClicked: stack.push(colorCommand, rec);
        }
        Button {
            color: stack.canUndo ? "blue" : "gray"
            text: "Undo"
            onClicked: stack.undo();
        }
        Button {
            color: stack.canRedo ? "green" : "gray"
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
