/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "QtAwesomeAnim.h"

#include <cmath>
#include <QPainter>
#include <QRect>
#include <QTimer>
#include <QWidget>


QtAwesomeAnimation::QtAwesomeAnimation(QWidget *parentWidget, int interval, int step)
    : parentWidgetRef_( parentWidget )
    , timer_( 0 )
    , interval_( interval )
    , step_( step )
    , angle_( 0.0f )
{

}

void QtAwesomeAnimation::setup( QPainter &painter, const QRect &rect)
{
    // first time set the timer
    if( !timer_ )
    {
        timer_ = new QTimer();
        connect(timer_,SIGNAL(timeout()), this, SLOT(update()) );
        timer_->start(interval_);
    }
    else
    {
        //timer, angle, self.step = self.info[self.parent_widget]
        float x_center = rect.width() * 0.5;
        float y_center = rect.height() * 0.5;
        painter.translate(x_center, y_center);
        painter.rotate(angle_);
        painter.translate(-x_center, -y_center);
    }
}


void QtAwesomeAnimation::update()
{
    angle_ += step_;
    angle_ = std::fmod( angle_, 360);
    parentWidgetRef_->update();
}
