from WarThunder import telemetry
from WarThunder import mapinfo


lats = []
lons = []
hdgs = []
alts = []
iass = []


if __name__ == '__main__':
    try:
        telem = telemetry.TelemInterface()
        
        while not telem.get_telemetry():
            pass
        
        for obj in telem.map_info.map_objs:
            if obj.icon == 'bombing_point':
                print('Bombing Point: {}'.format(obj.position_ll))
        print(' ')
        
        for obj in telem.map_info.map_objs:
            if obj.type == "airfield" and obj.friendly:
                print('East Coordinate:\t{}'.format(obj.east_end_ll))
                print('South Coordinate:\t{}'.format(obj.south_end_ll))
                print('Runway Heading:\t\t{} °'.format(obj.runway_dir))
                print('Runway Length:\t\t{} km'.format(mapinfo.coord_dist(*obj.east_end_ll, *obj.south_end_ll)))
                
                lats.append(obj.east_end_ll[0])
                lons.append(obj.east_end_ll[1])
                
                runway_ell = obj.east_end_ll
                runway_sll = obj.south_end_ll
                
                break
        
        print('Position:\t\t[{}, {}]'.format(telem.basic_telemetry['lat'], telem.basic_telemetry['lon']))
        print('Heading:\t\t{} °'.format(telem.basic_telemetry['heading']))
        print('Altitude:\t\t{} m'.format(telem.basic_telemetry['altitude']))
        
        runway_alt = telem.basic_telemetry['altitude']
        
        hdgs.append(telem.basic_telemetry['heading'])
        alts.append(100 + telem.basic_telemetry['altitude'])
        iass.append(350)
        
        for i in range(2):
            new_lat, new_lon = mapinfo.coord_coord(lats[-1], lons[-1], 8, hdgs[-1])
            lats.append(new_lat)
            lons.append(new_lon)
            hdgs.append(hdgs[-1])
            alts.append(alts[-1] + 400)
            iass.append(350)
        
        hew_hdg = (hdgs[-1] + 180) % 360
        new_lat, new_lon = mapinfo.coord_coord(lats[-1], lons[-1], 8, hew_hdg)
        lats.append(new_lat)
        lons.append(new_lon)
        hdgs.append(hew_hdg)
        alts.append(alts[-1] - 400)
        iass.append(300)
        
        length = len(lats) - 2
        
        for i in range(length):
            lats.append(new_lat)
            lons.append(new_lon)
            hdgs.append(hew_hdg)
            
            if i == (length - 1):
                new_lat, new_lon = mapinfo.coord_coord(lats[-1], lons[-1], mapinfo.coord_dist(*runway_ell, *runway_sll) / 2, hdgs[-1])
                alts.append(runway_alt)
                iass.append(0)
            elif i == (length - 2):
                new_lat, new_lon = mapinfo.coord_coord(lats[-1], lons[-1], 8, hdgs[-1])
                alts.append(alts[-1] - 400)
                iass.append(200)
            else:
                new_lat, new_lon = mapinfo.coord_coord(lats[-1], lons[-1], 8, hdgs[-1])
                alts.append(alts[-1] - 400)
                iass.append(350)
        
        print('\nWaypoints:')
        
        for i in range(len(lats)):
            print('{}, {}, {}, {}, {}'.format(lats[i], lons[i], hdgs[i], alts[i], iass[i]))
    
    except KeyboardInterrupt:
        print('Closing')