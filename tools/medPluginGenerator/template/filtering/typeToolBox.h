// /////////////////////////////////////////////////////////////////
// Generated by medPluginGenerator
// /////////////////////////////////////////////////////////////////

#ifndef %2TOOLBOX_H
#define %2TOOLBOX_H

#include <medToolBoxFilteringCustom.h>
#include "%1PluginExport.h"

namespace %4
{
    
    class %1ToolBoxPrivate;
    
    class %2PLUGIN_EXPORT %1ToolBox : public medToolBoxFilteringCustom
    {
        Q_OBJECT
        
    public:
        %1ToolBox(QWidget *parent = 0);
        ~%1ToolBox(void);
        
        dtkAbstractData *processOutput();
        
        static bool registered(void);
        
    signals:
        void success(void);
        void failure(void);
        
    public slots:
        void run(void);
        
    private:
        %1ToolBoxPrivate *d;
    };
    
    medToolBoxRegistrationCustom *create%3ToolBox(QWidget *parent);
    
} // end namespace %4

#endif
