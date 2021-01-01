import os
import json
import random
import string
from time import sleep
from threading import Thread
from pygame import mixer
from inputs import devices, get_gamepad, UnpluggedError
from flask import Flask, render_template
from pySerialTransfer import pySerialTransfer as txfer


app = Flask(__name__)
app.secret_key = ''.join(random.choice(string.printable) for i in range(1023))
app.config['MAX_CONTENT_LENGTH'] = 1000 * 1024 * 1024 # 1 GB file max


ARDUINO_PORT = r'/dev/ttyACM0'
BYTE_FORMAT = '<'
INPUTS = ['ABS_HAT0X',
          'ABS_HAT0Y',
          'ABS_RX',
          'ABS_RY',
          'ABS_RZ',
          'ABS_X',
          'ABS_Y',
          'ABS_Z',
          'BTN_EAST',
          'BTN_NORTH',
          'BTN_SELECT',
          'BTN_SOUTH',
          'BTN_START',
          'BTN_TL',
          'BTN_TR',
          'BTN_WEST']

data = {}
link = txfer.SerialTransfer(ARDUINO_PORT)


def play_mp3(mixer, voice_command):
    mp3_path = r'mp3/{}.mp3'.format(voice_command)
                    
    if os.path.exists(mp3_path):
        mixer.music.load('mp3/{}.mp3'.format(voice_command))
        mixer.music.play()

def handleGamepad():
    sleep(3)
    controller_connected = False
    
    while True:
        try:
            if devices.gamepads and not controller_connected:
                controller_connected = True
                play_mp3(mixer, 15)
                
            elif not devices.gamepads and controller_connected:
                controller_connected = False
                play_mp3(mixer, 16)
            
            events = get_gamepad()
        
            for event in events:
                if event.code in INPUTS:
                    send_size = link.tx_obj(int(event.state))
                    link.send(send_size, INPUTS.index(event.code))
                    sleep(0.002)
            
        except UnpluggedError:
            if controller_connected:
                controller_connected = False
                play_mp3(mixer, 16)

def getData():
    global data
    mixer.init()
    
    play_mp3(mixer, 14)
    
    while True:
        if link.available():
            if link.idByte == 0:
                altitude         = link.rxBuff[0] + (link.rxBuff[1] << 8)
                courseAngleIMU   = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=2, byte_format=BYTE_FORMAT))
                rollAngle        = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=6, byte_format=BYTE_FORMAT))
                pitchAngle       = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=10, byte_format=BYTE_FORMAT))
                pitotPressure    = link.rxBuff[14] + (link.rxBuff[15] << 8)
                validFlags       = link.rxBuff[16]
                latitude         = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=17, byte_format=BYTE_FORMAT))
                longitude        = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=21, byte_format=BYTE_FORMAT))
                UTC_year         = link.rxBuff[25] + (link.rxBuff[26] << 8)
                UTC_month        = link.rxBuff[27]
                UTC_day          = link.rxBuff[28]
                UTC_hour         = link.rxBuff[29]
                UTC_minute       = link.rxBuff[30]
                UTC_second       = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=31, byte_format=BYTE_FORMAT))
                speedOverGround  = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=35, byte_format=BYTE_FORMAT))
                courseOverGround = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=39, byte_format=BYTE_FORMAT))

                limiter_enable        = bool(link.rxBuff[43])
                manual_control_enable = bool(link.rxBuff[44])
                pitch_command         = link.rxBuff[45] + (link.rxBuff[46] << 8)
                roll_command          = link.rxBuff[47] + (link.rxBuff[48] << 8)
                yaw_command           = link.rxBuff[49] + (link.rxBuff[50] << 8)
                throttle_command      = link.rxBuff[51] + (link.rxBuff[52] << 8)
                autopilot_command     = link.rxBuff[53] + (link.rxBuff[54] << 8)
                limiter_command       = link.rxBuff[55] + (link.rxBuff[56] << 8)
                gear_command          = link.rxBuff[57] + (link.rxBuff[58] << 8)
                flaps_command         = link.rxBuff[59] + (link.rxBuff[60] << 8)

                linkConnected = bool(link.rxBuff[61])
                
                data['roll'] = rollAngle
                data['pitch'] = pitchAngle
                data['heading'] = courseAngleIMU
                data['vario'] = -2
                data['airspeed'] = speedOverGround
                data['altitude'] = altitude
            
            elif link.idByte == 1:
                voice_command  = link.rx_obj(obj_type=int,
                                             obj_byte_size=4)
                play_mp3(mixer, voice_command)
        
        elif link.status < 0:
            if link.status == txfer.CRC_ERROR:
                print('ERROR: CRC_ERROR')
            elif link.status == txfer.PAYLOAD_ERROR:
                print('ERROR: PAYLOAD_ERROR')
            elif link.status == txfer.STOP_BYTE_ERROR:
                print('ERROR: STOP_BYTE_ERROR')
            else:
                print('ERROR: {}'.format(link.status))


@app.route(r'/')
def home_form():
    return render_template(r'indicators.html')

@app.route(r'/indicatorData')
def indicator_data():
    return json.dumps(data)


if __name__ == '__main__':
    link.open()
    
    th1 = Thread(target=getData)
    th1.start()
    th2 = Thread(target=handleGamepad)
    th2.start()
    
    app.run(host='127.0.0.1', port=5000, debug=False, use_reloader=False, threaded=True)
    
    th1.join()
    th2.join()
