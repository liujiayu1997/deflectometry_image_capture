#include "ImageShow.h"

int ImageShow::ui_counter = 0;
int ImageShow::save_success_num = 0;


ImageShow::ImageShow(std::string ID, ApiController& root_controler,QWidget *parent)
	: QWidget(parent),m_ApiController(root_controler),m_ID(ID)
{
	ui.setupUi(this);
	ui_counter++;
	setWindowTitle(QString("Current Camera: ") + QString::fromStdString(m_ID));
}

ImageShow::~ImageShow()
{
}

void ImageShow::show_picture()
{
    VmbErrorType err = m_ApiController.StartContinuousImageAcquisition(m_ID);
    // Set up Qt image
    if (VmbErrorSuccess == err)
    {
        m_image = QImage(m_ApiController.GetWidth(),
            m_ApiController.GetHeight(),
            QImage::Format_Grayscale8);
        QObject::connect(m_ApiController.GetFrameObserver(), SIGNAL(FrameReceivedSignal(int)), this, SLOT(OnFrameReady(int)));
    }
}

VmbErrorType ImageShow::CopyToImage(VmbUchar_t* pInBuffer, VmbPixelFormat_t ePixelFormat, QImage& pOutImage, const float* Matrix)
{
    const int           nHeight = m_ApiController.GetHeight();
    const int           nWidth = m_ApiController.GetWidth();

    VmbImage            SourceImage, DestImage;
    VmbError_t          Result;
    SourceImage.Size = sizeof(SourceImage);
    DestImage.Size = sizeof(DestImage);

    Result = VmbSetImageInfoFromPixelFormat(ePixelFormat, nWidth, nHeight, &SourceImage);
    if (VmbErrorSuccess != Result)
    {
        QMessageBox::warning(this, tr("Copy Image"), tr("Could not set source image info"));
        return static_cast<VmbErrorType>(Result);
    }
    QString             OutputFormat;
    const int           bytes_per_line = pOutImage.bytesPerLine();
    switch (pOutImage.format())
    {
    default:
        QMessageBox::warning(this, tr("Copy Image"), tr("unknown output format"));
        return VmbErrorBadParameter;
    case QImage::Format_RGB888:
        if (nWidth * 3 != bytes_per_line)
        {
            QMessageBox::warning(this, tr("Copy Image"), tr("image transform does not support stride"));
            return VmbErrorWrongType;
        }
        OutputFormat = "RGB24";
        break;
    case QImage::Format_Grayscale8:
        if (nWidth != bytes_per_line)
        {
            QMessageBox::warning(this, tr("Copy Image"), tr("image transform does not support stride"));
            return VmbErrorWrongType;
        }
        OutputFormat = "Mono8";
        break;
    }

    Result = VmbSetImageInfoFromString(OutputFormat.toStdString().c_str(), OutputFormat.length(), nWidth, nHeight, &DestImage);
    if (VmbErrorSuccess != Result)
    {
        QMessageBox::warning(this, tr("Copy Image"), tr("could not set output image info"));
        return static_cast<VmbErrorType>(Result);
    }
    SourceImage.Data = pInBuffer;
    DestImage.Data = pOutImage.bits();
    // do color processing?
    if (NULL != Matrix)
    {
        VmbTransformInfo TransformParameter;
        Result = VmbSetColorCorrectionMatrix3x3(Matrix, &TransformParameter);
        if (VmbErrorSuccess == Result)
        {
            Result = VmbImageTransform(&SourceImage, &DestImage, &TransformParameter, 1);
        }
        else
        {
            QMessageBox::warning(this, tr("Copy Image"), tr("could not set matrix t o transform info"));
            return static_cast<VmbErrorType>(Result);
        }
    }
    else
    {
        Result = VmbImageTransform(&SourceImage, &DestImage, NULL, 0);
    }
    if (VmbErrorSuccess != Result)
    {
        QMessageBox::warning(this, tr("Copy Image"), tr("could not transform image"));
        return static_cast<VmbErrorType>(Result);
    }
    return static_cast<VmbErrorType>(Result);
}

void ImageShow::close_camera()
{
    VmbErrorType err = m_ApiController.StopContinuousImageAcquisition();
    // Clear all frames that we have not picked up so far
    m_ApiController.ClearFrameQueue();
    m_image = QImage();
}

void ImageShow::on_set_exposition()
{
}

void ImageShow::on_set_white_balance()
{
}

void ImageShow::on_set_gain()
{
}

void ImageShow::OnFrameReady(int status)
{
    // Pick up frame
    FramePtr pFrame = m_ApiController.GetFrame();
    if (SP_ISNULL(pFrame))
    {
        QMessageBox::warning(this, tr("Get frame warnning"), tr("frame pointer is NULL, late frame ready message"));
        return;
    }
    // See if it is not corrupt
    if (VmbFrameStatusComplete == status)
    {
        VmbUchar_t* pBuffer;
        VmbErrorType err = SP_ACCESS(pFrame)->GetImage(pBuffer);
        if (VmbErrorSuccess == err)
        {
            VmbUint32_t nSize;
            err = SP_ACCESS(pFrame)->GetImageSize(nSize);
            if (VmbErrorSuccess == err)
            {
                VmbPixelFormatType ePixelFormat = m_ApiController.GetPixelFormat();
                if (!m_image.isNull())
                {
                    // 当想使用RGB图片时选择这句
                    //CopyToImage(pBuffer, ePixelFormat, m_image);
                    // Display it
                    m_image = QImage(pBuffer, m_ApiController.GetWidth(),
                        m_ApiController.GetHeight(),
                        QImage::Format_Grayscale8);
                    ui.graphicsView->setImage(m_image);
                }
            }
        }

        // And queue it to continue streaming
        m_ApiController.QueueFrame(pFrame);
    }
}


