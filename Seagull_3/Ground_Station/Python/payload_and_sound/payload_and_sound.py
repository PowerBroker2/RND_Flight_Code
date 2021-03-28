import os
import pickle
import pygame
from time import sleep
from pygame import mixer, joystick
from pySerialTransfer import pySerialTransfer as txfer


ARDUINO_PORT = r'/dev/ttyACM0'
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
            altitude         = link.rx_obj('H',   0)
            courseAngleIMU   = link.rx_obj(float, 2)
            rollAngle        = link.rx_obj(float, 6)
            pitchAngle       = link.rx_obj(float, 10)
            pitotPressure    = link.rx_obj('H',   14)
            validFlags       = link.rx_obj('c',   16)
            latitude         = link.rx_obj(float, 17)
            longitude        = link.rx_obj(float, 21)
            UTC_year         = link.rx_obj('H',   25)
            UTC_month        = link.rx_obj('c',   27)
            UTC_day          = link.rx_obj('c',   28)
            UTC_hour         = link.rx_obj('c',   29)
            UTC_minute       = link.rx_obj('c',   30)
            UTC_second       = link.rx_obj(float, 31)
            speedOverGround  = link.rx_obj(float, 35)
            courseOverGround = link.rx_obj(float, 39)

            limiter_enable        = link.rx_obj('?', 43)
            manual_control_enable = link.rx_obj('?', 44)
            pitch_command         = link.rx_obj('H', 45)
            roll_command          = link.rx_obj('H', 47)
            yaw_command           = link.rx_obj('H', 49)
            throttle_command      = link.rx_obj('H', 51)
            autopilot_command     = link.rx_obj('H', 53)
            limiter_command       = link.rx_obj('H', 55)
            gear_command          = link.rx_obj('H', 57)
            flaps_command         = link.rx_obj('H', 59)

            linkConnected = link.rx_obj('?', 61)
            
            data['roll']     = rollAngle
            data['pitch']    = pitchAngle
            data['heading']  = courseAngleIMU
            data['vario']    = -2
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
