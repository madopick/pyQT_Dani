"""
This is UI for project_23_02_15
This UI will accept 10 integer input (but it will accepted as String)
send it to the STM32 so the program can sparate and parse it to 10 integer.
This UI also show the output of STM32
The different between project_23_02_06 and this one is
This program will save the value on variabel so it will be callable anytime and static
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

def getValues():
    # ser.write(b'g')
    stmData = ser.readline().decode('ascii')
    print("Terima: ", stmData)
    return stmData

def sendValues(userInput):
    print("Kirim: ", userInput)
    ser.write(userInput.encode())
    stmData = ser.readline().decode('ascii')
    return stmData

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
        Form.setObjectName("qt")
        Form.resize(1200, 900)
        Form.setCursor(QtGui.QCursor(QtCore.Qt.ArrowCursor))
        Form.setWindowTitle("My Program")

        self.centralwidget = QtWidgets.QWidget(Form)
        self.centralwidget.setObjectName("centralwidget")
        self.gridLayoutWidget = QtWidgets.QWidget(self.centralwidget)
        self.gridLayoutWidget.setGeometry(QtCore.QRect(10, 310, 781, 381))
        self.gridLayoutWidget.setObjectName("gridLayoutWidget")
        self.gridLayout = QtWidgets.QGridLayout(self.gridLayoutWidget)
        self.gridLayout.setSizeConstraint(QtWidgets.QLayout.SetNoConstraint)
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.gridLayout.setObjectName("gridLayout")

        """############################################################################################################
        ############################################################################################################"""
        """HEADER AND CONNECT SERIAL"""
        self.frame_3 = QtWidgets.QFrame(Form)
        self.frame_3.setGeometry(QtCore.QRect(10, 20, 1086, 100))
        self.frame_3.setMinimumSize(QtCore.QSize(0, 100))
        self.frame_3.setMaximumSize(QtCore.QSize(16777215, 100))
        self.frame_3.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame_3.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame_3.setObjectName("frame_3")

        self.titleLabel = QtWidgets.QLabel(self.frame_3)
        self.titleLabel.setGeometry(QtCore.QRect(20, 10, 1061, 30))
        font = QtGui.QFont()
        font.setPointSize(12)
        font.setBold(True)
        font.setWeight(75)
        self.titleLabel.setFont(font)
        self.titleLabel.setText(
            "Program untuk Menerima Multi Input dari Beberapa Form ke Serial - Yohanes Vianney Dani")

        self.serial_label = QtWidgets.QLabel(self.frame_3)
        self.serial_label.setGeometry(QtCore.QRect(20, 50, 80, 35))
        self.serial_label.setText("Select Port:")
        self.cb_Port = QtWidgets.QComboBox(self.frame_3)
        self.cb_Port.setGeometry(QtCore.QRect(100, 50, 100, 35))
        self.cb_Port.setEditable(True)
        self.cb_Port.setCurrentText("")
        self.cb_Port.setObjectName("cb_Port")
        self.connectButton = QtWidgets.QPushButton(self.frame_3)
        self.connectButton.setGeometry(QtCore.QRect(210, 50, 200, 35))
        self.connectButton.setText("Connect to Serial")

        """HEADER AND CONNECT SERIAL ### END"""
        """############################################################################################################
        ############################################################################################################"""



        """############################################################################################################
        ############################################################################################################"""
        """INPUT FRAME"""
        self.inputFrame = QtWidgets.QFrame(Form)
        self.inputFrame.setGeometry(QtCore.QRect(10, 127, 1112, 513))
        self.inputFrame.setMaximumSize(QtCore.QSize(16777215, 550))
        self.inputFrame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.inputFrame.setFrameShadow(QtWidgets.QFrame.Raised)

        self.horizontalLayout = QtWidgets.QHBoxLayout(self.inputFrame)

        ### FORM 1 --- START ###
        self.formArea1 = QtWidgets.QFrame(self.inputFrame)
        self.verticalLayout1_1 = QtWidgets.QVBoxLayout(self.formArea1)
        self.form1Label = QtWidgets.QLabel(self.formArea1)
        self.form1Label.setText("Form 1")
        self.scrollArea1 = QtWidgets.QScrollArea(self.formArea1)
        self.scrollAreaWidgetContents1 = QtWidgets.QWidget()
        self.verticalLayout1_2 = QtWidgets.QVBoxLayout(self.scrollAreaWidgetContents1)
        self.insideScrollArea1 = QtWidgets.QFrame(self.scrollAreaWidgetContents1)

        # Input In form 1
        self.input1 = []
        self.inputLabel1 = []

        for i in range(10):
            self.input1.append(QtWidgets.QLineEdit(self.insideScrollArea1))
            self.inputLabel1.append(QtWidgets.QLabel(self.insideScrollArea1))
        # Input In form 1 -- END

        self.writeButton1 = QtWidgets.QPushButton(self.insideScrollArea1)
        self.writeButton1.clicked.connect(self.send_form1)

        self.readButton1 = QtWidgets.QPushButton(self.insideScrollArea1)
        self.readButton1.setText("Read Data")
        self.readButton1.setGeometry(QtCore.QRect(120, 810, 100, 50))
        self.readButton1.clicked.connect(self.readSerial1)

        self.input_area(self.formArea1, self.verticalLayout1_1, self.form1Label, self.scrollArea1,
                        self.scrollAreaWidgetContents1, self.verticalLayout1_2, self.insideScrollArea1,
                        self.writeButton1, self.input1, self.inputLabel1)
        self.horizontalLayout.addWidget(self.formArea1)


        ### FORM 1 --- END ###

        ### FORM 2 --- START ###
        self.formArea2 = QtWidgets.QFrame(self.inputFrame)
        self.verticalLayout2_1 = QtWidgets.QVBoxLayout(self.formArea2)
        self.form2Label = QtWidgets.QLabel(self.formArea2)
        self.form2Label.setText("Form 2")
        self.scrollArea2 = QtWidgets.QScrollArea(self.formArea2)
        self.scrollAreaWidgetContents2 = QtWidgets.QWidget()
        self.verticalLayout2_2 = QtWidgets.QVBoxLayout(self.scrollAreaWidgetContents2)
        self.insideScrollArea2 = QtWidgets.QFrame(self.scrollAreaWidgetContents2)

        # Input In form 2
        self.input2 = []
        self.inputLabel2 = []

        for i in range(10):
            self.input2.append(QtWidgets.QLineEdit(self.insideScrollArea2))
            self.inputLabel2.append(QtWidgets.QLabel(self.insideScrollArea2))
        # Input In form 2 -- END

        self.writeButton2 = QtWidgets.QPushButton(self.insideScrollArea2)
        self.writeButton2.clicked.connect(self.send_form2)

        self.readButton2 = QtWidgets.QPushButton(self.insideScrollArea2)
        self.readButton2.setText("Read Data")
        self.readButton2.setGeometry(QtCore.QRect(120, 810, 100, 50))
        self.readButton2.clicked.connect(self.readSerial2)

        self.input_area(self.formArea2, self.verticalLayout2_1, self.form2Label, self.scrollArea2,
                        self.scrollAreaWidgetContents2, self.verticalLayout2_2, self.insideScrollArea2,
                        self.writeButton2, self.input2, self.inputLabel2)
        self.horizontalLayout.addWidget(self.formArea2)
        ### FORM 2 --- END ###

        ### FORM 3 --- START ###
        self.formArea3 = QtWidgets.QFrame(self.inputFrame)
        self.verticalLayout3_1 = QtWidgets.QVBoxLayout(self.formArea3)
        self.form3Label = QtWidgets.QLabel(self.formArea3)
        self.form3Label.setText("Form 3")
        self.scrollArea3 = QtWidgets.QScrollArea(self.formArea3)
        self.scrollAreaWidgetContents3 = QtWidgets.QWidget()
        self.verticalLayout3_2 = QtWidgets.QVBoxLayout(self.scrollAreaWidgetContents3)
        self.insideScrollArea3 = QtWidgets.QFrame(self.scrollAreaWidgetContents3)

        # Input In form 3
        self.input3 = []
        self.inputLabel3 = []

        for i in range(10):
            self.input3.append(QtWidgets.QLineEdit(self.insideScrollArea3))
            self.inputLabel3.append(QtWidgets.QLabel(self.insideScrollArea3))
        # Input In form 3 -- END

        self.writeButton3 = QtWidgets.QPushButton(self.insideScrollArea3)
        self.writeButton3.clicked.connect(self.send_form3)

        self.readButton3 = QtWidgets.QPushButton(self.insideScrollArea3)
        self.readButton3.setText("Read Data")
        self.readButton3.setGeometry(QtCore.QRect(120, 810, 100, 50))
        self.readButton3.clicked.connect(self.readSerial3)

        self.input_area(self.formArea3, self.verticalLayout3_1, self.form3Label, self.scrollArea3,
                        self.scrollAreaWidgetContents3, self.verticalLayout3_2, self.insideScrollArea3,
                        self.writeButton3, self.input3, self.inputLabel3)
        self.horizontalLayout.addWidget(self.formArea3)
        ### FORM 3 --- END ###

        """INPUT FRAME ### END"""
        """############################################################################################################
        ############################################################################################################"""

        """############################################################################################################
        ############################################################################################################"""
        """OUTPUT FRAME"""

        self.outputFrame = QtWidgets.QFrame(Form)
        self.outputFrame.setGeometry(QtCore.QRect(10, 646, 1086, 200))
        self.outputFrame.setMinimumSize(QtCore.QSize(0, 200))
        self.outputFrame.setMaximumSize(QtCore.QSize(16777215, 200))
        self.outputFrame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.outputFrame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.outputFrame.setObjectName("outputFrame")

        self.clearButton = QtWidgets.QPushButton(self.outputFrame)
        self.clearButton.setText("Clear")
        self.clearButton.setGeometry(QtCore.QRect(30, 30, 100, 40))
        self.clearButton.clicked.connect(self.clear)

        self.saveTXTButton = QtWidgets.QPushButton(self.outputFrame)
        self.saveTXTButton.setText("Save to .txt")
        self.saveTXTButton.setGeometry(QtCore.QRect(30, 80, 100, 40))
        self.saveTXTButton.clicked.connect(self.saveToTXT)

        self.textEdit_3 = QtWidgets.QTextEdit(self.outputFrame)
        self.textEdit_3.setGeometry( QtCore.QRect(150, 10, 1000, 150))
        self.textEdit_3.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOn)
        self.textEdit_3.setObjectName("textEdit_3")

        self.command = QtWidgets.QLabel(self.outputFrame)
        self.command.setGeometry(QtCore.QRect(10, 180, 70, 30))
        self.command.setText("Commend: ")
        self.command_label = QtWidgets.QLabel(self.outputFrame)
        self.command_label.setGeometry(QtCore.QRect(85, 188, 200, 30))
        self.command_label.setText("")


        self.status = QtWidgets.QLabel(self.outputFrame)
        self.status.setGeometry(QtCore.QRect(850, 180, 70, 30))
        self.status.setText("Port Status: ")
        self.status_label = QtWidgets.QLabel(self.outputFrame)
        self.status_label.setGeometry(QtCore.QRect(925, 180, 120, 30))
        self.status_label.setText("Not Connected")

        QtCore.QMetaObject.connectSlotsByName(Form)

        """OUTPUT FRAME ### END"""
        """############################################################################################################
        ############################################################################################################"""

        Form.setCentralWidget(self.centralwidget)
        QtCore.QMetaObject.connectSlotsByName(Form)

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
            self.textEdit_3.append("{}".format(i))


    """TXT Save"""
    def saveToTXT(self):
        fileName = QFileDialog.getSaveFileName(self.textEdit_3, 'Select location to Log', "", '*.txt')
        if not fileName:
            return

        with open(fileName[0], 'w') as f:
            my_text = self.textEdit_3.toPlainText()
            f.write(my_text)

        self.textEdit_3.append("Log Saved in :" + fileName[0] + "\r\n")
        """self.saveTXTButton = True"""


    def clear(self):
        self.textEdit_3.setText('')

    """Function to UI"""
    def output_template(self, outputName, x, y):
        outputName.setGeometry(QtCore.QRect(x, y, 171, 51))
        outputName.setStyleSheet("background-color: rgb(255, 255, 255);")

    def input_template(self, labelName, areaName, y):
        labelName.setGeometry(QtCore.QRect(30, y, 260, 30))
        labelName.setMinimumSize(QtCore.QSize(0, 10))
        font = QtGui.QFont()
        font.setPointSize(10)
        font.setBold(True)
        font.setWeight(75)
        labelName.setFont(font)

        areaName.setGeometry(QtCore.QRect(10, y+30, 260, 30))

    def input_area(self, form, vertical, label, scrollArea, sAWidget, vertical2, inside, sendButton, groupInput, groupInputLabel):
        form.setMinimumSize(QtCore.QSize(350, 0))
        form.setFrameShape(QtWidgets.QFrame.StyledPanel)
        form.setFrameShadow(QtWidgets.QFrame.Raised)

        vertical.setObjectName("verticalLayout")

        label.setMinimumSize(QtCore.QSize(0, 31))
        label.setMaximumSize(QtCore.QSize(16777215, 31))
        font = QtGui.QFont()
        font.setPointSize(16)
        font.setBold(True)
        font.setWeight(75)
        label.setFont(font)

        vertical.addWidget(label)

        scrollArea.setMinimumSize(QtCore.QSize(0, 80))
        scrollArea.setMaximumSize(QtCore.QSize(16777215, 700))
        scrollArea.setWidgetResizable(True)

        sAWidget.setGeometry(QtCore.QRect(0, 0, 327, 922))

        inside.setMinimumSize(QtCore.QSize(0, 900))
        inside.setMaximumSize(QtCore.QSize(16777215, 900))
        inside.setFrameShape(QtWidgets.QFrame.StyledPanel)
        inside.setFrameShadow(QtWidgets.QFrame.Raised)

        for i in range (10):
            self.input_template(groupInputLabel[i], groupInput[i], 10+(i*80))
            groupInputLabel[i].setText("Input " + str(i+1))

        sendButton.setGeometry(QtCore.QRect(10, 810, 101, 51))
        sendButton.setText("Send Data")
        vertical2.addWidget(inside)
        scrollArea.setWidget(sAWidget)
        vertical.addWidget(scrollArea)

    def send_form1(self):
        mytext = "{CF1:"
        for i in range (len(self.input1)):
            mytext += self.input1[i].text()
            if(i<len(self.input1)-1):
                mytext += ";"
        mytext += "}\r\n"

        str = "Write to serial " + mytext
        self.command_label.setText(str)
        ser.write(mytext.encode())

    def send_form2(self):
        mytext = "{CF2:"
        for i in range(len(self.input2)):
            mytext += self.input2[i].text()
            if (i < len(self.input2) - 1):
                mytext += ";"
        mytext += "}\r\n"

        str = "Write to serial " + mytext
        self.command_label.setText(str)
        ser.write(mytext.encode())

    def send_form3(self):
        mytext = "{CF3:"
        for i in range(len(self.input3)):
            mytext += self.input3[i].text()
            if (i < len(self.input3) - 1):
                mytext += ";"
        mytext += "}\r\n"

        str = "Write to serial " + mytext
        self.command_label.setText(str)
        ser.write(mytext.encode())

    def readSerial1(self):
        mytext = "{RD1}\r\n"
        str = "Read to serial " + mytext
        self.command_label.setText(str)

        ser.write(mytext.encode())

    def readSerial2(self):
        mytext = "{RD2}\r\n"
        str = "Read to serial " + mytext
        self.command_label.setText(str)

        ser.write(mytext.encode())

    def readSerial3(self):
        mytext = "{RD3}\r\n"
        str = "Read to serial " + mytext
        self.command_label.setText(str)

        ser.write(mytext.encode())

def run():
    app = QApplication(sys.argv)
    qt = QMainWindow()
    ui = Ui_Form()
    ui.setupUi(qt)
    qt.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    run()