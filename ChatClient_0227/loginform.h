#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include "chatclient.h"

namespace Ui {
class loginForm;
}

class loginForm : public QWidget
{
    Q_OBJECT

public:
    explicit loginForm(QWidget *parent = nullptr);
    ~loginForm();
    ChatClient* chatClient;
    void closeEvent(QCloseEvent *event);
private:
    Ui::loginForm *ui;

};

#endif // LOGINFORM_H
