from PyQt5.QtWidgets import(
    QApplication,
    QLabel,
    QMainWindow,
    QPushButton,
    QTabWidget,
    QWidget,
    QGridLayout,
    QGroupBox,
    QLabel,
    QScrollArea,
    QTextEdit,
    QSizePolicy,
    QProgressBar
)

from PyQt5.QtCore import Qt

class MainWindow(QMainWindow):

    def __init__(self):

        super().__init__()

        self.setWindowTitle("hpr-sim")

        # Define tab widgets
        tabInput        = TabInput()
        tabOutput       = TabOutput()
        #tabRecovery     = TabRecovery()
        #tabPerformance  = TabPerformance()
        #tabOptimization = TabOptimization()

        tabs = QTabWidget()
        tabs.insertTab(0, tabInput , "Simulation" )
        tabs.insertTab(1, tabOutput, "Data")

        self.setCentralWidget(tabs)

class TabInput(QWidget):

    def __init__(self):

        super().__init__()

        #---------------------------------------------------------------------#

        groupIO = QGroupBox("Simulation I/O")

        layoutIO = QGridLayout()
        buttonInput = QPushButton("get input")
        buttonInput.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)
        textInput = QTextEdit("file/path")

        layoutIO.addWidget(buttonInput, 0, 0)
        layoutIO.addWidget(textInput, 0, 1)
        groupIO.setLayout(layoutIO)

        #---------------------------------------------------------------------#

        groupParam = QGroupBox("Simulation Parameters")

        label  = QLabel()
        scroll = QScrollArea()

        for i in range(10):
            label.setText(label.text() + f"test line {i}\n")

        scroll.setWidget(label)
        scroll.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
        scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        scroll.setWidgetResizable(True)

        layoutParam = QGridLayout()
        layoutParam.addWidget(scroll, 0, 0)
        groupParam.setLayout(layoutParam)

        #---------------------------------------------------------------------#

        groupControl = QGroupBox("Simulation Control")

        buttonRun = QPushButton("Run Sim")
        buttonRun.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)

        progressBar = QProgressBar()
        progressBar.setValue(50)

        layoutControl = QGridLayout()
        layoutControl.addWidget(buttonRun, 0, 0)
        layoutControl.addWidget(progressBar, 0, 1)
        groupControl.setLayout(layoutControl)

        #---------------------------------------------------------------------#

        layoutTab = QGridLayout()
        layoutTab.addWidget(groupIO, 0, 0, 1, 1)
        layoutTab.addWidget(groupParam, 1, 0, 1, 1)
        layoutTab.addWidget(groupControl, 2, 0, 1, 1)

        self.setLayout(layoutTab)

class TabOutput(QWidget):

    def __init__(self):

        super().__init__()
