from PyQt5.QtWidgets import(
    QApplication,
    QLabel,
    QMainWindow,
    QPushButton,
    QTabWidget,
    QWidget,
)

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
        tabs.insertTab(0, tabInput , "Input" )
        tabs.insertTab(1, tabOutput, "Output")

        self.setCentralWidget(tabs)

class TabInput(QWidget):

    def __init__(self):

        super().__init__()

class TabOutput(QWidget):

    def __init__(self):

        super().__init__()
