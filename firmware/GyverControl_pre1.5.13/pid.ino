void checkPID() {
#if (USE_PID == 1)
  for (byte i = 0; i < PID_CH_AMOUNT; i++) {
    if (channels[PIDchs[i]].mode == 4 && millis() - PIDtimers[i] >= (PID[i].dT * 1000.0)) {
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
  input[channel] = sensorVals[PID[channel].sensor];
  float error = PID[channel].setpoint - input[channel];        // ошибка регулирования
  float delta_input = input[channel] - prevInput[channel];    // изменение входного сигнала
  prevInput[channel] = input[channel];

  output[channel] = (float)error * PID[channel].kP;  // пропорционально ошибке регулирования
  output[channel] -= (float)PID[channel].kD * delta_input / PID[channel].dT; // дифференциальная составляющая
  integralSum[channel] += (float)error * PID[channel].dT;   // расчёт интегральной составляющей
  if (PID[channel].kI > 0 && integralSum[channel] < 0) integralSum[channel] = 0;
  output[channel] += integralSum[channel] * PID[channel].kI;             // прибавляем

  if (channel != 6) output[channel] = constrain(output[channel], PID[channel].minSignal, PID[channel].maxSignal);    // ограничиваем. Для ШИМ и серво!
  else {
#if (USE_DRIVE == 1)
    output[channel] *= 100;    // умножаем на 100. То есть при П регуляторе ошибка в 1 единицу двинет привод в течение 100 мс
    int8_t dir = (output[channel] > 0) ? 10 : -10;
    if (output[channel] > dir * PID[channel].minSignal) output[channel] = 0;
    if (output[channel] < dir * PID[channel].maxSignal) output[channel] = dir * PID[channel].maxSignal;

#endif
  }
#endif
}
