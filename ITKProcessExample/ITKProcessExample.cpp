// /////////////////////////////////////////////////////////////////
// Generated by dtkPluginGenerator
// /////////////////////////////////////////////////////////////////

#include "ITKProcessExample.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include "itkImage.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"

// /////////////////////////////////////////////////////////////////
// ITKProcessExamplePrivate
// /////////////////////////////////////////////////////////////////

class ITKProcessExamplePrivate
{
public:
    dtkAbstractData *input;
    dtkAbstractData *output;
    double sigma;
    template <class PixelType> int update(void);
};

template <class PixelType> int ITKProcessExamplePrivate::update(void)
{
    qDebug() << "entering method d->update<" << typeid(PixelType).name() << ">";

    typedef itk::Image< PixelType, 3 > ImageType;

//     typedef itk::DiscreteGaussianImageFilter< ImageType, ImageType >  FilterType;
    
    typedef itk::SmoothingRecursiveGaussianImageFilter< ImageType, ImageType >  FilterType;

    typename FilterType::Pointer gaussianFilter = FilterType::New();

    gaussianFilter->SetInput(dynamic_cast<ImageType *>((itk::Object*)(input->data())));

//     gaussianFilter->SetVariance(variance);

    gaussianFilter->SetSigma(sigma);

    gaussianFilter->Update();

    output->setData(gaussianFilter->GetOutput());

    return EXIT_SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// ITKProcessExample
// /////////////////////////////////////////////////////////////////

ITKProcessExample::ITKProcessExample(void) : dtkAbstractProcess(), d(new ITKProcessExamplePrivate)
{
    d->output = NULL;
    d->sigma = 4.0;
}

ITKProcessExample::~ITKProcessExample(void)
{
    delete d;
    d = NULL;
}

bool ITKProcessExample::registered(void)
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("ITKProcessExampleGaussianBlur", createITKProcessExample);
}

QString ITKProcessExample::description(void) const
{
    return "ITKProcessExampleGaussianBlur";
}

void ITKProcessExample::setInput(dtkAbstractData *data)
{
    if (!data)
        return;

    QString identifier = data->identifier();

    d->output = dtkAbstractDataFactory::instance()->create (identifier);

    d->input = data;
    qDebug() << "in method setInput, d->input =" << d->input;
}

void ITKProcessExample::setParameter(double  data, int channel)
{
    switch (channel){

        case (0):
                d->sigma = data;
                break;
        default :
                return;
        }
}

int ITKProcessExample::update (void)
{
    qDebug() << "entering method update";

    if (!d->input)
	{
	    qDebug() << "in update method : d->input == NULL";
            return -1;
	}

    QString descr = d->input->identifier();

    if (descr == "itkDataImageChar3") {
        d->update<char>();
     }
    else if (descr == "itkDataImageUChar3") {
        d->update<unsigned char>();
     }
    else if (descr == "itkDataImageShort3") {
        d->update<short>();
     }
    else if (descr == "itkDataImageUShort3") {
        d->update<unsigned short>();
     }
    else if (descr == "itkDataImageInt3") {
        d->update<int>();
     }
    else if (descr == "itkDataImageUInt3") {
        d->update<unsigned int>();
     }
    else if (descr == "itkDataImageLong3") {
        d->update<long>();
     }
    else if (descr== "itkDataImageULong3") {
        d->update<unsigned long>();
     }
    else if (descr == "itkDataImageFloat3") {
        d->update<float>();
     }
    else if (descr == "itkDataImageDouble3") {
        d->update<double>();
     }
    else
    {
        qDebug() << "Error : pixel type not yet implemented ("
                 << descr
                 << ")";
        return -1;
    }

    return EXIT_SUCCESS;
}

dtkAbstractData * ITKProcessExample::output(void)
{
    return (d->output);
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createITKProcessExample(void)
{
    return new ITKProcessExample;
}
