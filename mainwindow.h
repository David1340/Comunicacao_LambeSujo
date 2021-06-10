#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QStringList>
#include <string.h>
#include <QTimer>
#include <stdio.h>
#include <QThread>
//Só é necessário para a comunicação Serial o QSerialPortInfo e o QSerialPort

/* MANDAR VELOCIDADE NULA SEM PRECISAR DO CAST SEMPRE */

//#define C0 (unsigned char)0
#define C0 (char)0
//#define C0 '0'
/* DEFINES RELATIVOS À POSIÇÃO NO BUFFER DAS VELOCIDADES ENVIADAS */

#define L1 1
#define R1 2
#define L2 3
#define R2 4
#define L3 5
#define R3 6
#define L4 7
#define R4 8
#define L5 9
#define R5 10


namespace Ui {
class MainWindow;
}

class QSerialPort;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QSerialPort *serialPort; //reservando mémoria para um objeto do tipo QSerialPORT

    unsigned char converter_write (int x);
    int converter_read (unsigned char c);
    void enviar_buffer (void);
    void ler_buffer (void);


private slots:
    void on_Connect_Disconect_clicked();

    void on_comboBox_activated(int index);

    void on_CheckSerial_Button_clicked();

    void on_Read_clicked();

    void on_Write_clicked();

    void on_spinBox_vel_R_valueChanged(int arg1);

    void on_verticalSlider_vel_R_valueChanged(int value);

    void on_spinBox_vel_L_valueChanged(int arg1);

    void on_verticalSlider_vel_L_valueChanged(int value);



    void on_spinBox_texte_valueChanged(int arg1);

    void on_spinBox_texte_2_valueChanged(int arg1);

    void on_Select_Robot_activated(int index);

    void on_Girar_clicked();

    void on_spinBox_texte_3_valueChanged(int arg1);

public slots:
    void connectToSerial();
    void checkSerial();
    void closeSerialPort();
    //void write_Data(const QByteArray &data);
    void write_Data();
    void read_Data();
    void initActionsConnections();
    void showStatusMessage(const QString &message);

private:
    Ui::MainWindow *ui;
    bool flag_comunicacao = 0; // quando 1 indica comunicação ativa
    int flag_robos = 0; // 0 = nenhum, 1-5 = robô 1-5
    unsigned char write_buf[11] = {C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0}; // buffer de escrita inicado com velocidades zero
    unsigned char read_buf[11] = {C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0};  // buffer de leitura inicado com velocidades zero
    const QByteArray read_buf2;
    QTimer *timer;

};

#endif // MAINWINDOW_H
