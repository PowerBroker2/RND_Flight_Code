import os
import pickle
import pygame
from time import sleep
from pygame import mixer, joystick
from pySerialTransfer import pySerialTransfer as txfer


ARDUINO_PORT = r'/dev/ttyACM0'
BYTE_FORMAT = '<'
controller_connected = False


def play_mp3(voice_command):
    mp3_path = r'mp3/{}.mp3'.format(voice_command)
                    
    if os.path.exists(mp3_path):
        mixer.music.load('mp3/{}.mp3'.format(voice_command))
        mixer.music.play()

def handleGamepad(link, gamepad, clock):
    clock.tick()

    for event in pygame.event.get():
        event_members = dir(event)
        
        if 'button' in event_members or 'axis' in event_members or 'hat' in event_members:
            print(event.dict)
            print('\n')

def getData(link):
    data = {}
    
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
            
            fp = open('shared.pkl','wb')
            pickle.dump(data, fp)
        
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


if __name__ == '__main__':
    try:
        link = txfer.SerialTransfer(ARDUINO_PORT)
        link.open()
        
        pygame.init()
        clock = pygame.time.Clock()
        mixer.init()
        
        play_mp3(14)
        
        sleep(2.5)
        
        while not joystick.get_count():
            pass
            
        gamepad = pygame.joystick.Joystick(0)
        
        while(True):
            handleGamepad(link, gamepad, clock)
            getData(link)
    
    except KeyboardInterrupt:
        pass
    
    except:
        import traceback
        traceback.print_exc()
        
    link.close()
