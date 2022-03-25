#include "registerfrom.h"
#include "ui_registerfrom.h"

registerFrom::registerFrom(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registerFrom)
{
    ui->setupUi(this);
    chatClient = ChatClient::getTcpSocketClient();

    connect(ui->pushButton, &QPushButton::clicked, this, [=](){
        QString register_account = ui->lineEdit_account->text();
        QString register_username = ui->lineEdit_username->text();
        QString register_password = ui->lineEdit_password->text();

        QJsonObject registerInfo;
        registerInfo.insert("account", register_account);
        registerInfo.insert("password", register_password);
        registerInfo.insert("username", register_username);
        QJsonDocument doc;
        doc.setObject(registerInfo);
        string loginMsg = doc.toJson(QJsonDocument::Compact).toStdString();

        chatClient->writeText(register_account.toUtf8().toInt(), loginMsg, REGISTER);


        chatClient->getTcpClient()->waitForReadyRead(-1);
        qDebug() << "收到后台的注册确认信息";

        chatClient->receiveData();


        QString Msg = QString::fromStdString(chatClient->getLoginMsg());
        QJsonObject registerRespon = QJsonDocument::fromJson(Msg.toUtf8()).object();

        int statue = registerRespon["statue"].toInt();
        qDebug() << registerRespon["statue"];
        if(statue == REGISTER_SUCCESS){
            qDebug() << "注册成功";
            // 设置account，username信息
            chatClient->setUserInfo(register_account.toUtf8().toInt(), registerRespon["username"].toString().toStdString() );
            QMessageBox::information(this, "注册账号提示", "注册成功，您可以使用该账号");
            login->show();
            this->hide();

        }else if(statue == REGISTER_FAIL){
            QMessageBox::information(this, "注册账号提示", "账号或密码不符合要求，请重试");
        }



    });
}

registerFrom::~registerFrom()
{
    delete ui;
}
