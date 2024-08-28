import os
import numpy as np
import pathlib

from PyQt5.QtWidgets import(
    QLabel,
    QMainWindow,
    QPushButton,
    QTabWidget,
    QWidget,
    QGridLayout,
    QGroupBox,
    QScrollArea,
    QLineEdit,
    QProgressBar,
    QComboBox,
    QSpinBox,
    QCheckBox
)

from PyQt5.QtGui import (
    QDoubleValidator
)

from PyQt5.QtCore import Qt
import pyqtgraph as pg

# Project modules
import exec
import gui_common
import postproc_flight

#------------------------------------------------------------------------------#

class MainWindow(QMainWindow):

    def __init__(self):

        super().__init__()

        self.setWindowTitle("hpr-sim")

        # Define tab widgets
        self.tabInput  = TabInput()
        self.tabOutput = TabOutput()
        #tabRecovery     = TabRecovery()
        #tabPerformance  = TabPerformance()
        #tabOptimization = TabOptimization()

        self.tabs = QTabWidget()
        self.tabs.insertTab(0, self.tabInput , "Simulation" )
        self.tabs.insertTab(1, self.tabOutput, "Data")

        self.setCentralWidget(self.tabs)

#------------------------------------------------------------------------------#

class TabInput(QWidget):

    def __init__(self):

        super().__init__()

        #----------------------------------------------------------------------#

        # Group: Simulation I/O
 
        self.groupIO  = QGroupBox("Simulation I/O")
        self.layoutIO = QGridLayout()
        self.groupIO.setLayout(self.layoutIO)

        # Input File
        self.labelInput  = gui_common.Label("Input File:")
        self.lineInput   = QLineEdit("file/path")
        self.buttonInput = gui_common.PushButton("<icon>")
        self.buttonInput.clicked.connect(gui_common.action_get_file(self,self.lineInput))

        # Output File
        self.labelOutput  = gui_common.Label("Output File:")
        self.lineOutput   = QLineEdit("file/path")
        self.buttonOutput = gui_common.PushButton("<icon>")
        self.buttonOutput.clicked.connect(gui_common.action_get_directory(self,self.lineOutput))

        # Populate group layout
        self.layoutIO.addWidget(self.labelInput  , 0, 0)
        self.layoutIO.addWidget(self.lineInput   , 0, 1)
        self.layoutIO.addWidget(self.buttonInput , 0, 2)
        self.layoutIO.addWidget(self.labelOutput , 0, 3)
        self.layoutIO.addWidget(self.lineOutput  , 0, 4)
        self.layoutIO.addWidget(self.buttonOutput, 0, 5)

        #----------------------------------------------------------------------#

        # Group: Simulation Parameters

        self.groupParam  = QGroupBox("Simulation Parameters")
        self.layoutParam = QGridLayout()
        self.groupParam.setLayout(self.layoutParam)

        self.labelModel = gui_common.Label("Model:")
        self.comboModel = QComboBox()
        self.comboModel.insertItems(0, ["item1", "item2", "item3"])

        self.labelParameter = gui_common.Label("Parameter:")
        self.comboParameter = QComboBox()
        self.comboParameter.insertItems(0, ["item1", "item2", "item3"])

        self.labelProperties  = QLabel()
        self.scrollProperties = QScrollArea()

        for i in range(10):
            self.labelProperties.setText(self.labelProperties.text() + f"test line {i}\n")

        self.scrollProperties.setWidget(self.labelProperties)
        self.scrollProperties.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
        self.scrollProperties.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.scrollProperties.setWidgetResizable(True)

        # Populate group layout
        self.layoutParam.addWidget(self.labelModel      , 0, 0, 1, 1)
        self.layoutParam.addWidget(self.comboModel      , 0, 1, 1, 1)
        self.layoutParam.addWidget(self.labelParameter  , 0, 2, 1, 1)
        self.layoutParam.addWidget(self.comboParameter  , 0, 3, 1, 1)
        self.layoutParam.addWidget(self.scrollProperties, 1, 0, 1, 10)

        #----------------------------------------------------------------------#

        # Group: Simulation Control

        self.groupControl  = QGroupBox("Simulation Control")
        self.layoutControl = QGridLayout()
        self.groupControl.setLayout(self.layoutControl)

        self.labelMcMode = gui_common.Label("MC Mode:")
        self.comboMcMode = QComboBox()
        self.comboMcMode.insertItems(0, ["nominal", "montecarlo"])

        self.labelSeed = gui_common.Label("Seed:")
        self.spinSeed  = gui_common.SpinBox(0, gui_common.SpinBox.spinBoxMax)

        self.labelNumMC = gui_common.Label("Num MC:")
        self.spinNumMC  = gui_common.SpinBox(1, gui_common.SpinBox.spinBoxMax)

        self.labelProcMode = gui_common.Label("Proc Mode:")
        self.comboProcMode = QComboBox()
        self.comboProcMode.insertItems(0, ["serial", "parallel"])

        self.labelNumProc = gui_common.Label("Num Proc:")
        self.spinNumProc  = gui_common.SpinBox(1, os.cpu_count())

        self.progressBar = QProgressBar()
        self.progressBar.setValue(50)

        self.labelRun = gui_common.Label("50/100")

        self.buttonRun = gui_common.PushButton("RUN")
        self.buttonRun.clicked.connect(self.action_run_exec)

        # Populate group layout
        self.layoutControl.addWidget(self.labelMcMode  , 0,  0, 1,  1)
        self.layoutControl.addWidget(self.comboMcMode  , 0,  1, 1,  1)
        self.layoutControl.addWidget(self.labelSeed    , 0,  2, 1,  1)
        self.layoutControl.addWidget(self.spinSeed     , 0,  3, 1,  1)
        self.layoutControl.addWidget(self.labelNumMC   , 0,  4, 1,  1)
        self.layoutControl.addWidget(self.spinNumMC    , 0,  5, 1,  1)
        self.layoutControl.addWidget(self.labelProcMode, 0,  6, 1,  1)
        self.layoutControl.addWidget(self.comboProcMode, 0,  7, 1,  1)
        self.layoutControl.addWidget(self.labelNumProc , 0,  8, 1,  1)
        self.layoutControl.addWidget(self.spinNumProc  , 0,  9, 1,  1)
        self.layoutControl.addWidget(self.labelRun     , 1, 10, 1,  1)
        self.layoutControl.addWidget(self.progressBar  , 1,  0, 1, 10)
        self.layoutControl.addWidget(self.buttonRun    , 0, 10, 1,  1)

        #----------------------------------------------------------------------#

        # Populate tab layout
        self.layoutTab = QGridLayout()
        self.layoutTab.addWidget(self.groupIO     ,  0, 0,  1, 1)
        self.layoutTab.addWidget(self.groupParam  ,  1, 0, 10, 1)
        self.layoutTab.addWidget(self.groupControl, 11, 0,  1, 1)

        self.setLayout(self.layoutTab)

    def action_run_exec(self):

        inputPath  = pathlib.Path(self.lineInput.text())
        outputPath = pathlib.Path(self.lineOutput.text())
        configPath = pathlib.Path(__file__).parent.parent.parent / "config"
        exec.run(inputPath, outputPath, configPath)

