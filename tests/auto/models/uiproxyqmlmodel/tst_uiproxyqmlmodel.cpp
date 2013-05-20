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

#include <QtTest/QtTest>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <UiHelpers/UiProxyQmlModel>

QT_USE_NAMESPACE_UIHELPERS

class tst_UiProxyQmlModel: public QObject
{
    Q_OBJECT

public slots:
    void init();
    void cleanup();

private:
    QObject *object;
    UiProxyQmlModel arrayList;
    UiProxyQmlModel quickList;
    UiProxyQmlModel invalidList;

private slots:
    void type();
    void count();
    void roles();
    void data();
};

void tst_UiProxyQmlModel::init()
{
    QQmlEngine *engine = new QQmlEngine;
    QQmlContext *context = engine->rootContext();
    QQmlComponent *component = new QQmlComponent(engine);

    QFile file("models.qml");
    file.open(QIODevice::ReadOnly);
    component->setData(file.readAll(), QUrl());
    object = component->create(context);
    file.close();
}

void tst_UiProxyQmlModel::cleanup()
{
    delete object;
}

void tst_UiProxyQmlModel::type()
{
    QCOMPARE(arrayList.updateSource(object->property("arrayList")), UiProxyQmlModel::ArrayList);
    QCOMPARE(quickList.updateSource(object->property("quickList")), UiProxyQmlModel::QuickList);
    QCOMPARE(invalidList.updateSource(object->property("invalidList")), UiProxyQmlModel::InvalidList);
}

void tst_UiProxyQmlModel::count()
{
    QCOMPARE(arrayList.rowCount(), 4);
    QCOMPARE(quickList.rowCount(), 3);
    QCOMPARE(invalidList.rowCount(), 0);
}

void tst_UiProxyQmlModel::roles()
{
    const QHash<int, QByteArray> &arrayListRoles = arrayList.roleNames();
    QCOMPARE(arrayListRoles.size(), 1);
    QCOMPARE(arrayListRoles[Qt::DisplayRole], QByteArray("display"));

    const QHash<int, QByteArray> &quickListRoles = quickList.roleNames();
    QCOMPARE(quickListRoles.size(), 2);
    QVERIFY(quickListRoles.key("name", -1) != -1);
    QVERIFY(quickListRoles.key("age", -1) != -1);
}

void tst_UiProxyQmlModel::data()
{
    QModelIndex index = arrayList.index(0, 0);
    QCOMPARE(arrayList.data(index, Qt::DisplayRole), QVariant(QString("Brazil")));
    index = arrayList.index(1, 0);
    QCOMPARE(arrayList.data(index, Qt::DisplayRole), QVariant(QString("Monaco")));
    index = arrayList.index(2, 0);
    QCOMPARE(arrayList.data(index, Qt::DisplayRole), QVariant(1234));
    index = arrayList.index(3, 0);
    QCOMPARE(arrayList.data(index, Qt::DisplayRole), QVariant(true));

    const QHash<int, QByteArray> &roles = quickList.roleNames();
    index = quickList.index(0, 0);
    QCOMPARE(quickList.data(index, roles.key("name")), QVariant(QString("John")));
    QCOMPARE(quickList.data(index, roles.key("age")), QVariant(20));
    index = quickList.index(1, 0);
    QCOMPARE(quickList.data(index, roles.key("name")), QVariant(QString("Paul")));
    QCOMPARE(quickList.data(index, roles.key("age")), QVariant(32));
    index = quickList.index(2, 0);
    QCOMPARE(quickList.data(index, roles.key("name")), QVariant(QString("Peter")));
    QCOMPARE(quickList.data(index, roles.key("age")), QVariant(14));
}

QTEST_MAIN(tst_UiProxyQmlModel)
#include "tst_uiproxyqmlmodel.moc"
