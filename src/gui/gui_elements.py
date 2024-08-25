import os
import numpy as np

from PyQt5.QtWidgets import(
    QApplication,
    QLabel,
    QMainWindow,
    QPushButton,
    QTabWidget,
    QWidget,
    QGridLayout,
    QGroupBox,
    QScrollArea,
    QLineEdit,
    QSizePolicy,
    QProgressBar,
    QComboBox,
    QSpinBox,
    QCheckBox
)

from PyQt5.QtCore import Qt
import pyqtgraph as pg


import functools
import gui_actions

spinBoxMax = 2147483647 # Enforced by QSpinBox.setMaximum

#------------------------------------------------------------------------------#

class Label(QLabel):

    # QLabel w/ desired default settings

    def __init__(self,text):

        super().__init__(text)

        # Center text
        self.setAlignment(Qt.AlignCenter)

class PushButton(QPushButton):

    # QPushButton w/ desired default settings

    def __init__(self,text):

        super().__init__(text)

        # Automatic rescaling in both dimensions
        self.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)

#------------------------------------------------------------------------------#

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

#------------------------------------------------------------------------------#

class TabInput(QWidget):

    def __init__(self):

        super().__init__()

        #----------------------------------------------------------------------#

        # Group: Simulation I/O
 
        groupIO  = QGroupBox("Simulation I/O")
        layoutIO = QGridLayout()
        groupIO.setLayout(layoutIO)

        # Input File
        labelInput  = Label("Input File:")
        lineInput   = QLineEdit("file/path")
        buttonInput = PushButton("<icon>")
        buttonInput.clicked.connect(gui_actions.get_file_action(self,lineInput))

        # Output File
        labelOutput  = Label("Output File:")
        lineOutput   = QLineEdit("file/path")
        buttonOutput = PushButton("<icon>")
        buttonOutput.clicked.connect(gui_actions.get_directory_action(self,lineOutput))

        # Populate group layout
        layoutIO.addWidget(labelInput  , 0, 0)
        layoutIO.addWidget(lineInput   , 0, 1)
        layoutIO.addWidget(buttonInput , 0, 2)
        layoutIO.addWidget(labelOutput , 0, 3)
        layoutIO.addWidget(lineOutput  , 0, 4)
        layoutIO.addWidget(buttonOutput, 0, 5)

        #----------------------------------------------------------------------#

        # Group: Simulation Parameters

        groupParam  = QGroupBox("Simulation Parameters")
        layoutParam = QGridLayout()
        groupParam.setLayout(layoutParam)

        labelModel = Label("Model:")
        comboModel = QComboBox()
        comboModel.insertItems(0, ["item1", "item2", "item3"])

        labelParameter = Label("Parameter:")
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

        # Populate group layout
        layoutParam.addWidget(labelModel, 0, 0, 1, 1)
        layoutParam.addWidget(comboModel, 0, 1, 1, 1)
        layoutParam.addWidget(labelParameter, 0, 2, 1, 1)
        layoutParam.addWidget(comboParameter, 0, 3, 1, 1)
        layoutParam.addWidget(scroll, 1, 0, 1, 10)

        #----------------------------------------------------------------------#

        # Group: Simulation Control

        groupControl  = QGroupBox("Simulation Control")
        layoutControl = QGridLayout()
        groupControl.setLayout(layoutControl)

        labelMcMode   = Label("MC Mode:")
        comboMcMode   = QComboBox()
        comboMcMode.insertItems(0, ["nominal", "montecarlo"])

        labelSeed = Label("Seed:")
        spinSeed  = QSpinBox()
        spinSeed.setMinimum(0)
        spinSeed.setMaximum(spinBoxMax)

        labelNumMC = Label("Num MC:")
        spinNumMC  = QSpinBox()
        spinNumMC.setMinimum(1)
        spinNumMC.setMaximum(spinBoxMax)

        labelProcMode = Label("Proc Mode:")
        comboProcMode = QComboBox()
        comboProcMode.insertItems(0, ["serial", "parallel"])

        labelNumProc = Label("Num Proc:")
        spinNumProc  = QSpinBox()
        spinNumProc.setMinimum(1)
        spinNumProc.setMaximum(os.cpu_count())

        progressBar = QProgressBar()
        progressBar.setValue(50)

        labelRun = Label("50/100")

        buttonRun = PushButton("RUN")

        # Populate group layout
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

        #----------------------------------------------------------------------#

        # Populate tab layout
        layoutTab = QGridLayout()
        layoutTab.addWidget(groupIO     , 0, 0, 1, 1)
        layoutTab.addWidget(groupParam  , 1, 0, 10, 1)
        layoutTab.addWidget(groupControl, 11, 0, 1, 1)

        self.setLayout(layoutTab)

