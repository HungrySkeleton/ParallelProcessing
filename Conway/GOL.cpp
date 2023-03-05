#include <iostream>
#include <mpi.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>



void apply_rules(cv::Mat current, cv::Mat next);
void simulate(cv::Mat &current, cv::Mat &next, int iterations);
std::string generateName(int iteration);
int wrapIndex(int i, int i_max);

void apply_rules(cv::Mat &current, cv::Mat &next, int x, int y, int count){
    //Alive
    bool alive = current.at<unsigned char>(x,y) == 255;
    if(alive)
    {
        if(count == 2 || count == 3)
        {
            next.at<unsigned char>(x,y) = 255;
        }
        else
        {
            next.at<unsigned char>(x,y) = 0;
        }
    }
    else
    {
        if(count == 3)
        {
            next.at<unsigned char>(x,y) = 255;
        }
    }
}

int countNeighbors(cv::Mat &current, int x, int y)
{
    int kernelRadius = 1;
    int xStart = x - kernelRadius;
    int xEnd = x + kernelRadius + 1;
    int yStart = y - kernelRadius;
    int yEnd = y + kernelRadius + 1;

    int rows = current.rows;
    int cols = current.cols;
    int aliveCount = 0;
    int r =  wrapIndex(10, current.rows);
    for(int i = xStart; i < xEnd; i++)
    {
        for(int j = yStart; j < yEnd; j++)
        {
            int offsetX = wrapIndex(i, current.rows);
            int offsetY = wrapIndex(j, current.cols);
            if(offsetX == x && offsetY == y)
            {

            }
            else
            {
                if(current.at<unsigned char>(offsetX, offsetY) == 255)
                {
                    aliveCount++;
                }
            }
        }
    }
    return aliveCount;
}

int wrapIndex(int i, int i_max) {
   return ((i % i_max) + i_max) % i_max;
}

std::string generateName(int iteration){
    std::ostringstream oss;
    std::ofstream out_to_file;
    std::string s;

    oss << "./SerialImage" << iteration <<".png"; //Create File Name
    s = oss.str(); //Convert OSS to string  
    return s;
}

void simulate(cv::Mat &current, cv::Mat &next, int iterations)
{
    std::cout << "current = " << std::endl << " "  << current << std::endl << std::endl;
    for(int i = 0; i < iterations; i++)
    {
        //Iterate across every cell
        for(int j = 0; j < current.rows; j++)
        {
            for(int k = 0; k < current.cols ; k++)
            {
                int aliveCount = countNeighbors(current,j,k);
                apply_rules(current, next, j, k, aliveCount);
                std::cout << "(" << aliveCount <<")";
            }
            std::cout << std::endl;
        }
        std::cout << "next = " << std::endl << " "  << next << std::endl << std::endl;
        std::string s = generateName(i);
        cv::imwrite(s, next);
        current = next.clone();
        next = cv::Mat::zeros(current.rows, current.cols, CV_8U);
        std::cout << "current = " << std::endl << " "  << current << std::endl << std::endl;
        std::cout << "next = " << std::endl << " "  << next << std::endl << std::endl;
        std::cout << "X:" << next.rows << "Y:" << next.cols <<std::endl;
       //cv::imwrite(s, current);
   }
}

void addLineTriomino(cv::Mat &current)
{
    // int centreX = 0;
    // int centreY = 0;

    int centreX = current.rows / 2;
    int centreY = current.cols / 2;

    int x1 = 0;
    int x2 = 1;
    int x3 = 2;
    //int x3 = current.rows - 1;

    int y1 = 0;
    int y2 = 0;
    int y3 = 0;

    int offsetx1 = centreX + x1;
    int offsetx2 = centreX + x2;
    int offsetx3 = centreX + x3;

    int offsety1 = centreY + y1;
    int offsety2 = centreY + y2;
    int offsety3 = centreY + y3;

    current.at<unsigned char>(offsetx1, offsety1) = 255;
    current.at<unsigned char>(offsetx2, offsety2) = 255;
    current.at<unsigned char>(offsetx3, offsety3) = 255;
    std::cout << "CX:" << centreX << "CY:" << centreY << std::endl;
    std::cout << "O1X:" << offsetx1 << "O1Y:" << offsety1 << std::endl;
    std::cout << "O2X:" << offsetx2 << "O2Y:" << offsety2 << std::endl;
    std::cout << "O3X:" << offsetx3 << "O3Y:" << offsety3 << std::endl;
}

void addGlider(cv::Mat &current)
{
    int centreX = current.rows / 2;
    int centreY = current.cols / 2;

    int x1 = 0;
    int x2 = 0;
    int x3 = 0;
    int x4 = 1;
    int x5 = 2;

    int y1 = 0;
    int y2 = 1;
    int y3 = 2;
    int y4 = 0;
    int y5 = 1;

    int offsetx1 = centreX + x1;
    int offsetx2 = centreX + x2;
    int offsetx3 = centreX + x3;
    int offsetx4 = centreX + x4;
    int offsetx5 = centreX + x5;

    int offsety1 = centreY + y1;
    int offsety2 = centreY + y2;
    int offsety3 = centreY + y3;
    int offsety4 = centreY + y4;
    int offsety5 = centreY + y5;

    current.at<unsigned char>(offsetx1, offsety1) = 255;
    current.at<unsigned char>(offsetx2, offsety2) = 255;
    current.at<unsigned char>(offsetx3, offsety3) = 255;
    current.at<unsigned char>(offsetx4, offsety4) = 255;
    current.at<unsigned char>(offsetx5, offsety5) = 255;
    std::cout << "CX:" << centreX << "CY:" << centreY << std::endl;
    std::cout << "O1X:" << offsetx1 << "O1Y:" << offsety1 << std::endl;
    std::cout << "O2X:" << offsetx2 << "O2Y:" << offsety2 << std::endl;
    std::cout << "O3X:" << offsetx3 << "O3Y:" << offsety3 << std::endl;
    std::cout << "O4X:" << offsetx4 << "O4Y:" << offsety4 << std::endl;
    std::cout << "O5X:" << offsetx5 << "O5Y:" << offsety5 << std::endl;
}

int main()
{
    std::cout  << "Total time to perform serial image processing " << " seconds" << std::endl;
    cv::Mat image = cv::Mat::zeros(19,19,CV_8U);
    cv::Mat out = cv::Mat::zeros(19,19,CV_8U);
    double low = -500.0;
    double high = 500.0;
    int iterations = 1;
    //addLineTriomino(image);
    addGlider(image);
    //cv::randu(image, cv::Scalar(low), cv::Scalar(high));
    cv::imwrite("./SerialImage.png", image);

    simulate(image, out, iterations);
    return 0;
}