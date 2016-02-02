#ifndef CAMERARECORDWIDGET_H
#define CAMERARECORDWIDGET_H

#include <QWidget>

#include <VideoCodecSettings>
#include <AudioCodecSettings>
#include <Encoder>
#include <AbstractGrabber>

namespace Ui {
class CameraRecordWidget;
}

class Recorder;
class CameraGrabber;
class AudioGrabber;

class CameraRecordWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CameraRecordWidget(QWidget *parent = 0);
    ~CameraRecordWidget();

    QString pathToSaveMovie() const;
    void setPathToSaveMovie(const QString &pathToSaveMovie);

private slots:
    void on_startStopButton_clicked();

    void showFrame(const QImage &frame);

    void onEncoderError(Encoder::Error error);
    void onGrabberError(AbstractGrabber::Error error);

private:
    void initFrameLabel();

    VideoCodecSettings videoCodecSettings() const;
    AudioCodecSettings audioCodecSettings() const;

    Ui::CameraRecordWidget *ui;
    Recorder *m_recorder;
    CameraGrabber *m_cameraGrabber;
    AudioGrabber *m_audioGrabber;

    QString m_pathToSaveMovie;

signals:
    void recordFinished(QString a_path);
};

#endif // CAMERARECORDWIDGET_H
