#pragma once
#include <opencv2/core/core.hpp>
class Game
{
    private:
    int width;
    int height;
    cv::Mat current;
    cv::Mat next;

    public:
    Game();
    Game(int width, int height);
    int area();
    void initializeEmptyGame();
    void simulate(cv::Mat &current, cv::Mat &next, int iterations);
};