#include <iostream>
#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

enum class IoOperation
{
    show,
    save,
    show_save
};

cv::Mat normalizeImage(cv::Mat &img)
{
    cv::Mat out;
    img.convertTo(out, CV_8UC1);
    cv::normalize(out, out, 0, 255, cv::NORM_MINMAX);
    return out;
}

int main(int argc, char **argv)
{

    std::filesystem::path inDirectory(argv[1]);
    std::filesystem::path outDirectory(argv[2]);

    if (!std::filesystem::is_directory(inDirectory))
    {
        std::cout << "No such a directory" << std::endl;
        exit(-1);
    }
    if (!std::filesystem::is_directory(outDirectory))
        std::filesystem::create_directories(outDirectory);

    IoOperation ioOp;

    if (strcmp(argv[3], "show") == 0)
        ioOp = IoOperation::show;
    else if (strcmp(argv[3], "save") == 0)
        ioOp = IoOperation::save;
    else
        ioOp = IoOperation::show_save;

    cv::Mat image;
    std::string fileStr;
    for (const std::filesystem::directory_entry &
             file : std::filesystem::directory_iterator(inDirectory))
    {
        if (!file.path().filename().string().ends_with("ab.pgm"))
            continue;

        image = cv::imread(file.path().string(), -1);
        if (image.empty())
            std::cout << "Image is empty " << file.path().string() << std::endl;

        image = normalizeImage(image);

        std::filesystem::path outputImg = outDirectory / file.path().filename();
        switch (ioOp)
        {
        case IoOperation::save:
            cv::imwrite(outputImg.string(), image);
            break;
        case IoOperation::show:
            cv::imshow(file.path().filename().string(), image);
            cv::waitKey();
            cv::destroyAllWindows();
            break;
        case IoOperation::show_save:
            cv::imwrite(outputImg.string(), image);
            cv::imshow(file.path().filename().string(), image);
            cv::waitKey();
            cv::destroyAllWindows();
            break;
        default:
            break;
        }
    }
}
