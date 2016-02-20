#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QMessageBox>
#include <QInputDialog>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlerror.h>
#include <QtSql/QSql>
#include <QSqlQuery>
#include <QFileDialog>
#include <QList>
#include <QDate>
#include <QDateTime>
#include <QFileInfo>
#include <QDebug>
#include <QSqlTableModel>
#define lengthArrays(x) (sizeof(x)/sizeof(x[0]))

//funsiones de arranque--------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->frameCrear->setVisible(false);
    ui->frameResize->setVisible(false);
    ui->frameNewFormat->setVisible(false);
    ui->frameChangeName->setVisible(false);
    ui->frameDelete->setVisible(false);
    ui->frameAyuda->setVisible(false);
    ui->cbFormato->addItems(QStringList()<<"Fat16"<<"Fat32");
    ui->cdFMNewFormat->addItems(QStringList()<<"Fat16"<<"Fat32");
    particiones();

}

MainWindow::~MainWindow()
{
    delete ui;
}
//fin funsiones arranque y destruccion-------------------------------------------------------------------------------------------

//Variables globales------------------------------------------------------------------------------------------------
bool cambioMenus[6];
//fin de variables globales-----------------------------------------------------------------------------------------

//funciones--------------------------------------------------------------------------------------------------------

bool verifyName(QString name) {
    char namec[name.length()];
    strcpy(namec, name.toStdString().c_str());
    for(int i = 0; i < name.length(); i++){
        if(namec[i]=='/'||namec[i]=='.'||namec[i]==' '){
            return false;
        }
    }
    return true;
}
bool verifySize(QString type, long long size){

    long long limit = 0;
    if(type == "Fat16"){
        limit = 2*1024;
    }else if(type == "Fat32"){
            limit = 8192*1024;

    }
    if(size == 0){
        return false;
    }
    if(size>limit){
        return false;
    }
    return true;
}

long long limiteArchivo(QString type){
    if(type=="Fat16"){
        return 2048;
    }
    if(type=="Fat32"){
        return 496;
    }
    if(type=="Ext2"||type=="Ext3"){
        return 2000;
    }
}
void querysInsert(QString cadena){
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    if (db.open()){
        QSqlQuery query;
       if(query.exec(cadena)){
       }else {
        }
       db.close();
    }else{
        messageBox.critical(0,"Error","Imposible crear coneccion");
    }
}

