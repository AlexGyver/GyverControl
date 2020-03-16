#if (USE_DRIVE == 1)
void driveTick() {
  if (channels[9].state || serviceFlag || manualControl) {
    if (driveState == 1) {
      driveState = 2;
      driveTimer = millis();
      boolean thisDirection;
      if (channels[9].mode != 4) {
        thisDirection = channelStates[9];
      } else {
        if (pwmVal[6] > 0) {
          thisDirection = channels[9].direction;
        } else {
          thisDirection = !channels[9].direction;
        }
      }
      if (serviceFlag) thisDirection = channelStatesServ[9];
      if (manualControl) thisDirection = manualPos;

      if (thisDirection) {
        digitalWrite(DRV_SIGNAL1, !DRIVER_LEVEL);
        digitalWrite(DRV_SIGNAL2, DRIVER_LEVEL);
      } else {
        digitalWrite(DRV_SIGNAL1, DRIVER_LEVEL);
        digitalWrite(DRV_SIGNAL2, !DRIVER_LEVEL);
      }
    }

    long thisTimeout;
    if (channels[9].mode == 4) thisTimeout = abs(pwmVal[6]);
    else thisTimeout = (long)settings.driveTimeout * 1000;
    if (driveState == 2 && millis() - driveTimer >= thisTimeout) {
      driveState = 0;
      digitalWrite(DRV_SIGNAL1, DRIVER_LEVEL);
      digitalWrite(DRV_SIGNAL2, DRIVER_LEVEL);
      manualControl = false;
    }
  }
}
#endif
