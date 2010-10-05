#ifndef _itk_DataImageUChar3_h_
#define _itk_DataImageUChar3_h_

#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkGlobal.h>

#include "itkDataImagePluginExport.h"


class itkDataImageUChar3Private;

class ITKDATAIMAGEPLUGIN_EXPORT itkDataImageUChar3 : public dtkAbstractDataImage
{
  Q_OBJECT
    
 public:
    itkDataImageUChar3(void);
    ~itkDataImageUChar3(void);
    virtual QString description(void) const;
    static bool registered(void);

    virtual QImage        &thumbnail(void)   const;
    virtual QList<QImage> &thumbnails (void) const;

 public slots:
    // derived from dtkAbstractData
    
    void *output(void);
    void *data(void);
    void setData(void* data);
    void update(void);

    void onMetaDataSet(QString key, QString value);
    void onPropertySet(QString key, QString value);

 public:
    // derived from dtkAbstractDataImage

    int xDimension(void);
    int yDimension(void);
    int zDimension(void);

    int minRangeValue(void);
    int maxRangeValue(void);

    int scalarValueCount(int value);
    int scalarValueMinCount(void);
    int scalarValueMaxCount(void);
    
 private:
    void computeRange(void);

    itkDataImageUChar3Private* d;
  
};

dtkAbstractData* createItkDataImageUChar3 (void);

#endif
