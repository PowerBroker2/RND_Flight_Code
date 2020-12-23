import os
from time import sleep
from pygame import mixer
from inputs import get_gamepad, UnpluggedError
from pySerialTransfer import pySerialTransfer as txfer


ARDUINO_PORT = r'/dev/ttyACM0'
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

controller_connected = False

altitude         = None
courseAngleIMU   = None
rollAngle        = None
pitchAngle       = None
pitotPressure    = None
validFlags       = None
latitude         = None
longitude        = None
UTC_year         = None
UTC_month        = None
UTC_day          = None
UTC_hour         = None
UTC_minute       = None
UTC_second       = None
speedOverGround  = None
courseOverGround = None

limiter_enable        = None
manual_control_enable = None
pitch_command         = None
roll_command          = None
yaw_command           = None
throttle_command      = None
autopilot_command     = None
limiter_command       = None
gear_command          = None
flaps_command         = None

linkConnected = None


def play_mp3(mixer, voice_command):
    mp3_path = r'mp3/{}.mp3'.format(voice_command)
                    
    if os.path.exists(mp3_path):
        mixer.music.load('mp3/{}.mp3'.format(voice_command))
        mixer.music.play()


if __name__ == '__main__':
    try:
        mixer.init()
        
        play_mp3(mixer, 14)
        
        link = txfer.SerialTransfer(ARDUINO_PORT)
        link.open()
        
        while True:
            try:
                events = get_gamepad()
                
                if not controller_connected:
                    controller_connected = True
                    play_mp3(mixer, 15)
            
                for event in events:
                    if event.code in INPUTS:
                        send_size = link.tx_obj(int(event.state))
                        link.send(send_size, INPUTS.index(event.code))
                        sleep(0.002)
            except UnpluggedError:
                if controller_connected:
                    controller_connected = False
                    play_mp3(mixer, 16)
            
            if link.available():
                if link.idByte == 0:
                    altitude         = link.rxBuff[1] + (link.rxBuff[0] << 8)
                    courseAngleIMU   = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=2))
                    rollAngle        = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=6))
                    pitchAngle       = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=10))
                    pitotPressure    = link.rxBuff[16] + (link.rxBuff[15] << 8)
                    validFlags       = link.rxBuff[17]
                    latitude         = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=18))
                    longitude        = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=22))
                    UTC_year         = link.rxBuff[27] + (link.rxBuff[26] << 8)
                    UTC_month        = link.rxBuff[28]
                    UTC_day          = link.rxBuff[29]
                    UTC_hour         = link.rxBuff[30]
                    UTC_minute       = link.rxBuff[31]
                    UTC_second       = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=32))
                    speedOverGround  = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=36))
                    courseOverGround = float(link.rx_obj(obj_type=float, obj_byte_size=4, start_pos=40))

                    limiter_enable        = bool(link.rxBuff[41])
                    manual_control_enable = bool(link.rxBuff[42])
                    pitch_command         = link.rxBuff[44] + (link.rxBuff[43] << 8)
                    roll_command          = link.rxBuff[46] + (link.rxBuff[45] << 8)
                    yaw_command           = link.rxBuff[48] + (link.rxBuff[47] << 8)
                    throttle_command      = link.rxBuff[50] + (link.rxBuff[49] << 8)
                    autopilot_command     = link.rxBuff[52] + (link.rxBuff[51] << 8)
                    limiter_command       = link.rxBuff[54] + (link.rxBuff[53] << 8)
                    gear_command          = link.rxBuff[56] + (link.rxBuff[55] << 8)
                    flaps_command         = link.rxBuff[58] + (link.rxBuff[57] << 8)

                    linkConnected = bool(link.rxBuff[59])
                
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
        
    except KeyboardInterrupt:
        try:
            link.close()
        except:
            pass
    
    except:
        import traceback
        traceback.print_exc()
        
        try:
            link.close()
        except:
            pass
