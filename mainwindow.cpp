#include "mainwindow.h"

#include <iostream>
#include <fstream>

#include <sys/sysinfo.h>
#include <QSound>
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QVector>

#include <sys/types.h>
#include <signal.h>

QLabel *label;
QWidget *w=0;

MainWindow::MainWindow()
{
    connect(&timer,&QTimer::timeout,this,&MainWindow::setTimer);
    timer.start(1000);
    setTimer();
}

unsigned long long get_mem_Available() {
    QString block;
    QFile file("/proc/meminfo");
    if (file.open(QIODevice::ReadOnly))
    {
        block=file.readAll();
        file.close();
    }

    QString block1;
    QTextStream readStream(&block);
    while (!readStream.atEnd()) {
        readStream >> block1;
        if(block1=="MemAvailable:")
        {
            readStream >> block1;
            unsigned long long mem=block1.toULongLong();
            return mem;
        }
    }
    return 0;
}

struct Pid
{
    Pid() {}
    int pid;
    QString name;
    unsigned long long size;
};

QList<Pid> pidSpis;

void FindFileAndDir( QString szDir,int& vl){
    vl++;
    QDir dir( szDir);
    foreach( QFileInfo fi, dir.entryInfoList()){
        QString szFileName = fi.absoluteFilePath();
        if( fi.isDir() && vl<2){
            if( fi.fileName()=="." || fi.fileName()=="..")
                continue;

            FindFileAndDir( szFileName,vl);
            vl--;
        }
        if( fi.isFile())
        {
            if(fi.fileName().contains("status")){
                QString block;
                QFile file(szFileName);
                if (file.open(QIODevice::ReadOnly)){
                    block=file.readAll();
                    file.close();
                }

                QString block1;
                QTextStream readStream(&block);
                if(block.contains("VmSize:"))
                {
                    pidSpis<<Pid();
                    while (!readStream.atEnd()) {
                        readStream >> block1;
                        if(block1=="Name:"){
                            readStream >> block1;
                            pidSpis.last().name=block1;
                        }
                        if(block1=="Pid:"){
                            readStream >> block1;
                            int mem=block1.toInt();
                            pidSpis.last().pid=mem;
                        }
                        if(block1=="RssAnon:"){
                            readStream >> block1;
                            unsigned long long mem=block1.toULongLong();
                            pidSpis.last().size=mem;
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::setTimer()
{
    unsigned long free=get_mem_Available();
    free=free/1024;
    qDebug() << free;
    if(free<1000)
    {
        if(timer.interval()==1000)
            timer.setInterval(200);
        if(!w)
        {
            w=new QWidget();
            w->setAttribute( Qt::WA_DeleteOnClose );
            w->setStyleSheet( "QWidget{ background-color : rgba( 160, 160, 160, 255); border-radius : 7px;  }" );
            connect( w, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)) );
            label = new QLabel(w);
            QHBoxLayout *layout = new QHBoxLayout();
            layout->addWidget(label);
            w->setLayout(layout);
            w->show();
        }
        label->setText("Осталось "+QString().number(free));
        QSound::play("1.wav");

        pidSpis.clear();
        int z=0;
        FindFileAndDir("/proc",z);

        unsigned long long xz=0;
        int pid=-1;
        for(int i=0;i<pidSpis.size();i++){
            if(pidSpis.at(i).size>xz && pidSpis.at(i).name!="cinnamon"){
                xz=pidSpis.at(i).size;
                pid=pidSpis.at(i).pid;
            }
        }
        kill(pid,SIGKILL);
    }
    else
    {
        if(timer.interval()==200)
            timer.setInterval(1000);
        if(w)
        {
            delete w;
            w=0;
        }
    }
}

void MainWindow::widgetDestroyed(QObject*ww)
{
    if(w==ww)
        w=0;
}

MainWindow::~MainWindow()
{

}
