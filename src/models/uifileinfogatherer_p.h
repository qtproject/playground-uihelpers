/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef UIFILEINFOGATHERER_H
#define UIFILEINFOGATHERER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//
#include "uihelpersglobal.h"
#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <qfilesystemwatcher.h>
#include <qpair.h>
#include <qstack.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qelapsedtimer.h>

#include <private/qfilesystemengine_p.h>
#include <qcoreapplication.h>

QT_BEGIN_NAMESPACE_UIHELPERS

class UiExtendedInformation {
public:
    enum Type { Dir, File, System };

    UiExtendedInformation() {}
    UiExtendedInformation(const QFileInfo &info) : mFileInfo(info) {}

    inline bool isDir() { return type() == Dir; }
    inline bool isFile() { return type() == File; }
    inline bool isSystem() { return type() == System; }

    bool operator ==(const UiExtendedInformation &fileInfo) const {
       return mFileInfo == fileInfo.mFileInfo
       && displayType == fileInfo.displayType
       && permissions() == fileInfo.permissions();
    }

#ifndef QT_NO_FSFILEENGINE
    bool isCaseSensitive() const {
        return QFileSystemEngine::isCaseSensitive();
    }
#endif

    QFile::Permissions permissions() const {
        return mFileInfo.permissions();
    }

    Type type() const {
        if (mFileInfo.isDir()) {
            return UiExtendedInformation::Dir;
        }
        if (mFileInfo.isFile()) {
            return UiExtendedInformation::File;
        }
        if (!mFileInfo.exists() && mFileInfo.isSymLink()) {
            return UiExtendedInformation::System;
        }
        return UiExtendedInformation::System;
    }

    bool isSymLink() const {
        return mFileInfo.isSymLink();
    }

    bool isHidden() const {
        return mFileInfo.isHidden();
    }

    QFileInfo fileInfo() const {
        return mFileInfo;
    }

    QDateTime lastModified() const {
        return mFileInfo.lastModified();
    }

    qint64 size() const {
        qint64 size = -1;
        if (type() == UiExtendedInformation::Dir)
            size = 0;
        if (type() == UiExtendedInformation::File)
            size = mFileInfo.size();
        if (!mFileInfo.exists() && !mFileInfo.isSymLink())
            size = -1;
        return size;
    }

    QString displayType;

private :
    QFileInfo mFileInfo;
};

#ifndef QT_NO_FILESYSTEMMODEL

class Q_AUTOTEST_EXPORT UiFileInfoGatherer : public QThread
{
Q_OBJECT

Q_SIGNALS:
    void updates(const QString &directory, const QList<QPair<QString, QFileInfo> > &updates);
    void newListOfFiles(const QString &directory, const QStringList &listOfFiles) const;
    void nameResolved(const QString &fileName, const QString &resolvedName) const;
    void directoryLoaded(const QString &path);

public:
    UiFileInfoGatherer(QObject *parent = 0);
    ~UiFileInfoGatherer();

    void clear();
    void removePath(const QString &path);
    UiExtendedInformation getInfo(const QFileInfo &info) const;

public Q_SLOTS:
    void list(const QString &directoryPath);
    void fetchExtendedInformation(const QString &path, const QStringList &files);
    void updateFile(const QString &path);
    void setResolveSymlinks(bool enable);
    bool resolveSymlinks() const;

protected:
    void run();
    void getFileInfos(const QString &path, const QStringList &files);

private:
    void fetch(const QFileInfo &info, QElapsedTimer &base, bool &firstTime, QList<QPair<QString, QFileInfo> > &updatedFiles, const QString &path);
    QString translateDriveName(const QFileInfo &drive) const;

    QString _type(const QFileInfo &info) const
    {
        if (info.isRoot())
            return QCoreApplication::translate("QFileDialog", "Drive");
        if (info.isFile()) {
            if (!info.suffix().isEmpty())
                return info.suffix() + QLatin1Char(' ') + QCoreApplication::translate("QFileDialog", "File");
            return QCoreApplication::translate("QFileDialog", "File");
        }

        if (info.isDir())
    #ifdef Q_OS_WIN
            return QCoreApplication::translate("QFileDialog", "File Folder", "Match Windows Explorer");
    #else
            return QCoreApplication::translate("QFileDialog", "Folder", "All other platforms");
    #endif
        // Windows   - "File Folder"
        // OS X      - "Folder"
        // Konqueror - "Folder"
        // Nautilus  - "folder"

        if (info.isSymLink())
    #ifdef Q_OS_MAC
            return QCoreApplication::translate("QFileDialog", "Alias", "Mac OS X Finder");
    #else
            return QCoreApplication::translate("QFileDialog", "Shortcut", "All other platforms");
    #endif
        // OS X      - "Alias"
        // Windows   - "Shortcut"
        // Konqueror - "Folder" or "TXT File" i.e. what it is pointing to
        // Nautilus  - "link to folder" or "link to object file", same as Konqueror

        return QCoreApplication::translate("QFileDialog", "Unknown");
    }

    QMutex mutex;
    QWaitCondition condition;
    volatile bool abort;

    QStack<QString> path;
    QStack<QStringList> files;

#ifndef QT_NO_FILESYSTEMWATCHER
    QFileSystemWatcher *watcher;
#endif
    bool m_resolveSymlinks;
#ifndef Q_OS_WIN
    uint userId;
    uint groupId;
#endif
};
#endif // QT_NO_FILESYSTEMMODEL


QT_END_NAMESPACE_UIHELPERS
#endif // UIFILEINFOGATHERER_H

