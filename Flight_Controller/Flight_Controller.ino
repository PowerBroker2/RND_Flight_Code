#include "IFC_Tools.h"




void setup()
{
  //initialize the core flight controller software class
  myIFC.begin();
}




void loop()
{
  if(myIFC.grabData_GPS())
  {
    //optional debugging prints
    /*IFC_DEBUG_PORT.print("Latitude:\t\t");       IFC_DEBUG_PORT.println(myIFC.telemetry.latitude, 5);
    IFC_DEBUG_PORT.print("Longitude:\t\t");        IFC_DEBUG_PORT.println(myIFC.telemetry.longitude, 5);
    IFC_DEBUG_PORT.print("UTC_year:\t\t");         IFC_DEBUG_PORT.println(myIFC.telemetry.UTC_year);
    IFC_DEBUG_PORT.print("UTC_month:\t\t");        IFC_DEBUG_PORT.println(myIFC.telemetry.UTC_month);
    IFC_DEBUG_PORT.print("UTC_day:\t\t");          IFC_DEBUG_PORT.println(myIFC.telemetry.UTC_day);
    IFC_DEBUG_PORT.print("UTC_hour:\t\t");         IFC_DEBUG_PORT.println(myIFC.telemetry.UTC_hour);
    IFC_DEBUG_PORT.print("UTC_minute:\t\t");       IFC_DEBUG_PORT.println(myIFC.telemetry.UTC_minute);
    IFC_DEBUG_PORT.print("UTC_second:\t\t");       IFC_DEBUG_PORT.println(myIFC.telemetry.UTC_second);
    IFC_DEBUG_PORT.print("Speed Over Ground:\t");  IFC_DEBUG_PORT.println(myIFC.telemetry.speedOverGround, 3);
    IFC_DEBUG_PORT.print("Course Over Ground:\t"); IFC_DEBUG_PORT.println(myIFC.telemetry.courseOverGround, 3);
    IFC_DEBUG_PORT.println();*/
  }

  if(myIFC.grabData_IMU())
  {
    //optional debugging prints
    /*IFC_DEBUG_PORT.print("Pitch Angle:\t\t");  IFC_DEBUG_PORT.println(myIFC.telemetry.pitchAngle, 5);
    IFC_DEBUG_PORT.print("Roll Angle:\t\t");     IFC_DEBUG_PORT.println(myIFC.telemetry.rollAngle, 5);
    //IFC_DEBUG_PORT.print("Course Angle:\t\t"); IFC_DEBUG_PORT.println(myIFC.telemetry.courseAngle, 5);
    IFC_DEBUG_PORT.println();*/
  }

  if(myIFC.grabData_LiDAR())
  {
    //optional debugging prints
    /*IFC_DEBUG_PORT.print("Converted Altitude:\t"); IFC_DEBUG_PORT.println(myIFC.telemetry.convertedAltitude);
    IFC_DEBUG_PORT.println();*/
  }

  if(myIFC.grabData_Pitot())
  {
    //optional debugging prints
    /*IFC_DEBUG_PORT.print("Converted Airspeed:\t"); IFC_DEBUG_PORT.println(myIFC.telemetry.velocity);
    IFC_DEBUG_PORT.println();*/
  }

  //update all servo positions based on the new data
  myIFC.updateServos();

  //send the telemetry to the radio
  myIFC.sendTelem();
}
