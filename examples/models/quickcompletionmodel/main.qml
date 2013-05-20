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
import Playground.UiHelpers.Models 1.0

Rectangle {
    id: root

    width: 420
    height: 400

    CompletionModel {
        id: completionModel
        sourceModel: ["Recife", "New York", 4.5556, "Ha!", "NEW ORLEANS", "Hawaii",
                      "Ascension Island", "Andorra", "United Arab Emirates", "Afghanistan",
                      "Antigua and Barbuda", "Anguilla", "Albania", "Armenia",
                      "Netherlands Antilles", "Angola", "Antarctica", true]
        completionPrefix: input.text
        caseSensitivity: Qt.CaseInsensitive
    }

    CompletionModel {
        id: completionModel2
        sourceModel: ListModel {
            ListElement { name: "NEW ORLEANS"; zip: "00000" }
            ListElement { name: "Hawaii"; zip: "12345" }
            ListElement { name: "Andorra"; zip: "11258" }
            ListElement { name: "Afghanistan"; zip: "99988" }
            ListElement { name: "Recife"; zip: "25479" }
        }
        completionRoleName: "name"
        completionPrefix: input2.text
        caseSensitivity: Qt.CaseInsensitive
    }

    Rectangle {
        id: center
        width: 20
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        color: "black"
    }

    Rectangle {
        id: rec
        anchors {
            top: parent.top
            left: parent.left
            right: center.left
        }
        color: "lightblue"
        height: 30

        TextInput {
            id: input
            anchors.fill: parent
            focus: true
        }
    }

    Rectangle {
        id: rec2
        anchors {
            top: parent.top
            left: center.right
            right: parent.right
        }
        color: "lightblue"
        height: 30

        TextInput {
            id: input2
            anchors.fill: parent
        }
    }

    ListView {
        anchors {
            top: rec.bottom
            bottom: parent.bottom
            left: parent.left
            right: center.right
            topMargin: 10
        }
        model: completionModel
        delegate: Text {
            text: modelData
        }
    }

    ListView {
        anchors {
            top: rec2.bottom
            bottom: parent.bottom
            left: center.right
            right: parent.right
            topMargin: 10
        }
        model: completionModel2
        delegate: Text {
            text: name
        }
    }
}
