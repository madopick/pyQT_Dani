"""
STM32Nucleo buffer
This is UI for project_23_03_14
Make reparation concept
UI for I2C multi input and for the first Connected to Serial, the form will show the default value from Slave
This UI will accept string input, send it to Master as ASCII decode.
Data will parsing into integer and will be send by I2C to Slave and it will be saved.
This UI also show the output from Slave when asked.
"""

import sys, os, serial, serial.tools.list_ports, warnings
from PyQt5.QtCore import *
import time
from PyQt5.QtWidgets import *
from PyQt5.uic import loadUi
import serial.tools.list_ports as port_list
from PyQt5 import QtCore, QtGui, QtWidgets
import csv

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
        Form.setObjectName("I2C")
        Form.setMinimumSize(1200, 900)
        Form.setMaximumSize(1200, 900)
        Form.setCursor(QtGui.QCursor(QtCore.Qt.ArrowCursor))
        Form.setWindowTitle("My Program")

        """############################################################################################################
        ############################################################################################################"""
        """HEADER AND CONNECT SERIAL"""
        self.header = QtWidgets.QFrame(Form)
        self.header.setGeometry(QtCore.QRect(10, 20, 1086, 100))
        self.header.setMinimumSize(QtCore.QSize(1000, 100))
        self.header.setMaximumSize(QtCore.QSize(16777215, 100))
        self.header.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.header.setFrameShadow(QtWidgets.QFrame.Raised)
        self.header.setObjectName("header")

        self.titleLabel = QtWidgets.QLabel(self.header)
        self.titleLabel.setGeometry(QtCore.QRect(20, 10, 1061, 30))
        self.titleLabel.setFont(self.setFontStyle())
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
        self.connectButton.setStyleSheet("background-color: rgb(150, 255, 170);\n"
                                         "color: rgb(50, 50, 50);")
        self.connectButton.setFont(self.setFontStyle(7))
        self.connectButton.setGeometry(QtCore.QRect(210, 50, 200, 35))
        self.connectButton.setText("Connect to Serial")
        """HEADER AND CONNECT SERIAL ### END"""
        """############################################################################################################
        ############################################################################################################"""

        self.workFrame = QtWidgets.QFrame(Form)
        self.workFrame.setGeometry(QtCore.QRect(10, 127, 1112, 513))
        self.workFrame.setMinimumSize(QtCore.QSize(1000, 300))
        self.workFrame.setMaximumSize(QtCore.QSize(16777215, 490))

        self.horizontalLayout = QtWidgets.QHBoxLayout(self.workFrame)

        """############################################################################################################
        ############################################################################################################"""
        """INPUT FRAME"""

        """### FORM 1 --- START ###"""
        self.formArea1 = QtWidgets.QFrame(self.workFrame)
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
        self.writeButton1.clicked.connect(self.write1)

        self.readButton1 = QtWidgets.QPushButton(self.insideScrollArea1)
        self.readButton1.clicked.connect(self.read1)

        self.input_area(self.formArea1, self.verticalLayout1_1, self.form1Label, self.scrollArea1,
                        self.scrollAreaWidgetContents1, self.verticalLayout1_2, self.insideScrollArea1,
                        self.writeButton1, self.readButton1, self.input1, self.inputLabel1)
        self.horizontalLayout.addWidget(self.formArea1)

        """### FORM 1 --- END ###"""

        """### FORM 2 --- START ###"""
        self.formArea2 = QtWidgets.QFrame(self.workFrame)
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
        self.writeButton2.clicked.connect(self.write2)

        self.readButton2 = QtWidgets.QPushButton(self.insideScrollArea2)
        self.readButton2.clicked.connect(self.read2)

        self.input_area(self.formArea2, self.verticalLayout2_1, self.form2Label, self.scrollArea2,
                        self.scrollAreaWidgetContents2, self.verticalLayout2_2, self.insideScrollArea2,
                        self.writeButton2, self.readButton2, self.input2, self.inputLabel2)
        self.horizontalLayout.addWidget(self.formArea2)
        """ ### FORM 2 --- END ### """

        """ ### FORM 3 --- START ### """
        self.formArea3 = QtWidgets.QFrame(self.workFrame)
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
        self.writeButton3.clicked.connect(self.write3)

        self.readButton3 = QtWidgets.QPushButton(self.insideScrollArea3)
        self.readButton3.clicked.connect(self.read3)

        self.input_area(self.formArea3, self.verticalLayout3_1, self.form3Label, self.scrollArea3,
                        self.scrollAreaWidgetContents3, self.verticalLayout3_2, self.insideScrollArea3,
                        self.writeButton3, self.readButton3, self.input3, self.inputLabel3)
        self.horizontalLayout.addWidget(self.formArea3)
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

        self.clearButton_input = QtWidgets.QPushButton(self.outputFrame)
        self.clearButton_input.setText("Clear All Input")
        self.clearButton_input.setGeometry(QtCore.QRect(200, 10, 150, 40))
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
        self.saveOption.addItems(["TXT", "CSV", "PDF"])
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

        self.status = QtWidgets.QLabel(self.footer)
        self.status.setGeometry(QtCore.QRect(1000, 10, 70, 30))
        self.status.setText("Port Status: ")
        self.status_value = QtWidgets.QLabel(self.footer)
        self.status_value.setGeometry(QtCore.QRect(1075, 10, 120, 30))
        self.status_value.setText("Not Connected")
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
        self.getAll = 0
        self.sendAll = 0

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
        self.status_value.setText("Not Connected")
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
            mytext = "{RDA}\r\n"  # Send first enter
            self.getAll = 1

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
            self.status_value.setText("Connected to Serial")

        else:
            print("USB Thread here")

    def onIntReady(self, i):
        global line
        if i != '':
            self.output.append("{}".format(i))

            if (self.getAll==1):
                startPoin = ["R1: ", "R2: ", "R3: ", "END"]  # Header Penanda

                #for j in range(1):
                end = 4
                start = 4
                count_indeks = 0
                while (i[end] != '\n'):
                    if (i[end] == ';'):
                        text = ""  # Tempat untuk tiap output token
                        if (i[start] == ' '):
                            start += 1
                        text += i[start:end]
                        if (i[0:4] == startPoin[0]):
                            self.input1[count_indeks].setText(text)
                        elif (i[0:4] == startPoin[1]):
                            self.input2[count_indeks].setText(text)
                        elif (i[0:4] == startPoin[2]):
                            self.input3[count_indeks].setText(text)
                        count_indeks += 1
                        start = end + 1

                    end += 1

                self.output.setText('')

                if (i[0:3] == startPoin[3]):
                    self.getAll = 0


    def clear(self):
        self.output.setText('')

    def clear2(self):
        for i in range (len(self.input1)):
            self.input1[i].setText('')
            self.input2[i].setText('')
            self.input3[i].setText('')

    """Save Format"""
    def save(self):
        fileName = ""
        if (self.saveOption.currentText() == "TXT"):
            fileName = QFileDialog.getSaveFileName(self.output, 'Select location to Log', "", '*.txt')

            if not fileName:
                return

            with open(fileName[0], 'w') as f:
                my_text = self.output.toPlainText()
                f.write(my_text)

        elif (self.saveOption.currentText()=="CSV"):
            value = "Variable,inpt1,inpt2,inpt3,inpt4,inpt5,inpt6,inpt7,inpt8,inpt9,inpt10\n"
            startPoin = ["RES1:", "RES2:", "RES3:"]  # Header Penanda

            for i in range (len(self.output.toPlainText())):
                if(self.output.toPlainText()[i:i+5]==startPoin[0]):
                    end = i+6
                    start = i+6
                    text = "RES1,"
                    while (self.output.toPlainText()[end-1] != '\n'):
                        if (self.output.toPlainText()[end] == ';'):
                            text = text + self.output.toPlainText()[start:end] + ","
                            start = end + 2
                        if (self.output.toPlainText()[end] == '\n'):
                            text = text + self.output.toPlainText()[start:end]
                        end+=1
                    text+="\n"
                    value+=text
                    i=end

                if (self.output.toPlainText()[i:i + 5] == startPoin[1]):
                    end = i + 6
                    start = i + 6
                    text = "RES2,"
                    while (self.output.toPlainText()[end - 1] != '\n'):
                        if (self.output.toPlainText()[end] == ';'):
                            text = text + self.output.toPlainText()[start:end] + ","
                            start = end + 2
                        if (self.output.toPlainText()[end] == '\n'):
                            text = text + self.output.toPlainText()[start:end]
                        end += 1
                    text += "\n"
                    value += text
                    i=end

                if (self.output.toPlainText()[i:i + 5] == startPoin[2]):
                    end = i + 6
                    start = i + 6
                    text = "RES3,"
                    while (self.output.toPlainText()[end - 1] != '\n'):
                        if (self.output.toPlainText()[end] == ';'):
                            text = text + self.output.toPlainText()[start:end] + ","
                            start = end + 2
                        if (self.output.toPlainText()[end] == '\n'):
                            text = text + self.output.toPlainText()[start:end]
                        end += 1
                    text += "\n"
                    value += text
                    i=end

            fileName = QFileDialog.getSaveFileName(self.output_CSV, 'Select location to Log', "", '*.csv')

            if not fileName:
                return

            with open(fileName[0], 'w') as f:
                f.write(value)


        elif (self.saveOption.currentText()=="PDF"):
            fileName = QFileDialog.getSaveFileName(self.output, 'Select location to Log', "", '*.pdf')

        self.output.append("Log Saved in :" + fileName[0] + "\r\n")


    def write1(self):
        mytext = "{WR1:"
        for i in range (len(self.input1)):
            mytext += self.input1[i].text()
            if(i<len(self.input1)-1):
                mytext += ";"
        mytext += "}\r\n"

        str = "Write to serial " + mytext
        self.command_value.setText(str)
        ser.write(mytext.encode())

    def write2(self):
        mytext = "{WR2:"
        for i in range(len(self.input2)):
            mytext += self.input2[i].text()
            if (i < len(self.input2) - 1):
                mytext += ";"
        mytext += "}\r\n"

        str = "Write to serial " + mytext
        self.command_value.setText(str)
        ser.write(mytext.encode())

    def write3(self):
        mytext = "{WR3:"
        for i in range(len(self.input3)):
            mytext += self.input3[i].text()
            if (i < len(self.input3) - 1):
                mytext += ";"
        mytext += "}\r\n"

        str = "Write to serial " + mytext
        self.command_value.setText(str)
        ser.write(mytext.encode())

    def writeAll(self):
        """mytext = "{WRA:"
        for i in range(len(self.input1)):
            mytext += self.input1[i].text()
            mytext += ";"
        for i in range(len(self.input2)):
            mytext += self.input2[i].text()
            mytext += ";"
        for i in range(len(self.input3)):
            mytext += self.input3[i].text()
            if (i < len(self.input3) - 1):
                mytext += ";"
        mytext += "}\r\n"
        print(mytext)"""

        mytext = "{WRA:"
        for i in range(len(self.input1)):
            mytext += self.input1[i].text() + ";"
        for i in range(len(self.input2)):
            mytext += self.input2[i].text() + ";"
        for i in range(len(self.input3)):
            mytext += self.input3[i].text()
            if (i < len(self.input3) - 1):
                mytext += ";"
        mytext += "}\r\n"

        str = "Write to serial " + mytext
        self.command_value.setText(str)
        ser.write(mytext.encode())

    def read0(self):
        mytext = "{RD0}\r\n"
        str = "Read to serial " + mytext
        self.command_value.setText(str)

        ser.write(mytext.encode())

    def read1(self):
        mytext = "{RD1}\r\n"
        str = "Read to serial " + mytext
        self.command_value.setText(str)

        ser.write(mytext.encode())

    def read2(self):
        mytext = "{RD2}\r\n"
        str = "Read to serial " + mytext
        self.command_value.setText(str)

        ser.write(mytext.encode())

    def read3(self):
        mytext = "{RD3}\r\n"
        str = "Read to serial " + mytext
        self.command_value.setText(str)

        ser.write(mytext.encode())

    """Function to UI"""

    def setFontStyle(self, size=10, bold=False):
        font = QtGui.QFont()
        font.setPointSize(size)
        font.setBold(bold)
        font.setWeight(75)

        return font

    def input_template(self, labelName, areaName, y):
        labelName.setGeometry(QtCore.QRect(30, y, 260, 30))
        labelName.setMinimumSize(QtCore.QSize(0, 10))
        labelName.setFont(self.setFontStyle())

        areaName.setGeometry(QtCore.QRect(10, y + 30, 260, 30))

    def input_area(self, form, vertical, label, scrollArea, sAWidget, vertical2, inside, writeButton, readButton, groupInput,
                   groupInputLabel):
        form.setMinimumSize(QtCore.QSize(350, 0))

        label.setMinimumSize(QtCore.QSize(0, 31))
        label.setMaximumSize(QtCore.QSize(16777215, 31))
        label.setFont(self.setFontStyle())

        vertical.addWidget(label)

        scrollArea.setMinimumSize(QtCore.QSize(0, 80))
        scrollArea.setMaximumSize(QtCore.QSize(16777215, 700))
        scrollArea.setWidgetResizable(True)

        sAWidget.setGeometry(QtCore.QRect(0, 0, 327, 922))

        inside.setMinimumSize(QtCore.QSize(0, 900))
        inside.setMaximumSize(QtCore.QSize(16777215, 900))

        for i in range(10):
            self.input_template(groupInputLabel[i], groupInput[i], 10 + (i * 80))
            groupInputLabel[i].setText("Input " + str(i + 1))

        writeButton.setGeometry(QtCore.QRect(10, 810, 101, 51))
        writeButton.setText("Send Data")

        readButton.setGeometry(QtCore.QRect(120, 810, 100, 50))
        readButton.setText("Read Data")
        vertical2.addWidget(inside)
        scrollArea.setWidget(sAWidget)
        vertical.addWidget(scrollArea)

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    Form = QtWidgets.QWidget()
    ui = Ui_Form()
    ui.setupUi(Form)
    Form.show()
    sys.exit(app.exec_())
