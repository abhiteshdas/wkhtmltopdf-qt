/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmouseeventtransition.h"

#ifndef QT_NO_STATEMACHINE

#include "qbasicmouseeventtransition_p.h"
#include <QtCore/qwrappedevent.h>
#include <QtGui/qpainterpath.h>
#include <private/qeventtransition_p.h>

QT_BEGIN_NAMESPACE

/*!
  \class QMouseEventTransition

  \brief The QMouseEventTransition class provides a transition for mouse events.

  \since 4.6
  \ingroup statemachine

  QMouseEventTransition is part of \l{The State Machine Framework}.

  \sa QState::addTransition()
*/

/*!
    \property QMouseEventTransition::button

    \brief the button that this mouse event transition is associated with
*/

/*!
    \property QMouseEventTransition::modifiersMask

    \brief the keyboard modifiers mask that this mouse event transition checks for
*/

class QMouseEventTransitionPrivate : public QEventTransitionPrivate
{
    Q_DECLARE_PUBLIC(QMouseEventTransition)
public:
    QMouseEventTransitionPrivate();

    QBasicMouseEventTransition *transition;
};

QMouseEventTransitionPrivate::QMouseEventTransitionPrivate()
{
}

/*!
  Constructs a new mouse event transition with the given \a sourceState.
*/
QMouseEventTransition::QMouseEventTransition(QState *sourceState)
    : QEventTransition(*new QMouseEventTransitionPrivate, sourceState)
{
    Q_D(QMouseEventTransition);
    d->transition = new QBasicMouseEventTransition();
}

/*!
  Constructs a new mouse event transition for events of the given \a type for
  the given \a object, with the given \a button and \a sourceState.
*/
QMouseEventTransition::QMouseEventTransition(QObject *object, QEvent::Type type,
                                             Qt::MouseButton button,
                                             QState *sourceState)
    : QEventTransition(*new QMouseEventTransitionPrivate, object, type, sourceState)
{
    Q_D(QMouseEventTransition);
    d->transition = new QBasicMouseEventTransition(type, button);
}

/*!
  Constructs a new mouse event transition for events of the given \a type for
  the given \a object, with the given \a button, \a targets and \a
  sourceState.
*/
QMouseEventTransition::QMouseEventTransition(QObject *object, QEvent::Type type,
                                             Qt::MouseButton button,
                                             const QList<QAbstractState*> &targets,
                                             QState *sourceState)
    : QEventTransition(*new QMouseEventTransitionPrivate, object, type, targets, sourceState)
{
    Q_D(QMouseEventTransition);
    d->transition = new QBasicMouseEventTransition(type, button);
}

/*!
  Destroys this mouse event transition.
*/
QMouseEventTransition::~QMouseEventTransition()
{
    Q_D(QMouseEventTransition);
    delete d->transition;
}

/*!
  Returns the button that this mouse event transition checks for.
*/
Qt::MouseButton QMouseEventTransition::button() const
{
    Q_D(const QMouseEventTransition);
    return d->transition->button();
}

/*!
  Sets the \a button that this mouse event transition will check for.
*/
void QMouseEventTransition::setButton(Qt::MouseButton button)
{
    Q_D(QMouseEventTransition);
    d->transition->setButton(button);
}

/*!
  Returns the keyboard modifiers mask that this mouse event transition checks
  for.
*/
Qt::KeyboardModifiers QMouseEventTransition::modifiersMask() const
{
    Q_D(const QMouseEventTransition);
    return d->transition->modifiersMask();
}

/*!
  Sets the keyboard \a modifiers mask that this mouse event transition will
  check for.
*/
void QMouseEventTransition::setModifiersMask(Qt::KeyboardModifiers modifiersMask)
{
    Q_D(QMouseEventTransition);
    d->transition->setModifiersMask(modifiersMask);
}

/*!
  Returns the path for this mouse event transition.
*/
QPainterPath QMouseEventTransition::path() const
{
    Q_D(const QMouseEventTransition);
    return d->transition->path();
}

/*!
  Sets the \a path for this mouse event transition.
  If a valid path has been set, the transition will only trigger if the mouse
  event position (QMouseEvent::pos()) is inside the path.

  \sa QPainterPath::contains()
*/
void QMouseEventTransition::setPath(const QPainterPath &path)
{
    Q_D(QMouseEventTransition);
    d->transition->setPath(path);
}

/*!
  \reimp
*/
bool QMouseEventTransition::eventTest(QEvent *event)
{
    Q_D(const QMouseEventTransition);
    if (!QEventTransition::eventTest(event))
        return false;
    QWrappedEvent *we = static_cast<QWrappedEvent*>(event);
    d->transition->setEventType(we->event()->type());
    return QAbstractTransitionPrivate::get(d->transition)->callEventTest(we->event());
}

/*!
  \reimp
*/
void QMouseEventTransition::onTransition(QEvent *event)
{
    QEventTransition::onTransition(event);
}

QT_END_NAMESPACE

#endif //QT_NO_STATEMACHINE