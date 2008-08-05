/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2008 Scientific Computing and Imaging Institute,
   University of Utah.

   
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/

/**
	\file		TransferFunction2D.h
	\author		Jens Krueger
				SCI Institute
				University of Utah
	\version	1.0
	\date		July 2008
*/

#pragma once

#ifndef TRANSFERFUNCTION2D
#define TRANSFERFUNCTION2D

#include <fstream>
#include <string>
#include <vector>
#include <Basics/Vectors.h>
#include <Basics/Grids.h>

typedef Grid2D<unsigned int> Histogram2D;
typedef Grid2D<float> NormalizedHistogram2D;
typedef Grid2D<FLOATVECTOR4> ColorData2D;

typedef std::pair< float, FLOATVECTOR4 > GradientStop;

class TFPolygon {
	public:
		TFPolygon() {}

		void Load(std::ifstream& file);
		void Save(std::ofstream& file);

		std::vector< FLOATVECTOR2 > pPoints;
		FLOATVECTOR2 pGradientCoords[2];
		std::vector< GradientStop > pGradientStops;
};


class TransferFunction2D
{
public:
	TransferFunction2D();
	TransferFunction2D(const UINTVECTOR2& iSize);
	~TransferFunction2D(void);
	
	void Resize(const Histogram2D& hist) {Resize(hist.GetSize());}
	void Resize(const NormalizedHistogram2D& hist) {Resize(hist.GetSize());}
	void Resize(const UINTVECTOR2& iSize);

	bool Load(const std::string& filename);
	bool Save(const std::string& filename);

	void GetByteArray(unsigned char** pcData, unsigned char cUsedRange=255);
	void GetShortArray(unsigned short** psData, unsigned short sUsedRange=4095);
	void GetFloatArray(float** pfData);

	ColorData2D pColorData;
	std::vector< TFPolygon > m_Swatches;
};

#endif // TRANSFERFUNCTION2D