/****************************************************************************
**
** Copyright (C) 2012 Instituto Nokia de Tecnologia (INdT).
** Contact: http://www.qt-project.org/
**
** This file is part of the UiHelpers playground module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
