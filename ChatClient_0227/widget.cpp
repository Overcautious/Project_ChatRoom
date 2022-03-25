#include "widget.h"
#include "ui_widget.h"

#include <QMessageBox>
#include <QListWidget>
#include <QTime>
#include <QTimer>

widget::widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget)
{
    ui->setupUi(this);

    chatClient = ChatClient::getTcpSocketClient();
    heartbeatTime = 5000;   // 心跳包发送间隔时间
    heartbeatCount = 0;     // 心跳包累计丢失次数
    heartbeatTimer = new QTimer(this);
    connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeatFunc()));
    // 一般默认是不启动，需要手动启动，start中的参数是间隔多长时间，定时器Timer自动触发timeout()，单位是ms




    chatClient->writeText(chatClient->getAccount(), "", READ );
    qDebug() << chatClient->getAccount();
    qDebug() << "chatClient->getAccount()";
    ui->label_userInfo->setText("用户" + QString::fromStdString(chatClient->getUsername()) + "(" + QString::fromStdString(std::to_string(chatClient->getAccount())) + ")");

    connect(ui->SendTextPushButton, &QPushButton::clicked, this, [=](){
        QString text = ui->InputTextEdit->toPlainText();

        if(text.trimmed().length() == 0){   // trimmed() 去除文字的前后空格
            QMessageBox::information(this, "发送提示", "不能发送空内容");
            return;
        }else{
            chatClient->writeText(chatClient->getAccount(), text.toStdString(), SEND );
            //ui->ContentListWidget->addItem(text);
        }

        ui->InputTextEdit->clear();

        restartTimer();

    });

    connect(chatClient, &ChatClient::onlineDisplayAble, this, [=](){
        ui->listWidget_online->clear();
        ui->listWidget_online->addItem(QString::fromStdString(chatClient->getContent()));
        chatClient->makeContentEmpty();
        restartTimer();
    });
    // notice框的显示
    connect(chatClient, &ChatClient::noticeDisplayAble, this, [=](){
       ui->listWidget_notice->addItem(QString::fromStdString(chatClient->getNotice()));
       restartTimer();
    });

    // 收到消息的时候，显示在聊天框中
    connect(chatClient, &ChatClient::textDisplayAble, this, [=](){
        ui->ContentListWidget->addItem(QString::fromStdString(chatClient->getContent()));
        chatClient->makeContentEmpty();
        restartTimer();
    });


    QTcpSocket* socket = chatClient->getTcpClient();//
    connect(socket, &QTcpSocket::readyRead, this, [=](){
        //QByteArray _recvBuf = socket->readAll();
        //qDebug() << "recvBuf: " << _recvBuf ;

        chatClient->receiveData(); //读取数据头
        restartTimer();

    });


    if(!heartbeatTimer->isActive())
        heartbeatTimer->start(heartbeatTime);


}

void widget::heartbeatFunc(){
    if(! chatClient->Heartbeat()){
        heartbeatCount++;
        if(heartbeatCount > 5){
            qDebug() << "客户端已掉线，正在重新连接服务器...";

            chatClient = ChatClient::reconnect(chatClient->getAccount(), chatClient->getPassword());
            if(chatClient != nullptr)
               qDebug() << "重连成功";
        }
    }else{
        heartbeatCount = 0;
    }
}

void widget::restartTimer(){
    if(heartbeatTimer->isActive()){
        heartbeatTimer->stop();
        heartbeatTimer->start(heartbeatTime);
    }
}

void widget::closeEvent(QCloseEvent *event){
    this->m_loginForm->close();
}
widget::~widget()
{
    delete ui;
}
