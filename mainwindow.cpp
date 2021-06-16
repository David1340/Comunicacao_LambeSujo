#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>
#include <QLineEdit>
#include <stdlib.h>
#include <QString>
#include <QIODevice>
#include <iostream>
#include <time.h>
#include <QTime>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    //chama a função a cada 10 milisegundos
    /*
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout,
           this, &MainWindow::read_Data);
    timer->start(10);
    */
    ui->setupUi(this); //ja veio por default do QT
    this->serialPort = new QSerialPort; //instanciando porta serial
    QStringList portas; //Lista de Strings
    const auto serialPortInfos = QSerialPortInfo::availablePorts();//método estático
    int i =0;
    //alimentando o combox com as informações da porta serial
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos)
    {
        //Listando as portas disponíveis
        portas.insert(i, serialPortInfo.portName());
        i++;
    }
    ui->comboBox->insertItems(0, portas); //Dessa forma funciona para qualquer sistema operacional
    connect(ui->Connect_Disconect,SIGNAL(clicked(bool)),this,SLOT(connectToSerial())); // conexão serial
    ui->label->setText("Desconectado");
    ui->Baud_Rate->setText(""); //limpando label do Baud Rate
}


void MainWindow::checkSerial(){  // funcão criada para tentar reconectar
    //const auto serialPortInfos = QSerialPortInfo::availablePorts();
    auto serialPortInfos = QSerialPortInfo::availablePorts();
    QStringList portas;
    portas.clear();
    int i =0;
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos){
        portas.insert(i, serialPortInfo.portName());
        i++;
    }
    ui->comboBox->clear();
    ui->comboBox->insertItems(0, portas);
}

