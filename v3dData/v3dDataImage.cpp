// /////////////////////////////////////////////////////////////////
// Generated by dtkPluginGenerator
// /////////////////////////////////////////////////////////////////

#include "v3dDataImage.h"

#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkAnalyzeImageIOFactory.h>
#include <itkGDCMImageIOFactory.h>
#include <itkVTKImageIOFactory.h>

#include "vtkSmartPointer.h"
#include <vtkImageData.h>
#include <vtkMetaImageData.h>

// /////////////////////////////////////////////////////////////////
// v3dDataImagePrivate
// /////////////////////////////////////////////////////////////////

class v3dDataImagePrivate
{
public:
    vtkSmartPointer<vtkImageData> image;
};

// /////////////////////////////////////////////////////////////////
// v3dDataImage
// /////////////////////////////////////////////////////////////////

v3dDataImage::v3dDataImage(void) : dtkAbstractDataImage(), d(new v3dDataImagePrivate)
{
    d->image = NULL;
}

v3dDataImage::~v3dDataImage(void)
{
    delete d;
    d = 0;
}

bool v3dDataImage::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataType("v3dDataImage", createV3dDataImage);
}

QString v3dDataImage::description(void) const
{
    return "v3dDataImage";
}

// /////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////

bool v3dDataImage::read(QString file)
{
    if(dtkAbstractData::read(file))
        return true;

    vtkMetaImageData* metaimage = vtkMetaImageData::New();

    try
    {
        metaimage->Read(file.toAscii().constData());
    }
    catch (vtkErrorCode::ErrorIds &)
    {
        qDebug() << "Error: cannot open file" << file.toAscii().constData();
    }

    d->image = metaimage->GetImageData();

    if(!d->image)
        return false;

    return true;
}

bool v3dDataImage::write(QString file)
{
    if(dtkAbstractData::write(file))
        return true;

    return false;
}

void v3dDataImage::setData(void *data)
{
    if(vtkImageData *image = dynamic_cast<vtkImageData*>((static_cast<vtkObject*>(data))))
        d->image = image;
}

void *v3dDataImage::data(void)
{
    return d->image;
}

void *v3dDataImage::output(void)
{
    return d->image;
}

int v3dDataImage::xDimension (void)
{
  if (!d->image)
    return -1;

  int *dimensions = d->image->GetDimensions();
  return dimensions[0];
}

int v3dDataImage::yDimension (void)
{
  if (!d->image)
    return -1;

  int *dimensions = d->image->GetDimensions();
  return dimensions[1];
}

int v3dDataImage::zDimension (void)
{
  if (!d->image)
    return -1;

  int *dimensions = d->image->GetDimensions();
  return dimensions[2];
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractData *createV3dDataImage(void)
{
    return new v3dDataImage;
}

