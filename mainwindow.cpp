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
    ui->label->setText(""); //limpando label
    //ui->Baud_Rate->setText(""); //limpando label do Baud Rate

    MainWindow::graph_config();

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
        //ui->label->setText("Desconectado");
        ui->Connect_Disconect->setText("Conectar");
        //ui->Baud_Rate->setText(""); //limpando label do Baud Rate
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
    this->serialPort->QSerialPort::setReadBufferSize(11); // Definição do tamanho do buffer de entrada
    }

    if(this->serialPort->open(QIODevice::ReadWrite)){ // Verificação se a porta realmente abriu        
        /*ui->label->setText("Conectado");
        ui->Baud_Rate->setText("Communication Paramaters\n"
                               "Baudrate: 115200\n"
                               "Data bits: 8\n"
                               "Parity: None\n"
                               "Stop bits: 1\n"
                               "Flow control: None");
        */
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
        this->serialPort->write(write_buf3); // ENVIA OS DADOS
        //this->serialPort->write(write_buf);
       //qint64 i = QIODevice::writeData(const char 'a',11);
    }

    // PROCEDIMENTOS PARA EXIBIÇÃO NA INTERFACE
    //const QString texto = write_buf3;
    //ui->lineEdit->setText(texto);

    if(int(write_buf3.at(0) >= 0)){
        //ui->spinBox_texte->setValue((int)write_buf3.at(0));
        ui->lcdNumber_id->display((int)write_buf3.at(0));
    }
    else{
        //ui->spinBox_texte->setValue(-(128 + (int)write_buf3.at(0)));
        //ui->spinBox_texte->setValue((int)(unsigned char)write_buf3.at(1));
        ui->lcdNumber_id->display(-(128 + (int)write_buf3.at(0)));
    }

    //ui->lcdNumber_L1->display(converter_read(write_buf3.at(0)));
    ui->lcdNumber_L1->display(converter_read(write_buf3.at(1)));
    ui->lcdNumber_R1->display(converter_read(write_buf3.at(2)));
    ui->lcdNumber_L2->display(converter_read(write_buf3.at(3)));
    ui->lcdNumber_R2->display(converter_read(write_buf3.at(4)));
    ui->lcdNumber_L3->display(converter_read(write_buf3.at(5)));
    ui->lcdNumber_R3->display(converter_read(write_buf3.at(6)));
    ui->lcdNumber_L4->display(converter_read(write_buf3.at(7)));
    ui->lcdNumber_R4->display(converter_read(write_buf3.at(8)));
    ui->lcdNumber_L5->display(converter_read(write_buf3.at(9)));
    ui->lcdNumber_R5->display(converter_read(write_buf3.at(10)));


}


