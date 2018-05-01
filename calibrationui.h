#ifndef CALICRATIONUI_H
#define CALICRATIONUI_H

#include <QMainWindow>

namespace Ui {
class CalicrationUI;
}

class CalicrationUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit CalicrationUI(QWidget *parent = 0);
    ~CalicrationUI();

private:
    Ui::CalicrationUI *ui;
};

#endif // CALICRATIONUI_H
