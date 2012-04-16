/****************************************************************************
**
** Copyright (C) 2012 Instituto Nokia de Tecnologia (INdT)
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

#include <QtCore/QTemporaryFile>
#include <QtCore/QUrl>
#include <QtTest/QtTest>
#include <UiHelpers/UiTextFileModel>

class tst_UiTextFileModel: public QObject
{
    Q_OBJECT

public slots:
    void init();

private:
    int rowsCount;
    int columnsCount;
    QTemporaryFile file;

private slots:
    void commaSeparatorCount();
    void twoLettersSeparatorCount();
    void carriageReturnSeparatorCount();
    void carriageReturnLineFeedSeparatorCount();
    void specialCharacterSeparatorCount();
    void withoutSeparatorCount();
};

void tst_UiTextFileModel::init()
{
    rowsCount = 10;
    columnsCount = 10;
    file.open();
    QString text = "Example File";
    for (int i=0; i < rowsCount; i++) {
        text.append("\nComeçando nova linha: "); // language: portuguese (test special characters)
        for (int j=0; j < columnsCount; j++) {
            text.append(QString::number(j));
            text.append(", ");
        }
    }
    file.write(text.toAscii());
    file.flush();
    file.close();
}

void tst_UiTextFileModel::commaSeparatorCount()
{
    UiHelpers::UiTextFileModel model;
    model.setSeparator(",");
    model.setSource(file.fileName());
    QCOMPARE(model.rowCount(), rowsCount * columnsCount + 1);
}

void tst_UiTextFileModel::twoLettersSeparatorCount()
{
    UiHelpers::UiTextFileModel model;
    model.setSeparator(", 8, 9");
    model.setSource(file.fileName());
    QCOMPARE(model.rowCount(), rowsCount + 1);
}

void tst_UiTextFileModel::carriageReturnSeparatorCount()
{
    UiHelpers::UiTextFileModel model;
    model.setSeparator("\n");
    model.setSource(file.fileName());
    QCOMPARE(model.rowCount(), rowsCount + 1);
}

void tst_UiTextFileModel::carriageReturnLineFeedSeparatorCount()
{
    UiHelpers::UiTextFileModel model;
    model.setSeparator("\n\r");
    model.setSource(file.fileName());
    QCOMPARE(model.rowCount(), 1);
}

void tst_UiTextFileModel::specialCharacterSeparatorCount()
{
    UiHelpers::UiTextFileModel model;
    model.setSeparator("ç");
    model.setSource(file.fileName());
    QCOMPARE(model.rowCount(), rowsCount + 1);
}

void tst_UiTextFileModel::withoutSeparatorCount()
{
    UiHelpers::UiTextFileModel model;
    model.setSource(file.fileName());
    QCOMPARE(model.rowCount(), rowsCount + 1);
}

QTEST_MAIN(tst_UiTextFileModel)
#include "tst_uitextfilemodel.moc"
