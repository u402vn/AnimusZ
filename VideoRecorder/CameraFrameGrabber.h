#ifndef CAMERAFRAMEGRABBER_H
#define CAMERAFRAMEGRABBER_H

#include <QVideoSink>
#include <QList>

class CameraFrameGrabber final : public QVideoSink
{
    Q_OBJECT
    bool _verticalMirror;
    quint32 _videoConnectionId;
public:
    explicit CameraFrameGrabber(QObject *parent, quint32 videoConnectionId, bool verticalMirror);

    bool present(const QVideoFrame &frame);
signals:
    void frameAvailable(const QImage &frame, quint32 videoConnectionId);
};

#endif // CAMERAFRAMEGRABBER_H
