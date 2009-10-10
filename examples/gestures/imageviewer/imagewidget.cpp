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

#include "imagewidget.h"

#include <QtGui>

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent),
    position(0),
    horizontalOffset(0),
    verticalOffset(0),
    rotationAngle(0),
    scaleFactor(1)

{
    setObjectName("ImageWidget");
    setMinimumSize(QSize(100,100));

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);
}

bool ImageWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    return QWidget::event(event);
}

void ImageWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::white);

    float iw = currentImage.width();
    float ih = currentImage.height();
    float wh = height();
    float ww = width();

    p.translate(ww/2, wh/2);
    p.translate(horizontalOffset, verticalOffset);
    p.rotate(rotationAngle);
    p.scale(scaleFactor, scaleFactor);
    p.translate(-iw/2, -ih/2);
    p.drawImage(0, 0, currentImage);
}

void ImageWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    rotationAngle = 0;
    scaleFactor = 1;
    verticalOffset = 0;
    horizontalOffset = 0;
    update();
}

bool ImageWidget::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pan = event->gesture(Qt::PanGesture)) {
        panTriggered(static_cast<QPanGesture*>(pan));
        return true;
    } else if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
        pinchTriggered(static_cast<QPinchGesture*>(pan));
        return true;
    } else if (QGesture *swipe = event->gesture(Qt::SwipeGesture)) {
        swipeTriggered(static_cast<QSwipeGesture*>(pan));
        return true;
    }
    return false;
}

void ImageWidget::panTriggered(QPanGesture *gesture)
{
#ifndef QT_NO_CURSOR
    switch (gesture->state()) {
        case Qt::GestureStarted:
        case Qt::GestureUpdated:
            setCursor(Qt::SizeAllCursor);
            break;
        default:
            setCursor(Qt::ArrowCursor);
    }
#endif
    QSizeF lastOffset = gesture->property("lastOffset").toSizeF();
    horizontalOffset += lastOffset.width();
    verticalOffset += lastOffset.height();
    update();
}

void ImageWidget::pinchTriggered(QPinchGesture *gesture)
{
    QPinchGesture::WhatChanged whatChanged = gesture->property("whatChanged").value<QPinchGesture::WhatChanged>();
    if (whatChanged & QPinchGesture::RotationAngleChanged) {
        qreal value = gesture->property("rotationAngle").toReal();
        qreal lastValue = gesture->property("lastRotationAngle").toReal();
        rotationAngle += value - lastValue;
    }
    if (whatChanged & QPinchGesture::ScaleFactorChanged) {
        qreal value = gesture->property("scaleFactor").toReal();
        qreal lastValue = gesture->property("lastScaleFactor").toReal();
        scaleFactor += value - lastValue;
    }
    update();
}

void ImageWidget::swipeTriggered(QSwipeGesture *gesture)
{
    if (gesture->horizontalDirection() == QSwipeGesture::Left
            || gesture->verticalDirection() == QSwipeGesture::Up)
        goPrevImage();
    else
        goNextImage();
    update();
}

void ImageWidget::resizeEvent(QResizeEvent*)
{
    update();
}

void ImageWidget::openDirectory(const QString &path)
{
    this->path = path;
    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.png";
    files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);

    position = 0;
    goToImage(0);
    update();
}

QImage ImageWidget::loadImage(const QString &fileName)
{
    QImageReader reader(fileName);
    if (!reader.canRead()) {
        qDebug() << fileName << ": can't load image";
        return QImage();
    }

    QImage image;
    if (!reader.read(&image)) {
        qDebug() << fileName << ": corrupted image";
        return QImage();
    }
    return image;
}

void ImageWidget::goNextImage()
{
    if (files.isEmpty())
        return;

    if (position < files.size()-1) {
        ++position;
        prevImage = currentImage;
        currentImage = nextImage;
        if (position+1 < files.size())
            nextImage = loadImage(path+QLatin1String("/")+files.at(position+1));
        else
            nextImage = QImage();
    }
    update();
}

void ImageWidget::goPrevImage()
{
    if (files.isEmpty())
        return;

    if (position > 0) {
        --position;
        nextImage = currentImage;
        currentImage = prevImage;
        if (position > 0)
            prevImage = loadImage(path+QLatin1String("/")+files.at(position-1));
        else
            prevImage = QImage();
    }
    update();
}

void ImageWidget::goToImage(int index)
{
    if (files.isEmpty())
        return;

    if (index < 0 || index >= files.size()) {
        qDebug() << "goToImage: invalid index: " << index;
        return;
    }

    if (index == position+1) {
        goNextImage();
        return;
    }

    if (position > 0 && index == position-1) {
        goPrevImage();
        return;
    }

    position = index;

    if (index > 0)
        prevImage = loadImage(path+QLatin1String("/")+files.at(position-1));
    else
        prevImage = QImage();
    currentImage = loadImage(path+QLatin1String("/")+files.at(position));
    if (position+1 < files.size())
        nextImage = loadImage(path+QLatin1String("/")+files.at(position+1));
    else
        nextImage = QImage();
    update();
}
