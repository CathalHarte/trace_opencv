/**
* \file color_matrix_test.cpp
*
* \brief ... unit test
*
* \author Cathal Harte  <cathal.harte@protonmail.com>
*/

/*******************************************************************************
* Includes
*******************************************************************************/

#include <gtest/gtest.h>
#include <gtest_helpers.h>
#include <cv_helpers.h>
#include <color_matrix.h>
#include <opencv2/opencv.hpp>
namespace
{

/*******************************************************************************
* Definitions and types
*******************************************************************************/

#define THREEXTHREEX1 (cv::Mat_<char>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1)

/*******************************************************************************
* Local Function prototypes
*******************************************************************************/

/*******************************************************************************
* Data
*******************************************************************************/

DISABLE_SHOW;

/*******************************************************************************
* Functions
*******************************************************************************/

// The behaviour we want is to be able to preset the cspace::Mat to have BGR, HSV, etc
// and then operator= to it a cv::Mat
// todo, ensure that a complaint occurs if we set an expectation of a cspace which clearly is
// not the assignment which is occurring
TEST(colorspace, is_colorspace_persistent)
{
    cspace::Mat temp(2, 2, CV_8UC3, cv::Scalar(11, 33, 99));
    temp.setColorspace(cspace::RGB);
    cv::Mat bgr_mat(2, 2, CV_8UC3, cv::Scalar(0, 0, 255));
    temp = bgr_mat;
    ASSERT_EQ(temp.getColorspace(), cspace::RGB);
}

TEST(colorspace, sanity_check_set_color_known_data)
{
    cspace::Mat known(2, 2, CV_8UC1, cv::Scalar(0));
    ASSERT_DEATH(known.setColorspace(cspace::BGR), ""); // todo : learn regex
}

TEST(colorspace, sanity_check_known_color_set_data)
{
    cspace::Mat sane;
    sane.setColorspace(cspace::RGB);

    cv::Mat gray_mat(2, 2, CV_8UC1, cv::Scalar(0)); // this matrix could never be RGB, it hasn't got the channels

    ASSERT_DEATH(sane = gray_mat, "");
}

// Function to simplify the colorspace conversion - we don't want to explicitly pass
// the current cspace to be able to convert to some desired end cspace
TEST(colorspace, change_colorspace)
{
    cspace::Mat coloured;
    coloured.setColorspace(cspace::BGR); // this is our only method, set colorspace after the fact
    cv::Mat bgr_mat(2, 2, CV_8UC3, cv::Scalar(0, 0, 255));
    coloured = bgr_mat;
    cspace::Mat grey = coloured.toGray();

    ASSERT_EQ(grey.type(), CV_8UC1);
}

// Here is something a little tricky - the matrix is by default created as a 1 channel
// of course we have made the choice above to be able to set cspace on an empty matrix
// so for now this just demonstrates a rather surprising result
// todo is to see if we can change type to the logical type - and that todo is best
// acheived as a failing test case
TEST(colorspace, default_Mat_type)
{
    cspace::Mat coloured;
    coloured.setColorspace(cspace::BGR);
    ASSERT_EQ(coloured.type(), CV_8UC3);
}

TEST(colorspace, hsv_conversion)
{
    cspace::Mat hsv_mat(2, 2, CV_8UC3, cv::Scalar(88, 72, 200));
    hsv_mat.setColorspace(cspace::HSV);
    cspace::Mat gray_mat = hsv_mat.toGray();

    EXPECT_EQ(gray_mat.getColorspace(), cspace::GRAY);
    EXPECT_EQ(gray_mat.type(), CV_8UC1);
}

TEST(colorspace, assign_cspace_Mat_to_cv_Mat)
{
    cspace::Mat rgb_mat(2, 2, CV_8UC3, cv::Scalar(1, 2, 3));
    rgb_mat.setColorspace(cspace::RGB);
    cv::Mat temp = rgb_mat;

    ASSERT_EQ(2, rgb_mat.rows);
    ASSERT_EQ(2, rgb_mat.cols);
    for (int i = 0; i < rgb_mat.rows; i++)
    {
        for (int j = 0; j < rgb_mat.cols; j++)
        {
            EXPECT_EQ(rgb_mat.at<uchar>(i, j), temp.at<uchar>(i, j));
        }
    }
}

TEST(colorspace, BGR_to_gray)
{
    cspace::Mat flowers;
    flowers.setColorspace(cspace::BGR);
    flowers = cv::imread("../../../images/unchalee_flowers.jpg");

    flowers = flowers.toGray();

    EXPECT_EQ(flowers.getColorspace(), cspace::GRAY);
    show(flowers, TEST_NAME);
}

TEST(colorspace, BGR_to_HSV)
{
    cspace::Mat flowers;
    flowers.setColorspace(cspace::BGR);
    flowers = cv::imread("../../../images/unchalee_flowers.jpg");

    flowers = flowers.toHSV();
    EXPECT_EQ(flowers.getColorspace(), cspace::HSV);
    show(flowers, TEST_NAME);
}

TEST(colorspace, convert_image_to_3_channel_gray)
{
    cspace::Mat flowers;
    flowers.setColorspace(cspace::BGR);
    flowers = cv::imread("../../../images/unchalee_flowers.jpg");

    flowers = flowers.to3ChannelGray(cspace::BGR);
    ASSERT_EQ(flowers.getColorspace(), cspace::BGR);
    ASSERT_EQ(flowers.type(), CV_8UC3);
    show(flowers, TEST_NAME);

    uchar red, green, blue;
    cv::MatIterator_<cv::Vec3b> it, end;
    for (it = flowers.begin<cv::Vec3b>(), end = flowers.end<cv::Vec3b>(); it != end; std::advance(it, 1))
    {
        blue = (*it)[0];
        green = (*it)[1];
        red = (*it)[2];
        ASSERT_EQ(blue, green);
        ASSERT_EQ(blue, red);
    }
}

TEST(mixChannels, mix_example)
{
    // In the example below, the code splits a 4-channel BGRA image into a 3-channel BGR (with B and R
    // channels swapped) and a separate alpha-channel image:

    using namespace cv; // hopefully this is just within this particular test case

    Mat bgra(100, 100, CV_8UC4, Scalar(255, 0, 0, 255));
    Mat bgr(bgra.rows, bgra.cols, CV_8UC3);
    Mat alpha(bgra.rows, bgra.cols, CV_8UC1);

    // forming an array of matrices is a quite efficient operation,
    // because the matrix data is not copied, only the headers
    Mat out[] = {bgr, alpha};
    // bgra[0] -> bgr[2], bgra[1] -> bgr[1],
    // bgra[2] -> bgr[0], bgra[3] -> alpha[0]
    int from_to[] = {0, 2, 1, 1, 2, 0, 3, 3};
    mixChannels(&bgra, 1, out, 2, from_to, 4);

    show(out[0], TEST_NAME);
}

TEST(mix_channels, namespace)
{
    // Mat bgr; Evidence that the using namespace above is only for the particular test case
}

TEST(colorspace, highlight_over_background)
{
    cspace::Mat flowers;
    flowers.setColorspace(cspace::BGR);
    flowers = cv::imread("../../../images/unchalee_flowers.jpg");

    GTEST_COUT << "dimensions : " << flowers.size() << std::endl;

    cspace::Mat hl;
    hl.setColorspace(cspace::BGR);
    cspace::Mat *hl_sized_up = new cspace::Mat(flowers.size(), CV_8UC1, cv::Scalar(0));
    ;

    hl = cv::imread("../../../images/cathal_sig.png");
    hl = hl.toGray();

    GTEST_COUT << "dimension highlight : " << hl.size() << std::endl;
    hl.copyTo((*hl_sized_up)(cv::Rect(10, 10, hl.cols, hl.rows)));

    cspace::Mat out;
    out.setColorspace(cspace::BGR);

    out = cspace::highlightOverBg(flowers, *hl_sized_up);

    show(out, TEST_NAME);

    TODO_VERIFY;
}

// let's be more future proof, and allow ourselves to use a vector or list or queue, or whatever,
// as long as it has an iterator

TEST(iterator_func, using_STL_for_iterators)
{
    
    int n1 = 3;
    int n2 = 5;

    std::vector<int> v{0, 1, 2, 3, 4};

    std::begin(v);

    auto result1 = std::find(std::begin(v), std::end(v), n1);
    auto result2 = std::find(std::begin(v), std::end(v), n2);

    if (result1 != std::end(v))
    {
        GTEST_COUT << "v contains: " << n1 << std::endl;
    }
    else
    {
        GTEST_COUT << "v does not contain: " << n1 << std::endl;
    }

    if (result2 != std::end(v))
    {
        GTEST_COUT << "v contains: " << n2 << std::endl;
    }
    else
    {
        GTEST_COUT << "v does not contain: " << n2 << std::endl;
    }
}

void prepMultiHl(cspace::Mat &hl1, cspace::Mat &hl2, cspace::Mat &hl3)
{

    // hl1->setColorspace(cspace::BGR);
    // hl2->setColorspace(cspace::BGR);
    // hl3->setColorspace(cspace::BGR);
    cspace::Mat flowers;
    flowers.setColorspace(cspace::BGR);
    flowers = cv::imread("../../../images/unchalee_flowers.jpg");

    cspace::Mat hl1_i, hl2_i, hl3_i;
    hl1_i.setColorspace(cspace::BGR);
    hl2_i.setColorspace(cspace::BGR);
    hl3_i.setColorspace(cspace::BGR);

    hl1 = cspace::Mat(flowers.size(), CV_8UC1, cv::Scalar(0));
    hl2 = cspace::Mat(flowers.size(), CV_8UC1, cv::Scalar(0));
    hl3 = cspace::Mat(flowers.size(), CV_8UC1, cv::Scalar(0));

    hl1_i = cv::imread("../../../images/cathal_sig.png");
    hl2_i = cv::imread("../../../images/niamh_sig.png");
    hl3_i = cv::imread("../../../images/love.png");

    hl1_i = hl1_i.toGray();
    hl2_i = hl2_i.toGray();
    hl3_i = hl3_i.toGray();

    hl1_i.copyTo(hl1(cv::Rect(10, 10, hl1_i.cols, hl1_i.rows)));
    hl2_i.copyTo(hl2(cv::Rect(10, 200, hl2_i.cols, hl2_i.rows)));
    hl3_i.copyTo(hl3(cv::Rect(10, 400, hl3_i.cols, hl3_i.rows)));
}

TEST(colorspace, multi_hl_over_bg)
{
    cspace::Mat flowers;
    flowers.setColorspace(cspace::BGR);
    flowers = cv::imread("../../../images/unchalee_flowers.jpg");

    cspace::Mat hl1, hl2, hl3;
    prepMultiHl(hl1, hl2, hl3);

    std::list<cspace::Mat> hls;

    hls.push_back(hl1);
    hls.push_back(hl2);
    hls.push_back(hl3);

    cspace::Mat out;
    out.setColorspace(cspace::BGR);

    out = cspace::highlightOverBg(flowers, hls);

    show(out, TEST_NAME);

    TODO_VERIFY;
}

} // namespace
