import os
import csv
import time
import codecs
#from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import QThread
from WarThunder import telemetry, mapinfo
from pySerialTransfer import pySerialTransfer as txfer


class PilSimThread(QThread):
    def __init__(self, parent=None):
        super(PilSimThread, self).__init__(parent)
        
        self.parent        = parent
        self.telem         = telemetry.TelemInterface()
        self.link          = None
        self.connected     = False
        self.send_setpoint = False
        self.map_size      = 65
        self.anchor_coord  = [0, 0]
        
        self.waypoint_fields  = None
        self.waypoint_entries = []
        
        self.current_index = 0
        self.prev_lat = 0
        self.prev_lon = 0
        self.next_lat = 0
        self.next_lon = 0
        self.next_hdg = 0
        self.next_alt = 0
        self.next_ias = 0
        
        self.min_alt = 0
        self.max_alt = 0
        self.min_hdg = 0
        self.max_hdg = 0
        self.max_rad = 0
    
    def disconnect(self):
        self.parent.ui.waypoint_file_enable.setEnabled(True)
        self.link.close()
        self.connected = False
    
    def read_csv(self):
        csvreader = csv.reader(codecs.open(self.parent.ui.waypoint_file_label.text(), 'rU', 'utf-16'))
        
        self.waypoint_fields  = next(csvreader)
        self.waypoint_entries = []
        
        for row in csvreader: 
            self.waypoint_entries.append(row)
        
        self.prev_lat = None
        self.prev_lon = None
        
        self.next_lat = float(self.waypoint_entries[self.current_index][0])
        self.next_lon = float(self.waypoint_entries[self.current_index][1])
        self.next_hdg = float(self.waypoint_entries[self.current_index][2])
        self.next_alt = float(self.waypoint_entries[self.current_index][3])
        self.next_ias = float(self.waypoint_entries[self.current_index][4])
        
        self.max_rad = 50
        
        self.ptp_dist = None
    
    def send_setpoint_data(self):
        if self.connected:
            send_size = 0
            
            float_ = float(self.parent.ui.pitch_box.value())
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.parent.ui.roll_box.value())
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.parent.ui.heading_box.value())
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.parent.ui.altitude_box.value())
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.parent.ui.airspeed_box.value())
            send_size = self.link.tx_obj(float_, send_size)
            
            self.link.send(send_size, 1)
        else:
            try:
                self.link = txfer.SerialTransfer(self.parent.ui.usb_port_box_to_arduino.currentText())
                self.link.open()
                self.connected = True
                
            except txfer.InvalidSerialPort:
                self.connected = False
                time.sleep(1)
    
    def update_map_info(self):
        if os.path.exists(mapinfo.MAP_PATH):
            #self.parent.ui.map.setPhoto(QPixmap(mapinfo.MAP_PATH))
            
            self.map_size = self.telem.map_info.grid_info['size_km']
            self.ULHC_lat = self.telem.map_info.grid_info['ULHC_lat']
            self.ULHC_lon = self.telem.map_info.grid_info['ULHC_lon']
            self.anchor_coord = [self.ULHC_lat, self.ULHC_lon]
    
    def send_data(self):
        if self.connected:
            send_size = 0
            
            float_ = float(self.telem.basic_telemetry['roll'])
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.telem.basic_telemetry['pitch'])
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.telem.basic_telemetry['heading'])
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.telem.basic_telemetry['altitude'])
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.telem.basic_telemetry['lat'])
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.telem.basic_telemetry['lon'])
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.telem.basic_telemetry['IAS'])
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.telem.basic_telemetry['flapState'])
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.telem.basic_telemetry['gearState'])
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.parent.ui.waypoint_file_enable.isChecked())
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.next_lat)
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.next_lon)
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.next_hdg)
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.next_alt)
            send_size = self.link.tx_obj(float_, send_size)
            
            float_ = float(self.next_ias)
            send_size = self.link.tx_obj(float_, send_size)
            
            try:
                self.link.send(send_size)
                
            except txfer.InvalidSerialPort:
                self.connected = False
                time.sleep(1)
        else:
            try:
                self.link = txfer.SerialTransfer(self.parent.ui.usb_port_box_to_arduino.currentText())
                self.link.open()
                self.connected = True
                
            except txfer.InvalidSerialPort:
                self.connected = False
                time.sleep(1)
    
    def process_next_wp(self):
        self.current_index += 1
        
        if self.current_index > len(self.waypoint_entries): # restart waypoints
            self.current_index = 0
        
        self.prev_lat = self.next_lat
        self.prev_lon = self.next_lon
        
        self.next_lat = float(self.waypoint_entries[self.current_index][0])
        self.next_lon = float(self.waypoint_entries[self.current_index][1])
        self.next_hdg = float(self.waypoint_entries[self.current_index][2])
        self.next_alt = float(self.waypoint_entries[self.current_index][3])
        self.next_ias = float(self.waypoint_entries[self.current_index][4])
        
        self.min_alt = self.next_alt - (self.next_alt * 1)
        self.max_alt = self.next_alt + (self.next_alt * 1)
        self.min_hdg = self.next_hdg - 22.5
        self.max_hdg = self.next_hdg + 22.5
        self.max_rad = 50
        
        self.ptp_dist = mapinfo.coord_dist(self.prev_lat,
                                           self.prev_lon,
                                           self.next_lat,
                                           self.next_lon) * 1000
    
    def update_gui(self):
        self.parent.ui.pitch_current.setText(str(round(self.telem.basic_telemetry['pitch'], 2)))
        self.parent.ui.roll_current.setText(str(round(self.telem.basic_telemetry['roll'], 2)))
        self.parent.ui.heading_current.setText(str(round(self.telem.basic_telemetry['heading'], 2)))
        self.parent.ui.altitude_current.setText(str(round(self.telem.basic_telemetry['altitude'], 2)))
        self.parent.ui.airspeed_current.setText(str(round(self.telem.basic_telemetry['IAS'], 2)))
        self.parent.ui.latitude_current.setText('{:.10f}'.format(self.telem.basic_telemetry['lat']))
        self.parent.ui.longitude_current.setText('{:.10f}'.format(self.telem.basic_telemetry['lon']))
        self.parent.ui.latitude_waypoint.setText('{:.10f}'.format(self.next_lat))
        self.parent.ui.longitude_waypoint.setText('{:.10f}'.format(self.next_lon))
    
    def run(self):
        self.parent.ui.waypoint_file_enable.setEnabled(False)
        
        if self.parent.ui.waypoint_file_enable.isChecked():
            self.read_csv()
        
        try:
            while True:
                if self.send_setpoint:
                    self.send_setpoint = False
                    self.send_setpoint_data()
                
                if self.telem.get_telemetry():
                    self.update_map_info()
                    
                    if self.parent.ui.waypoint_file_enable.isChecked():
                        if self.prev_lat == None or self.prev_lon == None:
                            self.prev_lat = self.telem.basic_telemetry['lat']
                            self.prev_lon = self.telem.basic_telemetry['lon']
                            
                            self.ptp_dist = mapinfo.coord_dist(self.prev_lat,
                                                               self.prev_lon,
                                                               self.next_lat,
                                                               self.next_lon) * 1000
                        
                        from_prev_dist = mapinfo.coord_dist(self.prev_lat,
                                                            self.prev_lon,
                                                            self.telem.basic_telemetry['lat'],
                                                            self.telem.basic_telemetry['lon']) * 1000
                        
                        if from_prev_dist <= self.max_rad:
                            self.process_next_wp()
                    
                    self.update_gui()
                    self.send_data()
        except:
            self.connected = False