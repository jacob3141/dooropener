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

// Own includes
#include "slidingstackedwidget.h"

SlidingStackedWidget::SlidingStackedWidget(QWidget *parent)
    : QStackedWidget(parent)
{
    _parent = parent ? parent : this;
    _verticalMode = false;
    _slideDuration = 250;
    _animationtype = QEasingCurve::OutCurve;
    _currentIndex = 0;
    _next = 0;
    _wrapping = false;
    _frameReferencePoint = QPoint(0,0);
    _active = false;
}

SlidingStackedWidget::~SlidingStackedWidget(){
}

void SlidingStackedWidget::setVerticalMode(bool vertical) {
    _verticalMode = vertical;
}

void SlidingStackedWidget::setSlideDuration(int slideDuration) {
    _slideDuration = slideDuration;
}

void SlidingStackedWidget::setAnimationType(enum QEasingCurve::Type animationtype) {
    _animationtype = animationtype;
}

void SlidingStackedWidget::setWrapping(bool wrapping) {
    _wrapping = wrapping;
}

void SlidingStackedWidget::slideInNextPage() {
    int index = currentIndex();
    if(_wrapping || (index < count() - 1)) {
        slideInAtIndex(index + 1);
    }
}

void SlidingStackedWidget::slideInPreviousPage() {
    int index = currentIndex();
    if(_wrapping || (index  >0)) {
        slideInAtIndex(index - 1);
    }
}

void SlidingStackedWidget::slideInAtIndex(int index, AnimationDirection direction) {
    if(index > count() - 1) {
        direction = _verticalMode ? TopToBottom : RightToLeft;
        index = index % count();
    } else
    if(index < 0) {
        direction =  _verticalMode ? BottomToTop: LeftToRight;
        index = (index + count()) % count();
    }
    slideInWidget(widget(index), direction);
}

void SlidingStackedWidget::slideInWidget(QWidget *targetWidget, AnimationDirection  direction) {

    if (_active) {
        return; // at the moment, do not allow re-entrance before an animation is completed.
        //other possibility may be to finish the previous animation abrupt, or
        //to revert the previous animation with a counter animation, before going ahead
        //or to revert the previous animation abrupt
        //and all those only, if the newwidget is not the same as that of the previous running animation.
    }
    else _active=true;

    AnimationDirection directionhint;
    int now=currentIndex();		//currentIndex() is a function inherited from QStackedWidget
    int next=indexOf(targetWidget);
    if (now==next) {
        _active=false;
        return;
    }
    else if (now<next){
        directionhint=_verticalMode ? TopToBottom : RightToLeft;
    }
    else {
        directionhint=_verticalMode ? BottomToTop : LeftToRight;
    }
    if (direction == Automatic) {
        direction=directionhint;
    }
    //NOW....
    //calculate the shifts

    int offsetx=frameRect().width(); //inherited from mother
    int offsety=frameRect().height();//inherited from mother

    //the following is important, to ensure that the new widget
    //has correct geometry information when sliding in first time
    widget(next)->setGeometry ( 0,  0, offsetx, offsety );

    if (direction==BottomToTop)  {
        offsetx=0;
        offsety=-offsety;
    }
    else if (direction==TopToBottom) {
        offsetx=0;
        //offsety=offsety;
    }
    else if (direction==RightToLeft) {
        offsetx=-offsetx;
        offsety=0;
    }
    else if (direction==LeftToRight) {
        //offsetx=offsetx;
        offsety=0;
    }
    //re-position the next widget outside/aside of the display area
    QPoint pnext=widget(next)->pos();
    QPoint pnow=widget(now)->pos();
    _frameReferencePoint=pnow;

    widget(next)->move(pnext.x()-offsetx,pnext.y()-offsety);
    //make it visible/show
    widget(next)->show();
    widget(next)->raise();

    //animate both, the now and next widget to the side, using animation framework
    QPropertyAnimation *animnow = new QPropertyAnimation(widget(now), "pos");
    animnow->setDuration(_slideDuration);
    animnow->setEasingCurve(_animationtype);
    animnow->setStartValue(QPoint(pnow.x(), pnow.y()));
    animnow->setEndValue(QPoint(offsetx+pnow.x(), offsety+pnow.y()));

    QPropertyAnimation *animnext = new QPropertyAnimation(widget(next), "pos");
    animnext->setDuration(_slideDuration);
    animnext->setEasingCurve(_animationtype);
    animnext->setStartValue(QPoint(-offsetx+pnext.x(), offsety+pnext.y()));
    animnext->setEndValue(QPoint(pnext.x(), pnext.y()));

    QParallelAnimationGroup *animgroup = new QParallelAnimationGroup;

    animgroup->addAnimation(animnow);
    animgroup->addAnimation(animnext);

    QObject::connect(animgroup, SIGNAL(finished()),this,SLOT(animationDone()));
    _next=next;
    _currentIndex=now;
    _active=true;
    animgroup->start();

    //note; the rest is done via a connect from the animation ready;
    //animation->finished() provides a signal when animation is done;
    //so we connect this to some post processing slot,
    //that we implement here below in animationDoneSlot.
}


void SlidingStackedWidget::animationDone(void) {
    //when ready, call the QStackedWidget slot setCurrentIndex(int)
    setCurrentIndex(_next);  //this function is inherit from QStackedWidget
    //then hide the outshifted widget now, and  (may be done already implicitely by QStackedWidget)
    widget(_currentIndex)->hide();
    //then set the position of the outshifted widget now back to its original
    widget(_currentIndex)->move(_frameReferencePoint);
    //so that the application could also still call the QStackedWidget original functions/slots for changings
    //widget(m_now)->update();
    //setCurrentIndex(m_next);  //this function is inherit from QStackedWidget
    _active=false;
    emit animationFinished();
}



/* REFERENCES

http://doc.trolltech.com/4.6/animation-overview.html#easing-curves
http://doc.trolltech.com/4.6/qpropertyanimation.html
http://doc.trolltech.com/4.6/qanimationgroup.html

*/



