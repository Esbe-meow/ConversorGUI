#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_selectButton_clicked();
    void on_convertButton_clicked();
    void onffmpegOutput();
    void onffmpegError();
    void onffmpegFinished(int exitCode, QProcess::ExitStatus status);

private:
    Ui::MainWindow *ui;
    QProcess *ffmpeg;
};
#endif // MAINWINDOW_H
