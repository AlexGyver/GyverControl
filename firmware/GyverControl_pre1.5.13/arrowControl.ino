void drawArrow() {
  if (currentChannel >= 0 && currentChannel < 10) {
    // ----------------- НАСТРОЙКИ КАНАЛОВ -----------------
    if (navDepth == 0) {
      space(0, 0);
      space(14, 0);
      space(0, 1);
      space(0, 2);
      space(0, 3);

      if (currentChannel > 6 && currentChannel < 9 && !navDepth) {
        if ( (currentChannel == 7 && SERVO1_RELAY == 0) ||
             (currentChannel == 8 && SERVO2_RELAY == 0) ) {
          space(10, 3);
          space(15, 3);
        }
      }

      if (arrowPos == 0)
        arrow(0, 0);
      else if (arrowPos == 1)
        arrow(14, 0);
      else if (arrowPos == 2)
        arrow(0, 1);
      else if (arrowPos == 3)
        arrow(0, 2);
      else if (arrowPos == 4) {
        if (currentChannel > 6 && currentChannel < 9 && !navDepth) {
          if ( (currentChannel == 7 && SERVO1_RELAY == 0) ||
               (currentChannel == 8 && SERVO2_RELAY == 0) )
            arrow(10, 3);
          else arrow(0, 3);
        } else {
          arrow(0, 3);
        }
      }
      else if (arrowPos == 5) {
        arrow(15, 3);
      }
    } else {
      // ------------- НАСТРОЙКИ РЕЖИМОВ (navDepth == 1) -------------
      byte thisMode = channels[currentChannel].mode;
      if (thisMode < 4) {
        space(0, 0);
        space(14, 0);
        if (arrowPos == 0) arrow(0, 0);
        if (arrowPos == 1) arrow(14, 0);
      }

      switch (thisMode) {
        case 0:
          space(0, 1);
          space(0, 2);
          space(0, 3);
          space(7, 1);
          colon(11, 1);
          colon(14, 1);
          space(8, 2);
          colon(11, 2);
          colon(14, 2);
          switch (arrowPos) {
            case 2: arrow(7, 1); break;
            case 3: arrow(11, 1); break;
            case 4: arrow(14, 1); break;
            case 5: arrow(8, 2); break;
            case 6: arrow(11, 2); break;
            case 7: arrow(14, 2); break;
          }
          break;
        case 1:
          space(0, 1);
          space(0, 2);
          space(0, 3);
          switch (arrowPos) {
            case 2: arrow(0, 1); break;
            case 3: arrow(0, 2); break;
            case 4: arrow(0, 3); break;
          }
          break;
        case 2:
          space(4, 1);
          space(6, 1);
          space(8, 1);
          space(10, 1);
          space(12, 1);
          space(14, 1);
          space(16, 1);
          space(3, 2);
          colon(6, 2);
          colon(9, 2);
          space(3, 3);
          colon(6, 3);
          colon(9, 3);
          colon(17, 3);
          switch (arrowPos) {
            case 2: arrow(4, 1); break;
            case 3: arrow(6, 1); break;
            case 4: arrow(8, 1); break;
            case 5: arrow(10, 1); break;
            case 6: arrow(12, 1); break;
            case 7: arrow(14, 1); break;
            case 8: arrow(16, 1); break;
            case 9: arrow(3, 2); break;
            case 10: arrow(6, 2); break;
            case 11: arrow(9, 2); break;
            case 12: arrow(3, 3); break;
            case 13: arrow(6, 3); break;
            case 14: arrow(9, 3); break;
            case 15: arrow(17, 3); break;
          }
          break;
        case 3:
          space(0, 1);
          space(0, 2);
          space(0, 3);
          colon(15, 3);
          colon(5, 3);
          switch (arrowPos) {
            case 2: arrow(0, 1); break;
            case 3: arrow(0, 2); break;
            case 4: arrow(5, 3); break;
            case 5: arrow(15, 3); break;
          }
          break;
      }
    }
  } else if (currentChannel == -1) {
    arrow(0, 0);
  } else if (currentChannel == -2) {
    // ------------------ НАСТРОЙКИ -------------------

  } else if (currentChannel == -3) {
    // ------------------ SERVICE -------------------
    space(0, 0); space(11, 0);
    colon(14, 0); colon(17, 0); space(0, 2);
    space(2, 2); space(4, 2); space(6, 2);
    space(8, 2); space(10, 2); space(12, 2);
    colon(9, 3); colon(16, 3); colon(1, 3);

    switch (arrowPos) {
      case 0: arrow(0, 0); break;
      case 1: arrow(11, 0); break;
      case 2: arrow(14, 0); break;
      case 3: arrow(17, 0); break;
      case 4: arrow(0, 2); break;
      case 5: arrow(2, 2); break;
      case 6: arrow(4, 2); break;
      case 7: arrow(6, 2); break;
      case 8: arrow(8, 2); break;
      case 9: arrow(10, 2); break;
      case 10: arrow(12, 2); break;
      case 11: arrow(1, 3); break;
      case 12: arrow(9, 3); break;
      case 13: arrow(16, 3); break;
    }
  }
}

void smartArrow(bool state) {  // рисует стрелку, галку или пробел
  lcd.write(state ? (controlState ? 62 : 126) : 32);
}
void arrow(byte col, byte row) {
  lcd.setCursor(col, row);
  smartArrow();
}
void space(byte col, byte row) {
  lcd.setCursor(col, row);
  lcd.print(' ');
}
void colon(byte col, byte row) {
  lcd.setCursor(col, row);
  lcd.print(':');
}
