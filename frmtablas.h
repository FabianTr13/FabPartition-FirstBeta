#ifndef FRMTABLAS_H
#define FRMTABLAS_H

#include <QMainWindow>

namespace Ui {
class frmTablas;
}

class frmTablas : public QMainWindow
{
    Q_OBJECT

public:
    explicit frmTablas(QWidget *parent = 0);
    ~frmTablas();

private:
    Ui::frmTablas *ui;
};

#endif // FRMTABLAS_H
