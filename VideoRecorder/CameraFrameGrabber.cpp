#include "CameraFrameGrabber.h"
#include <QImage>

CameraFrameGrabber::CameraFrameGrabber(QObject *parent, quint32 videoConnectionId, bool verticalMirror) :  QVideoSink(parent)
{
    _verticalMirror = verticalMirror;
    _videoConnectionId = videoConnectionId;
}

bool CameraFrameGrabber::present(const QVideoFrame &frame)
{
    if (!frame.isValid())
        return false;

    QVideoFrame cloneFrame(frame);
    auto image = frame.toImage();
    QImage outImage = _verticalMirror ? image.mirrored(false, true) : image;

    cloneFrame.unmap();
    emit frameAvailable(outImage, _videoConnectionId);

    return true;
}
