import os
import numpy as np

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
    QSizePolicy,
    QProgressBar,
    QComboBox,
    QSpinBox,
    QCheckBox
)

from PyQt5.QtCore import Qt
import pyqtgraph as pg


# Project modules
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
        self.labelInput  = Label("Input File:")
        self.lineInput   = QLineEdit("file/path")
        self.buttonInput = PushButton("<icon>")
        self.buttonInput.clicked.connect(gui_actions.get_file_action(self,self.lineInput))

        # Output File
        self.labelOutput  = Label("Output File:")
        self.lineOutput   = QLineEdit("file/path")
        self.buttonOutput = PushButton("<icon>")
        self.buttonOutput.clicked.connect(gui_actions.get_directory_action(self,self.lineOutput))

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

        self.labelModel = Label("Model:")
        self.comboModel = QComboBox()
        self.comboModel.insertItems(0, ["item1", "item2", "item3"])

        self.labelParameter = Label("Parameter:")
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

        self.labelMcMode   = Label("MC Mode:")
        self.comboMcMode   = QComboBox()
        self.comboMcMode.insertItems(0, ["nominal", "montecarlo"])

        self.labelSeed = Label("Seed:")
        self.spinSeed  = QSpinBox()
        self.spinSeed.setMinimum(0)
        self.spinSeed.setMaximum(spinBoxMax)

        self.labelNumMC = Label("Num MC:")
        self.spinNumMC  = QSpinBox()
        self.spinNumMC.setMinimum(1)
        self.spinNumMC.setMaximum(spinBoxMax)

        self.labelProcMode = Label("Proc Mode:")
        self.comboProcMode = QComboBox()
        self.comboProcMode.insertItems(0, ["serial", "parallel"])

        self.labelNumProc = Label("Num Proc:")
        self.spinNumProc  = QSpinBox()
        self.spinNumProc.setMinimum(1)
        self.spinNumProc.setMaximum(os.cpu_count())

        self.progressBar = QProgressBar()
        self.progressBar.setValue(50)

        self.labelRun = Label("50/100")

        self.buttonRun = PushButton("RUN")
        self.buttonRun.clicked.connect(gui_actions.run_exec_action(self.lineInput, self.lineOutput))

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

#------------------------------------------------------------------------------#

class TabOutput(QWidget):

    def __init__(self):

        super().__init__()

        #----------------------------------------------------------------------#

        self.groupIO  = QGroupBox("Data I/O")
        self.layoutIO = QGridLayout()
        self.groupIO.setLayout(self.layoutIO)

        # Output Path
        self.labelOutput  = Label("Output Path:")
        self.lineOutput   = QLineEdit("file/path")
        self.buttonOutput = PushButton("<icon>")
        self.buttonOutput.clicked.connect(gui_actions.get_directory_action(self, self.lineOutput))

        # Load data
        self.buttonLoad = PushButton("Load")
        #buttonLoad.clicked.connect(gui_actions.load_data_action(self, lineOutput))

        # Populate group layout
        self.layoutIO.addWidget(self.labelOutput , 0, 0)
        self.layoutIO.addWidget(self.lineOutput  , 0, 1)
        self.layoutIO.addWidget(self.buttonOutput, 0, 2)
        self.layoutIO.addWidget(self.buttonLoad  , 0, 3)

        #----------------------------------------------------------------------#

        self.groupVis = QGroupBox("Data Visualization")
        self.layoutVis = QGridLayout()
        self.groupVis.setLayout(self.layoutVis)

        self.labelField = Label("Field:")
        self.comboField = QComboBox()
        #comboField.insertItems(0, ["item1", "item2", "item3"])

        self.labelUnits = Label("Units:")
        self.comboUnits = QComboBox()
        #comboUnits.insertItems(0, ["item1", "item2", "item3"])

        self.labelRunNum = Label("Run Num:")

        self.spinRunNum = QSpinBox()
        self.spinRunNum.setMinimum(1)
        self.spinRunNum.setMaximum(1)

        self.labelRunAll  = Label("Plot All:")
        self.checkRunAll = QCheckBox()

        x = np.array([1, 2, 3])
        y = np.array([1, 4, 9])
        self.plot = pg.plot(x, y, pen='b')
        self.plot.setBackground('w')

        # Plot axis controls
        self.labelAxisXMin = Label("Axis X-Min:")
        self.lineAxisXMin  = QLineEdit()

        self.labelAxisXMax = Label("Axis X-Max:")
        self.lineAxisXMax  = QLineEdit()

        self.labelAxisYMin = Label("Axis Y-Min:")
        self.lineAxisYMin  = QLineEdit()

        self.labelAxisYMax = Label("Axis Y-Max:")
        self.lineAxisYMax  = QLineEdit()

        # Plot update
        self.buttonUpdate = PushButton("Update")

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
        self.layoutVis.addWidget(self.buttonUpdate , 10, 8, 1,  2)

        #----------------------------------------------------------------------#

        # Populate tab layout
        layoutTab = QGridLayout()
        layoutTab.addWidget(self.groupIO, 0, 0, 1, 1)
        layoutTab.addWidget(self.groupVis, 1, 0, 10, 1)
        self.setLayout(layoutTab)
