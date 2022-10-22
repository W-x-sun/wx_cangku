#include "ServerOperate.h"

#include <QApplication>
#pragma execution_character_set("utf-8")

//构造函数对帧成员初始化
ServerOperate::ServerOperate(QObject *parent)
    : QObject(parent)
{
    initOperate();//1.监听 2.取出通信套接字 3.有数据来触发read信号读取数据  4，完成数据读取释放空间
    //帧头+长度+类型
    //帧长4字节定值
     frameHead[0]={0x0F};
     frameHead[1]={(char)0xF0};
     frameHead[2]={0x00};
     frameHead[3]={(char)0xFF};
     //后边发送数据的时候根据需要重新赋值
     //长度加类型
     frameHead[4]={0x00};
     frameHead[5]={0x00};
     frameHead[6]={0x00};
    //帧尾
     frameTail[0]={0x0D};
     frameTail[1]={0x0A};
}

ServerOperate::~ServerOperate()
{
    dislisten();
}

QString ServerOperate::getFilePath() const
{
   // QMutexLocker locker(&dataMutex);
    return filePath;
}
//（slot）
void ServerOperate::setFilePath(const QString &path)
{
   // QMutexLocker locker(&dataMutex);
    filePath=path;
}
//判断是否在监听
bool ServerOperate::isListening() const
{
  //  QMutexLocker locker(&dataMutex);
    //这个锁没啥用，毕竟设置不是我控制的，待修改
    return server->isListening();
}
//（slot）监听，同时通过发送信号的形式使得ui界面上一些文本框和按钮使能关闭
void ServerOperate::listen(const QString &address, quint16 port)
{
    if(server->isListening())
        doDislisten();
    //启动监听
    const bool result=server->listen(QHostAddress(address),port);//监听，监听成功返回ture
    emit listenStateChanged(result);
    emit logMessage(result?"服务启动成功":"服务启动失败");
}

//（slot）更改监听状态
void ServerOperate::dislisten()
{
    doDislisten();//关闭监听，释放资源
    emit listenStateChanged(false);//监听状态改变为不监听
    emit logMessage("服务关闭");
}
//（slot）
void ServerOperate::cancelFileTransfer()
{
    //关闭文件
    doCancel();
    //发送停止传输指令
    sendData(0x04,QByteArray());
}

void ServerOperate::initOperate()
{
    server=new QTcpServer(this);
    //1.监听到新的客户端连接请求
    connect(server,&QTcpServer::newConnection,this,[this]{
        //2.如果有新的连接就取出通信套接字
        while(server->hasPendingConnections())
        {
            //nextPendingConnection返回下一个挂起的连接作为已连接的QTcpSocket对象
            QTcpSocket *new_socket=server->nextPendingConnection();
            emit logMessage(QString("新的客户端连接 [%1:%2]")//发射这个日志信号，对应的槽函数会把消息显示在文本框里
                            .arg(new_socket->peerAddress().toString())
                            .arg(new_socket->peerPort()));

            //demo只支持一个连接，多余的释放掉
            if(socket){
                new_socket->abort();
                new_socket->deleteLater();
                emit logMessage("目前已有客户端连接，新连接已释放");
                continue;
            }else{
                socket=new_socket;
            }

            //3.收到数据，触发readyRead，读取数据
            connect(socket,&QTcpSocket::readyRead,[this]{
                //没有可读的数据就返回
                if(socket->bytesAvailable()<=0)
                    return;
                //读取数据
                operateReceiveData(socket->readAll());
            });

            //4.读完数据就连接断开，销毁socket对象，释放空间
            connect(socket,&QTcpSocket::disconnected,[this]{
                emit logMessage(QString("客户端连接已断开 [%1:%2]")
                                .arg(socket->peerAddress().toString())
                                .arg(socket->peerPort()));
                socket->deleteLater();
                socket=nullptr;
            });
        }
    });
}
//关闭监听，释放资源
void ServerOperate::doDislisten()
{
    //关闭服务，断开socket连接，释放资源
    server->close();
    if(socket){
        socket->abort();
    }
    if(file){
        file->close();
    }
}

void ServerOperate::doCloseFile()
{
    if(file){
        file->close();
        delete file;
        file=nullptr;
    }
}
//关闭文件
void ServerOperate::doCancel()
{
    if(file){
        //关闭文件
        doCloseFile();
    }
}

bool ServerOperate::readyReceiveFile(qint64 size, const QString &filename)
{
    //重置状态
    fileSize=size;
    receiveSize=0;
    if(file){
        doCloseFile();
    }
    //创建qfile用于写文件
    file=new QFile(this);
    QString file_path=getFilePath();
    if(file_path.isEmpty())
        file_path=QApplication::applicationDirPath();
    file->setFileName(file_path+"/"+filename);
    //Truncate清掉原本内容
    if(!file->open(QIODevice::WriteOnly)){
        emit logMessage("创建文件失败，无法进行接收"+file->fileName());
        return false;
    }
    emit logMessage("创建文件成功，准备接收"+file->fileName());
    return true;
}

