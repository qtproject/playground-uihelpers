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

#ifndef QT_NO_TEXTFILEMODEL

#include "uitextfilemodel.h"
#include "uitextfilemodel_p.h"
#include "QtCore/qfile.h"
#include "QtCore/qscopedpointer.h"
#include "QtCore/qstring.h"
#include "QtCore/qstringlist.h"
#include "QtCore/qregexp.h"

QT_BEGIN_NAMESPACE_UIHELPERS

UiTextFileModelPrivate::UiTextFileModelPrivate(UiTextFileModel *qq)
    : separator("\n"), q_ptr(qq)
{
}

UiTextFileModelPrivate::~UiTextFileModelPrivate()
{
}

void UiTextFileModelPrivate::reload()
{
    Q_Q(UiTextFileModel);
    q->clear();

    QFile file(source);
    if (!file.open(QFile::ReadOnly)) {
        qWarning("UiTextFileModel: Could not open the file (%s)", qPrintable(file.fileName()));
        return;
    }

    QString text(file.readAll());
    file.close();

    if (text.isEmpty())
        return;

    if (!separator.isValid()) {
        qWarning("UiTextFileModel: separator is not valid");
        return;
    }

    QStringList list = text.split(separator, QString::SkipEmptyParts);
    foreach (const QString & textItem, list) {
        QStandardItem *item = new QStandardItem(textItem);
        q->appendRow(item);
    }
}

UiTextFileModel::UiTextFileModel(QObject *parent)
    : QStandardItemModel(parent), d_ptr(new UiTextFileModelPrivate(this))
{
}

UiTextFileModel::~UiTextFileModel()
{
}

QString UiTextFileModel::source() const
{
    Q_D(const UiTextFileModel);
    return d->source;
}

void UiTextFileModel::setSource(const QString &source)
{
    Q_D(UiTextFileModel);

    if (d->source == source)
        return;

    d->source = source;
    d->reload();

    emit sourceChanged();
}

QString UiTextFileModel::separator() const
{
    Q_D(const UiTextFileModel);
    return d->separator.pattern();
}

void UiTextFileModel::setSeparator(const QString &separator)
{
    Q_D(UiTextFileModel);

    if (d->separator.pattern() == separator)
        return;

    d->separator.setPattern(separator);

    if (!d->source.isEmpty())
        d->reload();

    emit separatorChanged();
}

Qt::CaseSensitivity UiTextFileModel::caseSensitivity() const
{
    Q_D(const UiTextFileModel);
    return d->separator.caseSensitivity();
}

void UiTextFileModel::setCaseSensitivity(const Qt::CaseSensitivity caseSensitivity)
{
    Q_D(UiTextFileModel);

    if (d->separator.caseSensitivity() != caseSensitivity)
        return;

    d->separator.setCaseSensitivity(caseSensitivity);

    if (!d->source.isEmpty())
        d->reload();

    emit caseSensitivityChanged();
}

QT_END_NAMESPACE_UIHELPERS

#endif // QT_NO_TEXTFILEMODEL
