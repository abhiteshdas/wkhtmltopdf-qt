/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QEASINGCURVE_H
#define QEASINGCURVE_H

#include <QtCore/qglobal.h>
#include <QtCore/qobjectdefs.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QEasingCurvePrivate;
class Q_CORE_EXPORT QEasingCurve
{
    Q_GADGET
    Q_ENUMS(Type)
public:
    enum Type {
        Linear,
        InQuad, OutQuad, InOutQuad, OutInQuad,
        InCubic, OutCubic, InOutCubic, OutInCubic,
        InQuart, OutQuart, InOutQuart, OutInQuart,
        InQuint, OutQuint, InOutQuint, OutInQuint,
        InSine, OutSine, InOutSine, OutInSine,
        InExpo, OutExpo, InOutExpo, OutInExpo,
        InCirc, OutCirc, InOutCirc, OutInCirc,
        InElastic, OutElastic, InOutElastic, OutInElastic,
        InBack, OutBack, InOutBack, OutInBack,
        InBounce, OutBounce, InOutBounce, OutInBounce,
        InCurve, OutCurve, SineCurve, CosineCurve,
        Custom, NCurveTypes
    };

    QEasingCurve(Type type = Linear);
    QEasingCurve(const QEasingCurve &other);
    ~QEasingCurve();

    QEasingCurve &operator=(const QEasingCurve &other);
    bool operator==(const QEasingCurve &other) const;
    inline bool operator!=(const QEasingCurve &other) const
    { return !(this->operator==(other)); }

    qreal amplitude() const;
    void setAmplitude(qreal amplitude);

    qreal period() const;
    void setPeriod(qreal period);

    qreal overshoot() const;
    void setOvershoot(qreal overshoot);

    Type type() const;
    void setType(Type type);
    typedef qreal (*EasingFunction)(qreal progress);
    void setCustomType(EasingFunction func);
    EasingFunction customType() const;

    qreal valueForProgress(qreal progress) const;
private:
    QEasingCurvePrivate *d_ptr;
    friend Q_CORE_EXPORT QDebug operator<<(QDebug debug, const QEasingCurve &item);
};

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug debug, const QEasingCurve &item);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif