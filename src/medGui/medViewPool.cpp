/* medDropSite.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 17:41:43 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb 22 21:15:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 18
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewPool.h"
#include <medCore/medMessageController.h>
#include <medCore/medAbstractView.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include "medCore/medMessageController.h"

class medViewPoolPrivate
{
public:
    QList<medAbstractView *>                 views;
    QMap<medAbstractView*, dtkAbstractData*> viewData;
    QHash<QString, QString>                  propertySet;
};


medViewPool::medViewPool(void) : d (new medViewPoolPrivate)
{
    connect(this,SIGNAL(showInfo(QObject*,const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showInfo(QObject*,const QString&,unsigned int)));
    connect(this,SIGNAL(showError(QObject*,const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(QObject*,const QString&,unsigned int)));
}

medViewPool::~medViewPool(void)
{
    delete d;
    d = NULL;
}

void medViewPool::appendView (medAbstractView *view)
{
    if (!view || d->views.contains (view))
        return;
    
    if (d->views.count()==0) { // view will become daddy      
        view->setProperty ("Daddy", "true");
    }
    
    d->views.append (view);
    
    connect (view, SIGNAL (propertySet(const QString&, const QString&)), this, SLOT (onViewPropertySet(const QString&, const QString &)));
    connect (view, SIGNAL (becomeDaddy(bool)),             this, SLOT (onViewDaddy(bool)));
    connect (view, SIGNAL (reg(bool)),                     this, SLOT (onViewReg(bool)));

    connect (view, SIGNAL (positionChanged  (const QVector3D  &, bool)), this, SLOT (onViewPositionChanged  (const QVector3D  &, bool)));
    connect (view, SIGNAL (zoomChanged      (double, bool)),             this, SLOT (onViewZoomChanged      (double, bool)));
    connect (view, SIGNAL (panChanged       (const QVector2D &, bool)),  this, SLOT (onViewPanChanged       (const QVector2D  &, bool)));
    connect (view, SIGNAL (windowingChanged (double, double, bool)),     this, SLOT (onViewWindowingChanged (double, double, bool)));
    
    connect (view, SIGNAL (cameraChanged     (const QVector3D &, const QVector3D &, const QVector3D &, double, bool)),
             this, SLOT (onViewCameraChanged (const QVector3D &, const QVector3D &, const QVector3D &, double, bool)));
    
    connect( view, SIGNAL(  obliqueSettingsChanged()), 
             this,   SLOT(onObliqueSettingsChanged()));

    // Tell any interested views that one is appended.
    foreach(medAbstractView * it , d->views ){
        if ( it != view ){
            it->onAppendViewToPool( view );
        }
    }

    // set properties
    QHashIterator<QString, QString> it(d->propertySet);
    while (it.hasNext()) {
        it.next();
        view->setProperty (it.key(), it.value());
    }
    
}

void medViewPool::removeView (medAbstractView *view)
{
    if (!view || !d->views.contains (view))
        return;
    
    // look if a view is a daddy
    medAbstractView *refView = this->daddy();
    
    if (refView) {
        if (refView==view) { // we are daddy, we need to find a new daddy
            // change daddy
            foreach(medAbstractView *lview, d->views) {
                if (lview!=refView && lview->property ("Daddy")=="false") {
                    lview->setProperty ("Daddy", "true");
                    break;
                }
            }
            medAbstractView *oldDaddy = refView;
            oldDaddy->setProperty ("Daddy", "false"); // not necessary
        }
    }

    // Tell any interested views that one is leaving.
    foreach(medAbstractView * it , d->views ){
        if ( it != view ) {
            it->onRemoveViewFromPool( view );
        }
    }

    disconnect (view, SIGNAL (propertySet(const QString&, const QString&)), this, SLOT (onViewPropertySet(const QString&, const QString &)));
    disconnect (view, SIGNAL (becomeDaddy(bool)),             this, SLOT (onViewDaddy(bool)));
    disconnect (view, SIGNAL (reg(bool)),                     this, SLOT (onViewReg(bool)));
    
    disconnect (view, SIGNAL (positionChanged  (const QVector3D  &, bool)), this, SLOT (onViewPositionChanged  (const QVector3D  &, bool)));
    disconnect (view, SIGNAL (zoomChanged      (double, bool)),             this, SLOT (onViewZoomChanged      (double, bool)));
    disconnect (view, SIGNAL (panChanged       (const QVector2D &, bool)),  this, SLOT (onViewPanChanged       (const QVector2D  &, bool)));
    disconnect (view, SIGNAL (windowingChanged (double, double, bool)),     this, SLOT (onViewWindowingChanged (double, double, bool)));
    
    disconnect (view, SIGNAL (cameraChanged     (const QVector3D &, const QVector3D &, const QVector3D &, double, bool)),
                this, SLOT (onViewCameraChanged (const QVector3D &, const QVector3D &, const QVector3D &, double, bool)));

    disconnect (view, SIGNAL(   obliqueSettingsChanged() ),
                this,   SLOT( onObliqueSettingsChanged() ) );

    d->views.removeOne (view);
}

medAbstractView *medViewPool::daddy (void)
{
    QList<medAbstractView *>::iterator it = d->views.begin();
    for( ; it!=d->views.end(); it++) {
        if ((*it)->property ("Daddy")=="true")
            return (*it);
    }
    return NULL;
}

void medViewPool::onViewDaddy (bool daddy)
{
    if (medAbstractView *view = dynamic_cast<medAbstractView *>(this->sender())) {
        
        if (daddy) { // view wants to become daddy
            
            medAbstractView *refView = this->daddy();
            
            // tell all views that they are not daddys
            QList<medAbstractView *>::iterator it = d->views.begin();
            for( ; it!=d->views.end(); it++)
                (*it)->setProperty ("Daddy", "false");
            
            // tell the sender it is now daddy
            view->setProperty ("Daddy", "true");
            
            // restore the previous data (if any)
            if ( d->viewData[view] ) {
                view->setData (d->viewData[view], 0);
                d->viewData[view] = NULL;
		if (view->widget()->isVisible())
		    view->update();
            }
        }
        else { // view does not want to become daddy
            view->setProperty ("Daddy", "false");
        }
    }
}

void medViewPool::onViewReg(bool value)
{
    if (medAbstractView *view = dynamic_cast<medAbstractView *>(this->sender())) {
        
        medAbstractView *refView = this->daddy();
        
        if (value) {
            
            if (refView==view) // do not register the view with itself
                return;
            
            if (refView) {
                
                dtkAbstractData *data1 = static_cast<dtkAbstractData *>(refView->data());
                dtkAbstractData *data2 = static_cast<dtkAbstractData *>(view->data());
                
                if (data1!=data2) {// do not register the same data
                    
                    dtkAbstractProcess *process = dtkAbstractProcessFactory::instance()->create("itkProcessRegistration");
                    if (!process)
                        return;
                    
                    process->setInput (data1, 0);
                    process->setInput (data2, 1);
                    
                    if (process->run()==0) {
                        dtkAbstractData *output = process->output();
                        d->viewData[view] = data2;
                        view->setData (output, 0);
                        view->blockSignals(true);
                        view->setPosition(refView->position());
                        view->setZoom(refView->zoom());
                        view->setPan(refView->pan());
                        QVector3D position, viewup, focal;
                        double parallelScale;
                        refView->camera(position, viewup, focal, parallelScale);
                        view->setCamera(position, viewup, focal, parallelScale);
                        view->blockSignals(false);
                        if (view->widget()->isVisible())
                            view->update();
                        emit showInfo (this, tr ("Automatic registration successful"),3000);
                    }
                    else {
                        emit showError(this, tr  ("Automatic registration failed"),3000);
                    }
                    process->deleteLater();
                }
                
            }
        }
        else { // restore the previous data (if any)
            if ( d->viewData[view] ) {
                dtkAbstractData *oldData = static_cast<dtkAbstractData*>( view->data() );
                view->setData (d->viewData[view], 0);
                view->blockSignals(true);
                view->setPosition(refView->position());
                view->setZoom(refView->zoom());
                view->setPan(refView->pan());
                QVector3D position, viewup, focal;
                double parallelScale;
                refView->camera(position, viewup, focal, parallelScale);
                view->setCamera(position, viewup, focal, parallelScale);
                view->blockSignals(false);
                d->viewData[view] = NULL;
                if (oldData)
                    oldData->deleteLater();
                if (view->widget()->isVisible())
                    view->update();
            }
        }
    }
}

void medViewPool::onViewPropertySet (const QString &key, const QString &value)
{
    // property that we do not want to synchronize
    if (key == "Daddy"           ||
        key == "PositionLinked"  ||
        key == "CameraLinked"    ||
        key == "WindowingLinked" ||
        key == "Orientation"     ||
        key == "LookupTable"     ||
        key == "Preset"          ||
        key == "3DMode")
        return;
    
    d->propertySet[key] = value;
    
    // first, block all signals
    foreach (medAbstractView *lview, d->views)
        lview->blockSignals (true);    
    
    // second, propagate properties
    foreach (medAbstractView *lview, d->views) {
        if (lview!=this->sender()) {
            lview->setProperty (key, value);
	    if (lview->widget()->isVisible())
	        lview->update();
        }
    }
    
    // third, restore signals
    foreach (medAbstractView *lview, d->views)
        lview->blockSignals (false); 
}

void medViewPool::onViewPositionChanged (const QVector3D &position, bool propagate)
{
    if (!propagate)
        return;

    medAbstractView *vsender = dynamic_cast<medAbstractView*>(this->sender());
    
    if (!vsender) {
        qDebug() << "Sender seems not to be a medAbstractView";
        return;
    }

    foreach (medAbstractView *lview, d->views) {
        if ( lview != vsender && lview->positionLinked() ) {
            lview->onPositionChanged(position);
            if (lview->widget()->isVisible())
                lview->update();
        }
    }
}

void medViewPool::onViewCameraChanged (const QVector3D &position,
                                       const QVector3D &viewup,
                                       const QVector3D &focal,
                                       double parallelScale,
                                       bool propagate)
{
    if (!propagate)
        return;

    medAbstractView *vsender = dynamic_cast<medAbstractView*>(this->sender());
    
    if (!vsender) {
        qDebug() << "Sender seems not to be a medAbstractView";
        return;
    }
    
    foreach (medAbstractView *lview, d->views) {
        if (lview!=this->sender() && lview->cameraLinked()) {
            lview->onCameraChanged(position, viewup, focal, parallelScale);
            if (lview->widget()->isVisible())
                lview->update();
        }
    }
}

void medViewPool::onViewZoomChanged (double zoom, bool propagate)
{
    if (!propagate)
        return;

    medAbstractView *vsender = dynamic_cast<medAbstractView*>(this->sender());
    
    if (!vsender) {
        qDebug() << "Sender seems not to be a medAbstractView";
        return;
    }
    
    foreach (medAbstractView *lview, d->views) {
        if (lview!=this->sender() && lview->cameraLinked()) {
            lview->onZoomChanged(zoom);
            if (lview->widget()->isVisible())
                lview->update();
        }
    }
}

void medViewPool::onViewPanChanged (const QVector2D &pan, bool propagate)
{
    if (!propagate)
        return;

    medAbstractView *vsender = dynamic_cast<medAbstractView*>(this->sender());
    
    if (!vsender) {
        qDebug() << "Sender seems not to be a medAbstractView";
        return;
    }
    
    foreach (medAbstractView *lview, d->views) {
        if (lview!=this->sender() && lview->cameraLinked()) {
            lview->onPanChanged(pan);
            if (lview->widget()->isVisible())
                lview->update();
        }
    }
}

void medViewPool::onViewWindowingChanged (double level, double window, bool propagate)
{
    if (!propagate)
        return;

    medAbstractView *vsender = dynamic_cast<medAbstractView*>(this->sender());
    
    if (!vsender) {
        qDebug() << "Sender seems not to be a medAbstractView";
        return;
    }
    
    foreach (medAbstractView *lview, d->views) {
        if (lview!=this->sender() && lview->windowingLinked()) {
            lview->onWindowingChanged(level, window);
            if (lview->widget()->isVisible())
                lview->update();
        }
    }
}

int medViewPool::count (void)
{
    return d->views.count();
}

void medViewPool::onObliqueSettingsChanged()
{
    medAbstractView *vsender = dynamic_cast<medAbstractView*>(this->sender());

    if (!vsender) {
        qDebug() << "Sender seems not to be a medAbstractView";
        return;
    }

    // first, block all signals
    foreach (medAbstractView *lview, d->views)
        lview->blockSignals (true);

    // second, propagate properties
    foreach (medAbstractView *lview, d->views) {
        if ( lview != vsender ) {
            lview->onObliqueSettingsChanged( vsender );
            if (lview->widget()->isVisible())
                lview->update();
        }
    }

    // third, restore signals
    foreach (medAbstractView *lview, d->views)
        lview->blockSignals (false);

}
