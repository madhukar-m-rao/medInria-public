/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkPlugin.h>

#include <medVtkMeshExport.h>

class MEDVTKMESHPLUGIN_EXPORT medVtkMeshPluginPrivate;

class MEDVTKMESHPLUGIN_EXPORT medVtkMeshPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     medVtkMeshPlugin(QObject *parent = 0);
    ~medVtkMeshPlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
     medVtkMeshPluginPrivate *d;
};

