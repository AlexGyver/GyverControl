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
        }
      }
    }
  }
  if (!startPID) startPID = true;
#endif
}

void computePID(byte channel) {
#if (USE_PID == 1)
  input[channel] = sensorVals[activePID.sensor];
  float error = activePID.setpoint - input[channel];        // ошибка регулирования
  float delta_input = input[channel] - prevInput[channel];    // изменение входного сигнала
  prevInput[channel] = input[channel];

  output[channel] = (float)error * activePID.kP;  // пропорционально ошибке регулирования
  output[channel] -= (float)activePID.kD * delta_input / activePID.dT; // дифференциальная составляющая
  integralSum[channel] += (float)error * activePID.dT;   // расчёт интегральной составляющей
  if (activePID.kI > 0 && integralSum[channel] < 0) integralSum[channel] = 0;
  output[channel] += integralSum[channel] * activePID.kI;             // прибавляем

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