#------------------------------------------------------------------------------#

class TabOutput(QWidget):

    def __init__(self):

        super().__init__()

        #----------------------------------------------------------------------#

        groupIO = QGroupBox("Data I/O")
        layoutIO = QGridLayout()
        groupIO.setLayout(layoutIO)

        # Output Path
        labelOutput  = Label("Output Path:")
        lineOutput   = QLineEdit("file/path")
        buttonOutput = PushButton("<icon>")
        buttonOutput.clicked.connect(gui_actions.get_directory_action(self,lineOutput))

        # Populate group layout
        layoutIO.addWidget(labelOutput , 0, 0)
        layoutIO.addWidget(lineOutput  , 0, 1)
        layoutIO.addWidget(buttonOutput, 0, 2)

        #----------------------------------------------------------------------#

        groupVis = QGroupBox("Data Visualization")
        layoutVis = QGridLayout()
        groupVis.setLayout(layoutVis)

        labelField = Label("Field:")
        comboField = QComboBox()
        comboField.insertItems(0, ["item1", "item2", "item3"])

        labelUnits = Label("Units:")
        comboUnits = QComboBox()
        comboUnits.insertItems(0, ["item1", "item2", "item3"])

        labelRunNum = Label("Run Num:")

        spinRunNum = QSpinBox()
        spinRunNum.setMinimum(1)
        spinRunNum.setMaximum(100)

        labelRunAll  = Label("Plot All:")
        checkRunAll = QCheckBox()

        x = np.array([1, 2, 3])
        y = np.array([1, 4, 9])
        plot = pg.plot(x, y, pen='b')
        plot.setBackground('w')

        # Plot axis controls
        labelAxisXMin = Label("Axis X-Min:")
        lineAxisXMin  = QLineEdit()

        labelAxisXMax = Label("Axis X-Max:")
        lineAxisXMax  = QLineEdit()

        labelAxisYMin = Label("Axis Y-Min:")
        lineAxisYMin  = QLineEdit()

        labelAxisYMax = Label("Axis Y-Max:")
        lineAxisYMax  = QLineEdit()

        # Plot update
        buttonUpdate = PushButton("Update")

        # Populate group layout
        layoutVis.addWidget(labelField   ,  0, 0, 1,  1)
        layoutVis.addWidget(comboField   ,  0, 1, 1,  1)
        layoutVis.addWidget(labelUnits   ,  0, 2, 1,  1)
        layoutVis.addWidget(comboUnits   ,  0, 3, 1,  1)
        layoutVis.addWidget(labelRunNum  ,  0, 4, 1,  1)
        layoutVis.addWidget(spinRunNum   ,  0, 5, 1,  1)
        layoutVis.addWidget(labelRunAll  ,  0, 6, 1,  1)
        layoutVis.addWidget(checkRunAll  ,  0, 7, 1,  1)
        layoutVis.addWidget(plot         ,  1, 0, 8, 10)
        layoutVis.addWidget(labelAxisXMin, 10, 0, 1,  1)
        layoutVis.addWidget(lineAxisXMin , 10, 1, 1,  1)
        layoutVis.addWidget(labelAxisXMax, 10, 2, 1,  1)
        layoutVis.addWidget(lineAxisXMax , 10, 3, 1,  1)
        layoutVis.addWidget(labelAxisYMin, 10, 4, 1,  1)
        layoutVis.addWidget(lineAxisYMin , 10, 5, 1,  1)
        layoutVis.addWidget(labelAxisYMax, 10, 6, 1,  1)
        layoutVis.addWidget(lineAxisYMax , 10, 7, 1,  1)
        layoutVis.addWidget(buttonUpdate , 10, 8, 1,  2)

        #----------------------------------------------------------------------#

        # Populate tab layout
        layoutTab = QGridLayout()
        layoutTab.addWidget(groupIO, 0, 0, 1, 1)
        layoutTab.addWidget(groupVis, 1, 0, 10, 1)
        self.setLayout(layoutTab)
