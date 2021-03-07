import os
import time
import datetime as dt
from PyQt5.QtCore import QThread
from WarThunder import telemetry
from pySerialTransfer import pySerialTransfer as txfer


LOG_NAME_TEMPLATE = '{day}_{month}_{year}_{hour}_{min}_{sec}_{ms}.csv'


class DatalogThread(QThread):
    def __init__(self, parent=None):
        super(DatalogThread, self).__init__(parent)
        
        self.parent      = parent
        self.log         = self.parent.ui.log_file_enable.isChecked()
        self.telem       = telemetry.TelemInterface()
        self.connected   = False
        self.link        = None
        self.log_columns = ['timestamp',
                            'roll',
                            'pitch',
                            'heading',
                            'altitude',
                            'lat',
                            'lon',
                            'IAS',
                            'flap_state',
                            'gear_state',
                            'use_waypoints',
                            'next_lat',
                            'next_lon',
                            'pitch_setpoint',
                            'roll_setpoint',
                            'heading_setpoint',
                            'altitude_setpoint',
                            'IAS_setpoint']
        
        now = dt.datetime.now()
        self.log_name = LOG_NAME_TEMPLATE.format(day   = str(now.day).zfill(2),
                                                 month = str(now.month).zfill(2),
                                                 year  = now.year,
                                                 hour  = str(now.hour).zfill(2),
                                                 min   = str(now.minute).zfill(2),
                                                 sec   = str(now.second).zfill(2),
                                                 ms    = str(int(now.microsecond / 1000)).zfill(3))
        self.log_path = os.path.join(self.parent.ui.log_file_label.text(), self.log_name)
        
        if not os.path.exists(self.parent.ui.log_file_label.text()):
            os.makedirs(self.parent.ui.log_file_label.text())
    
    def disconnect(self):
        self.link.close()
        self.connected = False
        
    def run(self):
        while True:
            if self.log:
                if not self.connected:
                        try:
                            self.link = txfer.SerialTransfer(self.parent.ui.usb_port_box_from_arduino.currentText())
                            self.link.open()
                            self.connected = True
                            
                        except txfer.InvalidSerialPort:
                            self.connected = False
                            time.sleep(1)
                            
                if not os.path.exists(self.log_path):
                    with open(self.log_path, 'w') as log:
                        for item in self.log_columns:
                                log.write(item)
                                
                                if item == self.log_columns[-1]:
                                    log.write('\n')
                                else:
                                    log.write(',')
                
                if self.connected:
                    try:
                        if self.link.available():
                            rec_list = self.link.rx_obj(obj_type      = list,
                                                        obj_byte_size = 4 * 5,
                                                        list_format   = 'f')
                            
                            self.pitch = rec_list[0]
                            self.roll  = rec_list[1]
                            self.hdg   = rec_list[2]
                            self.alt   = rec_list[3]
                            self.ias   = rec_list[4]
                            
                            with open(self.log_path, 'a') as log:
                                log.write('{},'.format(dt.datetime.now().isoformat()))
                                log.write('{},'.format(self.parent.ui.roll_current.text()))
                                log.write('{},'.format(self.parent.ui.pitch_current.text()))
                                log.write('{},'.format(self.parent.ui.heading_current.text()))
                                log.write('{},'.format(self.parent.ui.altitude_current.text()))
                                log.write('{},'.format(self.parent.ui.latitude_current.text()))
                                log.write('{},'.format(self.parent.ui.longitude_current.text()))
                                log.write('{},'.format(self.parent.ui.airspeed_current.text()))
                                log.write('{},'.format(0))
                                log.write('{},'.format(0))
                                log.write('{},'.format(str(float(self.parent.ui.waypoint_file_enable.isChecked()))))
                                log.write('{},'.format(self.parent.ui.latitude_waypoint.text()))
                                log.write('{},'.format(self.parent.ui.longitude_waypoint.text()))
                                log.write('{},'.format(self.pitch))
                                log.write('{},'.format(self.roll))
                                log.write('{},'.format(self.hdg))
                                log.write('{},'.format(self.alt))
                                log.write('{}\n'.format(self.ias))
                    except:
                        self.connected = False