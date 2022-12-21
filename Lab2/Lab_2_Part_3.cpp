#include <iostream>
#include <mpi.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

#define PIXEL_ARRAY_TAG 99

enum IMAGE_PROCESS_TYPE
{
    IMAGE_PROCESS_TYPE_MIN = 0,
    IMAGE_PROCESS_TYPE_BLUR,
    IMAGE_PROCESS_TYPE_RETAIN,
    IMAGE_PROCESS_TYPE_SATURATE,
    IMAGE_PROCESS_TYPE_MAX
};

typedef struct
{
    int numCols;           // The number of columns of the image
    int parameterH;        // The number of halo rows that pads the image
    int numRowsTotal;      // Total number of rows being transmitted including halo rows
    int subImageStart;     // The index of the first row of the sub-image (without halo rows)
    int subImageStop;      // The index of the last row of the sub-image (without halo rows)
    int subImageCount;     // The number of rows this processor is responsible for
    int subImageProcStart; // The row index rank x should start processing from
    int subImageProcStop;  // The row index rank x should stop processing at
    int process_type;      // The type of processing to be done on the image
    int blur_strength;     // A blur processing strength parameter. OPTIONAL. Set to 0 if no used
} subImage_descriptor;

// Function Prototypes
void parallelRange(int globalstart, int globalstop, int irank, int nproc, int &localstart, int &localstop, int &localcount);
void imageColor(const cv::Mat &in, cv::Mat &out, int retainRank, int rowStart, int rowStop);
void imageBlur(const cv::Mat &in, cv::Mat &out, int level, int rowStart, int rowStop);
void doAverageForSingleColourChannel(const cv::Mat &, cv::Mat &, int, int, int, int);
bool isCoordinateInBounds(int, int, int, int, int, int);
void computeSinglePixelChannelAverage(const cv::Mat &, cv::Mat &, int, int, int, int, int, int);

