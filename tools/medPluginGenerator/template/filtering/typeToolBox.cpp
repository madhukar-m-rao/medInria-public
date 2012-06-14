// /////////////////////////////////////////////////////////////////
// Generated by medPluginGenerator
// /////////////////////////////////////////////////////////////////


#include "%1.h"
#include "%1ToolBox.h"

#include <QtGui>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>


#include <medAbstractView.h>
#include <medRunnableProcess.h>
#include <medJobManager.h>

#include <medAbstractDataImage.h>

#include <medToolBoxFactory.h>
#include <medToolBoxFiltering.h>
#include <medProgressionStack.h>

namespace %4
{
    
    class %1ToolBoxPrivate
    {
    public:
        
        dtkAbstractProcess *process;
        medProgressionStack * progression_stack;
    };
    
    %1ToolBox::%1ToolBox(QWidget *parent) : medToolBoxFilteringCustom(parent), d(new %1ToolBoxPrivate)
    {
        QWidget *widget = new QWidget(this);
        
        QPushButton *runButton = new QPushButton(tr("Run"), this);
                
        this->setTitle("%2");
        
        // progression stack
        d->progression_stack = new medProgressionStack(widget);
        QHBoxLayout *progressStackLayout = new QHBoxLayout;
        progressStackLayout->addWidget(d->progression_stack);
        
        this->addWidget(runButton);
        this->addWidget(d->progression_stack);
        
        connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
    }
    
    %1ToolBox::~%1ToolBox(void)
    {
        delete d;
        
        d = NULL;
    }
    
    bool %1ToolBox::registered(void)
    {
        return medToolBoxFactory::instance()->
        registerCustomRegistrationToolBox("%1ToolBox",
                                          tr("Friendly name"),
                                          tr("short tooltip description"),
                                          create%3ToolBox);
    }
    
    dtkAbstractData* %1ToolBox::processOutput(void)
    {
        if(!d->process)
            return NULL;
        
        return d->process->output();
    }
    
    void %1ToolBox::run(void)
    {
        if(!this->parentToolBox())
            return;
        
        d->process = dtkAbstractProcessFactory::instance()->create("%1");
        
        if(!this->parentToolBox()->data())
            return;
        
        d->process->setInput(this->parentToolBox()->data());
        
        // Set your parameters here
        
        medRunnableProcess *runProcess = new medRunnableProcess;
        runProcess->setProcess (d->process);
        
        d->progression_stack->addJobItem(runProcess, "Progress:");
        
        d->progression_stack->disableCancel(runProcess);
        
        connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
        connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));
        connect (runProcess, SIGNAL (cancelled (QObject*)),  this, SIGNAL (failure ()));
        
        connect (runProcess, SIGNAL(activate(QObject*,bool)),
                 d->progression_stack, SLOT(setActive(QObject*,bool)));
        
        medJobManager::instance()->registerJobItem(runProcess);
        QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
    }
    
    medToolBoxFilteringCustom *create%3ToolBox(QWidget *parent)
    {
        return new %1ToolBox (parent);
    }
    
} // end namespace %4
