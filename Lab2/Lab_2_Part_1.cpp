#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <cassert>
#include <cmath>

using namespace std;

// FUNCTION PROTOTYPES
void imageColor(const cv::Mat &in, cv::Mat &out, int retainRank, int rowStart, int rowStop);
void imageBlur(const cv::Mat &in, cv::Mat &out, int level, int rowStart, int rowStop);
void imageSaturation(const cv::Mat &in, cv::Mat &out, int level, int rowStart, int rowStop);

int main(int argc, char **argv)
{
    cv::Mat image;
    image = cv::imread("./M_cali.jpg", 1); // Read the file

    if (!image.data) // Check for invalid input
    {
        cout << "Could not open or find the image" << std::endl;
    }
    else
    {
        cout << "Image dimension = " << image.rows << " x " << image.cols << endl;
        cout << "Image type = " << image.type() << endl;

        cv::Mat processed_image;
        imageColor(image, processed_image, 1, 0, image.rows);
        //imageBlur(image, processed_image, 2, 0, image.rows);

        cv::namedWindow("Original Image", cv::WINDOW_NORMAL);                     // Create a window for display.
        cv::resizeWindow("Original Image", image.rows / 1.25, image.cols / 1.25); // Resizing the windows to fit my screen nicely.
        cv::imshow("Original Image", image);                                      // Show our image inside it.

        cv::namedWindow("Processed Image", cv::WINDOW_NORMAL);                                         // Create display window.
        cv::resizeWindow("Processed Image", processed_image.rows / 1.25, processed_image.cols / 1.25); // Resizing the windows to fit my screen nicely.
        cv::imshow("Processed Image", processed_image);                                                // Show our image inside it.

        cv::imwrite("./Processed.png", processed_image);

        cv::waitKey(10000); //wait 10 seconds before closing image (or a keypress to close)

        /*
        while (1)
        {
            int key = cv::waitKey(0);
            if (key == 27) // Detecting only the escape key to quit
            {
                cout << "A key press has been detected. The key is: " << key << std::endl;
                cv::destroyAllWindows();
            }
        }*/
    }
}

