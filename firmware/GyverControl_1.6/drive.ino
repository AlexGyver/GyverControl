#if (USE_DRIVE == 1)
void driveTick() {
  channelsStruct temp = loadChannel(9);
  if (temp.state || serviceFlag || manualControl) {
    if (driveState == 1) {
      driveState = 2;
      driveTimer = millis();
      boolean thisDirection;
      if (temp.mode != 4) {
        thisDirection = channelStates[9];
      } else {
        if (pwmVal[6] > 0) {
          thisDirection = temp.direction;
        } else {
          thisDirection = !temp.direction;
        }
      }
      if (serviceFlag) thisDirection = channelStatesServ[9];
      if (manualControl) thisDirection = manualPos;
      else manualPos = thisDirection;

      lastDriveState = thisDirection;
      manualControl = false;

      if (thisDirection) {
        digitalWrite(DRV_SIGNAL1, !DRIVER_LEVEL);
        digitalWrite(DRV_SIGNAL2, DRIVER_LEVEL);
      } else {
        digitalWrite(DRV_SIGNAL1, DRIVER_LEVEL);
        digitalWrite(DRV_SIGNAL2, !DRIVER_LEVEL);
      }
    }

    long thisTimeout;
    if (temp.mode == 4) thisTimeout = abs(pwmVal[6]);
    else thisTimeout = settings.driveTimeout * 100L;
    if (driveState == 2 && millis() - driveTimer >= thisTimeout) {
      driveState = 0;
      digitalWrite(DRV_SIGNAL1, DRIVER_LEVEL);
      digitalWrite(DRV_SIGNAL2, DRIVER_LEVEL);      
    }
  }
}
#endif