int main(int argc, char **argv)
{
    assert(argc == 5);

    int rank = 0;
    int nproc = 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        ostringstream fileNameIn;
        fileNameIn << "./" << argv[1];
        int edit_type = atoi(argv[2]);
        int edit_strength = atoi(argv[3]);
        ostringstream fileNameOut;
        fileNameOut << "./" << argv[4];

        cv::Mat image;
        cv::Mat processed_image;
        cv::Mat serialImage;
        cv::Mat parallelImage;
        image = cv::imread(fileNameIn.str(), 1); // Read the file
        processed_image = image.clone();

        // Defining Image dimmensions
        int N = image.cols;    // Number of pixels within a row of the image
        int M = image.rows;    // Number of pixels within a column of the image
        int h = edit_strength; // Number of halo rows
        
        //Performing Serial Image Processing
        int t1 = MPI_Wtime();
        if (edit_type == 0)
        {
            imageColor(image, processed_image, 0, 0, M);
        }
        else if (edit_type == 1)
        {
            imageBlur(image, processed_image, edit_strength, 0, M);
        }
        else if (edit_type == 2)
        {
            imageColor(image, processed_image, 1, 0, M);
        }
        else if (edit_type == 3)
        {
            //saturate
        }
        else if (edit_type == 4)
        {
            imageColor(image, processed_image, 2, 0, M);
        }
        else
        {
            std::cout << "Error: invalid process type" << std::endl;
        }
        int t2 = MPI_Wtime();
        std::cout  << "Total time to perform serial image processing " << t2 - t1  << " seconds" << std::endl;

        string windowName = "SerialImage";
        cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE); // Create a window for display.
        cv::imshow(windowName, processed_image);          // Show our image inside it.
        cv::waitKey(1);                                   //wait 10 seconds before closing image (or a keypress to close)
        cv::imwrite("./SerialImage.png", processed_image);

        //Parallel Image Processing
        serialImage = processed_image.clone();
        processed_image = image.clone();
        t1 = MPI_Wtime();
        //Broadcast the number of columns to other ranks so they can also define row_of_N_pixels
        MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Defining contiguous data type for cv::Vec3b
        MPI_Datatype pixel;
        MPI_Type_contiguous(3, MPI_BYTE, &pixel);
        MPI_Type_commit(&pixel);

        // Defining contiguous data type for a row of n pixel
        MPI_Datatype row_of_N_pixels;
        MPI_Type_contiguous(N, pixel, &row_of_N_pixels);
        MPI_Type_commit(&row_of_N_pixels);

        if (!image.data) // Check for invalid input
        {
            cout << "Could not open or find the image" << std::endl;

            MPI_Type_free(&pixel);
            MPI_Type_free(&row_of_N_pixels);
            MPI_Finalize();
        }
        else
        {
            //--------------------------
            // Determine what to send
            //--------------------------
            std::vector<int> localstart(nproc), localstop(nproc), localcount(nproc);
            std::vector<subImage_descriptor> descriptors(nproc);

            for (unsigned int irank = 0; irank < nproc; irank++)
            {
                parallelRange(0, M - 1, irank, nproc, localstart[irank], localstop[irank], localcount[irank]);

                descriptors[irank].numCols = N;
                descriptors[irank].parameterH = h;
                descriptors[irank].numRowsTotal = localcount[irank];
                descriptors[irank].subImageStart = localstart[irank];
                descriptors[irank].subImageStop = localstop[irank] + 1; // OFFSET TO ALSO INCLUDE THE LOCALSTOP ROW IN PROCESSING
                descriptors[irank].subImageCount = descriptors[irank].subImageStop - descriptors[irank].subImageStart;
                descriptors[irank].subImageProcStart = localstart[irank] - (irank * localcount[irank]);
                descriptors[irank].subImageProcStop = descriptors[irank].subImageStop - (irank * descriptors[irank].numRowsTotal);
                descriptors[irank].process_type = edit_type;
                descriptors[irank].blur_strength = edit_strength;
            }

            //--------------------------
            // Offsetting the range of each process to accomodate halo rows
            //--------------------------
            for (unsigned int irank = 0; irank < nproc; irank++)
            {
                if (localstart[irank] != 0)
                {
                    localstart[irank] -= h;
                    localcount[irank] += h;
                    descriptors[irank].subImageProcStart += h;
                    descriptors[irank].subImageProcStop += h;
                }
                if (localstop[irank] != (M - 1))
                {
                    localstop[irank] += h;
                    localcount[irank] += h;
                }
                descriptors[irank].numRowsTotal = localcount[irank];
            }

            //---------------------------------
            // Send setup to processors
            //---------------------------------
            for (int irank = 1; irank < nproc; irank++)
            {
                MPI_Send(&descriptors[irank], sizeof(subImage_descriptor), MPI_BYTE, irank, PIXEL_ARRAY_TAG, MPI_COMM_WORLD);
            }

            //------------------------------------
            // Package and send data to processors
            //------------------------------------
            for (int irank = 1; irank < nproc; irank++)
            {
                MPI_Send(&image.at<cv::Vec3b>(localstart[irank], 0), localcount[irank], row_of_N_pixels, irank, PIXEL_ARRAY_TAG, MPI_COMM_WORLD);
            }

            //------------------------------------
            // Do Rank 0 work
            //------------------------------------
            processed_image = image.clone();

            if (descriptors[rank].process_type == 0)
            {
                imageColor(image, processed_image, 0, descriptors[rank].subImageProcStart, descriptors[rank].subImageProcStop);
            }
            else if (descriptors[rank].process_type == 1)
            {
                imageBlur(image, processed_image, descriptors[rank].blur_strength, descriptors[rank].subImageProcStart, descriptors[rank].subImageProcStop);
            }
            else if (descriptors[rank].process_type == 2)
            {
                imageColor(image, processed_image, 1, descriptors[rank].subImageProcStart, descriptors[rank].subImageProcStop);
            }
            else if (descriptors[rank].process_type == 3)
            {
                //saturate
            }
            else if (descriptors[rank].process_type == 4)
            {
                imageColor(image, processed_image, 2, descriptors[rank].subImageProcStart, descriptors[rank].subImageProcStop);
            }
            else
            {
                std::cout << "Error: invalid process type" << std::endl;
            }

            //------------------------------------
            // Displaying the half processed image
            //------------------------------------
            string windowName = "Rank " + to_string(rank) + " half baked image";
            cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE); // Create a window for display.
            cv::imshow(windowName, processed_image);          // Show our image inside it.
            cv::waitKey(1);                                   //wait 10 seconds before closing image (or a keypress to close)
            cv::imwrite("./P2_rank_0_half_baked.png", processed_image);

            //---------------------------------
            // Receive Processed Data
            //---------------------------------
            MPI_Status stat;
            for (int irank = 1; irank < nproc; irank++)
            {
                MPI_Recv(&processed_image.at<cv::Vec3b>(descriptors[irank].subImageStart, 0), descriptors[irank].subImageCount, row_of_N_pixels, irank, PIXEL_ARRAY_TAG, MPI_COMM_WORLD, &stat);
            }
            t2 = MPI_Wtime();
            std::cout  << "Total time to perform parallel image processing " << t2 - t1  << " seconds" << std::endl;
            //------------------------------------
            // Displaying the final image
            //------------------------------------
            cv::namedWindow("Final Image", cv::WINDOW_AUTOSIZE); // Create a window for display.
            cv::imshow("Final Image", processed_image);          // Show our image inside it.
            cv::waitKey(1);                                      //wait 10 seconds before closing image (or a keypress to close)
            cv::imwrite(fileNameOut.str(), processed_image);
            
            parallelImage = processed_image.clone();
            
            cv::Mat diff_im = serialImage - parallelImage;
            cv::namedWindow("Difference Image", cv::WINDOW_AUTOSIZE); // Create a window for display.
            cv::imshow("Difference Image", diff_im);          // Show our image inside it.
            cv::waitKey(1);                                      //wait 10 seconds before closing image (or a keypress to close)
            cv::imwrite("./Difference Image.png", diff_im);


            MPI_Type_free(&pixel);
            MPI_Type_free(&row_of_N_pixels);
        }
    }
    else
    {
        int N; // The number of columns in the image to be processed

        //Broadcast the number of columns to other ranks so they can also define row_of_N_pixels
        MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
        std::cout << "Rank " << rank << " received N = " << N << std::endl;

        // Defining contiguous data type for cv::Vec3b
        MPI_Datatype pixel;
        MPI_Type_contiguous(3, MPI_BYTE, &pixel);
        MPI_Type_commit(&pixel);

        // Defining contiguous data type for a row of n pixel
        MPI_Datatype row_of_N_pixels;
        MPI_Type_contiguous(N, pixel, &row_of_N_pixels);
        MPI_Type_commit(&row_of_N_pixels);

        // Creating the buffer the sub-image descriptor will be stored in
        subImage_descriptor myDescriptor;

        //------------------------------------
        // Get Sub-Image Descriptor and Setup Relevant Parameters
        //------------------------------------
        MPI_Status descriptorStatus;
        MPI_Recv(&myDescriptor, sizeof(subImage_descriptor), MPI_BYTE, 0, PIXEL_ARRAY_TAG, MPI_COMM_WORLD, &descriptorStatus);

        int subImage_numCols = myDescriptor.numCols;
        int haloParameter = myDescriptor.parameterH;
        int numRowsRecv = myDescriptor.numRowsTotal;
        int subImage_procStartIndex = myDescriptor.subImageProcStart;
        int subImage_procStopIndex = myDescriptor.subImageProcStop;
        int subImage_procCount = subImage_procStopIndex - subImage_procStartIndex;

        //------------------------------------
        // Create the Receive Buffer
        //------------------------------------
        cv::Mat subImage(numRowsRecv, subImage_numCols, CV_8UC3, cv::Scalar(0, 0, 0));
        cv::Mat subImageOut;

        //------------------------------------
        // Receive Data
        //------------------------------------
        MPI_Status dataStatus;
        MPI_Recv(&subImage.at<cv::Vec3b>(0, 0), numRowsRecv, row_of_N_pixels, 0, PIXEL_ARRAY_TAG, MPI_COMM_WORLD, &dataStatus);
        subImageOut = subImage.clone();
        std::cout << "Rank " << rank << " received a sub-image of size (including halo rows): " << subImage_numCols << " x " << numRowsRecv << std::endl;

        //------------------------------------
        // Displaying the received sub-image
        //------------------------------------
        string windowName = "Rank " + to_string(rank) + " received sub-image";
        cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE); // Create a window for display.
        cv::imshow(windowName, subImage);                 // Show our image inside it.
        cv::waitKey(1);                                   //wait 10 seconds before closing image (or a keypress to close)

        //------------------------------------
        // Generating a unique file name to output the received sub-image to
        //------------------------------------
        ostringstream recvSubImageName;
        recvSubImageName << "./Rank " << rank << " received sub-image.png";
        cv::imwrite(recvSubImageName.str(), subImage);

        //------------------------------------
        // Do work
        //------------------------------------
        if (myDescriptor.process_type == 0)
        {
            imageColor(subImage, subImageOut, 0, myDescriptor.subImageProcStart, myDescriptor.subImageProcStop);
        }
        else if (myDescriptor.process_type == 1)
        {
            imageBlur(subImage, subImageOut, myDescriptor.blur_strength, myDescriptor.subImageProcStart, myDescriptor.subImageProcStop);
        }
        else if (myDescriptor.process_type == 2)
        {
            imageColor(subImage, subImageOut, 1, myDescriptor.subImageProcStart, myDescriptor.subImageProcStop);
        }
        else if (myDescriptor.process_type == 3)
        {
            //saturate
        }
        else if (myDescriptor.process_type == 4)
        {
            imageColor(subImage, subImageOut, 2, myDescriptor.subImageProcStart, myDescriptor.subImageProcStop);
        }
        else
        {
            std::cout << "Error: invalid process type" << std::endl;
        }

        //------------------------------------
        // Displaying the processed sub-image
        //------------------------------------
        windowName = "Rank " + to_string(rank) + " processed sub-image";
        cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE); // Create a window for display.
        cv::imshow(windowName, subImageOut);              // Show our image inside it.
        cv::waitKey(1);                                   //wait 10 seconds before closing image (or a keypress to close)

        //------------------------------------
        // Generating a unique file name to output the processed sub-image to
        //------------------------------------
        ostringstream procSubImageName;
        procSubImageName << "./Rank " << rank << " processed sub-image.png";
        cv::imwrite(procSubImageName.str(), subImageOut);

        //------------------------------------
        // Send the processed data back
        //------------------------------------
        MPI_Send(&subImageOut.at<cv::Vec3b>(subImage_procStartIndex, 0), subImage_procCount, row_of_N_pixels, 0, PIXEL_ARRAY_TAG, MPI_COMM_WORLD);

        MPI_Type_free(&pixel);
        MPI_Type_free(&row_of_N_pixels);
    }

    MPI_Finalize();
}

