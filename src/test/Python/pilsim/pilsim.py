import os
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QFileDialog
from gui import Ui_PilSim
from pilsimThread import PilSimThread
from datalogThread import DatalogThread
from pySerialTransfer import pySerialTransfer as txfer


SCRIPT_PATH  = os.path.realpath(__file__)
SCRIPT_DIR   = os.path.dirname(SCRIPT_PATH)
LOG_DIR      = os.path.join(SCRIPT_DIR, 'log_files')
WAYPOINT_DIR = os.path.join(SCRIPT_DIR, 'waypoint_files')


class AppWindow(QMainWindow):
    '''
    Description:
    ------------
    Main GUI window class
    '''
    
    def __init__(self):
        super().__init__()
        self.ui = Ui_PilSim()
        self.ui.setupUi(self)
        self.show()
        
        self.boxes = [self.ui.pitch_box,
                      self.ui.roll_box,
                      self.ui.heading_box,
                      self.ui.altitude_box,
                      self.ui.airspeed_box]
        
        self.setup_other()
        self.setup_signals()
    
    def setup_signals(self):
        self.setup_buttons()
        self.setup_checkboxes()
        self.setup_comboboxes()
        self.setup_spinboxes()
        
    def setup_other(self):
        if not os.path.exists(LOG_DIR):
            os.makedirs(LOG_DIR)
        
        if not os.path.exists(WAYPOINT_DIR):
            os.makedirs(WAYPOINT_DIR)
        
        self.ui.log_file_label.setText(LOG_DIR)
        self.ui.waypoint_file_label.setText(WAYPOINT_DIR)
        self.pilsim_th = PilSimThread(self)
        self.data_th = DatalogThread(self)
    
    def setup_buttons(self):
        self.ui.waypoint_file_button.clicked.connect(self.change_waypoint_folder)
        self.ui.log_file_button.clicked.connect(self.change_log_folder)
        self.ui.refresh_ports.clicked.connect(self.update_ports)
    
    def change_waypoint_folder(self):
        path = QFileDialog.getOpenFileName(self, 'Open file', self.ui.waypoint_file_label.text(), '*.csv')[0]
        
        if path:
            self.ui.waypoint_file_label.setText(path)
    
    def change_log_folder(self):
        path = QFileDialog.getExistingDirectory(self)
        
        if path:
            self.ui.log_file_label.setText(path)
    
    def setup_checkboxes(self):
        self.ui.pilsim_enable.clicked.connect(self.pilsim_main)
        self.ui.waypoint_file_enable.clicked.connect(self.waypoint_file_check)
        
    def setup_comboboxes(self):
        self.update_ports()
    
    def setup_spinboxes(self):
        for box in self.boxes:
            box.valueChanged.connect(self.send_setpoints)
    
    def send_setpoints(self):
        self.pilsim_th.send_setpoint = True
    
    def update_ports(self):
        self.ui.usb_port_box_to_arduino.clear()
        self.ui.usb_port_box_to_arduino.addItems(txfer.serial_ports())
        self.ui.usb_port_box_from_arduino.clear()
        self.ui.usb_port_box_from_arduino.addItems(txfer.serial_ports())
    
    def pilsim_main(self):
        if self.ui.pilsim_enable.isChecked():
            self.pilsim_th.start()
            self.data_th.start()
        else:
            self.pilsim_th.disconnect()
            self.pilsim_th.terminate()
            self.data_th.disconnect()
            self.data_th.terminate()
    
    def waypoint_file_check(self):
        if not self.ui.waypoint_file_label.text().endswith('.csv') or not os.path.exists(self.ui.waypoint_file_label.text()):
            self.ui.waypoint_file_enable.setChecked(False)


def main():
    '''
    Description:
    ------------
    Main program to run
    '''
    
    try:
        app = QApplication(sys.argv)
        w   = AppWindow()
        w.show()
        sys.exit(app.exec_())
        
    except (KeyboardInterrupt, SystemExit):
        pass
    
    except:
        import traceback
        traceback.print_exc()
        
    try:
        if w.pilsim_th.isRunning():
            w.pilsim_th.disconnect()
            w.pilsim_th.terminate()
    except AttributeError:
        pass
    
    try:
        if w.data_th.isRunning():
            w.data_th.disconnect()
            w.data_th.terminate()
    except AttributeError:
        pass


if __name__ == '__main__':
    main()
    
    