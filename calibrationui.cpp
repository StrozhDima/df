#include "calicrationui.h"
#include "ui_calicrationui.h"

CalicrationUI::CalicrationUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CalicrationUI)
{
    ui->setupUi(this);
}

CalicrationUI::~CalicrationUI()
{
    delete ui;
}
