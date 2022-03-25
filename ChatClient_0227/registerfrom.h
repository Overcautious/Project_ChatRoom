#ifndef REGISTERFROM_H
#define REGISTERFROM_H

#include <QWidget>
#include "chatclient.h"
#include "loginform.h"

namespace Ui {
class registerFrom;
}

class registerFrom : public QWidget
{
    Q_OBJECT

public:
    explicit registerFrom(QWidget *parent = nullptr);
    ~registerFrom();
    loginForm* login;
private:
    Ui::registerFrom *ui;
    ChatClient* chatClient;



};

#endif // REGISTERFROM_H
