#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QIcon>
#include <QProcess>
#include <QTimer>

namespace {
constexpr int WINDOW_WIDTH = 650;
constexpr int ERROR_LOG_HEIGHT = 200;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ffmpeg(new QProcess(this))
{
    ui->setupUi(this);
    setWindowTitle("Conversor");
    setWindowIcon(QIcon(":/resources/icon.png"));

    ui->plainTextEdit->setVisible(false);
    ui->plainTextEdit->setMinimumHeight(150);

    ui->progressBar->setRange(0, 100);
    ui->progressBar->setTextVisible(true);
    ui->progressBar->setFormat("%p%");
    ui->progressBar->setValue(0);

    //Connect the signals with the slots
    connect(ffmpeg, &QProcess::readyReadStandardOutput,
            this, &MainWindow::onffmpegOutput);
    connect(ffmpeg, &QProcess::readyReadStandardError,
            this, &MainWindow::onffmpegError);
    connect(ffmpeg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onffmpegFinished);

    QTimer::singleShot(0, this, [this]() {
        adjustSize();
        m_compactHeight = height();
        setFixedSize(WINDOW_WIDTH, m_compactHeight);
    });
}

static QString ffmpegPath(const QString &binary)
{
    QString local = QCoreApplication::applicationDirPath() + "/" + binary;
#ifdef Q_OS_WIN
    local += ".exe";
#endif
    if (QFile::exists(local))
        return local;

    return binary;
}

double MainWindow::getDuration(const QString &file){
    QProcess probe;
    probe.start(ffmpegPath("ffprobe"), {
        "-v", "error",
        "-show_entries", "format=duration",
        "-of", "default=noprint_wrappers=1:nokey=1", file
    });
    probe.waitForFinished(5000);
    if (probe.exitCode() != 0){
        return 0.0;
    }

    QString out = QString::fromLocal8Bit(probe.readAllStandardOutput()).trimmed();
    bool ok = false;
    double duration =out.toDouble(&ok);
    return ok ? duration : 0.0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Select a file");
    if (!file.isEmpty()){
        ui->lineInput->setText(file);
    }
}

void MainWindow::on_convertButton_clicked()
{
    QString input = ui->lineInput->text().trimmed();
    QString format = ui->lineFormat->text().trimmed();

    if (input.isEmpty() || format.isEmpty()){
        QMessageBox::warning(this, "Missing data.", "Please choose a file and a format.");
        return;
    }

    if (ffmpeg->state() != QProcess::NotRunning){
        QMessageBox::warning(this, "Occupied.", "There's a conversion running.");
        return;
    }

    QFileInfo info(input);
    m_outputPath = info.absolutePath() + "/" + info.completeBaseName() + "." + format;
    m_errorBuffer.clear();
    m_stdoutBuffer.clear();

    m_duration = getDuration(input);

    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText("Converting " + input + "-> " + m_outputPath);
    ui->lineInput->setEnabled(false);
    ui->lineFormat->setEnabled(false);
    ui->selectButton->setEnabled(false);
    ui->convertButton->setEnabled(false);

    QStringList args;
    args << "-hide_banner" << "-nostats" << "-progress" << "pipe:1" << "-y" << "-i" << input << m_outputPath;

    ffmpeg->start(ffmpegPath("ffmpeg"), args);
    ui->plainTextEdit->setVisible(false);
    setFixedSize(width(), m_compactHeight);
}

void MainWindow::onffmpegOutput()
{
    m_stdoutBuffer += QString::fromLocal8Bit(ffmpeg->readAllStandardOutput());

    int pos;
    while ((pos = m_stdoutBuffer.indexOf('\n')) != -1) {
        QString line = m_stdoutBuffer.left(pos).trimmed();
        m_stdoutBuffer.remove(0, pos + 1);

        if (line.startsWith("out_time_us=")) {
            bool ok = false;
            qint64 us = line.mid(12).toLongLong(&ok);
            if (!ok || m_duration <= 0.0)
                continue;

            double seconds = us / 1000000.0;
            double percent = (seconds / m_duration) * 100.0;
            if (percent < 0) percent = 0;
            if (percent > 100) percent = 100;

            ui->progressBar->setValue(static_cast<int>(percent));
            ui->labelStatus->setText(
                QString("Converting... %1%").arg(static_cast<int>(percent))
            );
        }
    }
}

void MainWindow::onffmpegError()
{
    m_errorBuffer += QString::fromLocal8Bit(ffmpeg->readAllStandardError());
}

void MainWindow::onffmpegFinished(int exitCode, QProcess::ExitStatus status)
{
    m_errorBuffer += QString::fromLocal8Bit(ffmpeg->readAllStandardError());
    m_stdoutBuffer += QString::fromLocal8Bit(ffmpeg->readAllStandardOutput());

    ui->lineInput->setEnabled(true);
    ui->lineFormat->setEnabled(true);
    ui->selectButton->setEnabled(true);
    ui->convertButton->setEnabled(true);

    if (status == QProcess::NormalExit && exitCode == 0) {
        ui->progressBar->setValue(100);
        ui->labelStatus->setText("Conversion completed.");
        ui->plainTextEdit->setVisible(false);
    } else {
        ui->progressBar->setValue(0);
        ui->labelStatus->setText("Failed to convert.");
        ui->plainTextEdit->setVisible(true);
        setFixedSize(WINDOW_WIDTH, m_compactHeight + ERROR_LOG_HEIGHT);
        ui->plainTextEdit->setPlainText(
            "Error (code " + QString::number(exitCode) + ").\n\n"
            "--- ffmpeg details ---\n" + m_errorBuffer
        );
    }
}