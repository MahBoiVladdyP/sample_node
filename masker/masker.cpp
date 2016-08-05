//Program that applies a mask to the picture that I want to make a model with
//Created by Trevor Russo
//Carnegie Mellon University
//Search-Based Planning Lab
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <cstdio>
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/core/core_c.h"

#include "opencv2/core/types_c.h"
//#include "/home/trevor/masker/opencv_contrib/modules/xphoto/include/opencv2/xphoto/white_balance.hpp"

using namespace std;
using namespace cv;

enum WhitebalanceTypes
{
	/** perform smart histogram adjustments (ignoring 4% pixels with minimal and maximal
	  values) for each channel */
	WHITE_BALANCE_SIMPLE = 0,
	WHITE_BALANCE_GRAYWORLD = 1
};


	template <typename T>
void balanceWhite(vector < Mat_<T> > &src, Mat &dst,
		const float inputMin, const float inputMax,
		const float outputMin, const float outputMax, const int algorithmType)
{
	switch ( algorithmType )
	{
		case WHITE_BALANCE_SIMPLE:
			{
				/********************* Simple white balance *********************/
				float s1 = 2.0f; // low quantile
				float s2 = 2.0f; // high quantile

				int depth = 2; // depth of histogram tree
				if (src[0].depth() != CV_8U)
					++depth;
				int bins = 16; // number of bins at each histogram level

				int nElements = int( pow((float)bins, (float)depth) );
				// number of elements in histogram tree

				for (size_t i = 0; i < src.size(); ++i)
				{
					std::vector <int> hist(nElements, 0);

					typename Mat_<T>::iterator beginIt = src[i].begin();
				typename Mat_<T>::iterator endIt = src[i].end();

					for (typename Mat_<T>::iterator it = beginIt; it != endIt; ++it)
						// histogram filling
					{
						int pos = 0;
						float minValue = inputMin - 0.5f;
						float maxValue = inputMax + 0.5f;
						T val = *it;

						float interval = float(maxValue - minValue) / bins;

						for (int j = 0; j < depth; ++j)
						{
							int currentBin = int( (val - minValue + 1e-4f) / interval );
							++hist[pos + currentBin];

							pos = (pos + currentBin)*bins;

							minValue = minValue + currentBin*interval;
							maxValue = minValue + interval;

							interval /= bins;
						}
					}

					int total = int( src[i].total() );

					int p1 = 0, p2 = bins - 1;
					int n1 = 0, n2 = total;

					float minValue = inputMin - 0.5f;
					float maxValue = inputMax + 0.5f;

					float interval = (maxValue - minValue) / float(bins);

					for (int j = 0; j < depth; ++j)
						// searching for s1 and s2
					{
						while (n1 + hist[p1] < s1 * total / 100.0f)
						{
							n1 += hist[p1++];
							minValue += interval;
						}
						p1 *= bins;

						while (n2 - hist[p2] > (100.0f - s2) * total / 100.0f)
						{
							n2 -= hist[p2--];
							maxValue -= interval;
						}
						p2 = p2*bins - 1;

						interval /= bins;
					}

					src[i] = (outputMax - outputMin) * (src[i] - minValue)
						/ (maxValue - minValue) + outputMin;
				}
				/****************************************************************/
				break;
			}
		default:
			CV_Error_( CV_StsNotImplemented,
					("Unsupported algorithm type (=%d)", algorithmType) );
	}

	dst.create(/**/ src[0].size(), CV_MAKETYPE( src[0].depth(), int( src.size() ) ) /**/);
	merge(src, dst);
}

void balanceWhite(const Mat &src, Mat &dst, const int algorithmType,
		const float inputMin  = 0.0f, const float inputMax  = 255.0f,
		const float outputMin = 0.0f, const float outputMax = 255.0f) {
	//std::vector<Mat> src_mats = {src};
	balanceWhite({src}, dst, inputMin, inputMax, outputMin, outputMax, algorithmType);
}




int main(int argc, char** argv){
	//Initial image being masked-1st argument
	Mat img = imread(argv[1], 1);
	//Mask being applied-2nd argument
	Mat mask = imread(argv[2], 1);
	//Inverts mask to cover background instead of object
	mask = ~mask;
	//Creates white balanced empty Matrix
	Mat balimg = Mat::zeros(img.size(), img.type());
	//Empty Matrix to store new image in
	Mat newimg = Mat::zeros(balimg.size(), balimg.type());
	//Apply White Balance
	balanceWhite(img, balimg, WHITE_BALANCE_SIMPLE);
	//Apply the mask to the img and store that into NewImage
	balimg.copyTo(newimg, mask);
	//Writes newImg Mat into the third argument
	imwrite(argv[3], newimg);
	return 0;
}




