/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qapplication.h"
#include "qevent.h"
#ifdef Q_WS_S60
#include "qstyle.h"
#include "private/qt_s60_p.h"
#endif
#include "private/qsoftkeymanager_p.h"
#include "private/qobject_p.h"

QT_BEGIN_NAMESPACE

#ifdef Q_WS_S60
static const int s60CommandStart = 6000;
#endif

class QSoftKeyManagerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSoftKeyManager)

public:
    static void updateSoftKeys_sys(const QList<QAction*> &softKeys);

private:
    QHash<QAction*, Qt::Key> keyedActions;
    static QSoftKeyManager *self;
    static QWidget *softKeySource;
};

QWidget *QSoftKeyManagerPrivate::softKeySource = 0;
QSoftKeyManager *QSoftKeyManagerPrivate::self = 0;

const char *QSoftKeyManager::standardSoftKeyText(StandardSoftKey standardKey)
{
    const char *softKeyText = 0;
    switch (standardKey) {
    case OkSoftKey:
        softKeyText = QT_TRANSLATE_NOOP("QSoftKeyManager", "Ok");
        break;
    case SelectSoftKey:
        softKeyText = QT_TRANSLATE_NOOP("QSoftKeyManager", "Select");
        break;
    case DoneSoftKey:
        softKeyText = QT_TRANSLATE_NOOP("QSoftKeyManager", "Done");
        break;
    case MenuSoftKey:
        softKeyText = QT_TRANSLATE_NOOP("QSoftKeyManager", "Options");
        break;
    case CancelSoftKey:
        softKeyText = QT_TRANSLATE_NOOP("QSoftKeyManager", "Cancel");
        break;
    default:
        break;
    };

    return softKeyText;
}

QSoftKeyManager *QSoftKeyManager::instance()
{
    if (!QSoftKeyManagerPrivate::self)
        QSoftKeyManagerPrivate::self = new QSoftKeyManager;

    return QSoftKeyManagerPrivate::self;
}

QSoftKeyManager::QSoftKeyManager() : QObject(*(new QSoftKeyManagerPrivate), 0)
{
}

QAction *QSoftKeyManager::createAction(StandardSoftKey standardKey, QWidget *actionWidget)
{
    const char* text = standardSoftKeyText(standardKey);
    QAction *action = new QAction(QSoftKeyManager::tr(text), actionWidget);
    QAction::SoftKeyRole softKeyRole = QAction::NoSoftKey;
    switch (standardKey) {
    case OkSoftKey:
    case SelectSoftKey:
    case DoneSoftKey:
    case MenuSoftKey:
        softKeyRole = QAction::PositiveSoftKey;
        break;
    case CancelSoftKey:
        softKeyRole = QAction::NegativeSoftKey;
        break;
    }
    action->setSoftKeyRole(softKeyRole);
    action->setEnabled(actionWidget->isEnabled());
    return action;
}

/*! \internal

  Creates a QAction and registers the 'triggered' signal to send the given key event to
  \a actionWidget as a convenience.

*/
QAction *QSoftKeyManager::createKeyedAction(StandardSoftKey standardKey, Qt::Key key, QWidget *actionWidget)
{
    QScopedPointer<QAction> action(createAction(standardKey, actionWidget));

    connect(action.data(), SIGNAL(triggered()), QSoftKeyManager::instance(), SLOT(sendKeyEvent()));

    QSoftKeyManager::instance()->d_func()->keyedActions.insert(action.data(), key);
    return action.take();
}

void QSoftKeyManager::sendKeyEvent()
{
    Q_D(QSoftKeyManager);
    QAction *action = qobject_cast<QAction*>(sender());

    if (!action)
        return;

    Qt::Key keyToSend = d->keyedActions.value(action, Qt::Key_unknown);

    if (keyToSend != Qt::Key_unknown)
        QApplication::postEvent(action->parentWidget(),
                                new QKeyEvent(QEvent::KeyPress, keyToSend, Qt::NoModifier));
}

