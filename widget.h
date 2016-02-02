#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <VideoCodecSettings>
#include <AudioCodecSettings>
#include <Encoder>
#include <AbstractGrabber>

namespace Ui {
class Widget;
}

class Recorder;
class CameraGrabber;
class AudioGrabber;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    QString pathToSaveMovie() const;
    void setPathToSaveMovie(const QString &pathToSaveMovie);

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();

    void showFrame(const QImage &frame);

    void onEncoderError(Encoder::Error error);
    void onGrabberError(AbstractGrabber::Error error);

private:
    void initFrameLabel();

    VideoCodecSettings videoCodecSettings() const;
    AudioCodecSettings audioCodecSettings() const;

    Ui::Widget *ui;
    Recorder *m_recorder;
    CameraGrabber *m_cameraGrabber;
    AudioGrabber *m_audioGrabber;

    QString m_pathToSaveMovie;

signals:
    void recordFinished(QString a_path);
};

#endif // WIDGET_H
