from PyQt5.QtWidgets import(
    QApplication,
    QLabel,
    QMainWindow,
    QPushButton,
    QTabWidget,
    QWidget,
)

import util_misc
import gui_elements

def exec():

    util_misc.qt_setup()

    app = QApplication([])
    window = gui_elements.MainWindow()
    window.show()
    app.exec()

if __name__ == "__main__":
    pass