// Color Retention
void imageColor(const cv::Mat &in, cv::Mat &out, int retainRank, int rowStart, int rowStop)
{

    assert(in.type() == CV_8UC3);

    out = in.clone();
    cout << "Performing color retention on the input image" << std::endl;

    switch (retainRank)
    {
    // Retaining the mainimum channel
    case 0:
        cout << "retaining the minimum channel" << endl;
        for (int irow = rowStart; irow < rowStop; irow++)
        {
            for (int icol = 0; icol < out.cols; icol++)
            {
                int blue = in.at<cv::Vec3b>(irow, icol).val[0];
                int green = in.at<cv::Vec3b>(irow, icol).val[1];
                int red = in.at<cv::Vec3b>(irow, icol).val[2];
                int minValue = std::min({blue, green, red});

                // Detecting if the color is black, white or gray, if so leave them be
                if (abs(blue - green) < 5 && abs(green - red) < 5)
                {
                    //Do nothing
                }
                else if (minValue == blue)
                {
                    out.at<cv::Vec3b>(irow, icol).val[1] = 128;
                    out.at<cv::Vec3b>(irow, icol).val[2] = 128;
                }
                else if (minValue == green)
                {
                    out.at<cv::Vec3b>(irow, icol).val[0] = 128;
                    out.at<cv::Vec3b>(irow, icol).val[2] = 128;
                }
                else if (minValue == red)
                {
                    out.at<cv::Vec3b>(irow, icol).val[0] = 128;
                    out.at<cv::Vec3b>(irow, icol).val[1] = 128;
                }
                else
                {
                    cout << "an error occurred determining the minimum channel" << endl;
                }
            }
        }
        break;
    // Retaining the median channel
    case 1:
        cout << "retaining the median channel" << endl;
        for (int irow = rowStart; irow < rowStop; irow++)
        {
            for (int icol = 0; icol < out.cols; icol++)
            {
                int blue = in.at<cv::Vec3b>(irow, icol).val[0];
                int green = in.at<cv::Vec3b>(irow, icol).val[1];
                int red = in.at<cv::Vec3b>(irow, icol).val[2];
                int minValue = std::min({blue, green, red});
                int maxValue = std::max({blue, green, red});

                // Detecting if the color is black, white or gray, if so leave them be
                if (abs(blue - green) < 5 && abs(green - red) < 5)
                {
                    //Do nothing
                }
                else if (blue != minValue && blue != maxValue)
                {
                    out.at<cv::Vec3b>(irow, icol).val[1] = 128;
                    out.at<cv::Vec3b>(irow, icol).val[2] = 128;
                }
                else if (green != minValue && green != maxValue)
                {
                    out.at<cv::Vec3b>(irow, icol).val[0] = 128;
                    out.at<cv::Vec3b>(irow, icol).val[2] = 128;
                }
                else if (red != minValue && red != maxValue)
                {
                    out.at<cv::Vec3b>(irow, icol).val[0] = 128;
                    out.at<cv::Vec3b>(irow, icol).val[1] = 128;
                }
                // EDGE CASE: when there is no median value (i.e. B=255, G = 0, R = 255), pick one of the two equal values
                else if (blue == green || green == red || blue == red)
                {
                    if (blue == green)
                    {
                        out.at<cv::Vec3b>(irow, icol).val[1] = 128;
                        out.at<cv::Vec3b>(irow, icol).val[2] = 128;
                    }
                    if (green == red)
                    {
                        out.at<cv::Vec3b>(irow, icol).val[0] = 128;
                        out.at<cv::Vec3b>(irow, icol).val[2] = 128;
                    }
                    if (blue == red)
                    {
                        out.at<cv::Vec3b>(irow, icol).val[0] = 128;
                        out.at<cv::Vec3b>(irow, icol).val[1] = 128;
                    }
                }
                else
                {
                    cout << "an error occurred determining the median channel" << endl;
                    cout << "BGR" << blue << green << red << std::endl;
                }
            }
        }
        break;
    // Retaining the maximum channel
    case 2:
        cout << "retaining the maximum channel" << endl;
        for (int irow = rowStart; irow < rowStop; irow++)
        {
            for (int icol = 0; icol < out.cols; icol++)
            {
                int blue = in.at<cv::Vec3b>(irow, icol).val[0];
                int green = in.at<cv::Vec3b>(irow, icol).val[1];
                int red = in.at<cv::Vec3b>(irow, icol).val[2];
                int maxValue = std::max({blue, green, red});

                // Detecting if the color is black, white or gray, if so leave them be
                if (abs(blue - green) < 5 && abs(green - red) < 5)
                {
                    //Do nothing
                }
                else if (maxValue == blue)
                {
                    out.at<cv::Vec3b>(irow, icol).val[1] = 128;
                    out.at<cv::Vec3b>(irow, icol).val[2] = 128;
                }
                else if (maxValue == green)
                {
                    out.at<cv::Vec3b>(irow, icol).val[0] = 128;
                    out.at<cv::Vec3b>(irow, icol).val[2] = 128;
                }
                else if (maxValue == red)
                {
                    out.at<cv::Vec3b>(irow, icol).val[0] = 128;
                    out.at<cv::Vec3b>(irow, icol).val[1] = 128;
                }
                else
                {
                    cout << "an error occurred determining the maximim channel" << endl;
                }
            }
        }
        break;

    default:
        cout << "unrecognized image retain rank" << endl;
    }
}
// Blur
void imageBlur(const cv::Mat &in, cv::Mat &out, int level, int rowStart, int rowStop)
{
    assert(in.type() == CV_8UC3);
    out = in.clone();
    cout << "Performing blur on the input image" << std::endl;

    //Kernel properties
    int kernel_x_start;
    int kernel_y_start;
    int kernel_x_curr;
    int kernel_y_curr;
    int kernel_x_end;
    int kernel_y_end;

    double kernel_channel_sum;
    int kernel_num_entries;
    double kernel_channel_avg;

    if (level > 1)
    {
        for (int irow = rowStart; irow < rowStop; irow++)
        {
            for (int icol = 0; icol < out.cols; icol++)
            {
                for (int ichannel = 0; ichannel < 3; ichannel++)
                {
                    // Computing the kernel for a given channel
                    kernel_y_start = irow - level;
                    kernel_y_end = irow + level;
                    kernel_y_curr = kernel_y_start;

                    while (kernel_y_curr < kernel_y_end && kernel_y_curr < rowStop)
                    {
                        kernel_x_start = icol - level;
                        kernel_x_end = icol + level;
                        kernel_x_curr = kernel_x_start;

                        while (kernel_x_curr < kernel_x_end & kernel_x_curr < icol)
                        {
                            kernel_channel_sum += in.at<cv::Vec3b>(kernel_y_curr, kernel_x_curr).val[ichannel];
                            kernel_num_entries++;
                            kernel_x_curr++;
                        }
                        kernel_y_curr++;
                    }

                    kernel_channel_avg = kernel_channel_sum / kernel_num_entries;
                    kernel_channel_sum = 0;
                    kernel_num_entries = 0;

                    out.at<cv::Vec3b>(irow, icol).val[ichannel] = kernel_channel_avg;
                }
            }
        }
    }
}
// Saturation
void imageSaturation(const cv::Mat &in, cv::Mat &out, int level, int rowStart, int rowStop)
{
}