void parallelRange(int globalstart, int globalstop, int irank, int nproc, int &localstart, int &localstop, int &localcount)
{
    int nrows = globalstop - globalstart + 1;
    int divisor = nrows / nproc;
    int remainder = nrows % nproc;
    int offset;
    if (irank < remainder)
        offset = irank;
    else
        offset = remainder;

    localstart = irank * divisor + globalstart + offset;
    localstop = localstart + divisor - 1;
    if (remainder > irank)
        localstop += 1;
    localcount = localstop - localstart + 1;
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
    for (int i = 0; i < 3; i++)
    {
        doAverageForSingleColourChannel(in, out, rowStart, rowStop, level, i);
        //std::cout << "Colour Channel:" << i << std::endl;
    }
}

void doAverageForSingleColourChannel(const cv::Mat &in, cv::Mat &out, int rowStart, int rowStop, int level, int channelIndex)
{
    //For Every Pixel
    for (int irow = rowStart; irow < rowStop; irow++)
    {
        for (int icol = 0; icol < out.cols; icol++)
        {
            //std::cout << "(" << irow << "," << icol << ") ";
            computeSinglePixelChannelAverage(in, out, irow, icol, rowStart, rowStop, level, channelIndex);
        }
        //std::cout << std::endl;
    }
}

