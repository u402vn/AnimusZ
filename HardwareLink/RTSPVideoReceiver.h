#ifndef RTSPVIDEORECEIVER_H
#define RTSPVIDEORECEIVER_H

#include <QObject>
#include <QThread>
#include <QImage>
#include <QTimer>
#include <QUrl>

class RTSPVideoReceiverWorker final : public QObject
{
    Q_OBJECT
   bool  _verticalMirror;
   QUrl _url;
public:
    explicit RTSPVideoReceiverWorker(QObject *parent, bool verticalMirror, const QUrl url);
    ~RTSPVideoReceiverWorker();
public slots:
    void startProcessing();
signals:
    void workerFrameAvailable(const QImage &frame);
};

class RTSPVideoReceiver : public QObject
{
    Q_OBJECT

    QThread * _thread;
    quint32 _videoConnectionId;
public:
    explicit RTSPVideoReceiver(QObject *parent, quint32 videoConnectionId, bool verticalMirror, const QUrl url);
    ~RTSPVideoReceiver();
private slots:
    void frameAvailableInternal(const QImage &frame);
signals:
    void frameAvailable(const QImage &frame, quint32 videoConnectionId);
};

#endif // RTSPVIDEORECEIVER_H

