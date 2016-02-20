#include "frmtablas.h"
#include "ui_frmtablas.h"

frmTablas::frmTablas(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::frmTablas)
{
    ui->setupUi(this);
}

frmTablas::~frmTablas()
{
    delete ui;
}
