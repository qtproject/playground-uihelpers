/****************************************************************************
**
** Copyright (C) 2012 Instituto Nokia de Tecnologia (INdT).
** Contact: http://www.qt-project.org/
**
** This file is part of the UiHelpers playground module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