void QSoftKeyManager::updateSoftKeys()
{
    QEvent *event = new QEvent(QEvent::UpdateSoftKeys);
    QApplication::postEvent(QSoftKeyManager::instance(), event);
}

bool QSoftKeyManager::event(QEvent *e)
{
    if (e->type() == QEvent::UpdateSoftKeys) {
        QList<QAction*> softKeys;
        QWidget *source = QApplication::focusWidget();
        do {
            if (source) {
                QList<QAction*> actions = source->actions();
                for (int i = 0; i < actions.count(); ++i) {
                    if (actions.at(i)->softKeyRole() != QAction::NoSoftKey)
                        softKeys.append(actions.at(i));
                }

                QWidget *parent = source->parentWidget();
                if (parent && softKeys.isEmpty())
                    source = parent;
                else
                    break;
            } else {
                source = QApplication::activeWindow();
            }
        } while (source);

        QSoftKeyManagerPrivate::softKeySource = source;
        QSoftKeyManagerPrivate::updateSoftKeys_sys(softKeys);
        return true;
    }
    return false;
}

#ifdef Q_WS_S60
void QSoftKeyManagerPrivate::updateSoftKeys_sys(const QList<QAction*> &softkeys)
{
    CEikButtonGroupContainer* nativeContainer = S60->buttonGroupContainer();
    nativeContainer->DrawableWindow()->SetPointerCapturePriority(1); //keep softkeys available in modal dialog
    QT_TRAP_THROWING(nativeContainer->SetCommandSetL(R_AVKON_SOFTKEYS_EMPTY_WITH_IDS));

    int position = -1;
    int command;
    bool needsExitButton = true;

    for (int index = 0; index < softkeys.count(); index++) {
        const QAction* softKeyAction = softkeys.at(index);
        switch (softKeyAction->softKeyRole()) {
        // Positive Actions go on LSK
        case QAction::PositiveSoftKey:
            position = 0;
            break;
        case QAction::SelectSoftKey:
            position = 0;
            break;
        // Negative Actions on the RSK
        case QAction::NegativeSoftKey:
            needsExitButton = false;
            position = 2;
            break;
        default:
            break;
        }

        command = (softKeyAction->objectName().contains("_q_menuSoftKeyAction"))
                    ? EAknSoftkeyOptions
                    : s60CommandStart + index;

        if (position != -1) {
            const int underlineShortCut = QApplication::style()->styleHint(QStyle::SH_UnderlineShortcut);
            QString iconText = softKeyAction->iconText();
            TPtrC text = qt_QString2TPtrC( underlineShortCut ? softKeyAction->text() : iconText);
            QT_TRAP_THROWING(nativeContainer->SetCommandL(position, command, text));
        }
    }

    if (needsExitButton)
        QT_TRAP_THROWING(nativeContainer->SetCommandL(2, EAknSoftkeyExit, qt_QString2TPtrC(QSoftKeyManager::tr("Exit"))));

    nativeContainer->DrawDeferred(); // 3.1 needs an extra invitation
}

bool QSoftKeyManager::handleCommand(int command)
{
    if (command >= s60CommandStart && QSoftKeyManagerPrivate::softKeySource) {
        int index = command - s60CommandStart;
        const QList<QAction*>& softKeys = QSoftKeyManagerPrivate::softKeySource->actions();
        for (int i = 0, j = 0; i < softKeys.count(); ++i) {
            QAction *action = softKeys.at(i);
            if (action->softKeyRole() != QAction::NoSoftKey) {
                if (j == index) {
                    if (action->isEnabled()) {
                        action->activate(QAction::Trigger);
                        return true;
                    }
                }
                j++;
            }
        }
    }

    return false;
}

#else

void QSoftKeyManagerPrivate::updateSoftKeys_sys(const QList<QAction*> &)
{
}

#endif

QT_END_NAMESPACE

