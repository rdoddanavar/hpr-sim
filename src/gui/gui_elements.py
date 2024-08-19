import os

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
    QProgressBar,
    QComboBox,
    QSpinBox
)

from PyQt5.QtCore import Qt

spinBoxMax = 2147483647 # Enforced by QSpinBox.setMaximum

class PushButton(QPushButton):

    # QPushButton w/ desired default settings

    def __init__(self,text):

        super().__init__(text)

        # Automatic rescaling in both dimensions
        self.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)

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

        # Group: Simulation I/O
 
        groupIO  = QGroupBox("Simulation I/O")
        layoutIO = QGridLayout()
        groupIO.setLayout(layoutIO)

        # Input File
        labelInput  = QLabel("Input File:")
        textInput   = QTextEdit("file/path")
        buttonInput = PushButton("<icon>")

        # Output File
        labelOutput  = QLabel("Output File:")
        textOutput   = QTextEdit("file/path")
        buttonOutput = PushButton("<icon>")

        layoutIO.addWidget(labelInput  , 0, 0)
        layoutIO.addWidget(textInput   , 0, 1)
        layoutIO.addWidget(buttonInput , 0, 2)
        layoutIO.addWidget(labelOutput , 0, 3)
        layoutIO.addWidget(textOutput  , 0, 4)
        layoutIO.addWidget(buttonOutput, 0, 5)

        #---------------------------------------------------------------------#

        # Group: Simulation Parameters

        groupParam = QGroupBox("Simulation Parameters")

        labelModel = QLabel("Model:")
        comboModel = QComboBox()
        comboModel.insertItems(0, ["item1", "item2", "item3"])

        labelParameter = QLabel("Parameter:")
        comboParameter = QComboBox()
        comboParameter.insertItems(0, ["item1", "item2", "item3"])

        label  = QLabel()
        scroll = QScrollArea()

        for i in range(10):
            label.setText(label.text() + f"test line {i}\n")

        scroll.setWidget(label)
        scroll.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
        scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        scroll.setWidgetResizable(True)

        layoutParam = QGridLayout()
        layoutParam.addWidget(labelModel, 0, 0, 1, 1)
        layoutParam.addWidget(comboModel, 0, 1, 1, 1)
        layoutParam.addWidget(labelParameter, 0, 2, 1, 1)
        layoutParam.addWidget(comboParameter, 0, 3, 1, 1)
        layoutParam.addWidget(scroll, 1, 0, 1, 10)
        groupParam.setLayout(layoutParam)

        #---------------------------------------------------------------------#

        # Group: Simulation Control

        groupControl  = QGroupBox("Simulation Control")
        layoutControl = QGridLayout()
        groupControl.setLayout(layoutControl)

        labelMcMode   = QLabel("MC Mode:")
        comboMcMode   = QComboBox()
        comboMcMode.insertItems(0, ["nominal", "montecarlo"])

        labelSeed = QLabel("Seed:")
        spinSeed  = QSpinBox()
        spinSeed.setMinimum(0)
        spinSeed.setMaximum(spinBoxMax)

        labelNumMC = QLabel("Num MC:")
        spinNumMC  = QSpinBox()
        spinNumMC.setMinimum(0)
        spinNumMC.setMaximum(spinBoxMax)

        labelProcMode = QLabel("Proc Mode:")
        comboProcMode = QComboBox()
        comboProcMode.insertItems(0, ["serial", "parallel"])

        labelNumProc = QLabel("Num Proc:")
        spinNumProc  = QSpinBox()
        spinNumProc.setMinimum(0)
        spinNumProc.setMaximum(os.cpu_count())

        progressBar = QProgressBar()
        progressBar.setValue(50)

        labelRun = QLabel("50/100")

        buttonRun = PushButton("RUN")

        layoutControl.addWidget(labelMcMode  , 0,  0, 1,  1)
        layoutControl.addWidget(comboMcMode  , 0,  1, 1,  1)
        layoutControl.addWidget(labelSeed    , 0,  2, 1,  1)
        layoutControl.addWidget(spinSeed     , 0,  3, 1,  1)
        layoutControl.addWidget(labelNumMC   , 0,  4, 1,  1)
        layoutControl.addWidget(spinNumMC    , 0,  5, 1,  1)
        layoutControl.addWidget(labelProcMode, 0,  6, 1,  1)
        layoutControl.addWidget(comboProcMode, 0,  7, 1,  1)
        layoutControl.addWidget(labelNumProc , 0,  8, 1,  1)
        layoutControl.addWidget(spinNumProc  , 0,  9, 1,  1)
        layoutControl.addWidget(labelRun     , 1, 10, 1,  1)
        layoutControl.addWidget(progressBar  , 1,  0, 1, 10)
        layoutControl.addWidget(buttonRun    , 0, 10, 1,  1)

        #---------------------------------------------------------------------#

        layoutTab = QGridLayout()
        layoutTab.addWidget(groupIO     , 0, 0, 1, 1)
        layoutTab.addWidget(groupParam  , 1, 0, 10, 1)
        layoutTab.addWidget(groupControl, 12, 0, 1, 1)

        self.setLayout(layoutTab)

class TabOutput(QWidget):

    def __init__(self):

        super().__init__()
