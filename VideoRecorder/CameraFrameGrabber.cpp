#include "CameraFrameGrabber.h"
#include <QImage>


// https://stackoverflow.com/questions/70605931/qt6-using-qvideosink-with-qcamera-to-process-every-frame

CameraFrameGrabber::CameraFrameGrabber(QObject *parent, quint32 videoConnectionId, bool verticalMirror) :  QVideoSink(parent)
{
    _verticalMirror = verticalMirror;
    _videoConnectionId = videoConnectionId;
    connect(this, &CameraFrameGrabber::videoFrameChanged, this, &CameraFrameGrabber::processFrameInternal);
    //this->setSource();
}

bool CameraFrameGrabber::present(const QVideoFrame &frame)
{

    /*
    if (!frame.isValid())
        return false;

    QVideoFrame cloneFrame(frame);
    auto image = frame.toImage();
    QImage outImage = _verticalMirror ? image.mirrored(false, true) : image;

    cloneFrame.unmap();



    emit frameAvailable(outImage, _videoConnectionId);
*/
    return true;
}

void CameraFrameGrabber::processFrameInternal(const QVideoFrame &frame)
{
    auto outImage = frame.toImage();
    emit frameAvailable(outImage, _videoConnectionId);
}
