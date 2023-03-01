"""
This is UI for project_23_03_01
This UI will accept string input, send it to STM32Nucleo buffer.
Data will parsing into integer and will be send by I2C to another modul and it will be sand back
This UI also show the output.
"""

import sys, os, serial, serial.tools.list_ports, warnings
from PyQt5.QtCore import *
import time
from PyQt5.QtWidgets import *
from PyQt5.uic import loadUi
import serial.tools.list_ports as port_list
from PyQt5 import QtCore, QtGui, QtWidgets

result = []
portindex = 0

def serial_ports():
    global portindex

    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass

    ports = list(port_list.comports())
    for p in ports:
        if "STLink" in str(p):
            break
        else:
            portindex += 1
    return result

class Worker(QObject):
    finished = pyqtSignal()
    intReady = pyqtSignal(str)

    @pyqtSlot()
    def __init__(self):
        super(Worker, self).__init__()
        self.working = True

    def work(self):
        while self.working:
            if ser.isOpen():
                line = ser.readline().decode('utf-8')
                #line = ser.readline().decode('ascii')
            else:
                line = ''

            if line != '':
                #print(line)
                time.sleep(0.1)
                self.intReady.emit(line)

        self.finished.emit()


class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.setMinimumSize(QtCore.QSize(1000, 475))
        Form.setMaximumSize(QtCore.QSize(1000, 475))

        """############################################################################################################
        ############################################################################################################"""
        """HEADER AND CONNECT SERIAL"""
        self.header = QtWidgets.QFrame(Form)
        self.header.setGeometry(QtCore.QRect(0, 0, 1000, 100))
        self.header.setMinimumSize(QtCore.QSize(1000, 100))
        self.header.setMaximumSize(QtCore.QSize(16777215, 100))
        self.header.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.header.setFrameShadow(QtWidgets.QFrame.Raised)

        self.titleLabel = QtWidgets.QLabel(self.header)
        self.titleLabel.setGeometry(QtCore.QRect(20, 10, 1070, 30))
        font = QtGui.QFont()
        font.setPointSize(10)
        font.setBold(True)
        font.setWeight(75)
        self.titleLabel.setFont(font)
        self.titleLabel.setText(
            "Program I2C - Master sending data to slave and slave send back and compare the data - Yohanes Vianney Dani")

        self.serial_label = QtWidgets.QLabel(self.header)
        self.serial_label.setGeometry(QtCore.QRect(20, 50, 80, 35))
        self.serial_label.setText("Select Port:")
        self.cb_Port = QtWidgets.QComboBox(self.header)
        self.cb_Port.setGeometry(QtCore.QRect(100, 50, 100, 35))
        self.cb_Port.setEditable(True)
        self.cb_Port.setCurrentText("")
        self.cb_Port.setObjectName("cb_Port")
        self.connectButton = QtWidgets.QPushButton(self.header)
        self.connectButton.setGeometry(QtCore.QRect(210, 50, 200, 35))
        self.connectButton.setText("Connect to Serial")

        """HEADER AND CONNECT SERIAL ### END"""
        """############################################################################################################
        ############################################################################################################"""

        self.workFrame = QtWidgets.QFrame(Form)
        self.workFrame.setGeometry(QtCore.QRect(0, 100, 1000, 300))
        self.workFrame.setMinimumSize(QtCore.QSize(1000, 300))
        self.workFrame.setMaximumSize(QtCore.QSize(1000, 550))

        """############################################################################################################
        ############################################################################################################"""
        """INPUT FRAME"""
        self.inputFrame = QtWidgets.QFrame(self.workFrame)
        self.inputFrame.setGeometry(QtCore.QRect(12, 12, 500, 276))
        self.inputFrame.setMinimumSize(QtCore.QSize(500, 0))
        self.inputFrame.setMaximumSize(QtCore.QSize(500, 550))

        self.input_label = QtWidgets.QLabel(self.inputFrame)
        self.input_label.setGeometry(QtCore.QRect(12, 12, 161, 18))
        self.input_label.setMinimumSize(QtCore.QSize(0, 0))
        self.input_label.setMaximumSize(QtCore.QSize(16777215, 16777215))
        font = QtGui.QFont()
        font.setPointSize(9)
        font.setBold(True)
        font.setWeight(75)
        self.input_label.setFont(font)
        self.input_label.setText("INPUT TO SEND")

        self.input = QtWidgets.QLineEdit(self.inputFrame)
        self.input.setGeometry(QtCore.QRect(10, 40, 450, 50))
        self.input.setMinimumSize(QtCore.QSize(450, 50))
        self.input.setMaximumSize(QtCore.QSize(450, 16777215))

        self.writeButton = QtWidgets.QPushButton(self.inputFrame)
        self.writeButton.setGeometry(QtCore.QRect(10, 110, 130, 50))
        self.writeButton.setText("WRITE")
        self.writeButton.clicked.connect(self.write)

        self.readButton = QtWidgets.QPushButton(self.inputFrame)
        self.readButton.setGeometry(QtCore.QRect(150, 110, 130, 50))
        self.readButton.setText("READ")
        self.readButton.clicked.connect(self.read)

        self.clearButton = QtWidgets.QPushButton(self.inputFrame)
        self.clearButton.setGeometry(QtCore.QRect(290, 110, 130, 50))
        self.clearButton.setText("CLEAR")
        self.clearButton.clicked.connect(self.clear)
        """INPUT FRAME ### END"""
        """############################################################################################################
        ############################################################################################################"""

        """############################################################################################################
        ############################################################################################################"""
        """OUTPUT FRAME"""
        self.outputFrame = QtWidgets.QFrame(self.workFrame)
        self.outputFrame.setGeometry(QtCore.QRect(504, 12, 500, 276))
        self.outputFrame.setMinimumSize(QtCore.QSize(500, 0))
        self.outputFrame.setMaximumSize(QtCore.QSize(500, 550))

        self.output = QtWidgets.QTextEdit(self.outputFrame)
        self.output.setGeometry(QtCore.QRect(10, 50, 460, 200))
        self.output.setAlignment(QtCore.Qt.AlignLeading | QtCore.Qt.AlignLeft | QtCore.Qt.AlignTop)
        self.output.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOn)

        self.output_label = QtWidgets.QLabel(self.outputFrame)
        self.output_label.setGeometry(QtCore.QRect(12, 12, 171, 31))
        font = QtGui.QFont()
        font.setPointSize(9)
        font.setBold(True)
        font.setWeight(75)
        self.output_label.setFont(font)
        self.output_label.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignVCenter)
        self.output_label.setText("OUTPUT")
        """OUTPUT FRAME ### END"""
        """############################################################################################################
        ############################################################################################################"""

        """############################################################################################################
        ############################################################################################################"""
        """FOOTER"""
        self.footer = QtWidgets.QFrame(Form)
        self.footer.setGeometry(QtCore.QRect(0, 400, 1000, 71))
        self.footer.setMinimumSize(QtCore.QSize(1000, 0))
        self.footer.setMaximumSize(QtCore.QSize(16777215, 550))

        self.command = QtWidgets.QLabel(self.footer)
        self.command.setGeometry(QtCore.QRect(20, 10, 70, 30))
        self.command.setText("Commend: ")
        self.command_label = QtWidgets.QLabel(self.footer)
        self.command_label.setGeometry(QtCore.QRect(95, 10, 200, 30))
        self.command_label.setText("tes")

        self.status = QtWidgets.QLabel(self.footer)
        self.status.setGeometry(QtCore.QRect(750, 10, 70, 30))
        self.status.setText("Port Status: ")
        self.status_label = QtWidgets.QLabel(self.footer)
        self.status_label.setGeometry(QtCore.QRect(825, 10, 120, 30))
        self.status_label.setText("Not Connected")
        """FOOTER ### END"""
        """############################################################################################################
        ############################################################################################################"""

        self.thread = None
        self.worker = None
        self.connectButton.clicked.connect(self.start_loop)
        self.pushBtnClicked = False
        self.CopyFlag = 0
        self.ConnectStatus = 0
        self.Port = "UART"

        # global result
        print("Available ports:" + str(serial_ports()))
        for x in range(len(result)):
            name = result[x]
            self.cb_Port.addItem(name)
        self.cb_Port.addItem("USB")

    def stop_loop(self):
        #print("Serial has been disconnected!!")
        self.worker.working = False
        self.connectButton.setText("Connect to Serial")
        self.status_label.setText("Not Connected")
        ser.close()


    def start_loop(self):
        global ser

        #Disconnect
        if(self.ConnectStatus == 1):
            self.ConnectStatus = 0
            self.stop_loop()
            return

        #Verify the correct COM Port
        try:
            mytext = "HELP\r\n"  # Send first enter

            if(self.cb_Port.currentText() == "USB"):
                print("USB HID Communication")
                self.Port = "USB"
            else:
                self.Port = "UART"
                ser = serial.Serial(self.cb_Port.currentText(), 115200, timeout=1)
                ser.write(mytext.encode())

            if(self.ConnectStatus == 0):
                self.ConnectStatus = 1
                self.connectButton.setText("Disconnect from Serial")

        except:
            msgBox = QMessageBox()
            msgBox.setWindowTitle("COM Port Error!")
            msgBox.setIcon(QMessageBox.Warning)
            msgBox.setText("Selected COM port does not exist. Please verify the COM port Number.")
            msgBox.exec()
            self.label_5.setText("Not Connected")
            self.label_5.setStyleSheet('color: red')
            return

        if (self.Port == "UART"):
            self.worker = Worker()
            self.thread = QThread()
            self.worker.moveToThread(self.thread)

            self.thread.started.connect(self.worker.work)

            self.worker.intReady.connect(self.onIntReady)

            self.worker.finished.connect(self.thread.quit)
            self.worker.finished.connect(self.worker.deleteLater)
            self.thread.finished.connect(self.thread.deleteLater)
            self.thread.start()
            self.status_label.setText("Connected to Serial")

        else:
            print("USB Thread here")

    def onIntReady(self, i):
        #print('SerialAlwaysRead')
        if i != '':
            self.output.append("{}".format(i))

    def clear(self):
        self.output.setText('')

    def write(self):
        mytext = "{WR1:"
        mytext += self.input.text()
        mytext += "}\r\n"

        str = "Write to serial " + mytext
        self.command_label.setText(str)
        ser.write(mytext.encode())

    def read(self):
        mytext = "{RD1}\r\n"
        str = "Read to serial " + mytext
        self.command_label.setText(str)

        ser.write(mytext.encode())

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    Form = QtWidgets.QWidget()
    ui = Ui_Form()
    ui.setupUi(Form)
    Form.show()
    sys.exit(app.exec_())
