/******************************************************************************/
/*!
 * @file  color_matrix.h
 * @brief 
 * 
 * @author Cathal Harte <cathal.harte@protonmail.com>
 */
#ifndef _COLOR_MATRIX_H
#define _COLOR_MATRIX_H

/*******************************************************************************
* Includes
******************************************************************************/

#include <list>
#include <opencv2/core.hpp>

/*! @defgroup color_matrix Color_matrix.
 *
 * @addtogroup color_matrix
 * @{
 * @brief 
 */

namespace cspace
{
/*******************************************************************************
* Definitions and types
*******************************************************************************/

typedef enum colorspace
{
    UNKNOWN,
    BGR,
    RGB,
    HSV,
    GRAY,
    WHITE_ON_BLACK
} colorspace_t;

/*******************************************************************************
* Data
*******************************************************************************/

/*******************************************************************************
* Class prototypes
*******************************************************************************/

// Rule of three, if a class requires a user defined constructor, copy constructor, or destructor
// it almost certainly requires all three

// So, I have a user defined constructor that sets the colorspace
// we can also create a cspace::Mat without setting the colorspace, but what good is that?

// we have the operator= defined, which comes near to the copy constructor, but is not a copy constructor
// so, with this derived class implementation we have to think carefully about how the rule of three is to
// apply. Should we have a copy constructor for the base class (this is probably inherited), and for the derived class?

// but the path of less resistance is simpler rule of zero
// and the logical conclusion of this is that the colorspace parameter being unknown is going to be a common occurrence

// so in order to make this class minimally involved, we are asking for a little discipline...
// that is, set the expectation of the type of colorspace, and you will be rewarded with a runtime error of what has not occurred

class Mat : public cv::Mat 
{
    public :
        using cv::Mat::Mat;

        Mat& toGray();
        Mat& toBGR();
        Mat& toHSV();
        Mat& operator = (const cv::Mat& m);
        void setColorspace( colorspace_t c);
        colorspace_t getColorspace() { return colorspace; }

        Mat& to3ChannelGray(colorspace_t c);

    protected :
        colorspace_t colorspace = UNKNOWN;

};
/*******************************************************************************
* Function prototypes
*******************************************************************************/

Mat& highlightOverBg(Mat bg, Mat hl);
Mat& highlightOverBg(Mat bg, std::vector<Mat> hls);
Mat &highlightOverBg(Mat bg, std::list<Mat> hls);


}

/*! @}
 */

#endif  // _COLOR_MATRIX_H
