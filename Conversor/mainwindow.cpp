#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ffmpeg(new QProcess(this))
{
    ui->setupUi(this);
    setWindowTitle("Conversor");
    setWindowIcon(QIcon(":/resources/icon.png"));
    QIcon testIcon(":/resources/icon.png");
    qDebug() << "DEBUG: icono isNull =" << testIcon.isNull();
    qDebug() << "DEBUG: tamaños disponibles =" << testIcon.availableSizes();
    setWindowIcon(testIcon);

    //Connect the signals with the slots
    connect(ffmpeg, &QProcess::readyReadStandardOutput,
            this, &MainWindow::onffmpegOutput);
    connect(ffmpeg, &QProcess::readyReadStandardError,
            this, &MainWindow::onffmpegError);
    connect(ffmpeg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onffmpegFinished);
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
    QString output = info.absolutePath() + "/" + info.completeBaseName() + "." + format;

    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText("Converting " + input + "->" + output);
    ui->convertButton->setEnabled(false);

    QStringList args;
    args << "-y" << "-i" << input << output;

    ffmpeg->start("ffmpeg", args);

}

void MainWindow::onffmpegOutput()
{
    ui->plainTextEdit->insertPlainText(QString::fromLocal8Bit(ffmpeg->readAllStandardOutput()));
}

void MainWindow::onffmpegError()
{
    ui->plainTextEdit->insertPlainText(QString::fromLocal8Bit(ffmpeg->readAllStandardError()));
}

void MainWindow::onffmpegFinished(int exitCode, QProcess::ExitStatus status)
{
    ui->convertButton->setEnabled(true);

    if (status == QProcess::NormalExit && exitCode == 0)
        ui->plainTextEdit->appendPlainText("\n Conversion completed.");
    else
        ui->plainTextEdit->appendPlainText("\n ffmpeg finalized with an error (code " + QString::number(exitCode) + ").");
}