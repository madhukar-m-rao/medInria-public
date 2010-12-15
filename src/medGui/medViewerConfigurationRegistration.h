#ifndef MEDVIEWERCONFIGURATIONREGISTRATION_H
#define MEDVIEWERCONFIGURATIONREGISTRATION_H


#include <QtCore>

#include "medViewerConfiguration.h"

class medViewContainerStack;
class medViewerConfigurationRegistrationPrivate;

class medViewerConfigurationRegistration : public medViewerConfiguration
{
    Q_OBJECT

public:
     medViewerConfigurationRegistration(QWidget *parent = 0);
    ~medViewerConfigurationRegistration(void);

    virtual QString description(void) const;
    
    void setupViewContainerStack (medViewContainerStack *container);

public slots:
    void onSetupLayoutCompare (void);
    void onSetupLayoutFuse (void);
    void patientChanged(int patientId);

private:
    medViewerConfigurationRegistrationPrivate *d;
};

medViewerConfiguration *createMedViewerConfigurationRegistration(void);


#endif // MEDVIEWERCONFIGURATIONREGISTRATION_H