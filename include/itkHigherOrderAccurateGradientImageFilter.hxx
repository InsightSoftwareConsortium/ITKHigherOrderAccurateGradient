/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkHigherOrderAccurateGradientImageFilter_hxx
#define itkHigherOrderAccurateGradientImageFilter_hxx

#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodInnerProduct.h"
#include "itkImageRegionIterator.h"
#include "itkHigherOrderAccurateDerivativeOperator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkOffset.h"

namespace itk
{

template <typename TInputImage, typename TOperatorValueType, typename TOutputValueType>
HigherOrderAccurateGradientImageFilter<TInputImage, TOperatorValueType, TOutputValueType>::
  HigherOrderAccurateGradientImageFilter()

  = default;


template <typename TInputImage, typename TOperatorValueType, typename TOutputValueType>
void
HigherOrderAccurateGradientImageFilter<TInputImage, TOperatorValueType, TOutputValueType>::
  GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the input and output
  InputImagePointer  inputPtr = const_cast<InputImageType *>(this->GetInput());
  OutputImagePointer outputPtr = this->GetOutput();

  if (!inputPtr || !outputPtr)
  {
    return;
  }

  // Build an operator so that we can determine the kernel size
  HigherOrderAccurateDerivativeOperator<OperatorValueType, ImageDimension> oper;
  oper.SetDirection(0);
  oper.SetOrder(1);
  oper.SetOrderOfAccuracy(this->m_OrderOfAccuracy);
  oper.CreateDirectional();
  unsigned long radius = oper.GetRadius()[0];

  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion;
  inputRequestedRegion = inputPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius(radius);

  // crop the input requested region at the input's largest possible region
  if (inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()))
  {
    inputPtr->SetRequestedRegion(inputRequestedRegion);
    return;
  }
  else
  {
    // Couldn't crop the region (requested region is outside the largest
    // possible region).  Throw an exception.

    // store what we tried to request (prior to trying to crop)
    inputPtr->SetRequestedRegion(inputRequestedRegion);

    // build an exception
    InvalidRequestedRegionError e(__FILE__, __LINE__);
    e.SetLocation(ITK_LOCATION);
    e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
    e.SetDataObject(inputPtr);
    throw e;
  }
}


template <typename TInputImage, typename TOperatorValueType, typename TOutputValueType>
void
HigherOrderAccurateGradientImageFilter<TInputImage, TOperatorValueType, TOutputValueType>::DynamicThreadedGenerateData(
  const OutputImageRegionType & outputRegionForThread)
{
  unsigned int    i;
  OutputPixelType gradient;

  ZeroFluxNeumannBoundaryCondition<InputImageType> nbc;

  ConstNeighborhoodIterator<InputImageType> nit;
  ImageRegionIterator<OutputImageType>      it;

  NeighborhoodInnerProduct<InputImageType, OperatorValueType, OutputValueType> SIP;

  // Get the input and output
  OutputImageType *      outputImage = this->GetOutput();
  const InputImageType * inputImage = this->GetInput();

  // Set up operators
  HigherOrderAccurateDerivativeOperator<OperatorValueType, ImageDimension> op[ImageDimension];

  for (i = 0; i < ImageDimension; i++)
  {
    op[i].SetDirection(0);
    op[i].SetOrder(1);
    op[i].SetOrderOfAccuracy(this->m_OrderOfAccuracy);
    op[i].CreateDirectional();

    // Reverse order of coefficients for the convolution with the image to
    // follow.
    op[i].FlipAxes();

    // Take into account the pixel spacing if necessary
    if (m_UseImageSpacing == true)
    {
      if (this->GetInput()->GetSpacing()[i] == 0.0)
      {
        itkExceptionMacro(<< "Image spacing cannot be zero.");
      }
      else
      {
        op[i].ScaleCoefficients(1.0 / this->GetInput()->GetSpacing()[i]);
      }
    }
  }

  // Calculate iterator radius
  Size<ImageDimension> radius;
  for (i = 0; i < ImageDimension; ++i)
  {
    radius[i] = op[0].GetRadius()[0];
  }

  // Find the data-set boundary "faces"
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType faceList;
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>                        bC;
  faceList = bC(inputImage, outputRegionForThread, radius);

  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;
  fit = faceList.begin();

  // Initialize the x_slice array
  nit = ConstNeighborhoodIterator<InputImageType>(radius, inputImage, *fit);

  std::slice          x_slice[ImageDimension];
  const unsigned long center = nit.Size() / 2;
  for (i = 0; i < ImageDimension; ++i)
  {
    x_slice[i] = std::slice(center - nit.GetStride(i) * radius[i], op[i].GetSize()[0], nit.GetStride(i));
  }

  // Process non-boundary face and then each of the boundary faces.
  // These are N-d regions which border the edge of the buffer.
  for (fit = faceList.begin(); fit != faceList.end(); ++fit)
  {
    nit = ConstNeighborhoodIterator<InputImageType>(radius, inputImage, *fit);
    it = ImageRegionIterator<OutputImageType>(outputImage, *fit);
    nit.OverrideBoundaryCondition(&nbc);
    nit.GoToBegin();

    while (!nit.IsAtEnd())
    {
      for (i = 0; i < ImageDimension; ++i)
      {
        gradient[i] = SIP(x_slice[i], nit, op[i]);
      }

      if (this->m_UseImageDirection)
      {
        inputImage->TransformLocalVectorToPhysicalVector(gradient, it.Value());
      }
      else
      {
        it.Value() = gradient;
      }
      ++nit;
      ++it;
    }
  }
}


template <typename TInputImage, typename TOperatorValueType, typename TOutputValueType>
void
HigherOrderAccurateGradientImageFilter<TInputImage, TOperatorValueType, TOutputValueType>::PrintSelf(
  std::ostream & os,
  Indent         indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "UseImageSpacing: " << (this->m_UseImageSpacing ? "On" : "Off") << std::endl;
  os << indent << "UseImageDirection = " << (this->m_UseImageDirection ? "On" : "Off") << std::endl;
  os << indent << "OrderOfAccuracy: " << this->m_OrderOfAccuracy << std::endl;
}

} // end namespace itk

#endif
