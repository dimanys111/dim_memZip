#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>

class MainWindow : public QObject
{
    Q_OBJECT

public:
     MainWindow();
    ~MainWindow();

private:
    QTimer timer;

private slots:
    void setTimer();
    void widgetDestroyed(QObject *ww);
};

#endif // MAINWINDOW_H
