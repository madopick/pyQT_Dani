from PyQt5 import QtCore, QtGui, QtWidgets
import serial
from past.builtins import raw_input

variabel = ''

class Ui_Form(object):
    def setupUi(self, Form):
        #self.variabel = ''
        Form.setObjectName("Form")
        Form.resize(560, 382)

        # 1. Label OUTPUT
        self.Output = QtWidgets.QLabel(Form)
        self.Output.setGeometry(QtCore.QRect(30, 80, 131, 51))
        font = QtGui.QFont()
        font.setPointSize(20)
        font.setBold(True)
        font.setWeight(75)
        self.Output.setFont(font)
        self.Output.setObjectName("Ouput")
        # 1. Label OUTPUT - END

        # 2. Label Tampil
        self.Tampil = QtWidgets.QLabel(Form)
        self.Tampil.setGeometry(QtCore.QRect(30, 140, 131, 51))
        font = QtGui.QFont()
        font.setPointSize(20)
        font.setBold(False)
        font.setWeight(50)
        self.Tampil.setFont(font)
        self.Tampil.setObjectName("Tampil")
        # 2. Label Tampil - END

        # 3. Label INPUT
        self.Input = QtWidgets.QLabel(Form)
        self.Input.setGeometry(QtCore.QRect(240, 20, 131, 51))
        font = QtGui.QFont()
        font.setPointSize(20)
        font.setBold(True)
        font.setWeight(75)
        self.Input.setFont(font)
        self.Input.setObjectName("Input")
        # 3. Label INPUT - END

        # 4. Slow Button
        self.slowButton = QtWidgets.QPushButton(Form)
        self.slowButton.setGeometry(QtCore.QRect(240, 90, 91, 81))
        font = QtGui.QFont()
        font.setPointSize(11)
        font.setBold(True)
        font.setWeight(75)
        self.slowButton.setFont(font)
        self.slowButton.setStyleSheet("background-color: rgb(0, 170, 255);\n"
"color: rgb(255, 255, 255);")
        self.slowButton.setCheckable(False)
        self.slowButton.setObjectName("slowButton")
        self.slowButton.clicked.connect(self.slow)
        # 4. Slow Button - END

        # 5. Fast Button
        self.fastButton = QtWidgets.QPushButton(Form)
        self.fastButton.setGeometry(QtCore.QRect(240, 190, 91, 81))
        font = QtGui.QFont()
        font.setPointSize(11)
        font.setBold(True)
        font.setWeight(75)
        self.fastButton.setFont(font)
        self.fastButton.setStyleSheet("background-color: rgb(255, 70, 70);\n"
"color: rgb(255, 255, 255);")
        self.fastButton.setCheckable(False)
        self.fastButton.setObjectName("fastButton")
        self.fastButton.clicked.connect(self.fast)
        # 5. Fast Button - END

        # 6. Full ON Button
        self.fullOnButton = QtWidgets.QPushButton(Form)
        self.fullOnButton.setGeometry(QtCore.QRect(240, 290, 91, 81))
        font = QtGui.QFont()
        font.setPointSize(11)
        font.setBold(True)
        font.setWeight(75)
        self.fullOnButton.setFont(font)
        self.fullOnButton.setStyleSheet("background-color: rgb(124, 255, 135);\n"
"color: rgb(255, 255, 255);")
        self.fullOnButton.setCheckable(False)
        self.fullOnButton.setObjectName("fullOnButton")
        self.fullOnButton.clicked.connect(self.full_On)
        #self.fullOnButton.addAction(self.alOn())
        # 6. Full ON Button - END

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        global variabel
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Form"))
        if variabel == 'm':
            self.Tampil.setText(_translate("Form", 'm'))
        elif variabel == 'n':
            self.Tampil.setText(_translate("Form", 'n'))
        elif variabel == 'b':
            self.Tampil.setText(_translate("Form", 'b'))
        else:
            self.Tampil.setText(_translate("Form", 'nothing'))
        self.Output.setText(_translate("Form", "Output"))
        self.Input.setText(_translate("Form", "INPUT"))
        self.slowButton.setText(_translate("Form", "SLOW"))
        self.fastButton.setText(_translate("Form", "FAST"))
        self.fullOnButton.setText(_translate("Form", "FULL ON"))

    def slow(self):
        # printing the form information
        print("Slow telah ditekan")
        global variabel
        variabel = 'm'
        print(variabel)

    def fast(self):
        # printing the form information
        print("Fast telah ditekan")
        global variabel
        variabel = 'n'

    def full_On(self):
        # printing the form information
        print("Full on telah ditekan")
        global variabel
        variabel = 'b'


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    Form = QtWidgets.QWidget()
    ui = Ui_Form()
    ui.setupUi(Form)
    Form.show()

    sys.exit(app.exec_())






