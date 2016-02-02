#include "camerarecordwidget.h"
#include "ui_camerarecordwidget.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QUrl>
#include <QImage>
#include <QDebug>

#include <Recorder>
#include <CameraGrabber>
#include <AudioGrabber>

CameraRecordWidget::CameraRecordWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraRecordWidget)
  , m_recorder(new Recorder(this))
  , m_cameraGrabber(new CameraGrabber(this))
  , m_audioGrabber(new AudioGrabber(this))
{
    ui->setupUi(this);

    ui->cbDevices->addItems(CameraGrabber::availableDeviceNames());

    //setup the camera grabber
    m_cameraGrabber->setLatency(65);
    m_recorder->setImageGrabber(m_cameraGrabber);

    //setup the audio grabber
    AudioFormat format;
    format.setChannelCount(2);
    format.setSampleRate(44100);
    format.setFormat(AudioFormat::SignedInt16);

    m_audioGrabber->setDeviceIndex(0);
    m_audioGrabber->setFormat(format);
    m_recorder->setAudioGrabber(m_audioGrabber);

    //setup the encoder
    m_recorder->encoder()->setVideoCodecSettings(videoCodecSettings());
    m_recorder->encoder()->setAudioCodecSettings(audioCodecSettings());
    m_recorder->encoder()->setVideoCodec(EncoderGlobal::H264);
    m_recorder->encoder()->setAudioCodec(EncoderGlobal::MP3);
    m_recorder->encoder()->setOutputPixelFormat(EncoderGlobal::YUV420P);

    initFrameLabel();

    connect(m_recorder->encoder(), SIGNAL(error(Encoder::Error)), this, SLOT(onEncoderError(Encoder::Error)));
    connect(m_cameraGrabber, SIGNAL(error(AbstractGrabber::Error)), this, SLOT(onGrabberError(AbstractGrabber::Error)));
    connect(m_audioGrabber, SIGNAL(error(AbstractGrabber::Error)), this, SLOT(onGrabberError(AbstractGrabber::Error)));

    ui->startButton->setEnabled(true);
}

CameraRecordWidget::~CameraRecordWidget()
{
    delete ui;
}

void CameraRecordWidget::on_startButton_clicked()
{
    if (ui->cbDevices->currentIndex() != -1) {
        if (pathToSaveMovie().isEmpty()) {
            setPathToSaveMovie(QFileDialog::getSaveFileName(this, tr("Escolha o local onde o "
                                                                    "vídeo será salvo"),
                                                            QDir::currentPath(),
                                                            tr("Videos (*.avi)")));
            m_recorder->encoder()->setFilePath(pathToSaveMovie());
        }
        m_cameraGrabber->setDeviceIndex(ui->cbDevices->currentIndex());
        m_recorder->encoder()->setVideoSize(CameraGrabber::maximumFrameSize(m_cameraGrabber->deviceIndex()));

        connect(m_cameraGrabber, SIGNAL(frameAvailable(QImage,int)), this, SLOT(showFrame(QImage)));
        m_recorder->start();

        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
    }
}

void CameraRecordWidget::on_stopButton_clicked()
{
    disconnect(m_cameraGrabber, SIGNAL(frameAvailable(QImage,int)), this, SLOT(showFrame(QImage)));
    m_recorder->stop();

    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);

    initFrameLabel();

    emit recordFinished(pathToSaveMovie());
}

void CameraRecordWidget::showFrame(const QImage &frame)
{
    ui->frameLabel->setPixmap(QPixmap::fromImage(frame));
}

void CameraRecordWidget::onEncoderError(Encoder::Error error)
{
    Q_UNUSED(error)

    qDebug()<<"Encoder's error number: "<<error;
}

void CameraRecordWidget::onGrabberError(AbstractGrabber::Error error)
{
    Q_UNUSED(error)

    qDebug()<<"Grabber's error number: "<<error;
}

void CameraRecordWidget::initFrameLabel()
{
    QImage blackImage(640, 480, QImage::Format_RGB888);
    blackImage.fill(Qt::black);

    ui->frameLabel->setPixmap(QPixmap::fromImage(blackImage));
    ui->frameLabel->repaint();
}

VideoCodecSettings CameraRecordWidget::videoCodecSettings() const
{
    //x264 loseless fast preset
    VideoCodecSettings settings;
    settings.setCoderType(EncoderGlobal::Vlc);
    settings.setFlags(EncoderGlobal::LoopFilter);
    settings.setMotionEstimationComparison(1);
    settings.setPartitions(EncoderGlobal::I4x4 | EncoderGlobal::P8x8);
    settings.setMotionEstimationMethod(EncoderGlobal::Hex);
    settings.setSubpixelMotionEstimationQuality(3);
    settings.setMotionEstimationRange(16);
    settings.setGopSize(250);
    settings.setMinimumKeyframeInterval(25);
    settings.setSceneChangeThreshold(40);
    settings.setIQuantFactor(0.71f);
    settings.setBFrameStrategy(1);
    settings.setQuantizerCurveCompressionFactor(0.6f);
    settings.setMinimumQuantizer(0);
    settings.setMaximumQuantizer(69);
    settings.setMaximumQuantizerDifference(4);
    settings.setDirectMvPredictionMode(EncoderGlobal::SpatialMode);
    settings.setFlags2(EncoderGlobal::FastPSkip);
    settings.setConstantQuantizerMode(0);
    settings.setPFramePredictionAnalysisMethod(EncoderGlobal::NoWpm);

    return settings;
}

AudioCodecSettings CameraRecordWidget::audioCodecSettings() const
{
    AudioCodecSettings settings;
    settings.setSampleRate(m_audioGrabber->format().sampleRate());
    settings.setChannelCount(m_audioGrabber->format().channelCount());
    settings.setSampleFormat(EncoderGlobal::Signed16);

    return settings;
}

QString CameraRecordWidget::pathToSaveMovie() const
{
    return m_pathToSaveMovie;
}

void CameraRecordWidget::setPathToSaveMovie(const QString &pathToSaveMovie)
{
    m_pathToSaveMovie = pathToSaveMovie;
}

