/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef UIHELPERS_GLOBAL_H
#define UIHELPERS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN)
#  if defined(QT_MAKEDLL)        /* create a Qt DLL library */
#      define UIHELPERS_EXPORT Q_DECL_EXPORT
#  elif defined(QT_DLL) /* use a Qt DLL library */
#    define UIHELPERS_EXPORT Q_DECL_IMPORT
#  endif
#else
#    if defined(UIHELPERS_LIB)
#      define UIHELPERS_EXPORT Q_DECL_EXPORT
#    else
#      define UIHELPERS_EXPORT Q_DECL_IMPORT
#    endif
#endif

#if defined(Q_CC_MINGW)
#    if defined(UIHELPERS_LIB)
#      define UIHELPERS_EXPORT_INLINE UIHELPERS_EXPORT inline
#    else
#      define UIHELPERS_EXPORT_INLINE inline
#    endif
#else
#  define UIHELPERS_EXPORT_INLINE UIHELPERS_EXPORT inline
#endif

#if defined(QT_NAMESPACE)
#  define QT_BEGIN_NAMESPACE_UIHELPERS namespace QT_NAMESPACE { namespace UiHelpers {
#  define QT_END_NAMESPACE_UIHELPERS } }
#  define QT_USE_NAMESPACE_UIHELPERS using namespace QT_NAMESPACE::UiHelpers;
#  define QT_PREPEND_NAMESPACE_UIHELPERS(name) ::QT_NAMESPACE::UiHelpers::name
#else
#  define QT_BEGIN_NAMESPACE_UIHELPERS namespace UiHelpers {
#  define QT_END_NAMESPACE_UIHELPERS }
#  define QT_USE_NAMESPACE_UIHELPERS using namespace UiHelpers;
#  define QT_PREPEND_NAMESPACE_UIHELPERS(name) ::UiHelpers::name
#endif

#endif // UIHELPERS_GLOBAL_H


