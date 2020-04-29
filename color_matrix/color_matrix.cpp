/******************************************************************************/
/*!
 * @file  color_matrix.cpp
 * @brief
 * 
 * @author Cathal Harte <cathal.harte@protonmail.com>
 */

/*******************************************************************************
* Includes
******************************************************************************/

#include "color_matrix.h"

#include <cassert>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
namespace cspace
{

/*******************************************************************************
* Definitions
*******************************************************************************/

/*******************************************************************************
* Types
*******************************************************************************/

/*******************************************************************************
* Internal function prototypes
*******************************************************************************/

/*******************************************************************************
* Classes
*******************************************************************************/

/*******************************************************************************
* Functions
*******************************************************************************/

// todo, no user defined conversion from cspace::Mat to const cv::Mat exists - why the hell not?

void checkColorspaceMatch(const cv::Mat &in, colorspace_t c)
{
    if (in.rows && in.cols)
    // otherwise the matrix data has not been initialised, and we aren't interested in the channels
    {
        switch (c)
        {
        case BGR:
        case RGB:
        case HSV:
            assert(("this is a 3 channel matrix", in.type() == CV_8UC3));
            break;
        case GRAY:
        case WHITE_ON_BLACK:
            assert(("this is a single channel matrix", in.type() == CV_8UC1));
            break;
        case UNKNOWN:
            // do nothing
            break;
        default:
            throw std::runtime_error("colorspace not implemented");
            break;
        }
    }
}

void Mat::setColorspace(colorspace_t c)
{
    checkColorspaceMatch(*this, c);

    colorspace = c;
}

Mat &Mat::toGray()
{
    Mat *out = new Mat();
    out->setColorspace(GRAY);

    switch (colorspace)
    {
    case GRAY:
    case WHITE_ON_BLACK:
        *out = *this;
        out->setColorspace(GRAY);
        break;
    case HSV:
        cvtColor(*this, *out, cv::COLOR_HSV2BGR);
        // Fallthrough - can't get directly to gray from HSV, so use BGR as proxy
    case BGR:
        cvtColor(*this, *out, cv::COLOR_BGR2GRAY);
        break;
    case RGB:
        cvtColor(*this, *out, cv::COLOR_RGB2GRAY);
        break;
    default:
        throw std::runtime_error("Colorspace not implemented");
        break;
    }

    return *out;
}

// https://en.cppreference.com/w/cpp/language/throw
// https://en.cppreference.com/w/cpp/error/assert
Mat &Mat::to3ChannelGray(colorspace_t c)
{
    assert(("known colorspace", c != UNKNOWN));

    Mat *out = new Mat;
    out->setColorspace(c);

    switch (colorspace)
    {
    case GRAY:
    case WHITE_ON_BLACK:
        *out = *this;
        out->setColorspace(GRAY);
        break;
    default:
        *out = toGray(); // rely on this function to find unsupported cspaces
        break;
    }

    switch (c)
    {
    case BGR:
    case RGB:
        // R=G=B for each pixel, so it doesn't matter which of the two conversions is used
        *out = out->toBGR();
        break;
    case HSV:
        *out = out->toHSV();
        break;
    default:
        throw std::runtime_error("colorspace 3 channel / not fully implemented");
        break;
    }

    return *out;
}

Mat &Mat::toHSV()
{
    Mat *out = new Mat;
    out->setColorspace(HSV);

    switch (colorspace)
    {
    case HSV:
        *out = *this;
        break;
    case BGR:
        cvtColor(*this, *out, cv::COLOR_BGR2HSV);
        break;
    case RGB:
        cvtColor(*this, *out, cv::COLOR_RGB2HSV);
        break;
    case GRAY:
    case WHITE_ON_BLACK:
        cvtColor(*this, *out, cv::COLOR_GRAY2BGR);
        cvtColor(*out, *out, cv::COLOR_BGR2HSV);
        break;
    default:
        throw std::runtime_error("colorspace not implemented");
        break;
    }

    return *out;
}

Mat &Mat::toBGR()
{
    Mat *out = new Mat;
    out->setColorspace(BGR);

    switch (colorspace)
    {
    case BGR:
        *out = *this;
    case HSV:
        cvtColor(*this, *out, cv::COLOR_HSV2BGR);
        break;
    case RGB:
        cvtColor(*this, *out, cv::COLOR_RGB2BGR);
        break;
    case GRAY:
    case WHITE_ON_BLACK:
        cvtColor(*this, *out, cv::COLOR_GRAY2BGR);
        break;
    default:
        throw std::runtime_error("colorspace not implemented");
        break;
    }

    return *out;
}

cspace::Mat &cspace::Mat::operator=(const cv::Mat &m)
{
    if (this != &m)
    {
        checkColorspaceMatch(m, this->getColorspace());

        if (m.type() == CV_8UC1)
        {
            setColorspace(GRAY);
        }

        if (m.type() == CV_8UC3)
        {
            if (colorspace == GRAY)
            {
                setColorspace(BGR); // we aren't sure, set to the default 3 channel colorscheme
            }
        }

        if (m.u)
        {
            CV_XADD(&m.u->refcount, 1);
        }
        release();
        flags = m.flags;
        if (dims <= 2 && m.dims <= 2)
        {
            dims = m.dims;
            rows = m.rows;
            cols = m.cols;
            step[0] = m.step[0];
            step[1] = m.step[1];
        }
        else
        {
            copySize(m);
        }
        data = m.data;
        datastart = m.datastart;
        dataend = m.dataend;
        datalimit = m.datalimit;
        allocator = m.allocator;
        u = m.u;
    }
    return *this;
}

Mat &highlightOverBg(Mat bg, Mat hl)
{
    Mat *out = new Mat;
    out->setColorspace(HSV);
    *out = bg.to3ChannelGray(HSV);

    cv::MatIterator_<uchar> it_hl = hl.begin<uchar>();
    cv::MatIterator_<cv::Vec3b> it_out = out->begin<cv::Vec3b>();
    for (;
         it_out != out->end<cv::Vec3b>();
         std::advance(it_out, 1), std::advance(it_hl, 1))
    {
        if ((*it_hl) > 127)
        {
            (*it_out)[0] = 100;
            (*it_out)[1] = 255;
            (*it_out)[2] = 255;
        }
    }
    return out->toBGR();
}

// this seems simple - both list and vector are acceptable for this operation, 
// and the code within is identical, however on account of auto, I cannot simplify both functions down to one

Mat &highlightOverBg(Mat bg, std::list<Mat> hls)
{
    Mat *out = new Mat;
    out->setColorspace(HSV);
    *out = bg.to3ChannelGray(HSV);

    auto it_hls = hls.begin();
    auto end_hls = hls.end();
    cv::MatIterator_<uchar> it_hl;
    cv::MatIterator_<cv::Vec3b> it_out = out->begin<cv::Vec3b>();

    assert(("color depth insufficient for visualization",
            hls.size() < 256));

    uchar num_hls = hls.size();
    uchar hue_step = 255 / num_hls;
    std::vector<uchar> hue_map;
    uchar hue;

    for (uchar i = 0; i < num_hls; i++)
    {
        hue_map.push_back((i + 1) * hue_step);
    }

    int hue_idx = 0;
    for ( ; it_hls != end_hls; std::advance(it_hls,1))
    {
        it_hl = it_hls->begin<uchar>();
        it_out = out->begin<cv::Vec3b>();
        hue = hue_map[hue_idx++];

        for (;
             it_out < out->end<cv::Vec3b>();
             std::advance(it_hl, 1),
             std::advance(it_out, 1))
        {
            if ((*it_hl) > 127)
            {
                (*it_out)[0] = hue;
                (*it_out)[1] = 255;
                (*it_out)[2] = 255;
            }
        }
    }

    return out->toBGR();
}

Mat &highlightOverBg(Mat bg, std::vector<Mat> hls)
{
    Mat *out = new Mat;
    out->setColorspace(HSV);
    *out = bg.to3ChannelGray(HSV);

    auto it_hls = hls.begin();
    auto end_hls = hls.end();
    cv::MatIterator_<uchar> it_hl;
    cv::MatIterator_<cv::Vec3b> it_out = out->begin<cv::Vec3b>();

    assert(("color depth insufficient for visualization",
            hls.size() < 256));

    uchar num_hls = hls.size();
    uchar hue_step = 255 / num_hls;
    std::vector<uchar> hue_map;
    uchar hue;

    for (uchar i = 0; i < num_hls; i++)
    {
        hue_map.push_back((i + 1) * hue_step);
    }

    int hue_idx = 0;
    for ( ; it_hls != end_hls; std::advance(it_hls,1))
    {
        it_hl = it_hls->begin<uchar>();
        it_out = out->begin<cv::Vec3b>();
        hue = hue_map[hue_idx++];

        for (;
             it_out < out->end<cv::Vec3b>();
             std::advance(it_hl, 1),
             std::advance(it_out, 1))
        {
            if ((*it_hl) > 127)
            {
                (*it_out)[0] = hue;
                (*it_out)[1] = 255;
                (*it_out)[2] = 255;
            }
        }
    }

    return out->toBGR();
}

} // namespace cspace