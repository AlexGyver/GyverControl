const byte PIDchs[] = {0, 1, 2, 3, 7, 8, 9};

void checkPID() {
#if (USE_PID == 1)
  for (byte i = 0; i < PID_CH_AMOUNT; i++) {
    if (channels[PIDchs[i]].mode == 4 && millis() - PIDtimers[i] >= (PID[i].dT * 1000)) {
      PIDtimers[i] = millis();
      computePID(i);
      if (startPID) {
        switch (i) {
          case 0: pwmVal[0] = output[0];
            break;
          case 1: pwmVal[1] = output[1];
            break;
          case 2: pwmVal[2] = output[2];
            break;
          case 3: pwmVal[3] = output[3];
            break;
          case 4: servoPos[0] = output[2];
            break;
          case 5: servoPos[1] = output[3];
            break;
          case 6: driveStep = output[4];
            drivePidFlag = true;
            driveState = 1;
            break;
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

  //int8_t thisDir = (channels[PIDchs[channel]].direction) ? 1 : -1;
  output[channel] = (float)error * PID[channel].kP /** thisDir*/;  // пропорционально ошибке регулирования
  output[channel] -= (float)PID[channel].kD * delta_input / PID[channel].dT /** thisDir*/; // дифференциальная составляющая
  integralSum[channel] += (float)error * PID[channel].dT/* * thisDir*/;   // расчёт интегральной составляющей
  if (PID[channel].kI > 0 && integralSum[channel] < 0) integralSum[channel] = 0;
  output[channel] += integralSum[channel] * PID[channel].kI;             // прибавляем
  if (channel != 6) output[channel] = constrain(output[channel], PID[channel].minSignal, PID[channel].maxSignal);    // ограничиваем. Для ШИМ и серво!
  else {
    output[channel] *= 100;    // умножаем на 100. То есть при П регуляторе ошибка в 1 единицу двинет привод в течение 100 мс
    if (output[channel] > 0) {
      // ограничиваем. Для привода
      if (output[channel] < PID[channel].minSignal * 10) output[channel] = 0;
      if (output[channel] > PID[channel].maxSignal * 10) output[channel] = PID[channel].maxSignal * 10;
    } else {
      if (output[channel] > -PID[channel].minSignal * 10) output[channel] = 0;
      if (output[channel] < -PID[channel].maxSignal * 10) output[channel] = -PID[channel].maxSignal * 10;
    }
  }
#endif
}