#------------------------------------------------------------------------------#

class TabOutput(QWidget):

    def __init__(self):

        super().__init__()

        #----------------------------------------------------------------------#

        self.groupIO  = QGroupBox("Data I/O")
        self.layoutIO = QGridLayout()
        self.groupIO.setLayout(self.layoutIO)

        # Output Path
        self.labelOutput  = gui_common.Label("Output Path:")
        self.lineOutput   = QLineEdit("file/path")
        self.buttonOutput = gui_common.PushButton("<icon>")
        self.buttonOutput.clicked.connect(gui_common.action_get_directory(self, self.lineOutput))

        # Load data
        self.buttonLoad = gui_common.PushButton("Load")
        self.buttonLoad.clicked.connect(self.action_load_data)

        # Populate group layout
        self.layoutIO.addWidget(self.labelOutput , 0, 0)
        self.layoutIO.addWidget(self.lineOutput  , 0, 1)
        self.layoutIO.addWidget(self.buttonOutput, 0, 2)
        self.layoutIO.addWidget(self.buttonLoad  , 0, 3)

        #----------------------------------------------------------------------#

        self.groupVis = QGroupBox("Data Visualization")
        self.layoutVis = QGridLayout()
        self.groupVis.setLayout(self.layoutVis)

        self.labelField = gui_common.Label("Field:")
        self.comboField = QComboBox()
        self.comboField.currentIndexChanged.connect(self.action_plot_update)

        self.labelUnits = gui_common.Label("Units:")
        self.comboUnits = QComboBox()
        self.comboUnits.currentIndexChanged.connect(self.action_plot_update)

        self.labelRunNum = gui_common.Label("Run Num:")
        self.spinRunNum  = gui_common.SpinBox(0,0)
        self.spinRunNum.valueChanged.connect(self.action_plot_update)

        self.labelRunAll = gui_common.Label("Plot All:")
        self.checkRunAll = QCheckBox()
        # connect(self.action_plot_update)

        x = np.array([1, 2, 3])
        y = np.array([1, 4, 9])
        self.plot = pg.plot(x, y, pen='b')
        self.plot.setBackground('w')

        # Plot axis controls
        self.labelAxisXMin = gui_common.Label("Axis X-Min:")
        self.lineAxisXMin  = QLineEdit()
        self.lineAxisXMin.returnPressed.connect(self.action_plot_update)
        self.lineAxisXMin.setValidator(QDoubleValidator())

        self.labelAxisXMax = gui_common.Label("Axis X-Max:")
        self.lineAxisXMax  = QLineEdit()
        self.lineAxisXMax.returnPressed.connect(self.action_plot_update)
        self.lineAxisXMax.setValidator(QDoubleValidator())

        self.labelAxisYMin = gui_common.Label("Axis Y-Min:")
        self.lineAxisYMin  = QLineEdit()
        self.lineAxisYMin.returnPressed.connect(self.action_plot_update)
        self.lineAxisYMin.setValidator(QDoubleValidator())

        self.labelAxisYMax = gui_common.Label("Axis Y-Max:")
        self.lineAxisYMax  = QLineEdit()
        self.lineAxisYMax.returnPressed.connect(self.action_plot_update)
        self.lineAxisYMax.setValidator(QDoubleValidator())

        # Populate group layout
        self.layoutVis.addWidget(self.labelField   ,  0, 0, 1,  1)
        self.layoutVis.addWidget(self.comboField   ,  0, 1, 1,  1)
        self.layoutVis.addWidget(self.labelUnits   ,  0, 2, 1,  1)
        self.layoutVis.addWidget(self.comboUnits   ,  0, 3, 1,  1)
        self.layoutVis.addWidget(self.labelRunNum  ,  0, 4, 1,  1)
        self.layoutVis.addWidget(self.spinRunNum   ,  0, 5, 1,  1)
        self.layoutVis.addWidget(self.labelRunAll  ,  0, 6, 1,  1)
        self.layoutVis.addWidget(self.checkRunAll  ,  0, 7, 1,  1)
        self.layoutVis.addWidget(self.plot         ,  1, 0, 8, 10)
        self.layoutVis.addWidget(self.labelAxisXMin, 10, 0, 1,  1)
        self.layoutVis.addWidget(self.lineAxisXMin , 10, 1, 1,  1)
        self.layoutVis.addWidget(self.labelAxisXMax, 10, 2, 1,  1)
        self.layoutVis.addWidget(self.lineAxisXMax , 10, 3, 1,  1)
        self.layoutVis.addWidget(self.labelAxisYMin, 10, 4, 1,  1)
        self.layoutVis.addWidget(self.lineAxisYMin , 10, 5, 1,  1)
        self.layoutVis.addWidget(self.labelAxisYMax, 10, 6, 1,  1)
        self.layoutVis.addWidget(self.lineAxisYMax , 10, 7, 1,  1)

        #----------------------------------------------------------------------#

        # Populate tab layout
        layoutTab = QGridLayout()
        layoutTab.addWidget(self.groupIO , 0, 0,  1, 1)
        layoutTab.addWidget(self.groupVis, 1, 0, 10, 1)
        self.setLayout(layoutTab)

    def action_load_data(self):

        outputPath = pathlib.Path(self.lineOutput.text())
        self.telem = postproc_flight.load_mc(outputPath)

        # Update comboboxes
        self.comboField.clear()
        self.comboField.addItems(self.telem[0]["fields"])

        #self.comboField.clear()
        #self.comboField.addItems()

        # Update spinbox
        self.spinRunNum.setMaximum(len(self.telem))

    def action_plot_update(self):

        runNum = self.spinRunNum.value()
        print(runNum)
