#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <itkDataImageWriterBase.h>
#include <itkDataImagePluginExport.h>

class ITKDATAIMAGEPLUGIN_EXPORT itkMetaDataImageWriter: public itkDataImageWriterBase
{
public:
    itkMetaDataImageWriter();
    virtual ~itkMetaDataImageWriter();

    virtual QString identifier()  const;
    virtual QString description() const;
    virtual QStringList handled() const;
    virtual QStringList supportedFileExtensions() const;

    static bool registered();

    static dtkAbstractDataWriter * create();
};
