#include "loginform.h"
#include "ui_loginform.h"
#include "widget.h"
#include "registerfrom.h"
loginForm::loginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loginForm)
{
    ui->setupUi(this);

    // 登录按钮
    connect(ui->pushButton_login, &QPushButton::clicked, this, [=](){
       qDebug() << "正在连接服务器...";
       chatClient = ChatClient::getTcpSocketClient();
       if(chatClient == nullptr){
           qDebug() << "连接服务器超时！";
           QMessageBox::information(this, "登录提示", "连接服务器超时");
           return;
       }
       QString account = ui->lineEdit_account->text();
       QString password = ui->lineEdit_password->text();
       qDebug() << "account:" << account;
       qDebug() << "password:" << password;

       QJsonObject loginInfo;
       loginInfo.insert("account", account);
       loginInfo.insert("password", password);
       QJsonDocument doc;
       doc.setObject(loginInfo);
       string loginMsg = doc.toJson(QJsonDocument::Compact).toStdString();

       chatClient->writeText(account.toUtf8().toInt(), loginMsg, LOGIN);

       chatClient->getTcpClient()->waitForReadyRead(-1);
       chatClient->receiveData();
       qDebug() << "收到后台的登录确认信息";


       QString Msg = QString::fromStdString(chatClient->getLoginMsg());
       QJsonObject loginRespon = QJsonDocument::fromJson(Msg.toUtf8()).object();

       int statue = loginRespon["statue"].toInt();
       qDebug() << loginRespon["statue"];
       if(statue == LOGIN_SUCCESS){
           qDebug() << "登录成功";
           // 设置account，username信息
           chatClient->setUserInfo(account.toUtf8().toInt(), loginRespon["username"].toString().toStdString(), password.toStdString() );
           widget* w = new widget();
           w->m_loginForm = this;
           w->show();
           this->hide();

       }else if(statue == LOGIN_FAIL){
           QMessageBox::information(this, "登录提示", "账号或密码错误，请重试");
       }else{
           QMessageBox::information(this, "登录提示", "该账号已登录，不可重复登录");
       }
    });


    //注册按钮
    connect(ui->pushButton_register, &QPushButton::clicked, this, [=](){
       qDebug() << "正在连接服务器...";
       chatClient = ChatClient::getTcpSocketClient();
       if(chatClient == nullptr){
           qDebug() << "连接服务器超时！";
           QMessageBox::information(this, "登录提示", "连接服务器超时");
           return;
       }
       registerFrom* regist = new registerFrom();
       regist->login =this;
       regist->show();
       this->hide();
    });
}

void loginForm::closeEvent(QCloseEvent *event){
    qDebug() << "关闭窗口";
    int account = chatClient->getAccount();
    chatClient->writeText(account, "", LOGOUT);
    chatClient->getTcpClient()->disconnectFromHost();
}

loginForm::~loginForm()
{
    delete ui;
}
