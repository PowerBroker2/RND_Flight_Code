from WarThunder import telemetry
from pySerialTransfer import pySerialTransfer as txfer


if __name__ == '__main__':
    try:
        telem = telemetry.TelemInterface()
        
        while True:
            try:
                link = txfer.SerialTransfer('COM4')
                link.open()
                break
                
            except:
                pass
        
        while True:
            if telem.get_telemetry():
                pitch  = telem.basic_telemetry['roll']
                roll  = telem.basic_telemetry['pitch']
                hdg   = telem.basic_telemetry['heading']
                alt   = telem.basic_telemetry['altitude']
                lat   = telem.basic_telemetry['lat']
                lon   = telem.basic_telemetry['lon']
                ias   = telem.basic_telemetry['IAS']
                flaps = telem.basic_telemetry['flapState']
                gear  = telem.basic_telemetry['gearState']
                
                telem_list = [pitch,
                              roll,
                              hdg,
                              alt,
                              lat,
                              lon,
                              float(ias),
                              float(flaps),
                              float(gear)]
                
                link.send(link.tx_obj(telem_list))
            
    except KeyboardInterrupt:
        print('Closing')