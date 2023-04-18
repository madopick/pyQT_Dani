"""
STM32Nucleo buffer
This is UI for project_23_04_17
UI for SPI multi input. For the first Connected to Serial, the form will show the default value from Slave
This UI will accept integer input, send it to Master as Full Byte.
Data will be sent by SPI to Slave, and it will be saved.
This UI also show the output from Slave when asked.
"""

import sys, os, serial, serial.tools.list_ports, warnings
from PyQt5.QtCore import *
from PyQt5.QtGui import QColor
import time
from PyQt5.QtWidgets import *
from PyQt5.uic import loadUi
from PyQt5.QtGui import QIntValidator
import serial.tools.list_ports as port_list
from PyQt5 import QtCore, QtGui, QtWidgets
import csv
import numpy as np
import pandas as pd

result = []
portindex = 0

str_cfg_header = [
    "{WB1:", "{WB2:", "{WB3:",
    "{RBA}",
	"{RD0}",
	"{RB1}", "{RB2}", "{RB3}"
]

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

def integer_to_byte(num):
    data = [0, 0, 0, 0]
    if (num > 0):
        data = [(num % 256), int(num / 256) % 256, int(num / 256 / 256) % 256, int(num / 256 / 256 / 256) % 128]
    elif (num < 0):
        temp = abs(num) - 1
        data = [255 - (temp % 256), 255 - int(temp / 256) % 256, 255 - int(temp / 256 / 256) % 256,
                128 - int(temp / 256 / 256 / 256) % 128 + 127]

    return data

class Worker(QObject):
    finished = pyqtSignal()
    intReady = pyqtSignal(str)
    byteReady = pyqtSignal(list)

    @pyqtSlot()
    def __init__(self):
        super(Worker, self).__init__()
        self.working = True

    def work_byte(self):
        while self.working:
            if ser.isOpen():
                line = ser.read(122)        # Setting length of byte data
            else:
                line = b''

            if line != b'':
                time.sleep(0.1)
                databyte = list(line)
                self.byteReady.emit(databyte)

        self.finished.emit()

