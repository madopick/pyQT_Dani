"""
This is UI for project_23_02_06
This UI will accept 10 integer input (but it will accepted as String)
send it to the STM32 so the program can sparate and parse it to 10 integer.
This UI also show the output of STM32
"""

from PyQt5 import QtCore, QtGui, QtWidgets
import serial
from past.builtins import raw_input

ser = []
global output
output = ["", "", "", "", "", "", "", "", "", ""]

def getValues():
    #ser.write(b'g')
    stmData = ser.readline().decode('ascii')
    return stmData

def sendValues(userInput):
    #print("jalan")
    ser.write(userInput.encode())
    stmData = ser.readline().decode('ascii')
    return stmData

def valid(num):
    if(num=='0' or num=='1' or num=='2' or num=='3' or num=='4' or num=='5' or num=='6' or num=='7' or num=='8' or num=='9'):
        return True
    return False

class Ui_Form(object):
    _translate = QtCore.QCoreApplication.translate
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(1118, 853)

        self.inputFrame = QtWidgets.QFrame(Form)
        self.inputFrame.setGeometry(QtCore.QRect(10, 127, 1112, 513))
        self.inputFrame.setMaximumSize(QtCore.QSize(16777215, 550))
        self.inputFrame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.inputFrame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.inputFrame.setObjectName("inputFrame")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.inputFrame)
        self.horizontalLayout.setObjectName("horizontalLayout")

        ### FORM 1 --- START ###
        self.formArea1 = QtWidgets.QFrame(self.inputFrame)
        self.verticalLayout1_1 = QtWidgets.QVBoxLayout(self.formArea1)
        self.form1Label = QtWidgets.QLabel(self.formArea1)
        self.scrollArea1 = QtWidgets.QScrollArea(self.formArea1)
        self.scrollAreaWidgetContents1 = QtWidgets.QWidget()
        self.verticalLayout1_2 = QtWidgets.QVBoxLayout(self.scrollAreaWidgetContents1)
        self.insideScrollArea1 = QtWidgets.QFrame(self.scrollAreaWidgetContents1)

        #Input In form 1
        self.input1_1 = QtWidgets.QLineEdit(self.insideScrollArea1)
        self.inputLabel1_1 = QtWidgets.QLabel(self.insideScrollArea1)
        self.input1_2 = QtWidgets.QLineEdit(self.insideScrollArea1)
        self.inputLabel1_2 = QtWidgets.QLabel(self.insideScrollArea1)
        self.input1_3 = QtWidgets.QLineEdit(self.insideScrollArea1)
        self.inputLabel1_3 = QtWidgets.QLabel(self.insideScrollArea1)
        self.input1_4 = QtWidgets.QLineEdit(self.insideScrollArea1)
        self.inputLabel1_4 = QtWidgets.QLabel(self.insideScrollArea1)
        self.input1_5 = QtWidgets.QLineEdit(self.insideScrollArea1)
        self.inputLabel1_5 = QtWidgets.QLabel(self.insideScrollArea1)
        self.input1_6 = QtWidgets.QLineEdit(self.insideScrollArea1)
        self.inputLabel1_6 = QtWidgets.QLabel(self.insideScrollArea1)
        self.input1_7 = QtWidgets.QLineEdit(self.insideScrollArea1)
        self.inputLabel1_7 = QtWidgets.QLabel(self.insideScrollArea1)
        self.input1_8 = QtWidgets.QLineEdit(self.insideScrollArea1)
        self.inputLabel1_8 = QtWidgets.QLabel(self.insideScrollArea1)
        self.input1_9 = QtWidgets.QLineEdit(self.insideScrollArea1)
        self.inputLabel1_9 = QtWidgets.QLabel(self.insideScrollArea1)
        self.input1_10 = QtWidgets.QLineEdit(self.insideScrollArea1)
        self.inputLabel1_10 = QtWidgets.QLabel(self.insideScrollArea1)

        self.groupInput1 = [
            self.input1_1, self.input1_2, self.input1_3, self.input1_4, self.input1_5,
            self.input1_6, self.input1_7, self.input1_8, self.input1_9, self.input1_10
        ]

        self.groupInputLabel1 = [
            self.inputLabel1_1, self.inputLabel1_2, self.inputLabel1_3, self.inputLabel1_4, self.inputLabel1_5,
            self.inputLabel1_6, self.inputLabel1_7, self.inputLabel1_8, self.inputLabel1_9, self.inputLabel1_10
        ]
        # Input In form 1 -- END

        self.writeButton1 = QtWidgets.QPushButton(self.insideScrollArea1)
        self.writeButton1.clicked.connect(self.send_form1)

        self.input_area(self.formArea1, self.verticalLayout1_1, self.form1Label, self.scrollArea1, self.scrollAreaWidgetContents1,
                        self.verticalLayout1_2, self.insideScrollArea1, self.writeButton1, self.groupInput1, self.groupInputLabel1)
        self.horizontalLayout.addWidget(self.formArea1)
        ### FORM 1 --- END ###

        ### FORM 2 --- START ###
        self.formArea2 = QtWidgets.QFrame(self.inputFrame)
        self.verticalLayout2_1 = QtWidgets.QVBoxLayout(self.formArea2)
        self.form2Label = QtWidgets.QLabel(self.formArea2)
        self.scrollArea2 = QtWidgets.QScrollArea(self.formArea2)
        self.scrollAreaWidgetContents2 = QtWidgets.QWidget()
        self.verticalLayout2_2 = QtWidgets.QVBoxLayout(self.scrollAreaWidgetContents2)
        self.insideScrollArea2 = QtWidgets.QFrame(self.scrollAreaWidgetContents2)

        # Input In form 1
        self.input2_1 = QtWidgets.QLineEdit(self.insideScrollArea2)
        self.inputLabel2_1 = QtWidgets.QLabel(self.insideScrollArea2)
        self.input2_2 = QtWidgets.QLineEdit(self.insideScrollArea2)
        self.inputLabel2_2 = QtWidgets.QLabel(self.insideScrollArea2)
        self.input2_3 = QtWidgets.QLineEdit(self.insideScrollArea2)
        self.inputLabel2_3 = QtWidgets.QLabel(self.insideScrollArea2)
        self.input2_4 = QtWidgets.QLineEdit(self.insideScrollArea2)
        self.inputLabel2_4 = QtWidgets.QLabel(self.insideScrollArea2)
        self.input2_5 = QtWidgets.QLineEdit(self.insideScrollArea2)
        self.inputLabel2_5 = QtWidgets.QLabel(self.insideScrollArea2)
        self.input2_6 = QtWidgets.QLineEdit(self.insideScrollArea2)
        self.inputLabel2_6 = QtWidgets.QLabel(self.insideScrollArea2)
        self.input2_7 = QtWidgets.QLineEdit(self.insideScrollArea2)
        self.inputLabel2_7 = QtWidgets.QLabel(self.insideScrollArea2)
        self.input2_8 = QtWidgets.QLineEdit(self.insideScrollArea2)
        self.inputLabel2_8 = QtWidgets.QLabel(self.insideScrollArea2)
        self.input2_9 = QtWidgets.QLineEdit(self.insideScrollArea2)
        self.inputLabel2_9 = QtWidgets.QLabel(self.insideScrollArea2)
        self.input2_10 = QtWidgets.QLineEdit(self.insideScrollArea2)
        self.inputLabel2_10 = QtWidgets.QLabel(self.insideScrollArea2)

        self.groupInput2 = [
            self.input2_1, self.input2_2, self.input2_3, self.input2_4, self.input2_5,
            self.input2_6, self.input2_7, self.input2_8, self.input2_9, self.input2_10
        ]

        self.groupInputLabel2 = [
            self.inputLabel2_1, self.inputLabel2_2, self.inputLabel2_3, self.inputLabel2_4, self.inputLabel2_5,
            self.inputLabel2_6, self.inputLabel2_7, self.inputLabel2_8, self.inputLabel2_9, self.inputLabel2_10
        ]

        # Input In form 2 -- END

        self.writeButton2 = QtWidgets.QPushButton(self.insideScrollArea2)
        self.writeButton2.clicked.connect(self.send_form2)

        self.input_area(self.formArea2, self.verticalLayout2_1, self.form2Label, self.scrollArea2,
                        self.scrollAreaWidgetContents2,
                        self.verticalLayout2_2, self.insideScrollArea2, self.writeButton2, self.groupInput2,
                        self.groupInputLabel2)
        self.horizontalLayout.addWidget(self.formArea2)
        ### FORM 2 --- END ###

        ### FORM 3 --- START ###
        self.formArea3 = QtWidgets.QFrame(self.inputFrame)
        self.verticalLayout3_1 = QtWidgets.QVBoxLayout(self.formArea3)
        self.form3Label = QtWidgets.QLabel(self.formArea3)
        self.scrollArea3 = QtWidgets.QScrollArea(self.formArea3)
        self.scrollAreaWidgetContents3 = QtWidgets.QWidget()
        self.verticalLayout3_2 = QtWidgets.QVBoxLayout(self.scrollAreaWidgetContents3)
        self.insideScrollArea3 = QtWidgets.QFrame(self.scrollAreaWidgetContents3)

        # Input In form 3
        self.input3_1 = QtWidgets.QLineEdit(self.insideScrollArea3)
        self.inputLabel3_1 = QtWidgets.QLabel(self.insideScrollArea3)
        self.input3_2 = QtWidgets.QLineEdit(self.insideScrollArea3)
        self.inputLabel3_2 = QtWidgets.QLabel(self.insideScrollArea3)
        self.input3_3 = QtWidgets.QLineEdit(self.insideScrollArea3)
        self.inputLabel3_3 = QtWidgets.QLabel(self.insideScrollArea3)
        self.input3_4 = QtWidgets.QLineEdit(self.insideScrollArea3)
        self.inputLabel3_4 = QtWidgets.QLabel(self.insideScrollArea3)
        self.input3_5 = QtWidgets.QLineEdit(self.insideScrollArea3)
        self.inputLabel3_5 = QtWidgets.QLabel(self.insideScrollArea3)
        self.input3_6 = QtWidgets.QLineEdit(self.insideScrollArea3)
        self.inputLabel3_6 = QtWidgets.QLabel(self.insideScrollArea3)
        self.input3_7 = QtWidgets.QLineEdit(self.insideScrollArea3)
        self.inputLabel3_7 = QtWidgets.QLabel(self.insideScrollArea3)
        self.input3_8 = QtWidgets.QLineEdit(self.insideScrollArea3)
        self.inputLabel3_8 = QtWidgets.QLabel(self.insideScrollArea3)
        self.input3_9 = QtWidgets.QLineEdit(self.insideScrollArea3)
        self.inputLabel3_9 = QtWidgets.QLabel(self.insideScrollArea3)
        self.input3_10 = QtWidgets.QLineEdit(self.insideScrollArea3)
        self.inputLabel3_10 = QtWidgets.QLabel(self.insideScrollArea3)

        self.groupInput3 = [
            self.input3_1, self.input3_2, self.input3_3, self.input3_4, self.input3_5,
            self.input3_6, self.input3_7, self.input3_8, self.input3_9, self.input3_10
        ]

        self.groupInputLabel3 = [
            self.inputLabel3_1, self.inputLabel3_2, self.inputLabel3_3, self.inputLabel3_4, self.inputLabel3_5,
            self.inputLabel3_6, self.inputLabel3_7, self.inputLabel3_8, self.inputLabel3_9, self.inputLabel3_10
        ]

        # Input In form 3 -- END

        self.writeButton3 = QtWidgets.QPushButton(self.insideScrollArea3)
        self.writeButton3.clicked.connect(self.send_form3)

        self.input_area(self.formArea3, self.verticalLayout3_1, self.form3Label, self.scrollArea3,
                        self.scrollAreaWidgetContents3,
                        self.verticalLayout3_2, self.insideScrollArea3, self.writeButton3, self.groupInput3,
                        self.groupInputLabel3)
        self.horizontalLayout.addWidget(self.formArea3)
        ### FORM 3 --- END ###


        self.outputFrame = QtWidgets.QFrame(Form)
        self.outputFrame.setGeometry(QtCore.QRect(10, 646, 1086, 200))
        self.outputFrame.setMinimumSize(QtCore.QSize(0, 200))
        self.outputFrame.setMaximumSize(QtCore.QSize(16777215, 200))
        self.outputFrame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.outputFrame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.outputFrame.setObjectName("outputFrame")

        self.pushButton = QtWidgets.QPushButton(self.outputFrame)
        self.pushButton.setGeometry(QtCore.QRect(30, 30, 111, 61))
        self.pushButton.setObjectName("pushButton")
        self.pushButton.clicked.connect(self.readSerial)

        self.output_1 = QtWidgets.QLabel(self.outputFrame)
        self.output_template(self.output_1, 150, 30)

        self.output_2 = QtWidgets.QLabel(self.outputFrame)
        self.output_template(self.output_2, 330, 30)

        self.output_3 = QtWidgets.QLabel(self.outputFrame)
        self.output_template(self.output_3, 510, 30)

        self.output_4 = QtWidgets.QLabel(self.outputFrame)
        self.output_template(self.output_4, 690, 30)

        self.output_5 = QtWidgets.QLabel(self.outputFrame)
        self.output_template(self.output_5, 870, 30)

        self.output_6 = QtWidgets.QLabel(self.outputFrame)
        self.output_template(self.output_6, 150, 100)

        self.output_7 = QtWidgets.QLabel(self.outputFrame)
        self.output_template(self.output_7, 330, 100)

        self.output_8 = QtWidgets.QLabel(self.outputFrame)
        self.output_template(self.output_8, 510, 100)

        self.output_9 = QtWidgets.QLabel(self.outputFrame)
        self.output_template(self.output_9, 690, 100)

        self.output_10 = QtWidgets.QLabel(self.outputFrame)
        self.output_template(self.output_10, 870, 100)

        self.frame_3 = QtWidgets.QFrame(Form)
        self.frame_3.setGeometry(QtCore.QRect(10, 20, 1086, 100))
        self.frame_3.setMinimumSize(QtCore.QSize(0, 100))
        self.frame_3.setMaximumSize(QtCore.QSize(16777215, 100))
        self.frame_3.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame_3.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame_3.setObjectName("frame_3")
        self.connectButton = QtWidgets.QPushButton(self.frame_3)
        self.connectButton.setGeometry(QtCore.QRect(10, 47, 181, 41))
        self.connectButton.setCursor(QtGui.QCursor(QtCore.Qt.BusyCursor))
        self.connectButton.setCheckable(True)
        self.connectButton.toggled.connect(self.connectSerial)
        self.titleLabel = QtWidgets.QLabel(self.frame_3)
        self.titleLabel.setGeometry(QtCore.QRect(20, 10, 1061, 31))
        font = QtGui.QFont()
        font.setPointSize(12)
        font.setBold(True)
        font.setWeight(75)
        self.titleLabel.setFont(font)
        self.titleLabel.setObjectName("titleLabel")

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        #_translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(self._translate("Form", "Form"))
        self.form1Label.setText(self._translate("Form", "Form 1"))
        self.form2Label.setText(self._translate("Form", "Form 2"))
        self.form3Label.setText(self._translate("Form", "Form 3"))
        self.pushButton.setText(self._translate("Form", "Read"))
        self.output_1.setText(self._translate("Form", output[0]))
        self.output_2.setText(self._translate("Form", output[1]))
        self.output_3.setText(self._translate("Form", output[2]))
        self.output_4.setText(self._translate("Form", output[3]))
        self.output_5.setText(self._translate("Form", output[4]))
        self.output_6.setText(self._translate("Form", output[5]))
        self.output_7.setText(self._translate("Form", output[6]))
        self.output_8.setText(self._translate("Form", output[7]))
        self.output_9.setText(self._translate("Form", output[8]))
        self.output_10.setText(self._translate("Form", output[9]))
        self.connectButton.setText(self._translate("Form", "Connect to Serial"))
        self.titleLabel.setText(self._translate("Form", "Program untuk Menerima Multi Input dari Beberapa Form ke Serial - Yohanes Vianney Dani"))

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


        #self.form = QtWidgets.QFrame(self.inputFrame)
        form.setMinimumSize(QtCore.QSize(350, 0))
        form.setFrameShape(QtWidgets.QFrame.StyledPanel)
        form.setFrameShadow(QtWidgets.QFrame.Raised)

        #self.verticalLayout = QtWidgets.QVBoxLayout(self.formArea1)
        vertical.setObjectName("verticalLayout")

        #self.form1Label = QtWidgets.QLabel(self.formArea1)
        label.setMinimumSize(QtCore.QSize(0, 31))
        label.setMaximumSize(QtCore.QSize(16777215, 31))
        font = QtGui.QFont()
        font.setPointSize(16)
        font.setBold(True)
        font.setWeight(75)
        label.setFont(font)

        vertical.addWidget(label)

        #self.scrollArea = QtWidgets.QScrollArea(self.formArea1)
        scrollArea.setMinimumSize(QtCore.QSize(0, 80))
        scrollArea.setMaximumSize(QtCore.QSize(16777215, 700))
        scrollArea.setWidgetResizable(True)

        #self.scrollAreaWidgetContents = QtWidgets.QWidget()
        sAWidget.setGeometry(QtCore.QRect(0, 0, 327, 922))

        #self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.scrollAreaWidgetContents)

        #self.insideScrollArea = QtWidgets.QFrame(self.scrollAreaWidgetContents)
        inside.setMinimumSize(QtCore.QSize(0, 900))
        inside.setMaximumSize(QtCore.QSize(16777215, 900))
        inside.setFrameShape(QtWidgets.QFrame.StyledPanel)
        inside.setFrameShadow(QtWidgets.QFrame.Raised)

        self.input_template(groupInputLabel[0], groupInput[0], 10)
        self.input_template(groupInputLabel[1], groupInput[1], 90)
        self.input_template(groupInputLabel[2], groupInput[2], 170)
        self.input_template(groupInputLabel[3], groupInput[3], 250)
        self.input_template(groupInputLabel[4], groupInput[4], 330)
        self.input_template(groupInputLabel[5], groupInput[5], 410)
        self.input_template(groupInputLabel[6], groupInput[6], 490)
        self.input_template(groupInputLabel[7], groupInput[7], 570)
        self.input_template(groupInputLabel[8], groupInput[8], 650)
        self.input_template(groupInputLabel[9], groupInput[9], 730)


        groupInputLabel[0].setText(self._translate("Form", "Input 1"))
        groupInputLabel[1].setText(self._translate("Form", "Input 2"))
        groupInputLabel[2].setText(self._translate("Form", "Input 3"))
        groupInputLabel[3].setText(self._translate("Form", "Input 4"))
        groupInputLabel[4].setText(self._translate("Form", "Input 5"))
        groupInputLabel[5].setText(self._translate("Form", "Input 6"))
        groupInputLabel[6].setText(self._translate("Form", "Input 7"))
        groupInputLabel[7].setText(self._translate("Form", "Input 8"))
        groupInputLabel[8].setText(self._translate("Form", "Input 9"))
        groupInputLabel[9].setText(self._translate("Form", "Input 10"))


        sendButton.setGeometry(QtCore.QRect(10, 810, 101, 51))
        sendButton.setText(self._translate("Form", "Send Data"))
        vertical2.addWidget(inside)
        scrollArea.setWidget(sAWidget)
        vertical.addWidget(scrollArea)

    def output_template(self, outputName, x, y):
        outputName.setGeometry(QtCore.QRect(x, y, 171, 51))
        outputName.setStyleSheet("background-color: rgb(255, 255, 255);")

    def connectSerial(self):
        # printing the form information

        if (self.connectButton.isChecked()==True):
            try:
                global ser
                ser = serial.Serial(
                    port='COM6', \
                    baudrate=9600, \
                    parity=serial.PARITY_NONE, \
                    stopbits=serial.STOPBITS_ONE, \
                    bytesize=serial.EIGHTBITS, \
                    timeout=0)
                # this will store the line
                line = []
            except:
                print("Serial Connect Failed!!")
            else:
                print("connected to: " + ser.portstr)

        elif (self.connectButton.isChecked()==False):
            try:
                ser.close()
            except:
                print("Disconnect to serial FAILED!!")
            else:
                print("Disconnect to serial success")

    def send_form1(self):
        global output
        str = self.input1_1.text() + "; " + self.input1_2.text() + "; " + self.input1_3.text() + "; " \
              + self.input1_4.text() + "; " + self.input1_5.text() + "; " + self.input1_6.text() + "; " \
              + self.input1_7.text() + "; " + self.input1_8.text() + "; " + self.input1_9.text() + "; " + self.input1_10.text() + ";\r\n"
        sendValues(str)
        output = ["", "", "", "", "", "", "", "", "", ""]
        print("Kirim sukses!")

    def send_form2(self):
        global output
        str = self.input2_1.text() + "; " + self.input2_2.text() + "; " + self.input2_3.text() + "; " \
              + self.input2_4.text() + "; " + self.input2_5.text() + "; " + self.input2_6.text() + "; " \
              + self.input2_7.text() + "; " + self.input2_8.text() + "; " + self.input2_9.text() + "; " + self.input2_10.text() + ";\r\n"
        sendValues(str)
        output = ["", "", "", "", "", "", "", "", "", ""]
        print("Kirim sukses!")

    def send_form3(self):
        global output
        str = self.input3_1.text() + "; " + self.input3_2.text() + "; " + self.input3_3.text() + "; " \
              + self.input3_4.text() + "; " + self.input3_5.text() + "; " + self.input3_6.text() + "; " \
              + self.input3_7.text() + "; " + self.input3_8.text() + "; " + self.input3_9.text() + "; " + self.input3_10.text() + ";\r\n"
        sendValues(str)
        output = ["", "", "", "", "", "", "", "", "", ""]
        print("Kirim sukses!")



    def readSerial(self):
        self.resultOutput = getValues()
        start = 0
        indeks = 0
        count = 0
        #print(len(self.resultOutput))
        while (count < len(self.resultOutput)-1):
            #print(i, ":")
            #print(self.resultOutput[i], "       ", valid(self.resultOutput[i]))
            if(valid(self.resultOutput[count])==False):
                sparator=count
                for j in range (start, sparator):
                    output[indeks]+=self.resultOutput[j]
                indeks += 1
                start=sparator+1
                count+=1

            count+=1

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

            #if(self.resultOutput[i]=="2"):
                #print("Ditemukan di: ", i, "       ", self.resultOutput[i])

        print(output)


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    Form = QtWidgets.QWidget()
    ui = Ui_Form()
    ui.setupUi(Form)
    Form.show()
    sys.exit(app.exec_())
