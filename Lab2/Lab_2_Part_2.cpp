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
} subImage_descriptor;

// Function Prototypes
void parallelRange(int globalstart, int globalstop, int irank, int nproc, int &localstart, int &localstop, int &localcount);

int main(int argc, char **argv)
{

    int rank = 0;
    int nproc = 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Defining Image dimmensions
    int N = 960; // Number of pixels within a row of the image
    int M = 540; // Number of pixels within a column of the image
    int h = 40;  // Number of halo rows

    // Defining contiguous data type for cv::Vec3b
    MPI_Datatype pixel;
    MPI_Type_contiguous(3, MPI_BYTE, &pixel);
    MPI_Type_commit(&pixel);

    // Defining contiguous data type for a row of n pixel
    MPI_Datatype row_of_N_pixels;
    MPI_Type_contiguous(N, pixel, &row_of_N_pixels);
    MPI_Type_commit(&row_of_N_pixels);

    if (rank == 0)
    {
        // Creating a black image of size m x n (540 x 960)
        cv::Mat image(M, N, CV_8UC3, cv::Scalar(0, 0, 0));

        //------------------------------------
        // Testing with an actual image
        //------------------------------------
        //cv::Mat image;
        //image = cv::imread("./Space.png", 1); // Read the file

        // Show the image
        cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE); // Create a window for display.
        cv::imshow("Original Image", image);                    // Show our image inside it.
        cv::waitKey(1);                                         //wait 10 seconds before closing image (or a keypress to close)
        cv::imwrite("./P2_generated_Image.png", image);

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
        cv::Mat processed_image;
        processed_image = image.clone();

        for (int irow = descriptors[rank].subImageProcStart; irow < descriptors[rank].subImageProcStop; irow++)
        {
            for (int icol = 0; icol < processed_image.cols; icol++)
            {
                processed_image.at<cv::Vec3b>(irow, icol).val[rank] = 255;
            }
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

        //------------------------------------
        // Displaying the final image
        //------------------------------------
        cv::namedWindow("Final Image", cv::WINDOW_AUTOSIZE); // Create a window for display.
        cv::imshow("Final Image", processed_image);          // Show our image inside it.
        cv::waitKey(1);                                      //wait 10 seconds before closing image (or a keypress to close)
        cv::imwrite("./Final_Image.png", processed_image);
    }
    else
    {
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

        //------------------------------------
        // Receive Data
        //------------------------------------
        MPI_Status dataStatus;
        MPI_Recv(&subImage.at<cv::Vec3b>(0, 0), numRowsRecv, row_of_N_pixels, 0, PIXEL_ARRAY_TAG, MPI_COMM_WORLD, &dataStatus);
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
        for (int irow = subImage_procStartIndex; irow < subImage_procStopIndex; irow++)
        {
            for (int icol = 0; icol < subImage.cols; icol++)
            {
                subImage.at<cv::Vec3b>(irow, icol).val[rank] = 255;
            }
        }

        //------------------------------------
        // Displaying the processed sub-image
        //------------------------------------
        windowName = "Rank " + to_string(rank) + " processed sub-image";
        cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE); // Create a window for display.
        cv::imshow(windowName, subImage);                 // Show our image inside it.
        cv::waitKey(1);                                   //wait 10 seconds before closing image (or a keypress to close)

        //------------------------------------
        // Generating a unique file name to output the processed sub-image to
        //------------------------------------
        ostringstream procSubImageName;
        procSubImageName << "./Rank " << rank << " processed sub-image.png";
        cv::imwrite(procSubImageName.str(), subImage);

        //------------------------------------
        // Send the processed data back
        //------------------------------------
        MPI_Send(&subImage.at<cv::Vec3b>(subImage_procStartIndex, 0), subImage_procCount, row_of_N_pixels, 0, PIXEL_ARRAY_TAG, MPI_COMM_WORLD);
    }

    MPI_Type_free(&pixel);
    MPI_Type_free(&row_of_N_pixels);
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