void ServerOperate::onReceiveFile(const char *data, qint64 size)
{
    if(!file||!file->isOpen()){
        doCancel();
        //发送停止传输指令
        sendData(0x04,QByteArray());
        emit logMessage("文件操作失败，取消接收");

        return;
    }
    if(size>0){
        const qint64 write_size = file->write(data,size);
        //感觉这个waitForBytesWritten没啥用啊
        if(write_size!=size && !file->waitForBytesWritten(3000)){
            doCancel();
            //发送停止传输指令
            sendData(0x04,QByteArray());
            emit logMessage("文件写入超时，取消接收");
            return;
        }
    }
    receiveSize+=size;
    //避免除零
    if(fileSize>0){
        emit progressChanged(receiveSize*100/fileSize);
    }
    if(receiveSize>=fileSize){
        doCancel();
        emit logMessage("文件接收完成");
        emit progressChanged(100);
        return;
    }
}
//发送数据
void ServerOperate::sendData(char type, const QByteArray &data)
{
    //传输协议
    //帧结构：帧头4+帧长2+帧类型1+帧数据N+帧尾2（没有校验段，懒得写）
    //帧头：4字节定值 0x0F 0xF0 0x00 0xFF
    //帧长：2字节数据段长度值 arr[4]*0x100+arr[5] 前面为高位后面为低位
    //帧类型：1字节
    //- 0x01 准备发送文件，后跟四字节文件长度和N字节utf8文件名，长度计算同帧长一样前面为高位后面为低位
    //- 0x02 文件数据
    //- 0x03 发送结束
    //- 0x04 取消发送
    //（服务端收到0x01 0x03开始和结束发送两个命令要进行应答，回同样的命令码无数据段）
    //帧尾：2字节定值 0x0D 0x0A
    if(!socket->isValid())
        return;
    frameHead[6]=type;//传进来的帧类型（0x01或其他）
    const quint64 data_size=data.count();
    frameHead[5]=data_size%0x100;//帧长（低位）
    frameHead[4]=data_size/0x100;//帧长（高位）

    //发送头+数据+尾
    socket->write(frameHead,7);
    socket->write(data);
    socket->write(frameTail,2);
}
//读取数据
void ServerOperate::operateReceiveData(const QByteArray &data)
{
    static QByteArray frame_head=QByteArray(frameHead,4);//4个字节的帧头定值
    //这里只是简单的处理，所以用了QByteArray容器做缓存
    dataTemp+=data;//数据缓存

    //处理数据
    while(true){
        //保证以帧头为起始，且后边有帧长（2个字节）数据，说明有效
        while(!dataTemp.startsWith(frame_head)&&dataTemp.size()>4){
            dataTemp.remove(0,1); //左边移除一字节
        }
        //小于最小帧长（帧头加帧尾<9,说明里边没有数据内容）
        if(dataTemp.size()<7+2)
            return;
        //取数据段长度，这里没有判断长度有效性
        const int data_size=uchar(dataTemp[4])*0x100+uchar(dataTemp[5]);//帧长
        if(dataTemp.size()<7+2+data_size)
            return;
        //帧尾不一致，无效数据--这里懒得写校验位了
        if(memcmp(dataTemp.constData()+7+data_size,frameTail,2)!=0){
            dataTemp.clear();
            return;
        }
        //取数据类型
        const char type=dataTemp[6];
        switch(type)
        {
        case 0x01: //开始发送数据
        {
            //这里也可以做个弹框，询问是否接收数据
            doCloseFile();
            if(data_size<5){
                //无效帧，没有长度和文件名
                break;
            }
            //解析长度和文件名
            qint64 file_size=0;
            //file_size+=uchar(dataTemp[7]);
            //file_size<<=8;
            //file_size+=uchar(dataTemp[8]);
            //file_size<<=8;
            //file_size+=uchar(dataTemp[9]);
            //file_size<<=8;
            //file_size+=uchar(dataTemp[10]);
            file_size=uchar(dataTemp[7])*0x1000000+
                    uchar(dataTemp[8])*0x10000+
                    uchar(dataTemp[9])*0x100+
                    uchar(dataTemp[10]);
            QString file_name=QString::fromUtf8(dataTemp.constData()+7+4,data_size-4);
            if(readyReceiveFile(file_size,file_name)){
                emit logMessage("准备好接收客户端文件");
                //应答
                sendData(0x01,QByteArray());
            }else
            {
                emit logMessage("准备接收客户端文件失败");
            }
        }
            break;
        case 0x02: //数据传输
            onReceiveFile(dataTemp.constData()+7,data_size);
            break;
        case 0x03: //发送数据完成
        {
            doCloseFile();
            emit logMessage("客户端文件发送完毕");
            //应答
            sendData(0x03,QByteArray(1,(char)((receiveSize==fileSize)?0x01:0x00)));
        }
            break;
        case 0x04: //客户端取消发送
            doCancel();
            emit logMessage("客户端取消发送，发送终止");
            break;
        default: break;
        }
        //移除处理完的字节
        dataTemp.remove(0,7+2+data_size);
    }
}