bool consultaExiste(QString name){

    QString cadena = "SELECT `Devices`.`Name` FROM `MasteTable`.`Devices` WHERE `Devices`.`Name`= '" +name+"';";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    QString lectura;

    if (db.open()){
        if(query.exec(cadena)){
            while(query.next()){
                lectura += query.value(0).toString();
            }
            if(lectura==""){
                return true;
            }else {
                messageBox.critical(0,"Error","Imposible ya existe un disco con mismo nombre y formato");
                return false;
            }
        }else{
            messageBox.critical(0,"Error","Error en el query");
            return false;
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
        return false;
    }
}

int idDevices(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    int lectura;

    if (db.open()){
        if(query.exec("SELECT max(`idDevices`) FROM `MasteTable`.`Devices`;")){
            while(query.next()){
                lectura = query.value(0).toInt();
            }

        }else{
            messageBox.critical(0,"Error","Error query");
            return false;
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
        return false;
    }
    return lectura+1;
}

QString sizePartition(QString name){
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    QString lectura;

    if (db.open()){
        if(query.exec("SELECT `Size` FROM `MasteTable`.`Devices` WHERE `Devices`.`Name`='"+name+"';")){
            while(query.next()){
                lectura = query.value(0).toString();
            }

        }else{
            messageBox.critical(0,"Error","Error query tamano");
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
    return lectura;
}

QString typePartition(QString name){
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    QString lectura;

    if (db.open()){
        if(query.exec("SELECT `Type` FROM `MasteTable`.`Devices` WHERE `Devices`.`Name`='"+name+"';")){
            while(query.next()){
                lectura = query.value(0).toString();
            }

        }else{
            messageBox.critical(0,"Error","Error query tamano");
            return false;
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
        return false;
    }
    return lectura;
}

void updateSize(long long size, QString name){
    QString newSize = QString::number(size);
    QString query = "UPDATE `MasteTable`.`Devices` SET `Size` = "+newSize+" WHERE `Name` = '"+name+"';";
    querysInsert(query);
}

void updateFormat(QString format, QString name, QString size,QString used){
    QString archivo = QString::number(limiteArchivo(format));
    QString cadena = "UPDATE `MasteTable`.`Devices` SET `Type` = '"+format+"', `LimiteArchivo` = "+archivo+", `Size` = "+size+" WHERE `Name` = '"+name+"';";
    querysInsert(cadena);
}
void limpiarAddress(QString name){
    QString cadenaCount = "SELECT count(*) FROM `MasteTable`.`Files` WHERE `Files`.`Devices`= '" +name+"';";
    QString cadena = "SELECT `Files`.`Hash` FROM `MasteTable`.`Files` WHERE `Files`.`Devices`= '" +name+"';";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    int count=0;

    //extraccion de cantidad de hashs
    if (db.open()){
        if(query.exec(cadenaCount)){
            while(query.next()){
                count= query.value(0).toInt();
            }
        }else{
            messageBox.critical(0,"Error","Error en el query");
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
    //recoleccion de hashs
    QString hashs[count];
    if(count!=0){
       if (db.open()){
           if(query.exec(cadena)){
               int iterador = 0;
               while(query.next()){
                   hashs[iterador] = query.value(0).toString();
                   iterador++;
               }
           }else{
               messageBox.critical(0,"Error","Error en el query");
           }
           db.close();
       }else{
           messageBox.critical(0,"Error","Imposible error en la coneccion");
       }
       //eliminacion de files


        for(int i=0;i<lengthArrays(hashs);i++){
            QString deleteCadena = "DELETE FROM `MasteTable`.`Address` WHERE `Address`.`idAddress`="+hashs[i]+";";
            querysInsert(deleteCadena);
        }
    }
}

int cantidadArchivosDir(QString name,QString op){
    QString cadenaCount = "SELECT count(*) FROM `MasteTable`.`Files` WHERE `Files`.`Devices`= '" +name+"';";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    int count=0;

    //extraccion de cantidad de hashs
    if (db.open()){
        if(query.exec(cadenaCount)){
            while(query.next()){
                count= query.value(0).toInt();
            }
        }else{
            messageBox.critical(0,"Error","Error en el query");
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
    return count;
}
void MainWindow::cambio(){
    QString nombre = ui->cbRMParticiones->currentText();
    ui->lbMRSize->setText(sizePartition(nombre));
    ui->lbRMFortmat->setText(typePartition(nombre));
}

long long usedPartition(QString name){
    QString cadenaCount = "SELECT count(*) FROM `MasteTable`.`Files` WHERE `Files`.`Devices`= '" +name+"';";
    QString cadena = "SELECT `Files`.`Hash` FROM `MasteTable`.`Files` WHERE `Files`.`Devices`= '" +name+"';";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    int count=0;

    //extraccion de cantidad de hashs
    if (db.open()){
        if(query.exec(cadenaCount)){
            while(query.next()){
                count= query.value(0).toInt();
            }
        }else{
            messageBox.critical(0,"Error","Error en el query");
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
    //recoleccion de hashs
    QString hashs[count];
    if(count!=0){
       if (db.open()){
           if(query.exec(cadena)){
               int iterador = 0;
               while(query.next()){
                   hashs[iterador] = query.value(0).toString();
                   iterador++;
               }
           }else{
               messageBox.critical(0,"Error","Error en el query");
           }
           db.close();
       }else{
           messageBox.critical(0,"Error","Imposible error en la coneccion");
       }
    }
    long long peso =0;
    for(int i=0;i<count;i++){
        QString consultaSize = "Select `Size` from `MasteTable`.`Address` Where `idAddress`="+hashs[i];
        if (db.open()){
            if(query.exec(consultaSize)){
                while(query.next()){
                    peso = peso + query.value(0).toLongLong();
                }
            }else{
                messageBox.critical(0,"Error","Error en el query");
            }
            db.close();
        }else{
            messageBox.critical(0,"Error","Imposible error en la coneccion");
        }
    }
    peso = peso/1048576;
    return peso;

}

void MainWindow::refreshPartition(){
    QString partition = ui->cbExPartition->currentText();
    long long size = sizePartition(partition).toLongLong();
    double sizer = size - 0.2;
    ui->lbInfSize->setText(QString::number(sizer));
    ui->lbInfCantFile->setText(QString::number(cantidadArchivosDir(partition,"0")));
    ui->lbInfType->setText(typePartition(partition));
    long long used = usedPartition(partition);
    used = used*100;
    used = used/size;
    ui->prOcupado->setValue(used);
    ui->prLibre->setValue(100-used);
    refreshFiles();
}


void deleteCarpeta(QString type,QString name){
    name.replace(" ","\\ ");
    QString directorio = QString("rm -R /home/arcatech/Documentos/Devices/"+type+"/"+name);
    char ter[directorio.length()];
    strcpy(ter, directorio.toStdString().c_str());
    system(ter);
}

void crearCarpeta(QString type, QString name){
    QString directorio = QString("mkdir /home/arcatech/Documentos/Devices/"+type+"/"+name);
    char ter[directorio.length()];
    strcpy(ter, directorio.toStdString().c_str());
    system(ter);
}


void moverCarpeta(QString typeA,QString name, QString typeB){
    name.replace(" ","\\ ");
    QString directorio = QString("mv /home/arcatech/Documentos/Devices/"+typeA+"/"+name+" /home/arcatech/Documentos/Devices/"+typeB);
    char ter[directorio.length()];
    strcpy(ter, directorio.toStdString().c_str());
    system(ter);
}

void idAddressChangeName(QString name,QString nameA){
    //modificar para cambie nombre de archivo en la url
        QString cadena = "UPDATE `MasteTable`.`Address` SET `Name` = '"+name+"' WHERE `Name` = '"+nameA+"';";
        querysInsert(cadena);
}
void devicesChangeName(QString name, QString nameA){

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    QString lectura;

    //lectura idDevice
    if (db.open()){
        if(query.exec("SELECT `Devices`.`idDevices` FROM `MasteTable`.`Devices` WHERE `Devices`.`Name`='"+nameA+"';")){
            while(query.next()){
                lectura = query.value(0).toString();
            }

        }else{
            messageBox.critical(0,"Error","Error query tamano");
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
    //actualizacion device
    QString cadena = "UPDATE `MasteTable`.`Devices` SET `Devices`.`Name` = '"+name+"' WHERE `Devices`.`idDevices` = "+lectura+";";
    querysInsert(cadena);
}

void changeNameFolder(QString name, QString nameA,QString type){
    nameA.replace(" ","\\ ");
    QString directorio = QString("mv /home/arcatech/Documentos/Devices/"+type+"/"+nameA+"/ /home/arcatech/Documentos/Devices/"+type+"/"+name);
    char ter[directorio.length()];
    strcpy(ter, directorio.toStdString().c_str());
    system(ter);
}

void limpiarDevices(QString name){

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    QString id;

    //lectura idDevice
    if (db.open()){
        if(query.exec("SELECT `Devices`.`idDevices` FROM `MasteTable`.`Devices` WHERE `Devices`.`Name`='"+name+"';")){
            while(query.next()){
                id = query.value(0).toString();
            }

        }else{
            messageBox.critical(0,"Error","Error query tamano");
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
    querysInsert("DELETE FROM `MasteTable`.`Devices` WHERE `Devices`.`idDevices` =" +id+";");
}

QString *limpiarFiles(QString name){
    QString cadenaCount = "SELECT count(*) FROM `MasteTable`.`Files` WHERE `Files`.`Devices`= '" +name+"';";
    QString cadena = "SELECT `Files`.`Hash` FROM `MasteTable`.`Files` WHERE `Files`.`Devices`= '" +name+"';";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    int count=0;

    //extraccion de cantidad de hashs
    if (db.open()){
        if(query.exec(cadenaCount)){
            while(query.next()){
                count= query.value(0).toInt();
            }
        }else{
            messageBox.critical(0,"Error","Error en el query");
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
    //recoleccion de hashs
    QString hashs[count];
    if(count!=0){
       if (db.open()){
           if(query.exec(cadena)){
               int iterador = 0;
               while(query.next()){
                   hashs[iterador] = query.value(0).toString();
                   iterador++;
               }
           }else{
               messageBox.critical(0,"Error","Error en el query");
           }
           db.close();
       }else{
           messageBox.critical(0,"Error","Imposible error en la coneccion");
       }
       //eliminacion de files
       for(int i =0;i<count;i++){
           QString deleteCadena = "DELETE FROM `MasteTable`.`Files` WHERE `Files`.`Hash`="+hashs[i]+";";
           querysInsert(deleteCadena);
       }

       return hashs;
    }
}

QString *changeAddress(QString name,QString sust, QString reemplazo,bool flag){
    QString cadenaCount = "SELECT count(*) FROM `MasteTable`.`Files` WHERE `Files`.`Devices`= '" +name+"';";
    QString cadena = "SELECT `Files`.`Hash` FROM `MasteTable`.`Files` WHERE `Files`.`Devices`= '" +name+"';";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    int count=0;
    QString particion;
    if(flag==true){
        particion=sust;
    }else{
        particion=name;
    }
    //extraccion de cantidad de hashs
    if (db.open()){
        if(query.exec(cadenaCount)){
            while(query.next()){
                count= query.value(0).toInt();
            }
        }else{
            messageBox.critical(0,"Error","Error en el query");
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
    //recoleccion de hashs
    QString hashs[count];
    if(count!=0){
       if (db.open()){
           if(query.exec(cadena)){
               int iterador = 0;
               while(query.next()){
                   hashs[iterador] = query.value(0).toString();
                   iterador++;
               }
           }else{
               messageBox.critical(0,"Error","Error en el query");
           }
           db.close();
       }else{
           messageBox.critical(0,"Error","Imposible error en la coneccion");
       }
    }
    //recoleccion de url
    QString urls[lengthArrays(hashs)];
    for(int i=0;i<lengthArrays(hashs);i++){

        QString recolectUrl ="Select `Address`.`UrlFile` from `MasteTable`.`Address` Where `Address`.`idAddress`="+hashs[i]+";";

        if (db.open()){
            if(query.exec(recolectUrl)){
                while(query.next()){
                    urls[i]= query.value(0).toString();
                }
            }else{
                messageBox.critical(0,"Error","Error en el query");
            }
            db.close();
        }else{
            messageBox.critical(0,"Error","Imposible error en la coneccion");
        }
    }
    //modificacion de url
    for(int i=0;i<lengthArrays(hashs);i++){
        urls[i].replace(reemplazo,sust);
    }
    //reubicar las url
    for(int i=0;i<lengthArrays(hashs);i++){
        QString newUrl="UPDATE `MasteTable`.`Address` SET `Address`.`UrlFile`='"+urls[i]+"', `Address`.`Device`='"+particion+"' Where `Address`.`idAddress`="+hashs[i]+";";
        querysInsert(newUrl);
    }
}

void changeNameFile(QString nameN,QString nameA){

    QString cadenaCount = "SELECT count(*) FROM `MasteTable`.`Files` WHERE `Files`.`Devices`= '" +nameA+"';";
    QString cadena = "SELECT `Files`.`Hash` FROM `MasteTable`.`Files` WHERE `Files`.`Devices`= '" +nameA+"';";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    int count=0;

    //extraccion de cantidad de hashs
    if (db.open()){
        if(query.exec(cadenaCount)){
            while(query.next()){
                count= query.value(0).toInt();
            }
        }else{
            messageBox.critical(0,"Error","Error en el query");
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
    //recoleccion de hashs
    QString hashs[count];
    if(count!=0){
       if (db.open()){
           if(query.exec(cadena)){
               int iterador = 0;
               while(query.next()){
                   hashs[iterador] = query.value(0).toString();
                   iterador++;
               }
           }else{
               messageBox.critical(0,"Error","Error en el query");
           }
           db.close();
       }else{
           messageBox.critical(0,"Error","Imposible error en la coneccion");
       }
    }

    //modificacion de nombre atravez de los hash
    for(int i=0;i<lengthArrays(hashs);i++){
        QString newName="UPDATE `MasteTable`.`Files` SET `Files`.`Devices`='"+nameN+"' Where `Files`.`Hash`="+hashs[i]+";";
        querysInsert(newName);
    }
}

bool consultaExisteHash(long long hash){
    QString hashing=QString::number(hash);

    QString cadena = "SELECT `Files`.`Hash` FROM `MasteTable`.`Files` WHERE `Files`.`Hash`="+hashing+";";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    QString lectura;

    //abre conexion
    if (db.open()){
        if(query.exec(cadena)){
            while(query.next()){
                lectura = query.value(0).toString();
            }
            if(lectura==""){
                return true;
            }else {
                return false;
            }
        }else{
            messageBox.critical(0,"Error","Error en el query");
            return false;
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
        return false;
    }

}

bool consultaExisteFile(QString name,QString particion){
    QString cadena = "SELECT `Files`.`idFiles` FROM `MasteTable`.`Files` WHERE `Files`.`idFiles`='"+name+"' AND `Files`.`Devices`='"+particion+"';";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    QString lectura;

    //abre conexion
    if (db.open()){
        if(query.exec(cadena)){
            while(query.next()){
                lectura = query.value(0).toString();
            }
            if(lectura==""){
                return true;
            }else {
                return false;
            }
        }else{
            messageBox.critical(0,"Error","Error en el query");
            return false;
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
        return false;
    }
}

QString hashing(){

    bool flag = true;
    long long hash;
    while(flag==true){
        long long hash = (qrand()%268173300); //numero de cluster
        if(consultaExisteHash(hash)){
            flag=false;
        }
    }
    return QString::number(hash);
}

void copiaArchivos(QString url, QString name){
    QString type = typePartition(name);
    url.replace(" ","\\ ");
    QString directorio = QString("cp "+url+" /home/arcatech/Documentos/Devices/"+type+"/"+name);
    char ter[directorio.length()];
    strcpy(ter, directorio.toStdString().c_str());
    system(ter);
}

QString takeExt(QString url){
    QString ext="";
    bool flag=true;
    for(int i=0;i<url.length();i++){
        QString ch=url.at(i);
        if(ch=="."){
            flag =false;
        }
        if(flag==false){
            ext=ext+url.at(i);
        }
    }
    return ext;
}

void tratarUrls(QString urls[],QString particion,int sizeArray){

    for(int i =0;i<sizeArray;i++){
        QFileInfo archivo;
        archivo.setFile(urls[i]);
        QString name = archivo.baseName();
        QString hash = hashing();
        QString size = QString::number(archivo.size());
        long long sizer = archivo.size();
        QString create = archivo.created().toString();
        QString editado = archivo.lastModified().toString();
        QString leido = archivo.lastRead().toString();
        //proceso de cambio
        QString takeExtFile = takeExt(urls[i]);
        QString urlFinal = "/home/arcatech/Documentos/Devices/"+typePartition(particion)+"/"+particion+"/"+name+takeExtFile;
        QString cadenaFiles = "INSERT INTO `MasteTable`.`Files` (`Hash`,`idFiles`, `Devices`) VALUES("+hash+", '"+name+"', '"+particion+"');";
        QString cadenaAddress = "INSERT INTO `MasteTable`.`Address` (`idAddress`,`Name`,`UrlFile`,`Size`,`CreateDate`,`EditDate`,`Access`,`Device`)VALUES("+hash+",'"+name+"','"+urlFinal+"',"+size+",'"+create+"','"+editado+"','"+leido+"','"+particion+"');";
        long long limite = limiteArchivo(typePartition(particion));
        limite = limite *1048576;
        QMessageBox msj;
        //ver si cabe
        long long used = usedPartition(particion);
        used=used*1048576;
        used = used+sizer;
        long long sizePart=sizePartition(particion).toLongLong();
        sizePart=sizePart*1048576;
        sizePart = sizePart-524288;
        //
        if(sizePart>used){
            if(limite<sizer){
                msj.critical(0,"Error","archivo excede tamano de la particion");
            }else{
                if(consultaExisteFile(name,particion)){
                    querysInsert(cadenaFiles);
                    querysInsert(cadenaAddress);
                    copiaArchivos(urls[i],particion);
                }else{
                    msj.critical(0,"Error","no se guardo: "+name+" ya esxiste un archivo igual");
                }
            }
        }else{
            msj.critical(0,"Error","Tamano excedido");
        }
    }
}

void MainWindow::runFile(){
        QString partition = ui->cbExPartition->currentText();
        QString name = ui->lwFiles->currentItem()->text();
        QString cadena = "SELECT `Files`.`Hash` FROM `MasteTable`.`Files` WHERE `Files`.`Devices`= '" +partition+"' AND `Files`.`idFiles`='"+name+"';";
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("localhost");
        db.setUserName("root");
        QMessageBox messageBox;
        QSqlQuery query;
        QString hashs;

           if (db.open()){
               if(query.exec(cadena)){
                   while(query.next()){
                       hashs = query.value(0).toString();
                   }
               }else{
                   messageBox.critical(0,"Error","Error en el query");
               }
               db.close();
           }else{
               messageBox.critical(0,"Error","Imposible error en la coneccion");
           }

            QString url="Select `Address`.`UrlFile` From `MasteTable`.`Address` Where `Address`.`idAddress`="+hashs;
            if (db.open()){
                       if(query.exec(url)){
                           while(query.next()){
                               url = query.value(0).toString();
                           }
                       }else{
                           messageBox.critical(0,"Error","Error en el query");
                       }
                       db.close();
                   }else{
                       messageBox.critical(0,"Error","Imposible error en la coneccion");
                   }
            url.replace(" ","\\ ");
            QString directorio = QString("gnome-open "+url);
            char ter[directorio.length()];
            strcpy(ter, directorio.toStdString().c_str());
            system(ter);
}

void MainWindow::loadDetail(){

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    QString nameFile = ui->lwFiles->currentItem()->text();

    //obtener datos segun hash
    QString cadena2="Select `Name`, `Size`, `CreateDate`, `EditDate`, `Access` From `MasteTable`.`Address` Where `Address`.`Name`='"+nameFile+"';";
    QString url;
    if (db.open()){
        if(query.exec(cadena2)){
            while(query.next()){
                long long peso = query.value(1).toLongLong();
                double tam = peso/1048476;
                QString lb="MB";
                if(tam==0){
                    tam=peso/1024;
                    lb="Kb";

                }
                ui->lbInfAName->setText(query.value(0).toString());
                ui->lbInfASize->setText(QString::number(tam,'f', 2)+lb);
                ui->lbInfCreated->setText(query.value(2).toString());
                ui->lbInfAEdit->setText(query.value(3).toString());
                ui->lbInfAAccess->setText(query.value(4).toString());
                refreshFiles();
            }
        }else{
            messageBox.critical(0,"Error","Error en el query");
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
}
void MainWindow::refreshFiles(){
    ui->lwFiles->clear();
    QString particion=ui->cbExPartition->currentText();
    QString cadena = "SELECT `Files`.`idFiles` FROM `MasteTable`.`Files` WHERE `Files`.`Devices`='"+particion+"';";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;
    //abre conexion
    if (db.open()){
        if(query.exec(cadena)){
            while(query.next()){
                ui->lwFiles->addItem(query.value(0).toString());
            }
        }else{
            messageBox.critical(0,"Error","Error en el query");
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }

}
void MainWindow::deleteFiles(){



    QString partition = ui->cbExPartition->currentText();
    QFileDialog dU;
    QList<QUrl> u = dU.getOpenFileUrls();
    QString urls[u.size()];
    for (int i = 0; i < u.size(); ++i) {
        urls[i]=u.at(i).toString().remove("file://");
    }
    QFileInfo archivo;
    archivo.setFile(urls[0]);
    if(consultaExisteFile(archivo.baseName(),partition)){
        QMessageBox msj;
        msj.critical(0,"Error","No pertenece a la particion");
    }else{
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("localhost");
        db.setUserName("root");
        QMessageBox messageBox;
        QSqlQuery query;

         QString hashs[lengthArrays(urls)];
         for(int i=0;i<lengthArrays(urls);i++){
            QString cadena = "SELECT `Address`.`idAddress` FROM `MasteTable`.`Address` WHERE `Address`.`UrlFile`= '" +urls[i]+"'";
            if (db.open()){
                if(query.exec(cadena)){
                    while(query.next()){
                        hashs[i] = query.value(0).toString();
                    }
                }else{
                    messageBox.critical(0,"Error","Error en el query");
                }
            }else{
                messageBox.critical(0,"Error","Imposible error en la coneccion");
            }
         }
            for(int i=0;i<lengthArrays(urls);i++){
                QString deleteFiles = "Delete from `MasteTable`.`Files` Where `Files`.`Hash`="+hashs[i];
                QString deleteAddress = "Delete from `MasteTable`.`Address` Where `Address`.`idAddress`="+hashs[i];
                querysInsert(deleteFiles);
                querysInsert(deleteAddress);

                urls[i].replace(" ","\\ ");
                QString directorio = QString("rm -r "+urls[i]);
                 char ter[directorio.length()];
                 strcpy(ter, directorio.toStdString().c_str());
                 system(ter);
            }
             refreshFiles();
             refreshPartition();
         }


}

void MainWindow::extraerF(){
    QMessageBox msgBox(QMessageBox::Question,trUtf8("Consulta"), "Que desea extraer?",QMessageBox::Yes | QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, trUtf8("Archivos"));
    msgBox.setButtonText(QMessageBox::No, trUtf8("Carpeta"));
    QString partition = ui->cbExPartition->currentText();
    if (msgBox.exec() == QMessageBox::Yes) {

        QFileDialog dU;
        QList<QUrl> u = dU.getOpenFileUrls();
        QString urls[u.size()];
        for (int i = 0; i < u.size(); ++i) {
            urls[i]=u.at(i).toString().remove("file://");
        }
        QFileInfo doc;
        doc.setFile(urls[0]);
        if(consultaExisteFile(doc.baseName(),partition)){
            QMessageBox me;
            me.critical(0,"error","No pertenece a la particion");
        }else{
            QFileDialog dU;
            QString url = dU.getExistingDirectoryUrl().toString().remove("file://");
            for(int i=0;i<lengthArrays(urls);i++){
            urls[i].replace(" ","\\ ");
            url.replace(" ","\\ ");
            QString directorio = QString("cp "+urls[i]+" "+url);
            char ter[directorio.length()];
            strcpy(ter, directorio.toStdString().c_str());
            system(ter);
            }
        }
    }else if(msgBox.exec()==QMessageBox::No){
        QFileDialog dU;
        QString u = dU.getExistingDirectoryUrl().toString().remove("file://");

        QFileInfo doc;
        doc.setFile(u);
        if(consultaExisteFile(doc.baseName(),partition)){
            QMessageBox me;
            me.critical(0,"error","No pertenece a la particion");
        }else{
            QFileDialog dU;
            QString url = dU.getExistingDirectoryUrl().toString().remove("file://");
            url.replace(" ","\\ ");
            u.replace(" ","\\ ");
            QString directorio = QString("cp "+u+" "+url);
            char ter[directorio.length()];
            strcpy(ter, directorio.toStdString().c_str());
            system(ter);

        }
    }
}

void MainWindow::crearCarpetaInPartition(){
    QString name = ui->cbExPartition->currentText();
    QString type= typePartition(name);
    QString newName;
    bool flag=true;
    while(flag==true){
    newName = QInputDialog::getText(this, tr("Nombre"),tr("Nombre de la carpeta"), QLineEdit::Normal);
    if(consultaExisteFile(newName,name)){
        flag=false;
    }
    }
    QString hash = hashing();
    QString directorio = QString("/home/arcatech/Documentos/Devices/"+type+"/"+name+"/"+newName);
    QDate date; date.currentDate().toString();
    QString cadenaFile = "Insert Into `MasteTable`.`Files` (`idFiles`,`Devices`,`Hash`)Values('"+newName+"','"+name+"',"+hash+");";
    QString cadenaAddress = "Insert Into `MasteTable`.`Address` (`idAddress`,`Name`,`UrlFile`,`Size`,`CreateDate`,`EditDate`,`Access`,`Device`)Values("+hash+",'"+newName+"','"+directorio+"',0,'"+date.currentDate().toString()+"','"+date.currentDate().toString()+"','"+date.currentDate().toString()+"','"+name+"');";
    querysInsert(cadenaFile);
    querysInsert(cadenaAddress);
    directorio.replace(" ","\\ ");
    directorio = "mkdir "+directorio;
    char ter[directorio.length()];
    strcpy(ter, directorio.toStdString().c_str());
    system(ter);
}

void MainWindow::cambioFormat(){
    QString nombre = ui->cdFMParticiones->currentText();
    ui->lbFMSize->setText(sizePartition(nombre));
    ui->lbFMFormat->setText(typePartition(nombre));

}
void MainWindow::particiones(){
    QString cadena = "SELECT `Devices`.`Name` FROM `MasteTable`.`Devices`;";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;

    if (db.open()){
        if(query.exec(cadena)){
            ui->cbRMParticiones->clear();
            ui->cbDMParticiones->clear();
            ui->cbNMParticiones->clear();
            ui->cdFMParticiones->clear();
            ui->cbExPartition->clear();
            while(query.next()){
                ui->cbRMParticiones->addItem(query.value(0).toString());
                ui->cbDMParticiones->addItem(query.value(0).toString());
                ui->cbNMParticiones->addItem(query.value(0).toString());
                ui->cdFMParticiones->addItem(query.value(0).toString());
                ui->cbExPartition->addItem(query.value(0).toString());
            }

        }else{
            messageBox.critical(0,"Error","Error en el query");
        }
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
}

void MainWindow::loadTableFiles(){
    QString particion=ui->cbExPartition->currentText();
    QString cadena = "SELECT `idFiles`,`Hash`,`Devices` FROM `MasteTable`.`Files` Where `Files`.`Devices`='"+particion+"';";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;

    if (db.open()){
        QSqlQueryModel *table = new QSqlQueryModel;
        table->setQuery(cadena);
        ui->tableView->setModel(table);
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
}

void MainWindow::loadTableDevices(){
    QString particion=ui->cbExPartition->currentText();
    QString cadena = "SELECT * FROM `MasteTable`.`Devices` Where `Devices`.`name`='"+particion+"';";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;

    if (db.open()){
        QSqlQueryModel *table = new QSqlQueryModel;
        table->setQuery(cadena);
        ui->tableView->setModel(table);
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
}

void MainWindow::loadTableAddress(){
    QString particion=ui->cbExPartition->currentText();
    QString cadena = "SELECT * FROM `MasteTable`.`Address` Where `Address`.`Device`='"+particion+"';";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    QMessageBox messageBox;
    QSqlQuery query;

    if (db.open()){
        QSqlQueryModel *table = new QSqlQueryModel;
        table->setQuery(cadena);
        ui->tableView->setModel(table);
        db.close();
    }else{
        messageBox.critical(0,"Error","Imposible error en la coneccion");
    }
}

//fin de funsiones-------------------------------------------------------------------------------------------------

//menu interactivo------------------------------------------------------------------------------------------------
void MainWindow::on_btnCrear_clicked(){
    QString nombre = ui->txtNombre->text().toLower();
    QString type = ui->cbFormato->currentText();
    long long size = ui->nudSize->value();
    QString sizec = QString::number(size);
    QString id = QString::number(idDevices());
    QString archivos = QString::number(limiteArchivo(type));
    QMessageBox messageBox;

    if(!verifyName(nombre)||nombre==""||nombre=="null"||nombre=="0"||!verifySize(type,size)||!consultaExiste(nombre)){
        messageBox.critical(0,"Error","Nombre o tamano invalido evite los caracteres '/' , '.' , 'Null'");
    }else{
        QString cadena;
        cadena = "INSERT INTO `MasteTable`.`Devices` (`Name`, `Size`, `Type`, `LimiteArchivo`,`idDevices`) VALUES ('"+nombre+"',"+sizec+",'"+type+"',"+archivos+","+id+")";
        querysInsert(cadena);
        crearCarpeta(type,nombre);
        particiones();
    }
}

void MainWindow::on_btnNew_clicked(){
    if(cambioMenus[0]==false){
        ui->frameResize->setVisible(false);
        ui->frameNewFormat->setVisible(false);
        ui->frameChangeName->setVisible(false);
        ui->frameDelete->setVisible(false);
        ui->frameAyuda->setVisible(false);
        ui->frameCrear->setVisible(true);
        cambioMenus[0]=true;
        cambioMenus[1]=cambioMenus[2]=cambioMenus[3]=cambioMenus[4]=cambioMenus[5]=false;
    }else{
        ui->frameCrear->setVisible(false);
        cambioMenus[0]=false;
    }


}

void MainWindow::on_btnResize_clicked(){
    if(cambioMenus[1]==false){
        ui->frameResize->setVisible(true);
        ui->frameNewFormat->setVisible(false);
        ui->frameChangeName->setVisible(false);
        ui->frameDelete->setVisible(false);
        ui->frameAyuda->setVisible(false);
        ui->frameCrear->setVisible(false);
        cambioMenus[1]=true;
        cambioMenus[0]=cambioMenus[2]=cambioMenus[3]=cambioMenus[4]=cambioMenus[5]=false;
    }else{
        ui->frameResize->setVisible(false);
        cambioMenus[1]=false;
    }
}

void MainWindow::on_btnNewFormat_clicked(){
    if(cambioMenus[2]==false){
        ui->frameResize->setVisible(false);
        ui->frameNewFormat->setVisible(true);
        ui->frameChangeName->setVisible(false);
        ui->frameDelete->setVisible(false);
        ui->frameAyuda->setVisible(false);
        ui->frameCrear->setVisible(false);
        cambioMenus[2]=true;
        cambioMenus[0]=cambioMenus[1]=cambioMenus[3]=cambioMenus[4]=cambioMenus[5]=false;
    }else{
        ui->frameNewFormat->setVisible(false);
        cambioMenus[2]=false;
    }
}

void MainWindow::on_btnChangeName_clicked(){
    if(cambioMenus[3]==false){
        ui->frameResize->setVisible(false);
        ui->frameNewFormat->setVisible(false);
        ui->frameChangeName->setVisible(true);
        ui->frameDelete->setVisible(false);
        ui->frameAyuda->setVisible(false);
        ui->frameCrear->setVisible(false);
        cambioMenus[3]=true;
        cambioMenus[1]=cambioMenus[2]=cambioMenus[0]=cambioMenus[4]=cambioMenus[5]=false;
    }else{
        ui->frameChangeName->setVisible(false);
        cambioMenus[3]=false;
    }
}

void MainWindow::on_btnDeletePartition_clicked(){
    if(cambioMenus[4]==false){
        ui->frameResize->setVisible(false);
        ui->frameNewFormat->setVisible(false);
        ui->frameChangeName->setVisible(false);
        ui->frameDelete->setVisible(true);
        ui->frameAyuda->setVisible(false);
        ui->frameCrear->setVisible(false);
        cambioMenus[4]=true;
        cambioMenus[1]=cambioMenus[2]=cambioMenus[3]=cambioMenus[0]=cambioMenus[5]=false;
    }else{
        ui->frameDelete->setVisible(false);
        cambioMenus[4]=false;
    }
}

void MainWindow::on_btnHelp_clicked(){
    if(cambioMenus[5]==false){
        ui->frameResize->setVisible(false);
        ui->frameNewFormat->setVisible(false);
        ui->frameChangeName->setVisible(false);
        ui->frameDelete->setVisible(false);
        ui->frameAyuda->setVisible(true);
        ui->frameCrear->setVisible(false);
        cambioMenus[5]=true;
        cambioMenus[1]=cambioMenus[2]=cambioMenus[3]=cambioMenus[4]=cambioMenus[0]=false;
    }else{
        ui->frameAyuda->setVisible(false);
        cambioMenus[5]=false;
    }
}

//fin menu interactivo---------------------------------------------------------------------------------------------

void MainWindow::on_btnResize_2_clicked(){
    QString name = ui->cbRMParticiones->currentText();
    QString particion = typePartition(name);
    QMessageBox message;
    long long size = ui->nudRMNewSize->value();
    if(size<sizePartition(name).toInt()){
      message.critical(0,"Error","Imposible reducir particion se prodian perder datos");
    }else{
        if(verifySize(particion,size)){
                updateSize(size,name);
                cambio();
        }else{
            message.critical(0,"Error","Tamano incompatible con el formato actual");
        }
    }
}

void MainWindow::on_btnNewFormat_2_clicked(){
    QString name = ui->cdFMParticiones->currentText();
    QString AFormat = ui->lbFMFormat->text();
    QString NFormat = ui->cdFMNewFormat->currentText();
    QMessageBox message;
    if(AFormat == NFormat){
        message.critical(0,"Error","Ambos formatos son iguales");
    }else if(AFormat=="Fat32" && NFormat=="Fat16"){                 //cambio de Fat32 a Fat16
        message.setWindowTitle("Error");
        message.setText("Este cambio probocara perdida \nde datos desea continuar");
        message.setStandardButtons(QMessageBox::Yes);
        message.addButton(QMessageBox::No);
        message.setDefaultButton(QMessageBox::No);
        if(message.exec() == QMessageBox::Yes){
            QString text="3000";
            while(text.toInt()>2048){
            text = QInputDialog::getText(this, tr("Size"),tr("Nuevo Tamano para Fat16"), QLineEdit::Normal);
                if(text.toInt()<=0){
                    text = "3000";
                }
            }
            updateFormat(NFormat,name,text,"0");
            limpiarAddress(name);
            limpiarFiles(name);
            deleteCarpeta(AFormat,name);
            crearCarpeta(NFormat,name);
        }
    }else if(AFormat=="Fat16" && NFormat=="Fat32"){
            QString usado = QString::number(usedPartition(name));
            updateFormat(NFormat,name,sizePartition(name),usado);
            changeAddress(name,"Fat32","Fat16",false);
            moverCarpeta(AFormat,name,NFormat);
    }
    cambioFormat();
    particiones();
}

void MainWindow::on_btnNMChangeName_clicked(){
    QMessageBox msj;
    QString name = ui->txtNMNewname->text();
    QString nameA = ui->cbNMParticiones->currentText();
    QString type = typePartition(nameA);
    if(consultaExiste(name)){

    changeAddress(nameA,name,nameA,true);
    changeNameFile(name,nameA);
    devicesChangeName(name,nameA);
    changeNameFolder(name,nameA,type);
    }
    particiones();
}

void MainWindow::on_btnDMDelete_clicked(){

    QString name = ui->cbDMParticiones->currentText();
    QString type= typePartition(name);
    limpiarAddress(name);
    limpiarFiles(name);
    limpiarDevices(name);
    deleteCarpeta(type,name);
    particiones();
}


void MainWindow::on_btnInsertar_clicked(){
    QString partition = ui->cbExPartition->currentText();
    QFileDialog dU;
    QList<QUrl> u = dU.getOpenFileUrls();
    QString urls[u.size()];
    for (int i = 0; i < u.size(); ++i) {
        urls[i]=u.at(i).toString().remove("file://");
    }
    tratarUrls(urls,partition,u.size());
    refreshFiles();
    refreshPartition();
}
