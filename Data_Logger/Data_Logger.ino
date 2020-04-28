#include <SdFat.h>
#include <SerialTransfer.h>
#include "Shared_Tools.h"




SerialTransfer telemTransfer;
SdFatSdioEX sd;
SdFile myFile;
File root;

char filename[20];

telemetry_struct telemetry;
controlInputs_struct controlInputs;




void setup()
{
  // turn on power led
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  
  Serial.begin(115200);
  Serial1.begin(2000000);

  telemTransfer.begin(Serial1);

  while (!sd.begin())
  {
    Serial.println(F("\nSD iniatialization failed"));
    delay(100);
  }
  
  setupSD();
}




void loop()
{
  if(telemTransfer.available())
  {
    uint16_t recLen;
    
    telemTransfer.rxObj(telemetry, sizeof(telemetry));
    recLen = sizeof(telemetry);

    telemTransfer.txObj(controlInputs, sizeof(controlInputs), recLen);
    recLen += sizeof(controlInputs);
    
    logData();
  }

  handleCmds();
}




void setupSD()
{
  unsigned int flightCount = 1;

  while (!sd.begin())
  {
    Serial.println(F("SD iniatialization failed"));
    delay(100);
  }

  sprintf(filename, "flight_%d.txt", flightCount);

  while(sd.exists(filename))
  {
    flightCount++;
    sprintf(filename, "flight_%d.txt", flightCount);
  }
  
  myFile.open(filename, FILE_WRITE);
  myFile.println(F("epoch_ms,alt_cm,roll_deg,pitch_deg,velocity_m/s,lat_dd,lon_dd,year,month,day,hour,min,sec,sog,cog,throttle_command,pitch_command,yaw_command,roll_command"));
  myFile.close();
}




void logData()
{
  char str_temp[6];
  char buff[100];
  char target[] = "%d,%s,%s,%s,%s,%s,%s,%d,%d,%d,%d,%d,%s,%s,%s,%d,%d,%d,%d";

  sprintf(buff, target, millis(),
                        dtostrf(telemetry.altitude, 4, 2, str_temp),
                        dtostrf(telemetry.rollAngle, 4, 2, str_temp),
                        dtostrf(telemetry.pitchAngle, 4, 2, str_temp),
                        dtostrf(telemetry.velocity, 4, 2, str_temp),
                        dtostrf(telemetry.latitude, 4, 2, str_temp),
                        dtostrf(telemetry.longitude, 4, 2, str_temp),
                        telemetry.UTC_year,
                        telemetry.UTC_month,
                        telemetry.UTC_day,
                        telemetry.UTC_hour,
                        telemetry.UTC_minute,
                        dtostrf(telemetry.UTC_second, 4, 2, str_temp),
                        dtostrf(telemetry.speedOverGround, 4, 2, str_temp),
                        dtostrf(telemetry.courseOverGround, 4, 2, str_temp),
                        controlInputs.throttle_command,
                        controlInputs.pitch_command,
                        controlInputs.yaw_command,
                        controlInputs.roll_command);
  
  myFile.open(filename, FILE_WRITE);
  myFile.println(buff);
  myFile.close();
}




void handleCmds()
{
  if (Serial.available())
  {
    char input[40] = { '\0' };
    
    readInput(input, sizeof(input));

    if (!strcmp(input, "ls"))
    {
      Serial.println(F("--------------------------------------------------"));
      
      root = sd.open("/");
      printDirectory(root, 0);
      
      Serial.println(F("--------------------------------------------------"));
      Serial.println();
    }
    else if (strstr(input, "rm "))
    {
      char* p = strstr(input, "rm ") + 3;

      char fileName[40] = { '\0' };

      for (byte i=0; i<sizeof(fileName); i++)
      {
        fileName[i] = *p;
        
        if (*p == '\0')
          break;

        p++;
      }

      if (sd.exists(fileName))
      {
        Serial.print("Deleting File: ");
        Serial.println(fileName);

        myFile.open(fileName, O_WRITE);
        myFile.remove();
      }
      else if (!strcmp(fileName, "all"))
      {
        char fileName_[20];
      
        while (true)
        {
          File entry =  root.openNextFile();
          
          if (!entry)
            break;
      
          entry.getName(fileName_, sizeof(fileName_));
          
          if (!entry.isDirectory())
          {
            Serial.print("Deleting: ");
            Serial.println(fileName_);
            
            myFile.open(fileName_, O_WRITE);
            myFile.remove();
          }
          
          entry.close();
        }
      }
      else
      {
        Serial.print(fileName);
        Serial.println(" not found");
      }
    }
    else if (sd.exists(input))
    {
      Serial.println(F("--------------------------------------------------"));
      Serial.print(input); Serial.println(" found:");

      myFile.open(input, FILE_READ);
      
      int data;
      while ((data = myFile.read()) >= 0)
        Serial.write(data);
        
      myFile.close();

      Serial.println(F("--------------------------------------------------"));
      Serial.println();
    }
    else
      Serial.println('?');
  }
}




void readInput(char input[], uint8_t inputSize)
{
  uint8_t i = 0;
  unsigned long markTime = millis();
  
  while (Serial.available())
  {
    char c = Serial.read();
    
    if ((millis() - markTime) >= 100)
      break;
    else if (c == '\n')
      break;
    else if (i >= inputSize)
      break;

    input[i] = c;
    i++;
  }
}




void printDirectory(File dir, int numTabs)
{
  char fileName[20];
  
  while (true)
  {
    File entry =  dir.openNextFile();
    
    if (!entry)
      break;
      
    for (uint8_t i = 0; i < numTabs; i++)
      Serial.print('\t');

    entry.getName(fileName, sizeof(fileName));
    Serial.print(fileName);
    
    if (entry.isDirectory())
    {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    }
    else
    {
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    
    entry.close();
  }
}




bool stris(const char *input, const uint8_t inputSize, const char *target, const uint8_t targetSize)
{
  char newInput[inputSize] = { '\0' };
  char newTarget[targetSize] = { '\0' };

  memcpy(newInput, input, inputSize);
  memcpy(newTarget, target, targetSize);

  if ((strstr(newInput, newTarget) == newInput) && (newInput[targetSize] == '\0'))
    return true;
  else
    return false;
}