void MainWindow::read_Data()
{

    //int index = ui->Select_Robot->currentIndex();
    //const QByteArray datareceive = this->serialPort->readAll();
    if (flag_comunicacao)
    {
        /*
        const QString read_id2 =(QString)'10';
        while(this->serialPort->readBufferSize()<1){} //Para aguardar o buffer chegar
        while(this->serialPort->bytesAvailable()>=1 && read_id2 != converter_write (20)){
                QByteArray read_id= this->serialPort->read(1);
                const QString read_id2 = read_id;
                //qint64 teste=this->serialPort->readBufferSize();
                //ui->lcdNumber_id_r->display((int)teste);
                //if(read_id2 == '20'){
                    //ui->lcdNumber_id_r->display(read_id2);
                //}
           }
        */
        while(this->serialPort->readBufferSize()<11){serialPort->waitForReadyRead(3);}

        if(this->serialPort->bytesAvailable()>=11 ){
            ui->lcdNumber_id_r->display((int)this->serialPort->bytesAvailable());
            QByteArray read_buf = this->serialPort->readAll();
            //const QString read_id2 =QByteArray::data( read_buf(0));
            //if(read_buf(0) =='5'){
              //  ui->lcdNumber_id_r->display((int)5);
            //}
            //ui->lcdNumber_id_r->display((int)read_buf.size());
            //read_buf[0]=(const char)read_id2;


            //QByteArray read_buf = this->serialPort->read(10);
            //const QString texto = read_buf;
            //ui->lineEdit_2->setText(texto);



            /*
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
            */



            /*
            if(int(read_buf.at(0) >= 0)){
                ui->lcdNumber_id_r->display((int)read_buf.at(0));
            }
            else{
                ui->lcdNumber_id_r->display(-(128 + (int)read_buf.at(0)));
            }
            */
            //ui->lcdNumber_id_r->display(converter_read(read_buf.at(0)));
            ui->lcdNumber_L1_r->display(converter_read(read_buf.at(1)));
            ui->lcdNumber_R1_r->display(converter_read(read_buf.at(2)));
            ui->lcdNumber_L2_r->display(converter_read(read_buf.at(3)));
            ui->lcdNumber_R2_r->display(converter_read(read_buf.at(4)));
            ui->lcdNumber_L3_r->display(converter_read(read_buf.at(5)));
            ui->lcdNumber_R3_r->display(converter_read(read_buf.at(6)));
            ui->lcdNumber_L4_r->display(converter_read(read_buf.at(7)));
            ui->lcdNumber_R4_r->display(converter_read(read_buf.at(8)));
            ui->lcdNumber_L5_r->display(converter_read(read_buf.at(9)));
            ui->lcdNumber_R5_r->display(converter_read(read_buf.at(10)));





            //Atualização do plot
            int index = ui->Select_Robot->currentIndex(); //Seleciona o Index
            yplot1.pop_front(); //Removi o primeiro elemento
            yplot1.push_back(converter_read(read_buf[2*index + 1])); //Adicionei um novo elemento no final
            yplot2.pop_front(); //Removi o primeiro elemento
            yplot2.push_back(converter_read(read_buf[2*index + 2])); //Adicionei um novo elemento no final
            ui->widget->graph(0)->setData(xplot, yplot1);
            ui->widget->graph(1)->setData(xplot, yplot2);
            ui->widget->replot();

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
    int index = ui->Select_Robot->currentIndex(); //Seleciona o Index
    write_buf[2*index + 1] = converter_write(ui->verticalSlider_vel_L->value()); // VELOCIDADE ESQUERDA DO ROBÔ(ID)
    write_buf[2*index + 2] = converter_write(ui->verticalSlider_vel_R->value()); // VELOCIDADE DIREITA DO ROBÔ(ID)
    write_Data();

}


// Controle de Velocidade para Teste
void MainWindow::on_spinBox_vel_R_valueChanged(int arg1)
{
    ui->verticalSlider_vel_R->setValue(arg1);

    if(flag_navegacao==1){   // Atualiza a velocidade da navegação
        int index = ui->Select_Robot->currentIndex(); //Seleciona o Index
        write_buf[2*index + 1] = converter_write(ui->verticalSlider_vel_L->value()); // VELOCIDADE ESQUERDA DO ROBÔ(ID)
        write_buf[2*index + 2] = converter_write(ui->verticalSlider_vel_R->value()); // VELOCIDADE DIREITA DO ROBÔ(ID)
        serialPort->flush();
        write_Data();
        QThread::msleep(100); // 1 ms de pause
        //serialPort->waitForReadyRead(100);
        //read_Data();
        read_Data();
    }
}

void MainWindow::on_spinBox_vel_L_valueChanged(int arg1)
{
    ui->verticalSlider_vel_L->setValue(arg1);

    if(flag_navegacao==1){  // Atualiza a velocidade da navegação
        int index = ui->Select_Robot->currentIndex(); //Seleciona o Index
        write_buf[2*index + 1] = converter_write(ui->verticalSlider_vel_L->value()); // VELOCIDADE ESQUERDA DO ROBÔ(ID)
        write_buf[2*index + 2] = converter_write(ui->verticalSlider_vel_R->value()); // VELOCIDADE DIREITA DO ROBÔ(ID)
        serialPort->flush();
        write_Data();
        QThread::msleep(100); // 1 ms de pause
        //serialPort->waitForReadyRead(100);
        //read_Data();
        read_Data();
    }
}

void MainWindow::on_verticalSlider_vel_R_valueChanged(int value)
{
    ui->spinBox_vel_R->setValue(value);

    if(flag_navegacao==1){  // Atualiza a velocidade da navegação
        int index = ui->Select_Robot->currentIndex(); //Seleciona o Index
        write_buf[2*index + 1] = converter_write(ui->verticalSlider_vel_L->value()); // VELOCIDADE ESQUERDA DO ROBÔ(ID)
        write_buf[2*index + 2] = converter_write(ui->verticalSlider_vel_R->value()); // VELOCIDADE DIREITA DO ROBÔ(ID)
        serialPort->flush();
        write_Data();
        QThread::msleep(100); // 1 ms de pause
        //serialPort->waitForReadyRead(100);
        //read_Data();
        read_Data();
    }
}

void MainWindow::on_verticalSlider_vel_L_valueChanged(int value)
{
    ui->spinBox_vel_L->setValue(value);

    if(flag_navegacao==1){   // Atualiza a velocidade da navegação
        int index = ui->Select_Robot->currentIndex(); //Seleciona o Index
        write_buf[2*index + 1] = converter_write(ui->verticalSlider_vel_L->value()); // VELOCIDADE ESQUERDA DO ROBÔ(ID)
        write_buf[2*index + 2] = converter_write(ui->verticalSlider_vel_R->value()); // VELOCIDADE DIREITA DO ROBÔ(ID)
        serialPort->flush();
        write_Data();
        QThread::msleep(100); // 1 ms de pause
        //serialPort->waitForReadyRead(100);
        //read_Data();
        read_Data();
    }
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



void MainWindow::on_Girar_clicked()
{
    //int index = ui->Select_Robot->currentIndex(); //Seleciona o Index
    write_buf[0] = (unsigned char) (46); // ID DO ROBÔ QUE RECEBERÁ A MENSAGEM
    //write_buf[0] = converter_write(249);
    for(int i=0; i<5;i++){
        write_buf[(2*i) + 1] = converter_write(100); // VELOCIDADE ESQUERDA DO ROBÔ(ID)
        write_buf[(2*i) + 2] = converter_write(-100); // VELOCIDADE DIREITA DO ROBÔ(ID)
    }
    //QThread::msleep(100); // 1 seg de pause
    //serialPort->flush();
    write_Data(); // comando para girar
    //serialPort->waitForReadyRead(50);
    //QThread::msleep(50); // 1 ms de pause
    //if(this->serialPort->clear(QSerialPort::AllDirections)){
    //read_Data();
    QThread::sleep(1); // 1 seg de pause TEMPO EM QUE O ROBÔ FICA GIRANDO
    for(int i=0; i<11;i++){
        if(i==0){
            write_buf[0] = (unsigned char) (46); // ID DO ROBÔ QUE RECEBERÁ A MENSAGEM
            //write_buf[0] = converter_write(250);
        }
        else{
            write_buf[i] = C0; // ZERA CAMPOS DO BUFFER DE ESCRITA QUE NÃO SÃO DE INTERESSE
        }
    }
    //serialPort->flush();
    write_Data(); // comando para parar
    //serialPort->waitForReadyRead(50);
    //QThread::msleep(50); // 1 ms de pause
    //if(this->serialPort->clear(QSerialPort::AllDirections)){
    //read_Data();
    //}
}



void MainWindow::on_navegar_clicked()
{
    float inicio;
    float fim;
    float tempo_medido = 0.0;
    //while(tempo_medido<5.0){
    if(flag_navegacao==0){
        ui->navegar->setText("Parar navegação");
        flag_navegacao = 1;

        int index = ui->Select_Robot->currentIndex(); //Seleciona o Index
        write_buf[2*index + 1] = converter_write(ui->verticalSlider_vel_L->value()); // VELOCIDADE ESQUERDA DO ROBÔ(ID)
        write_buf[2*index + 2] = converter_write(ui->verticalSlider_vel_R->value()); // VELOCIDADE DIREITA DO ROBÔ(ID)
        inicio = clock();
        serialPort->flush();
        write_Data();
        QThread::msleep(100); // 1 ms de pause
        //serialPort->waitForReadyRead(100);
        read_Data();
        read_Data();
    }
        else
    {
        ui->navegar->setText("Navegar");
        flag_navegacao = 0;
    }


            //if(ui->verticalSlider_vel_L->SliderValueChange() || ui->verticalSlider_vel_R->SliderValueChange()){
              //  write_Data(); // atualiza comando
            //}
            //if(ui->spinBox_vel_L->valueChanged())
        fim = clock();
        tempo_medido = tempo_medido +(fim-inicio);
    //}
}


void MainWindow::graph_config(){
    QVector<double> x(101), y(101); // initialize with entries 0..100

        xplot = x;
        yplot1 = y;
        yplot2 = y;

        for (int i=0; i<101; ++i)
        {
          xplot[i] = i; // Inicializando
          yplot1[i] = 0;
          yplot2[i] = 0;
        }
        // create graph and assign data to it:
        ui->widget->addGraph();

        ui->widget->addGraph();
        //ui->widget->graph(1)->setData(x, x);
        ui->widget->graph(1)->setPen(QPen(Qt::red));
        // give the axes some labels:
        ui->widget->yAxis->setLabel("Vel");
        // set axes ranges, so we see all data:
        ui->widget->xAxis->setRange(0, 100);
        ui->widget->yAxis->setRange(-105, 105);
        //tirando o label inferior
        ui->widget->xAxis->setVisible(false);
        ui->widget->replot();

}
