/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mousepangesturerecognizer.h"

#include <QEvent>
#include <QMouseEvent>
#include <QGesture>

MousePanGestureRecognizer::MousePanGestureRecognizer()
{
}

QGesture* MousePanGestureRecognizer::createGesture(QObject *) const
{
    return new QPanGesture;
}

QGestureRecognizer::Result MousePanGestureRecognizer::filterEvent(QGesture *state, QObject *, QEvent *event)
{
    QPanGesture *g = static_cast<QPanGesture *>(state);
    QMouseEvent *me = static_cast<QMouseEvent *>(event);
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
        g->setHotSpot(me->globalPos());
        g->setProperty("lastPos", me->globalPos());
        g->setProperty("pressed", QVariant::fromValue<bool>(true));
        return QGestureRecognizer::GestureTriggered | QGestureRecognizer::ConsumeEventHint;
    } else if (event->type() == QEvent::MouseMove) {
        if (g->property("pressed").toBool()) {
            QPoint pos = me->globalPos();
            QPoint lastPos = g->property("lastPos").toPoint();
            g->setLastOffset(g->offset());
            lastPos = pos - lastPos;
            g->setOffset(QSizeF(lastPos.x(), lastPos.y()));
            g->setTotalOffset(g->totalOffset() + QSizeF(lastPos.x(), lastPos.y()));
            g->setProperty("lastPos", pos);
            return QGestureRecognizer::GestureTriggered | QGestureRecognizer::ConsumeEventHint;
        }
        return QGestureRecognizer::NotGesture;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        return QGestureRecognizer::GestureFinished | QGestureRecognizer::ConsumeEventHint;
    }
    return QGestureRecognizer::Ignore;
}

void MousePanGestureRecognizer::reset(QGesture *state)
{
    QPanGesture *g = static_cast<QPanGesture *>(state);
    g->setTotalOffset(QSizeF());
    g->setLastOffset(QSizeF());
    g->setOffset(QSizeF());
    g->setAcceleration(0);
    g->setProperty("lastPos", QVariant());
    g->setProperty("pressed", QVariant::fromValue<bool>(false));
    QGestureRecognizer::reset(state);
}
