"""
This is UI for project_23_02_02
This UI will accept the user input and send it to STM32 process and controll the LED lamp
and read the feedback from STM32 to  UI
"""

from PyQt5 import QtCore, QtGui, QtWidgets
import serial
from past.builtins import raw_input

ser = []

def getValues():
    #ser.write(b'g')
    stmData = ser.readline().decode('ascii')
    return stmData

def sendValues(userInput):
    #print("jalan")
    ser.write(userInput.encode())
    stmData = ser.readline().decode('ascii')
    return stmData

class Ui_Form(object):
    def setupUi(self, Form):
        # User define variable
        self.resultOutput = "Result"
        self.input = ""

        Form.setObjectName("Form")
        Form.resize(777, 507)
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(Form)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.frame = QtWidgets.QFrame(Form)
        self.frame.setMinimumSize(QtCore.QSize(150, 50))
        self.frame.setMaximumSize(QtCore.QSize(200, 50))
        self.frame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame.setObjectName("frame")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.frame)
        self.verticalLayout.setObjectName("verticalLayout")

        # 1. Connect Serial Button
        self.connectButton = QtWidgets.QPushButton(self.frame)
        self.connectButton.setCursor(QtGui.QCursor(QtCore.Qt.PointingHandCursor))
        self.connectButton.setCheckable(True)
        self.connectButton.setObjectName("connectButton")
        self.connectButton.toggled.connect(self.connectSerial)
        self.verticalLayout.addWidget(self.connectButton)
        # 1. Connect Serial Button -- END


        self.verticalLayout_2.addWidget(self.frame)
        self.frame_2 = QtWidgets.QFrame(Form)
        self.frame_2.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame_2.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame_2.setObjectName("frame_2")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.frame_2)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.frame_3 = QtWidgets.QFrame(self.frame_2)
        self.frame_3.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame_3.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame_3.setObjectName("frame_3")

        # 2. Input Area
        self.inputArea = QtWidgets.QLineEdit(self.frame_3)
        self.inputArea.setGeometry(QtCore.QRect(10, 50, 351, 341))
        self.inputArea.setAlignment(QtCore.Qt.AlignLeading | QtCore.Qt.AlignLeft | QtCore.Qt.AlignTop)
        self.inputArea.setObjectName("inputArea")
        # 2. Input Area -- END

        # 3. Input Label
        self.input = QtWidgets.QLabel(self.frame_3)
        self.input.setGeometry(QtCore.QRect(20, 10, 81, 31))
        font = QtGui.QFont()
        font.setPointSize(12)
        font.setBold(True)
        font.setWeight(75)
        self.input.setFont(font)
        self.input.setObjectName("input")
        # 3. Input Label -- END

        self.horizontalLayout.addWidget(self.frame_3)
        self.frame_4 = QtWidgets.QFrame(self.frame_2)
        self.frame_4.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame_4.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame_4.setObjectName("frame_4")

        # 4. Write Button
        self.writeButton = QtWidgets.QPushButton(self.frame_4)
        self.writeButton.setGeometry(QtCore.QRect(10, 10, 140, 80))
        font = QtGui.QFont()
        font.setPointSize(16)
        font.setBold(True)
        font.setWeight(75)
        self.writeButton.setFont(font)
        self.writeButton.setCursor(QtGui.QCursor(QtCore.Qt.PointingHandCursor))
        self.writeButton.clicked.connect(self.writeSerial)
        self.writeButton.setObjectName("writeButton")
        # 4. Write Button -- END

        # 5. Read Button
        self.readButton = QtWidgets.QPushButton(self.frame_4)
        self.readButton.setGeometry(QtCore.QRect(10, 100, 140, 80))
        font = QtGui.QFont()
        font.setPointSize(16)
        font.setBold(True)
        font.setWeight(75)
        self.readButton.setFont(font)
        self.readButton.setCursor(QtGui.QCursor(QtCore.Qt.PointingHandCursor))
        self.readButton.clicked.connect(self.readSerial)
        self.readButton.setObjectName("readButton")
        # 5. Read Button - END

        # 6. Output Label
        self.output = QtWidgets.QLabel(self.frame_4)
        self.output.setGeometry(QtCore.QRect(20, 200, 81, 31))
        font = QtGui.QFont()
        font.setPointSize(12)
        font.setBold(True)
        font.setWeight(75)
        self.output.setFont(font)
        self.output.setObjectName("output")
        # 6. Output Label -- END

        # 7. Output Area
        self.result = QtWidgets.QLabel(self.frame_4)
        self.result.setGeometry(QtCore.QRect(20, 240, 341, 141))
        font = QtGui.QFont()
        font.setPointSize(12)
        font.setWeight(50)
        self.result.setFont(font)
        self.result.setAutoFillBackground(False)
        self.result.setStyleSheet("background-color: rgb(255, 255, 255);")
        self.result.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignTop)
        self.result.setObjectName("result")
        # 7. Output Area -- END

        self.horizontalLayout.addWidget(self.frame_4)
        self.verticalLayout_2.addWidget(self.frame_2)

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Form"))
        self.connectButton.setText(_translate("Form", "Connect To Serial"))
        self.input.setText(_translate("Form", "INPUT"))
        self.writeButton.setText(_translate("Form", "Write"))
        self.readButton.setText(_translate("Form", "Read"))
        self.output.setText(_translate("Form", "OUTPUT"))
        self.result.setText(_translate("Form", self.resultOutput))

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

    def writeSerial(self):
        print(self.inputArea.text())
        sendValues(self.inputArea.text())

    def readSerial(self):
        self.resultOutput = getValues()
        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    Form = QtWidgets.QWidget()
    ui = Ui_Form()
    ui.setupUi(Form)
    Form.show()
    sys.exit(app.exec_())
