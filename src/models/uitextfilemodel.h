/****************************************************************************
**
** Copyright (C) 2012 Instituto Nokia de Tecnologia (INdT)
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

#ifndef UITEXTFILEMODEL_H
#define UITEXTFILEMODEL_H

#include "uihelpersglobal.h"
#include "qstandarditemmodel.h"
#include "QtCore/qscopedpointer.h"
#include "QtCore/qurl.h"
#include "QtCore/qstring.h"
#include "QtCore/qregexp.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE_UIHELPERS

#ifndef QT_NO_TEXTFILEMODEL

class UiTextFileModelPrivate;

class UIHELPERS_EXPORT UiTextFileModel : public QStandardItemModel
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator NOTIFY separatorChanged)
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity
               WRITE setCaseSensitivity NOTIFY caseSensitivityChanged)

public:
    explicit UiTextFileModel(QObject *parent = 0);
    ~UiTextFileModel();

    QString source() const;
    void setSource(const QString &source);
    QString separator() const;
    void setSeparator(const QString &separator);
    Qt::CaseSensitivity caseSensitivity() const;
    void setCaseSensitivity(const Qt::CaseSensitivity caseSensitivity);

Q_SIGNALS:
    void sourceChanged();
    void separatorChanged();
    void caseSensitivityChanged();

private:
    Q_DISABLE_COPY(UiTextFileModel)
    Q_DECLARE_PRIVATE(UiTextFileModel)

    QScopedPointer<UiTextFileModelPrivate> d_ptr;
};

#endif // QT_NO_TEXTFILEMODEL

QT_END_NAMESPACE_UIHELPERS

QT_END_HEADER

#endif // UITEXTFILEMODEL_H

