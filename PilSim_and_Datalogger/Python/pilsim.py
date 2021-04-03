from WarThunder import telemetry
from pySerialTransfer import pySerialTransfer as txfer


if __name__ == '__main__':
    try:
        telem = telemetry.TelemInterface()
        link = txfer.SerialTransfer('COM4')
        link.open()
        
        while True:
            if telem.get_telemetry():
                roll  = telem.basic_telemetry['pitch']
                pich  = telem.basic_telemetry['roll']
                hdg   = telem.basic_telemetry['heading']
                alt   = telem.basic_telemetry['altitude']
                lat   = telem.basic_telemetry['lat']
                lon   = telem.basic_telemetry['lon']
                ias   = telem.basic_telemetry['IAS']
                flaps = telem.basic_telemetry['flapState']
                gear  = telem.basic_telemetry['gearState']
                
                telem_list = [roll,
                              pich,
                              hdg,
                              alt,
                              lat,
                              lon,
                              ias,
                              flaps,
                              gear]
                
                print(telem_list)
                
                link.send(link.tx_obj(telem_list))
            
    except KeyboardInterrupt:
        print('Closing')