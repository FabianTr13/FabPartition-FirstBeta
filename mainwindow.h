#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnCrear_clicked();

    void on_btnNew_clicked();

    void on_btnResize_clicked();

    void on_btnNewFormat_clicked();

    void on_btnChangeName_clicked();

    void on_btnDeletePartition_clicked();

    void on_btnHelp_clicked();

    void on_pushButton_clicked();

    void on_btnAcc_clicked();

    void on_bttton_clicked();

    void on_btnren_clicked();

    void on_btncons_clicked();

    void on_cbRMParticiones_currentTextChanged(const QString &arg1);

    void on_cbRMParticiones_currentIndexChanged(int index);

    void on_cbRMParticiones_currentIndexChanged(const QString &arg1);

    void on_btnResize_2_clicked();

    void on_btnNewFormat_2_clicked();

    void on_btnNMChangeName_clicked();

    void on_btnDMDelete_clicked();

    void on_btnCreateDir_clicked();

    void on_btnInsertar_clicked();

    void on_lwFiles_activated(const QModelIndex &index);

    void on_btnAbrir_clicked();

    void on_lwFiles_currentRowChanged(int currentRow);

    void on_btnExtraer_clicked();

public slots:
    void particiones();
    void cambio();
    void cambioFormat();
    void refreshPartition();
    void refreshFiles();
    void loadDetail();
    void runFile();
    void deleteFiles();
    void extraerF();
    void crearCarpetaInPartition();
    void loadTableFiles();
    void loadTableDevices();
    void loadTableAddress();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
