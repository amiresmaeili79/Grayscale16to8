#include <iostream>
#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

// Output operation
enum class IoOperation
{
    show,
    save,
    show_save
};

// Function to use 
enum class ConversionType
{
    norm,
    scale
};

cv::Mat normalizeImage(cv::Mat &img)
{
    cv::Mat out;
    double maxBrightness = *std::max_element(img.begin<short>(), img.end<short>());
    double scale = 255.0 / maxBrightness;
    img.convertTo(out, CV_8UC1, scale, 150);
    cv::normalize(out, out, 0, 255, cv::NORM_MINMAX);
    return out;
}

cv::Mat customScale(cv::Mat &img)
{
    cv::Mat out;
    double maxBrightness = *std::max_element(img.begin<short>(), img.end<short>());
    double scale = 256 / maxBrightness;
    img.convertTo(out, CV_8UC1, scale);
    return out;
}

void run(std::filesystem::path in, std::filesystem::path out, IoOperation op, ConversionType c)
{
    cv::Mat image;
    std::string fileStr;
    for (const std::filesystem::directory_entry &
             file : std::filesystem::directory_iterator(in))
    {

        if (!file.path().filename().string().ends_with("ab.pgm"))
            continue;

        image = cv::imread(file.path().string(), -1);
        if (image.empty())
            std::cout << "Image is empty " << file.path().string() << std::endl;

        switch (c)
        {
        case ConversionType::norm:
            image = normalizeImage(image);
            break;
        case ConversionType::scale:
            image = customScale(image);
            break;
        default:
            throw std::exception("Conversion type is not valid");
            break;
        }

        std::filesystem::path outputImg = out / file.path().filename();

        switch (op)
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
            throw std::exception("Type of IO operation is not supported");
            break;
        }
    }
}

int main(int argc, char **argv)
{

    std::filesystem::path inDirectory(argv[1]);
    std::filesystem::path outDirectory = inDirectory.parent_path() / "8bit";

    if (!std::filesystem::is_directory(inDirectory))
    {
        std::cout << "No such a directory" << std::endl;
        exit(-1);
    }
    if (!std::filesystem::is_directory(outDirectory))
        std::filesystem::create_directories(outDirectory);

    IoOperation ioOp;

    if (strcmp(argv[2], "show") == 0)
        ioOp = IoOperation::show;
    else if (strcmp(argv[2], "save") == 0)
        ioOp = IoOperation::save;
    else
        ioOp = IoOperation::show_save;

    ConversionType convType;

    if (strcmp(argv[3], "norm") == 0)
        convType = ConversionType::norm;
    else if (strcmp(argv[3], "scale") == 0)
        convType = ConversionType::scale;
    else
        convType = ConversionType::norm;

    try
    {
        run(inDirectory, outDirectory, ioOp, convType);
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}