void MainWindow::connectToSerial(){
    if(this->serialPort->isOpen()){ // se conectado
        closeSerialPort();
        //this->serialPort->close();
        ui->label->setText("Desconectado");
        ui->Connect_Disconect->setText("Conectar");
        ui->Baud_Rate->setText(""); //limpando label do Baud Rate      
        return;
    }
    // Parâmetros de Abertura da Porta Serial
    if(ui->comboBox->count()>=0){
    this->serialPort->setPortName(ui->comboBox->currentText());
    this->serialPort->setBaudRate(QSerialPort::Baud115200);
    this->serialPort->setDataBits(QSerialPort::Data8);
    this->serialPort->setParity(QSerialPort::NoParity);
    this->serialPort->setStopBits(QSerialPort::OneStop);
    this->serialPort->setFlowControl(QSerialPort::NoFlowControl);
    }

    if(this->serialPort->open(QIODevice::ReadWrite)){ // Verificação se a porta realmente abriu        
        ui->label->setText("Conectado");
        ui->Baud_Rate->setText("Communication Paramaters\n"
                               "Baudrate: 115200\n"
                               "Data bits: 8\n"
                               "Parity: None\n"
                               "Stop bits: 1\n"
                               "Flow control: None");

        ui->Connect_Disconect->setText("Desconectar");

        flag_comunicacao = !flag_comunicacao; // altera o flag de comunicação 0 - SemComunicação e 1 - ComComunicação

        return;
    }
    ui->label->setText("Falha ao tentar conetar");
    ui->Connect_Disconect->setText("Conectar");
    this->serialPort->close();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeSerialPort()
{
    if (this->serialPort->isOpen()){
        this->serialPort->close();
        flag_comunicacao = !flag_comunicacao; // altera o flag de comunicação 0 - SemComunicação e 1 - ComComunicação
    }
}

//void MainWindow::write_Data(const QByteArray &data)
void MainWindow::write_Data()
{

    int index = ui->Select_Robot->currentIndex(); //Seleciona o Index
    for(int i=0; i<11;i++){
        if(i!=index && i!=(2*index+1) && i!=(2*index+2)){
            write_buf[i] = C0; // ZERA O BUFFER DE ESCRITA
        }
    }
    write_buf[0] = (unsigned char) (250+index); // ID DO ROBÔ QUE RECEBERÁ A MENSAGEM
    //write_buf[2*index + 1] = converter_write(ui->verticalSlider_vel_L->value()); // VELOCIDADE ESQUERDA DO ROBÔ(ID)
    //write_buf[2*index + 2] = converter_write(ui->verticalSlider_vel_R->value()); // VELOCIDADE DIREITA DO ROBÔ(ID)
    //Buffer Completo
    //1 3 5 7 9
    //0 1 2 3 4
    QByteArray write_buf3;
    write_buf3 = QByteArray::fromRawData((char*) write_buf,11);

    //write_buf3 = QByteArray::fromRawData(write_buf,11);
    if (flag_comunicacao)
    {
        this->serialPort->write(write_buf3);
        //this->serialPort->write(write_buf);
       //qint64 i = QIODevice::writeData(const char 'a',11);
    }

    const QString texto = write_buf3;
    //const QString texto = {(char)write_buf3.at(0),(char)write_buf3.at(1),(char)write_buf3.at(2),(char)write_buf3.at(3),(char)write_buf3.at(4),(char)write_buf3.at(5),(char)write_buf3.at(6),(char)write_buf3.at(7),(char)write_buf3.at(8),(char)write_buf3.at(9),(char)write_buf3.at(10)};
    ui->lineEdit->setText(texto);

    if(int(write_buf3.at(0) >= 0)){
        ui->spinBox_texte->setValue((int)write_buf3.at(0));
        ui->lcdNumber_id->display((int)write_buf3.at(0));
    }
    else{
        ui->spinBox_texte->setValue(-(128 + (int)write_buf3.at(0)));
        //ui->spinBox_texte->setValue((int)(unsigned char)write_buf3.at(1));
        ui->lcdNumber_id->display(-(128 + (int)write_buf3.at(0)));
    }
    if(int(write_buf3.at(1) >= 0)){
        ui->lcdNumber_L1->display((int)write_buf3.at(1));
    }
    else{
        ui->lcdNumber_L1->display(-(128 + (int)write_buf3.at(1)));
    }
    if(int(write_buf3.at(2) >= 0)){
        ui->lcdNumber_R1->display((int)write_buf3.at(2));
    }
    else{
        ui->lcdNumber_R1->display(-(128 + (int)write_buf3.at(2)));
    }
    if(int(write_buf3.at(3) >= 0)){
        ui->lcdNumber_L2->display((int)write_buf3.at(3));
    }
    else{
        ui->lcdNumber_L2->display(-(128 + (int)write_buf3.at(3)));
    }
    if(int(write_buf3.at(4) >= 0)){
        ui->lcdNumber_R2->display((int)write_buf3.at(4));
    }
    else{
        ui->lcdNumber_R2->display(-(128 + (int)write_buf3.at(4)));
    }
    if(int(write_buf3.at(5) >= 0)){
        ui->lcdNumber_L3->display((int)write_buf3.at(5));
    }
    else{
        ui->lcdNumber_L3->display(-(128 + (int)write_buf3.at(5)));
    }
    if(int(write_buf3.at(6) >= 0)){
        ui->lcdNumber_R3->display((int)write_buf3.at(6));
    }
    else{
        ui->lcdNumber_R3->display(-(128 + (int)write_buf3.at(6)));
    }
    if(int(write_buf3.at(7) >= 0)){
        ui->lcdNumber_L4->display((int)write_buf3.at(7));
    }
    else{
        ui->lcdNumber_L4->display(-(128 + (int)write_buf3.at(7)));
    }
    if(int(write_buf3.at(8) >= 0)){
        ui->lcdNumber_R4->display((int)write_buf3.at(8));
    }
    else{
        ui->lcdNumber_R4->display(-(128 + (int)write_buf3.at(8)));
    }
    if(int(write_buf3.at(9) >= 0)){
        ui->lcdNumber_L5->display((int)write_buf3.at(9));
    }
    else{
        ui->lcdNumber_L5->display(-(128 + (int)write_buf3.at(9)));
    }
    if(int(write_buf3.at(10) >= 0)){
        ui->lcdNumber_R5->display((int)write_buf3.at(10));
    }
    else{
        ui->lcdNumber_R5->display(-(128 + (int)write_buf3.at(10)));
    }
}


void MainWindow::read_Data()
{

    //int index = ui->Select_Robot->currentIndex();
    //const QByteArray datareceive = this->serialPort->readAll();
    if (flag_comunicacao)
    {
        if(this->serialPort->bytesAvailable()){
            QByteArray read_buf = this->serialPort->read(11);
            const QString texto = read_buf;
            ui->lineEdit_2->setText(texto);

            if(int(read_buf.at(1) >= 0)){
                ui->spinBox_texte_3->setValue((int)read_buf.at(1)); //Vel. Esq.
            }
            else{
                ui->spinBox_texte_3->setValue(-(128 + (int)read_buf.at(1))); //Vel. Esq.
            }

            if(int(read_buf.at(2) >= 0)){
                ui->spinBox_texte_2->setValue((int)read_buf.at(2)); //Vel. Dir.
            }
            else{
                ui->spinBox_texte_2->setValue(-(128 + (int)read_buf.at(2))); //Vel. Dir.
            }




            if(int(read_buf.at(0) >= 0)){
                ui->lcdNumber_id_r->display((int)read_buf.at(0));
            }
            else{
                ui->lcdNumber_id_r->display(-(128 + (int)read_buf.at(0)));
            }
            if(int(read_buf.at(1) >= 0)){
                ui->lcdNumber_L1_r->display((int)read_buf.at(1));
            }
            else{
                ui->lcdNumber_L1_r->display(-(128 + (int)read_buf.at(1)));
            }
            if(int(read_buf.at(2) >= 0)){
                ui->lcdNumber_R1_r->display((int)read_buf.at(2));
            }
            else{
                ui->lcdNumber_R1_r->display(-(128 + (int)read_buf.at(2)));
            }
            if(int(read_buf.at(3) >= 0)){
                ui->lcdNumber_L2_r->display((int)read_buf.at(3));
            }
            else{
                ui->lcdNumber_L2_r->display(-(128 + (int)read_buf.at(3)));
            }
            if(int(read_buf.at(4) >= 0)){
                ui->lcdNumber_R2_r->display((int)read_buf.at(4));
            }
            else{
                ui->lcdNumber_R2_r->display(-(128 + (int)read_buf.at(4)));
            }
            if(int(read_buf.at(5) >= 0)){
                ui->lcdNumber_L3_r->display((int)read_buf.at(5));
            }
            else{
                ui->lcdNumber_L3_r->display(-(128 + (int)read_buf.at(5)));
            }
            if(int(read_buf.at(6) >= 0)){
                ui->lcdNumber_R3_r->display((int)read_buf.at(6));
            }
            else{
                ui->lcdNumber_R3_r->display(-(128 + (int)read_buf.at(6)));
            }
            if(int(read_buf.at(7) >= 0)){
                ui->lcdNumber_L4_r->display((int)read_buf.at(7));
            }
            else{
                ui->lcdNumber_L4_r->display(-(128 + (int)read_buf.at(7)));
            }
            if(int(read_buf.at(8) >= 0)){
                ui->lcdNumber_R4_r->display((int)read_buf.at(8));
            }
            else{
                ui->lcdNumber_R4_r->display(-(128 + (int)read_buf.at(8)));
            }
            if(int(read_buf.at(9) >= 0)){
                ui->lcdNumber_L5_r->display((int)read_buf.at(9));
            }
            else{
                ui->lcdNumber_L5_r->display(-(128 + (int)read_buf.at(9)));
            }
            if(int(read_buf.at(10) >= 0)){
                ui->lcdNumber_R5_r->display((int)read_buf.at(10));
            }
            else{
                ui->lcdNumber_R5_r->display(-(128 + (int)read_buf.at(10)));
            }

        }
    }
}





/* CONVERSÃO DOS DADOS ENVIADOS OU RECEBIDOS  */
unsigned char MainWindow::converter_write (int x)
{
    //OBSERVAÇÃO: SÓ FUNCIONA PRA VALORES MENORES QUE 127, MAS O PROTOCOLO É DE 0 A 100 COM BIT DE SINAL

    unsigned char c = C0; //Inicializando com zero
    if (x >= 0)
    {
        c = (unsigned char) x;
    }
    else
    {
        c = (unsigned char) (abs(x)+128); //Módulo + o bit de sinal
    }
    return c;
}

int MainWindow::converter_read (unsigned char c)
{
    int x = 0; //Inicializando com zero
    if (((int)c) >= 128)
        x = -(((int)c) - 128);
    else
        x = (int)c;
    return x;
}





void MainWindow::initActionsConnections()
{

}


void MainWindow::on_Connect_Disconect_clicked()
{
    //fica vazio mesmo
    //referente ao conect e disconect
}

void MainWindow::on_comboBox_activated(int index)
{
    //fica vazio mesmo
    //é referente ao botão de conecte e disconect
}

void MainWindow::on_CheckSerial_Button_clicked()
{
   checkSerial();
}

void MainWindow::on_Read_clicked()
{
    read_Data();
}

void MainWindow::on_Write_clicked()
{
    write_Data();
}


// Controle de Velocidade para Teste
void MainWindow::on_spinBox_vel_R_valueChanged(int arg1)
{
    ui->verticalSlider_vel_R->setValue(arg1);
}

void MainWindow::on_spinBox_vel_L_valueChanged(int arg1)
{
    ui->verticalSlider_vel_L->setValue(arg1);
}

void MainWindow::on_verticalSlider_vel_R_valueChanged(int value)
{
    ui->spinBox_vel_R->setValue(value);
}

void MainWindow::on_verticalSlider_vel_L_valueChanged(int value)
{
    ui->spinBox_vel_L->setValue(value);
}

void MainWindow::on_Select_Robot_activated(int index)
{
/*
    for(int i=0; i<11;i++){
        write_buf[i] = C0; // ZERA O BUFFER DE ESCRITA INTEIRO
    }

    write_buf[0] = (unsigned char) (250+index); // ID DO ROBÔ QUE RECEBERÁ A MENSAGEM

    write_buf[2*index] = converter_write(ui->verticalSlider_vel_L->value());
    write_buf[2*index + 1] = converter_write(ui->verticalSlider_vel_R->value());
*/
}

void MainWindow::on_spinBox_texte_valueChanged(int arg1)
{

}

void MainWindow::on_spinBox_texte_2_valueChanged(int arg1)
{

}

void MainWindow::on_spinBox_texte_3_valueChanged(int arg1)
{

}

void MainWindow::on_Girar_clicked()
{
  //QThread::msleep(10);
  //if(ui->verticalSlider_vel_L->value()==int(0)){
      //ui->verticalSlider_vel_L->setValue(100);
      //ui->spinBox_vel_L->setValue(100);
  //}
  //if(ui->verticalSlider_vel_R->value()==int(0)){
      //ui->verticalSlider_vel_R->setValue(-100);
      //ui->spinBox_vel_R->setValue(-100);
  //}
  //ui->verticalSlider_vel_R->setValue(100);
  //ui->verticalSlider_vel_L->setValue(100);
    int index = ui->Select_Robot->currentIndex(); //Seleciona o Index
    write_buf[2*index + 1] = converter_write(100); // VELOCIDADE ESQUERDA DO ROBÔ(ID)
    write_buf[2*index + 2] = converter_write(-100); // VELOCIDADE DIREITA DO ROBÔ(ID)
    //QThread::sleep(1); // 1 seg de pause
    write_Data(); // comando para girar
    QThread::sleep(1); // 1 seg de pause
    //if(this->serialPort->bytesAvailable()){
        read_Data();
    //}
    write_buf[2*index + 1] = converter_write(0); // VELOCIDADE ESQUERDA DO ROBÔ(ID)
    write_buf[2*index + 2] = converter_write(0); // VELOCIDADE DIREITA DO ROBÔ(ID)
    //ui->verticalSlider_vel_L->setValue(0); ui->spinBox_vel_L->setValue(0);
    //ui->verticalSlider_vel_R->setValue(0); ui->spinBox_vel_R->setValue(0);
    write_Data(); // comando para parar
    QThread::msleep(10); // 1 ms de pause
    //if(this->serialPort->bytesAvailable()){
        read_Data();
    //}
  //serialPort->waitForReadyRead(100);
  //QThread::msleep(5000);
//  pthread_cond_wait()
    /*
  serialPort->waitForBytesWritten(10);
  read_Data();
 // QThread::usleep(10);
 QThread::msleep(1000);
  ui->verticalSlider_vel_L->setValue(0);
  write_Data();
  //serialPort->waitForReadyRead(100);
 QThread::msleep(100);
  read_Data();*/
  // coment teste
}



void MainWindow::on_navegar_clicked()
{
    write_Data(); // comando para girar
    QThread::sleep(1); // 1 seg de pause
    //if(this->serialPort->bytesAvailable()){
        read_Data();
    //}
        //if(ui->verticalSlider_vel_L->SliderValueChange() || ui->verticalSlider_vel_R->SliderValueChange()){
          //  write_Data(); // atualiza comando
        //}
        //if(ui->spinBox_vel_L->valueChanged())
}
