//
// This file is part of DoorOpener.
// Copyright (c) 2014 Jacob Dawid <jacob@omg-it.works>
//
// DoorOpener is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// DoorOpener is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with DoorOpener.
// If not, see <http://www.gnu.org/licenses/>.
//

#ifndef SLIDINGSTACKEDWIDGET_H
#define SLIDINGSTACKEDWIDGET_H

// Qt includes
#include <QStackedWidget>
#include <QtGui>
#include <QWidget>
#include <QDebug>
#include <QEasingCurve>

/**
 * @class SlidingStackedWidget
 * Cleaned up and modified version of this tutorial:
 * http://developer.nokia.com/community/wiki/Code_Example_for_SlidingStackedWidget_class_in_Qt
 * @author Jacob Dawid ( jacob.dawid@omg-it.works )
 */
class SlidingStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    /** Defines an animation direction. */
    typedef enum {
        LeftToRight,
        RightToLeft,
        TopToBottom,
        BottomToTop,
        Automatic
    } AnimationDirection;

    /** Constructor */
    SlidingStackedWidget(QWidget *parent = 0);

    /** Destructor */
    virtual ~SlidingStackedWidget();

public slots:
    /**
     * Sets the duration of the animation in milliseconds.
     * @param speed Duration of the animation in milliseconds.
     */
    void setSlideDuration(int slideDuration);   //animation duration in milliseconds

    /**
    * Determines the type of animation, ie. how one page slides to
    * another. Possible animations would be bouncing off the edge,
    * or for example a linear or exponential shift.
    * @see QEasingCurve::Type for all possible animation types
    * @param animationtype The type of animation to use.
    */
    void setAnimationType(enum QEasingCurve::Type animationtype);

    /**
     * Set the orientation of the animation.
     * @param vertical
     */
    void setVerticalMode(bool vertical = true);

    /**
    * Defines wrapping behaviour, that is, if proceeding to the next
    * page after the last one leads back to the first page and vice
    * versa.
    */
    void setWrapping(bool wrapping);

    /** Slide in the next page. */
    void slideInNextPage();

    /** Slide in the previous page. */
    void slideInPreviousPage();

    /** Slide in the page with the given index. */
    void slideInAtIndex(int idx, AnimationDirection direction = Automatic);

signals:
    /** Emitted when the slide animation has finished. */
    void animationFinished();

protected slots:
    void animationDone();

protected:
    /**
     * Slides in the specified widget.
     * @param widget
     * @param direction
     */
    void slideInWidget(QWidget *targetWidget, AnimationDirection direction = Automatic);

    QWidget *_parent;

    int _slideDuration;
    enum QEasingCurve::Type _animationtype;
    bool _verticalMode;
    int _currentIndex;
    int _next;
    bool _wrapping;
    QPoint _frameReferencePoint;
    bool _active;
    QList<QWidget*> blockedPageList;
};

#endif // SLIDINGSTACKEDWIDGET_H