void computeSinglePixelChannelAverage(const cv::Mat &in, cv::Mat &out, int irow, int icol, int rowStart, int rowStop, int level, int channelIndex)
{
    double sum = 0.0;
    int counter = 0;
    double avg = 0.0;

    //Top Left Coordinate
    int xStart = irow - level;
    int yStart = icol - level;

    //Bottom Right Coordinate
    int xEnd = irow + level + 1;
    int yEnd = icol + level + 1;

    //Check of coordinate  xLowLimit <= x < xHighLimit AND yLowLimit <= y < yHighLimit
    int xLowLimit = 0;
    int xHighLimit = out.rows;
    int yLowLimit = 0;
    int yHighLimit = out.cols;

    //std::cout << "Center: (" << irow << "," << icol << ")" << std::endl;
    for (int i = xStart; i < xEnd; i++)
    {
        for (int j = yStart; j < yEnd; j++)
        {
            //std::cout << "Kernel (" << i << "," << j << ")";
            if (isCoordinateInBounds(i, j, xLowLimit, xHighLimit, yLowLimit, yHighLimit))
            {
                sum += in.at<cv::Vec3b>(i, j).val[channelIndex];
                counter++;
                //std::cout << "Coordinate In Bounds (" << i << "," << j << ")" << std::endl;
            }
            else
            {
                //std::cout << "Coordinate Out of Bounds (" << i << "," << j << ")" << std::endl;
            }
        }
        //std::cout << std::endl;
    }

    //std::cout << "Sum:" << sum << std::endl;
    //std::cout << "Count:" << counter << std::endl;
    avg = (int)(sum / counter) % 256;
    //std::cout << "Avg: " << avg << std::endl;
    out.at<cv::Vec3b>(irow, icol).val[channelIndex] = (int)avg;
}

bool isCoordinateInBounds(int x, int y, int xLowLimit, int xHighLimit, int yLowLimit, int yHighLimit)
{
    return (xLowLimit <= x && x < xHighLimit && yLowLimit <= y && y < yHighLimit);
}