#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "chatclient.h"
#include "readthread.h"
#include "loginform.h"
namespace Ui {
class widget;
}

class widget : public QWidget
{
    Q_OBJECT

public:
    explicit widget(QWidget *parent = nullptr);
    ~widget();
    loginForm* m_loginForm;
    void closeEvent(QCloseEvent *event);
    void restartTimer();


private slots:
    void heartbeatFunc();
private:
    Ui::widget *ui;
    ChatClient* chatClient = nullptr;

    QTimer *heartbeatTimer;
    int heartbeatTime ;   // 心跳包发送间隔时间
    int heartbeatCount ;     // 心跳包累计丢失次数
};

#endif // WIDGET_H
