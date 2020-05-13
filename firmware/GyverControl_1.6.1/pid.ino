void checkPID() {
#if (USE_PID == 1)
  for (byte i = 0; i < PID_CH_AMOUNT; i++) {
    activeChannel = loadChannel(PIDchs[i]);
    activePID = loadPID(i);
    if (activeChannel.mode == 4 && millis() - PIDtimers[i] >= (activePID.dT * 1000.0)) {
      PIDtimers[i] = millis();
      computePID(i);
      if (startPID) {
        pwmVal[i] = output[i];
        if (i == 6) {
          drivePidFlag = true;
          driveState = 1;
#if (PID_AUTOTUNE == 1)
          if (tunerSettings.channel == 7) pwmVal[6] = tuner.value;
#endif
        }
      }
#if (DEBUG_PID > 0 && PID_AUTOTUNE == 0)
      if (i == (DEBUG_PID - 1)) {
        uart.print(activePID.setpoint); uart.print(',');
        uart.print(input[i]); uart.print(',');
#if (SHOW_INTEGRAL == 1)
        uart.print(integralSum[i]/3); uart.print(',');
#endif
        uart.println(output[i]);
      }
#endif
    }
  }
  if (!startPID) startPID = true;
#endif
}

void computePID(byte channel) {
#if (USE_PID == 1)
  input[channel] = sensorVals[activePID.sensor];
  float error = activePID.setpoint - input[channel];          // ошибка регулирования
  float delta_input = prevInput[channel] - input[channel];    // изменение входного сигнала
  prevInput[channel] = input[channel];

  output[channel] = (float)error * activePID.kP;                        // прибавляем ошибку регулирования
  output[channel] += (float)activePID.kD * delta_input;                 // прибавляем диф
  integralSum[channel] += error;                                        // расчёт интегральной составляющей
  integralSum[channel] = constrain(integralSum[channel], PID_INT_MIN, PID_INT_MAX);
  output[channel] += integralSum[channel] * activePID.kI;               // прибавляем инт

  if (channel != 6) output[channel] = constrain(output[channel], activePID.minSignal, activePID.maxSignal);    // ограничиваем. Для ШИМ и серво!
  else {
#if (USE_DRIVE == 1)
    output[channel] *= 100;    // умножаем на 100. То есть при П регуляторе ошибка в 1 единицу двинет привод в течение 100 мс
    int8_t dir = (output[channel] > 0) ? 10 : -10;
    if (output[channel] > dir * activePID.minSignal) output[channel] = 0;
    if (output[channel] < dir * activePID.maxSignal) output[channel] = dir * activePID.maxSignal;

#endif
  }
#endif
}