class Ui_Form(object):
    def setupUi(self, Form):
        self.thread = None
        self.worker = None
        self.pushBtnClicked = False
        self.CopyFlag = 0
        self.ConnectStatus = 0
        self.Port = "UART"
        self.getAll = 0
        self.sendAll = 0
        self.data = [[],
                     [],
                     [],
                     []]
        self.numberInput = 10

        onlyInt = QIntValidator()
        onlyInt.setRange(-2147483640, 2147483640)

        """############################################################################################################
        ############################################################################################################"""
        """ UI Start """
        Form.setObjectName("I2C")
        Form.setMinimumSize(1200, 900)
        Form.setMaximumSize(1200, 900)
        Form.setCursor(QtGui.QCursor(QtCore.Qt.ArrowCursor))
        Form.setWindowTitle("I2C - Master sending data to slave and slave send back and compare the data")

        """############################################################################################################
        ############################################################################################################"""
        """HEADER AND CONNECT SERIAL"""
        self.header = QtWidgets.QFrame(Form)
        self.header.setGeometry(QtCore.QRect(10, 20, 1200, 100))
        self.header.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.header.setFrameShadow(QtWidgets.QFrame.Raised)
        self.header.setObjectName("header")

        self.serial_label = QtWidgets.QLabel(self.header)
        self.serial_label.setGeometry(QtCore.QRect(20, 0, 80, 35))
        self.serial_label.setText("Select Port:")
        self.cb_Port = QtWidgets.QComboBox(self.header)
        self.cb_Port.setGeometry(QtCore.QRect(100, 0, 100, 30))
        self.cb_Port.setEditable(True)
        self.cb_Port.setCurrentText("")
        self.cb_Port.setObjectName("cb_Port")
        self.connectButton = QtWidgets.QPushButton(self.header)
        self.connectButton.setStyleSheet("background-color: rgb(150, 255, 170);\n"
                                         "color: rgb(50, 50, 50);")
        self.connectButton.setFont(self.setFontStyle(7, weight=60))
        self.connectButton.setGeometry(QtCore.QRect(210, 0, 160, 30))
        self.connectButton.setText("Connect to Serial")
        self.connectButton.clicked.connect(self.start_loop)

        self.openButton = QtWidgets.QPushButton(self.header)
        self.openButton.setText("Open")
        self.openButton.setGeometry(QtCore.QRect(390, 0, 80, 30))
        self.openButton.setDisabled(True)
        self.openButton.clicked.connect(self.open_file)

        self.generalCmd1Label = QtWidgets.QLabel(self.header)
        self.generalCmd1Label.setText("Command 1: ")
        self.generalCmd1 = QtWidgets.QLineEdit(self.header)

        self.generalCmd1SendButton = QtWidgets.QPushButton(self.header)
        self.generalCmd1SendButton.clicked.connect(self.sendCmd1)

        self.generalCmd1ClearButton = QtWidgets.QPushButton(self.header)
        self.generalCmd1ClearButton.clicked.connect(self.clearCmd1)

        self.general_command(self.generalCmd1Label, self.generalCmd1, self.generalCmd1SendButton, self.generalCmd1ClearButton, 670, 0)

        self.generalCmd2Label = QtWidgets.QLabel(self.header)
        self.generalCmd2Label.setText("Command 2: ")
        self.generalCmd2 = QtWidgets.QLineEdit(self.header)

        self.generalCmd2SendButton = QtWidgets.QPushButton(self.header)
        self.generalCmd2SendButton.clicked.connect(self.sendCmd2)
        self.generalCmd2ClearButton = QtWidgets.QPushButton(self.header)
        self.generalCmd2ClearButton.clicked.connect(self.clearCmd2)

        self.general_command(self.generalCmd2Label, self.generalCmd2, self.generalCmd2SendButton, self.generalCmd2ClearButton, 670, 40)

        """HEADER AND CONNECT SERIAL ### END"""
        """############################################################################################################
        ############################################################################################################"""

        self.scroll = QtWidgets.QScrollArea(Form)
        self.scroll.setGeometry(QtCore.QRect(40, 130, 1100, 470))
        self.workFrame = QtWidgets.QWidget()
        self.workFrame.setGeometry(QtCore.QRect(0, 0, 5000, 440))
        self.scroll.setWidget(self.workFrame)

        """############################################################################################################
        ############################################################################################################"""
        """INPUT FRAME"""

        """### FORM 1 --- START ###"""
        self.formArea1 = QtWidgets.QFrame(self.workFrame)
        self.formArea1.setGeometry(QtCore.QRect(0, 0, 350, 450))

        self.form1Label = QtWidgets.QLabel(self.formArea1)
        self.form1Label.setText("Form 1")
        self.scrollArea1 = QtWidgets.QScrollArea(self.formArea1)
        self.scrollAreaWidgetContents1 = QtWidgets.QWidget()

        # Input In form 1
        self.input1 = []
        self.inputLabel1 = []

        for i in range(self.numberInput):
            self.input1.append(QtWidgets.QLineEdit(self.scrollAreaWidgetContents1))
            self.inputLabel1.append(QtWidgets.QLabel(self.scrollAreaWidgetContents1))
            self.input1[i].setValidator(onlyInt)            #only integer input
        # Input In form 1 -- END

        self.writeButton1 = QtWidgets.QPushButton(self.scrollAreaWidgetContents1)
        self.writeButton1.clicked.connect(self.write1)

        self.readButton1 = QtWidgets.QPushButton(self.scrollAreaWidgetContents1)
        self.readButton1.clicked.connect(self.read1)

        self.input_area(self.form1Label, self.scrollArea1,
                        self.scrollAreaWidgetContents1,
                        self.writeButton1, self.readButton1, self.input1, self.inputLabel1)
        """### FORM 1 --- END ###"""

        """### FORM 2 --- START ###"""
        self.formArea2 = QtWidgets.QFrame(self.workFrame)
        self.formArea2.setGeometry(QtCore.QRect(350, 0, 350, 450))

        self.form2Label = QtWidgets.QLabel(self.formArea2)
        self.form2Label.setText("Form 2")

        self.scrollArea2 = QtWidgets.QScrollArea(self.formArea2)
        self.scrollAreaWidgetContents2 = QtWidgets.QWidget()

        # Input In form 2
        self.input2 = []
        self.inputLabel2 = []

        for i in range(self.numberInput):
            self.input2.append(QtWidgets.QLineEdit(self.scrollAreaWidgetContents2))
            self.inputLabel2.append(QtWidgets.QLabel(self.scrollAreaWidgetContents2))
            self.input2[i].setValidator(onlyInt)  # only integer input
        # Input In form 2 -- END

        self.writeButton2 = QtWidgets.QPushButton(self.scrollAreaWidgetContents2)
        self.writeButton2.clicked.connect(self.write2)

        self.readButton2 = QtWidgets.QPushButton(self.scrollAreaWidgetContents2)
        self.readButton2.clicked.connect(self.read2)

        self.input_area(self.form2Label, self.scrollArea2,
                        self.scrollAreaWidgetContents2,
                        self.writeButton2, self.readButton2, self.input2, self.inputLabel2)
        """ ### FORM 2 --- END ### """

        """ ### FORM 3 --- START ### """
        self.formArea3 = QtWidgets.QFrame(self.workFrame)
        self.formArea3.setGeometry(QtCore.QRect(700, 0, 350, 450))

        self.form3Label = QtWidgets.QLabel(self.formArea3)
        self.form3Label.setText("Form 3")

        self.scrollArea3 = QtWidgets.QScrollArea(self.formArea3)
        self.scrollAreaWidgetContents3 = QtWidgets.QWidget()

        # Input In form 3
        self.input3 = []
        self.inputLabel3 = []

        for i in range(self.numberInput):
            self.input3.append(QtWidgets.QLineEdit(self.scrollAreaWidgetContents3))
            self.inputLabel3.append(QtWidgets.QLabel(self.scrollAreaWidgetContents3))
            self.input1[i].setValidator(onlyInt)  # only integer input
        # Input In form 3 -- END

        self.writeButton3 = QtWidgets.QPushButton(self.scrollAreaWidgetContents3)
        self.writeButton3.clicked.connect(self.write3)

        self.readButton3 = QtWidgets.QPushButton(self.scrollAreaWidgetContents3)
        self.readButton3.clicked.connect(self.read3)

        self.input_area(self.form3Label, self.scrollArea3,
                        self.scrollAreaWidgetContents3,
                        self.writeButton3, self.readButton3, self.input3, self.inputLabel3)
        """ ### FORM 3 --- END ### """

        """INPUT FRAME ### END"""
        """############################################################################################################
        ############################################################################################################"""

        """############################################################################################################
        ############################################################################################################"""
        """OUTPUT FRAME"""
        self.outputFrame = QtWidgets.QFrame(Form)
        self.outputFrame.setGeometry(QtCore.QRect(5, 600, 1200, 250))
        self.outputFrame.setMinimumSize(QtCore.QSize(0, 250))
        self.outputFrame.setMaximumSize(QtCore.QSize(16777215, 250))

        self.output = QtWidgets.QTextEdit(self.outputFrame)
        self.output.setGeometry(QtCore.QRect(150, 60, 1000, 200))
        self.output.setAlignment(QtCore.Qt.AlignLeading | QtCore.Qt.AlignLeft | QtCore.Qt.AlignTop)
        self.output.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOn)

        self.output_CSV = QtWidgets.QLineEdit(self.outputFrame)
        self.output_CSV.setGeometry(QtCore.QRect(150, 60, 0, 0))


        self.sendAllButton = QtWidgets.QPushButton(self.outputFrame)
        self.sendAllButton.setText("Send All Input")
        self.sendAllButton.setGeometry(QtCore.QRect(30, 10, 150, 40))
        self.sendAllButton.setStyleSheet("background-color: rgb(225, 250, 255);\n"
                                   "color: rgb(50, 50, 50);")
        self.sendAllButton.clicked.connect(self.writeAll)

        self.readAllButton = QtWidgets.QPushButton(self.outputFrame)
        self.readAllButton.setText("Read All Data")
        self.readAllButton.setGeometry(QtCore.QRect(200, 10, 150, 40))
        self.readAllButton.setStyleSheet("background-color: rgb(225, 250, 255);\n"
                                             "color: rgb(50, 50, 50);")
        self.readAllButton.clicked.connect(self.readAll)

        self.clearButton_input = QtWidgets.QPushButton(self.outputFrame)
        self.clearButton_input.setText("Clear All Input")
        self.clearButton_input.setGeometry(QtCore.QRect(370, 10, 150, 40))
        self.clearButton_input.setStyleSheet("background-color: rgb(255, 220, 220);\n"
                                             "color: rgb(50, 50, 50);")
        self.clearButton_input.clicked.connect(self.clear2)

        self.readBuffer = QtWidgets.QPushButton(self.outputFrame)
        self.readBuffer.setText("Read Buffer")
        self.readBuffer.setGeometry(QtCore.QRect(30, 60, 100, 30))
        self.readBuffer.clicked.connect(self.read0)

        self.clearButton_ouput = QtWidgets.QPushButton(self.outputFrame)
        self.clearButton_ouput.setText("Clear")
        self.clearButton_ouput.setGeometry(QtCore.QRect(30, 100, 100, 30))
        self.clearButton_ouput.clicked.connect(self.clear)

        self.saveOption = QtWidgets.QComboBox(self.outputFrame)
        self.saveOption.setGeometry(QtCore.QRect(30, 160, 100, 30))
        self.saveOption.addItems(["TXT", "CSV", "XLSX"])
        self.saveOption.setEditable(True)

        self.saveButton = QtWidgets.QPushButton(self.outputFrame)
        self.saveButton.setText("Save as option")
        self.saveButton.setGeometry(QtCore.QRect(30, 200, 100, 30))
        self.saveButton.clicked.connect(self.save)
        """OUTPUT FRAME ### END"""
        """############################################################################################################
        ############################################################################################################"""

        """############################################################################################################
        ############################################################################################################"""
        """FOOTER"""
        self.footer = QtWidgets.QFrame(Form)
        self.footer.setGeometry(QtCore.QRect(0, 840, 1400, 71))
        self.footer.setMinimumSize(QtCore.QSize(1000, 0))
        self.footer.setMaximumSize(QtCore.QSize(16777215, 550))

        self.command = QtWidgets.QLabel(self.footer)
        self.command.setGeometry(QtCore.QRect(20, 10, 70, 30))
        self.command.setText("Commend: ")
        self.command_value = QtWidgets.QLabel(self.footer)
        self.command_value.setGeometry(QtCore.QRect(95, 10, 300, 30))
        self.command_value.setText("tes")

        self.footerLabel = QtWidgets.QLabel(self.footer)
        self.footerLabel.setGeometry(QtCore.QRect(500, 10, 150, 30))
        self.footerLabel.setFont(self.setFontStyle(8, False, 30))
        self.footerLabel.setText("Yohanes Vianney Dani")

        self.status = QtWidgets.QLabel(self.footer)
        self.status.setGeometry(QtCore.QRect(1000, 10, 70, 30))
        self.status.setText("Port Status: ")
        self.status_value = QtWidgets.QLabel(self.footer)
        self.status_value.setGeometry(QtCore.QRect(1075, 10, 120, 30))
        self.status_value.setText("Not Connected")
        """FOOTER ### END"""
        """############################################################################################################
        ############################################################################################################"""

        # global result
        print("Available ports:" + str(serial_ports()))
        for x in range(len(result)):
            name = result[x]
            self.cb_Port.addItem(name)
        self.cb_Port.addItem("USB")

    def stop_loop(self):
        self.worker.working = False
        self.connectButton.setText("Connect to Serial")
        self.status_value.setText("Not Connected")
        self.openButton.setDisabled(True)
        self.ConnectStatus = 0
        ser.close()


    def start_loop(self):
        global ser

        #Disconnect
        if(self.ConnectStatus == 1):
            self.stop_loop()
            return

        #Verify the correct COM Port
        try:
            self.openButton.setDisabled(False)
            mytext = "{RBA}\r\n"  # Send first enter

            self.getAll = 1

            if (self.cb_Port.currentText() == "USB"):
                print("USB HID Communication")
                self.Port = "USB"
            else:
                self.Port = "UART"
                ser = serial.Serial(self.cb_Port.currentText(), 115200, timeout=1)
                ser.write(mytext.encode())

            if (self.ConnectStatus == 0):
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

            self.thread.started.connect(self.worker.work_byte)
            self.worker.byteReady.connect(self.onByteReady)

            self.worker.finished.connect(self.thread.quit)
            self.worker.finished.connect(self.worker.deleteLater)
            self.thread.finished.connect(self.thread.deleteLater)
            self.thread.start()
            self.status_value.setText("Connected to Serial")

        else:
            print("USB Thread here")

    def onByteReady(self, data):
        int_val = []
        #print(data)

        global line

        if (self.getAll == 1):
            arr_show = []
            for x in range(0, 10):
                start = (x * 4) + 2
                end = start + 4
                res = int.from_bytes(data[start:end], "little")
                if res > 2147483647:
                    res = res - 4294967296
                arr_show.append(res)

            self.output.setText("")

        start_poin = 0
        if data[0] != 16 and data[1] != 17:
            for i in range(len(data)):
                if (data[i] == 13 and data[i + 1] == 10):
                    temp2 = data[start_poin:i]
                    start_poin = i + 2
                    if (temp2[0] == 20 and temp2[1] == 21):
                        string = ''.join(map(chr, temp2[2:len(temp2)+2]))
                        self.output.append("{}".format(string))
                        if((temp2[2]==66 and temp2[3]==117) and ((temp2[9]==68 and temp2[10]==105) or (temp2[9]==115 and temp2[10]==97))):
                            self.output.append("{}".format("\r\n"))
                    if (temp2[0] == 18 and temp2[1] == 19):
                        arr = []
                        for x in range(0, 10):
                            strt = (x * 4) + 2
                            end = strt + 4
                            res = int.from_bytes(temp2[strt:end], "little")
                            if res > 2147483647:
                                res = res - 4294967296
                            arr.append(res)
                        self.output.append("{}".format(arr))
                        self.output.append("{}".format("\r\n"))

        if data[0] == 16 and data[1] == 17:             #{RBA}
            if (self.getAll == 1):
                for x in range(0, 81):
                    start = (x * 4) + 2
                    end = start + 4
                    res = int.from_bytes(data[start:end], "little")
                    if res > 2147483647:
                        res = res - 4294967296
                    int_val.append(res)

                for i in range(10):
                    self.input1[i].setText(str(int_val[i]))
                    self.input2[i].setText(str(int_val[i+10]))
                    self.input3[i].setText(str(int_val[i+20]))

                self.getAll = 0

        if data[0] == 16 and data[1] == 18:
            self.output.append("All value already change!!\r\n")


    def clear(self):
        self.output.setText('')

    def clear2(self):
        for i in range (len(self.input1)):
            self.input1[i].setText('')
            self.input2[i].setText('')
            self.input3[i].setText('')

    def clearCmd1(self):
        self.generalCmd1.setText('')

    def clearCmd2(self):
        self.generalCmd2.setText('')

    """Save Format"""
    def save(self):
        fileName = ""
        if (self.saveOption.currentText() == "TXT"):
            try:
                fileName, _ = QFileDialog.getSaveFileName(self.output, 'Select location to Log', "", '*.txt')

                if fileName != "":
                    with open(fileName, 'w') as f:
                        my_text = self.output.toPlainText()
                        f.write(my_text)
                        self.output.append("Log Saved in :" + fileName + "\r\n")
            except:
                return




        elif (self.saveOption.currentText()=="CSV"):
            try:
                header = ["form", "input1", "input2", "input3", "input4", "input5", "input6", "input7", "input8",
                          "input9", "input10"]
                data = np.empty([3, 11], dtype=int)
                data[0][0] = 1
                data[1][0] = 2
                data[2][0] = 3

                for i in range(10):
                    data[0][i + 1] = int(self.input1[i].text())
                    data[1][i + 1] = int(self.input2[i].text())
                    data[2][i + 1] = int(self.input3[i].text())

                df = pd.DataFrame(data, columns=header)

                fileName, _ = QFileDialog.getSaveFileName(self.output_CSV, 'Select location to Log', "", '*.csv')

                if fileName != "":
                    df.to_csv("{}".format(fileName, index=False))
                    self.output.append("Log Saved in :" + fileName + "\r\n")
            except:
                return

        elif (self.saveOption.currentText()=="XLSX"):
            try:
                header = ["input1", "input2", "input3", "input4", "input5", "input6", "input7", "input8", "input9",
                          "input10"]
                data = np.empty([3, 10], dtype=int)

                for i in range(10):
                    data[0][i] = int(self.input1[i].text())
                    data[1][i] = int(self.input2[i].text())
                    data[2][i] = int(self.input3[i].text())

                df = pd.DataFrame(data, columns=header)

                fileName, _ = QFileDialog.getSaveFileName(self.output_CSV, 'Select location to Log', "", '*.xlsx')

                if fileName != "":
                    df.to_excel("{}".format(fileName, index=False))
                    self.output.append("Log Saved in :" + fileName + "\r\n")
            except:
                return

    def open_file(self):
        fileName, _ = QFileDialog.getOpenFileName(filter="CSV (*.csv *.tsv *.xlsx)")
        filter = fileName[fileName.find("."):len(fileName)]

        if fileName and filter==".csv":
            df = pd.read_csv(fileName, sep=',')

            for i in range (10):
                value1 = str(df.values[0][i+1])
                self.input1[i].setText(value1)
                value2 = str(df.values[1][i+1])
                self.input2[i].setText(value2)
                value3 = str(df.values[2][i+1])
                self.input3[i].setText(value3)

        if fileName and filter==".xlsx":
            df = pd.read_excel(fileName)

            for i in range (10):
                self.input1[i].setText(str(df._get_value(0, "input{}".format(i + 1))))
                self.input2[i].setText(str(df._get_value(1, "input{}".format(i + 1))))
                self.input3[i].setText(str(df._get_value(2, "input{}".format(i + 1))))

    def write1(self):
        mytext_byte = [123, 87, 66, 49, 58]         #{WB1:

        for i in range(len(self.input1)):
            data = integer_to_byte(int(self.input1[i].text()))
            for j in range(4):
                mytext_byte.append(data[j])

        mytext_byte.append(125)
        mytext_byte.append(13)
        mytext_byte.append(10)

        ser.write(mytext_byte)

        self.command_value.setText("Write to serial WR1")


    def write2(self):
        mytext_byte = [123, 87, 66, 50, 58]  # {WB2:

        for i in range(len(self.input2)):
            data = integer_to_byte(int(self.input2[i].text()))
            for j in range(4):
                mytext_byte.append(data[j])

        mytext_byte.append(125)
        mytext_byte.append(13)
        mytext_byte.append(10)

        ser.write(mytext_byte)

        self.command_value.setText("Write to serial WR2")

    def write3(self):
        mytext_byte = [123, 87, 66, 51, 58]  # {WB2:

        for i in range(len(self.input3)):
            data = integer_to_byte(int(self.input3[i].text()))
            for j in range(4):
                mytext_byte.append(data[j])

        mytext_byte.append(125)
        mytext_byte.append(13)
        mytext_byte.append(10)

        ser.write(mytext_byte)

        self.command_value.setText("Write to serial WR3")

    def writeAll(self):
        mytext_byte = [123, 87, 66, 65, 58]  # {WBA:

        for i in range(len(self.input1)):
            data = integer_to_byte(int(self.input1[i].text()))
            for j in range(4):
                mytext_byte.append(data[j])
        for i in range(len(self.input2)):
            data = integer_to_byte(int(self.input2[i].text()))
            for j in range(4):
                mytext_byte.append(data[j])
        for i in range(len(self.input3)):
            data = integer_to_byte(int(self.input3[i].text()))
            for j in range(4):
                mytext_byte.append(data[j])

        mytext_byte.append(125)
        mytext_byte.append(13)
        mytext_byte.append(10)

        ser.write(mytext_byte)

        self.command_value.setText("Write to serial All")

    def sendCmd(self, text):
        mytext_ascii = text
        mytext_byte = []
        find = False

        msgBox = QMessageBox()
        msgBox.setWindowTitle("Command False")
        msgBox.setIcon(QMessageBox.Warning)
        msgBox.setFont(self.setFontStyle(8, False, 40))

        for i in range(len(str_cfg_header)):
            if (mytext_ascii[0:5] == str_cfg_header[i]):
                for j in range(5):
                    mytext_byte.append(ord(mytext_ascii[j]))

                if i < 3:
                    mytext_ascii = mytext_ascii[5:len(mytext_ascii)]
                    sparatePosition = mytext_ascii.find(";")
                    if (sparatePosition == -1):

                        msgBox.setText("Write command must be write as example!!\n"
                                       "{WB1:1;2;3;4;5;6;7;8;9;10}")
                        msgBox.exec()
                        return
                    else:
                        count_data = 0
                        while count_data < self.numberInput:
                            sparatePosition = mytext_ascii.find(";")
                            if i == 0:
                                self.input1[count_data].setText(mytext_ascii[0:sparatePosition])
                            elif i == 1:
                                self.input2[count_data].setText(mytext_ascii[0:sparatePosition])
                            elif i == 2:
                                self.input3[count_data].setText(mytext_ascii[0:sparatePosition])
                            data = integer_to_byte(int(mytext_ascii[0:sparatePosition]))
                            for j in range(4):
                                mytext_byte.append(data[j])
                            mytext_ascii = mytext_ascii[sparatePosition + 1:len(mytext_ascii)]
                            count_data += 1

                            if (count_data < self.numberInput and sparatePosition == -1):
                                msgBox.setText("Data writen not finish yet")
                                msgBox.exec()
                                return

                mytext_byte.append(13)
                mytext_byte.append(10)

                find = True

        if (find == False):
            msgBox.setText("Wrong command given!!\n"
                           "Command:\n"
                           "'{WB1:','{WB2:','{WB3:','{RBA}','{RD0}','{RB1}','{RB2}','{RB3}")
            msgBox.exec()
            return
        else:
            ser.write(mytext_byte)

    def sendCmd1(self):
        self.sendCmd(self.generalCmd1.text())

    def sendCmd2(self):
        self.sendCmd(self.generalCmd2.text())

    def read0(self):
        mytext_byte = [123, 82, 68, 48, 125, 13, 10]  # {RB0}\r\n
        ser.write(mytext_byte)

        self.command_value.setText("Read to serial {RD0}")

    def readAll(self):          # Only read from input place and used when you want to save to .xlxs or .csv
        for i in range(3):
            mytext = "R{}: {}; ".format(i + 1, i + 1)
            for j in range(self.numberInput):
                if (i == 0):
                    mytext += "{}".format(self.input1[j].text())
                elif (i == 1):
                    mytext += "{}".format(self.input2[j].text())
                elif (i == 2):
                    mytext += "{}".format(self.input3[j].text())

                if (j < self.numberInput - 1):
                    mytext += "; "
                else:
                    mytext += "\n"
            self.output.append(mytext)

    def read1(self):
        mytext_byte = [123, 82, 66, 49, 125, 13, 10]  # {RB1}\r\n
        ser.write(mytext_byte)

        self.command_value.setText("Write to serial RD1")

    def read2(self):
        mytext_byte = [123, 82, 66, 50, 125, 13, 10]  # {RB2}\r\n
        ser.write(mytext_byte)

        self.command_value.setText("Write to serial RD2")

    def read3(self):
        mytext_byte = [123, 82, 66, 51, 125, 13, 10]  # {RB3}\r\n
        ser.write(mytext_byte)

        self.command_value.setText("Write to serial RD3")

    """Function to UI"""

    def general_command(self, label, command, sendBtn, clearBtn, x, y):
        label.setGeometry(QtCore.QRect(x, y, 100, 25))

        command.setGeometry(QtCore.QRect(x+90, y, 180, 25))

        sendBtn.setGeometry(QtCore.QRect(x+280, y, 80, 25))
        sendBtn.setText("Send")

        clearBtn.setGeometry(QtCore.QRect(x+370, y, 80, 25))
        clearBtn.setText("Clear")

    def setFontStyle(self, size=10, bold=False, weight=75):
        font = QtGui.QFont()
        font.setPointSize(size)
        font.setBold(bold)
        font.setWeight(weight)

        return font

    def input_template(self, labelName, areaName, y):
        labelName.setGeometry(QtCore.QRect(30, y, 250, 30))
        labelName.setMinimumSize(QtCore.QSize(0, 10))
        labelName.setFont(self.setFontStyle())

        areaName.setGeometry(QtCore.QRect(10, y + 30, 250, 30))

    def input_area(self, label, scrollArea, sAWidget, writeButton, readButton,
                   groupInput,
                   groupInputLabel):

        label.setGeometry(20, 5, 100, 30)
        label.setFont(self.setFontStyle())

        scrollArea.setGeometry(20, 40, 310, 400)

        bottom = 0

        for i in range(self.numberInput):
            self.input_template(groupInputLabel[i], groupInput[i], 10 + (i * 80))
            groupInputLabel[i].setText("Input " + str(i + 1))
            bottom = 90 + (i * 80)

        writeButton.setGeometry(QtCore.QRect(10, bottom, 101, 51))
        writeButton.setText("Send Data")

        readButton.setGeometry(QtCore.QRect(120, bottom, 100, 50))
        readButton.setText("Read Data")

        sAWidget.setGeometry(QtCore.QRect(5, 0, 280, bottom+55))  # ini yang bikin bisa muncul scroll bar nya

        scrollArea.setWidget(sAWidget)

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    Form = QtWidgets.QWidget()
    ui = Ui_Form()
    ui.setupUi(Form)
    Form.show()
    sys.exit(app.exec_())