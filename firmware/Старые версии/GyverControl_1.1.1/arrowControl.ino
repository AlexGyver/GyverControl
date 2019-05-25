void drawArrow() {
  if (currentChannel >= 0) {
    // ----------------- НАСТРОЙКИ КАНАЛОВ -----------------
    space(0, 0);
    space(14, 0);
    space(0, 1);
    space(0, 2);
    space(0, 3);

    if (navDepth == 0) {
      if (currentChannel > 6 && currentChannel < 9 && !navDepth) {
        space(10, 3);
        space(15, 3);
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
          arrow(10, 3);
        } else {
          arrow(0, 3);
        }
      }
      else if (arrowPos == 5) {
        arrow(15, 3);
      }
    } else {
      // ------------- НАСТРОЙКИ РЕЖИМОВ (navDepth == 1) -------------
      space(0, 0);
      space(14, 0);
      space(0, 1);
      space(0, 2);
      space(0, 3);
      byte thisMode = channels[currentChannel].mode;
      if (thisMode == 0) {
        space(7, 1);
        colon(11, 1);
        colon(14, 1);
        space(8, 2);
        colon(11, 2);
        colon(14, 2);
      }

      switch (arrowPos) {
        case 0: arrow(0, 0);
          break;
        case 1: arrow(14, 0);
          break;
        case 2:
          if (thisMode == 0) arrow(7, 1);
          else arrow(0, 1);
          break;
        case 3:
          if (thisMode == 0) arrow(11, 1);
          else arrow(0, 2);
          break;
        case 4:
          if (thisMode == 0) arrow(14, 1);
          else if (thisMode == 3 || thisMode == 1) arrow(0, 3);
          break;
        case 5:
          if (thisMode == 0) arrow(8, 2);
          break;
        case 6:
          if (thisMode == 0) arrow(11, 2);
          break;
        case 7:
          if (thisMode == 0) arrow(14, 2);
          break;
      }
    }
  } else if (currentChannel == -1) {
    arrow(0, 0);
  } else if (currentChannel == -2) {
    // ------------------ НАСТРОЙКИ -------------------
    switch (arrowPos) {
      case 0: arrow(0, 0); colon(16, 0);
        break;
      case 1: space(0, 0); arrow(16, 0); colon(8, 1);
        break;
      case 2: colon(16, 0); arrow(8, 1); colon(16, 1);
        break;
      case 3: colon(8, 1); arrow(16, 1); colon(5, 2);    // реле 1
        break;
      case 4: colon(16, 1); arrow(5, 2); colon(16, 2);
        break;
      case 5: colon(5, 2); arrow(16, 2); colon(5, 3);
        break;
      case 6: colon(16, 2); arrow(5, 3); colon(16, 3);
        break;
      case 7: colon(5, 3); arrow(16, 3);
        break;
    }

  } else if (currentChannel == -3) {
    // ------------------ SERVICE -------------------
    //currentLine = 4;
    switch (arrowPos) {
      case 0: arrow(0, 0); space(11, 0);
        break;
      case 1: space(0, 0); arrow(11, 0); colon(14, 0);
        break;
      case 2: space(11, 0); arrow(14, 0); colon(17, 0);
        break;
      case 3: colon(14, 0); arrow(17, 0); space(0, 2);    // реле 1
        break;
      case 4: colon(17, 0); arrow(0, 2); space(2, 2);
        break;
      case 5: space(0, 2); arrow(2, 2); space(4, 2);
        break;
      case 6: space(2, 2); arrow(4, 2); space(6, 2);
        break;
      case 7: space(4, 2); arrow(6, 2); space(8, 2);
        break;
      case 8: space(6, 2); arrow(8, 2); space(10, 2);
        break;
      case 9: space(8, 2); arrow(10, 2); space(12, 2);
        break;
      case 10: space(10, 2); arrow(12, 2); colon(16, 1); // реле 6
        break;
      case 11: space(12, 2); arrow(16, 1); colon(16, 2);
        break;
      case 12: colon(16, 1); arrow(16, 2); colon(1, 3);
        break;
      case 13: colon(16, 2); arrow(1, 3); colon(9, 3);
        break;
      case 14: colon(1, 3); arrow(9, 3);
        break;
    }
  }
}

void arrow(byte col, byte row) {
  lcd.setCursor(col, row);
  lcd.write(126);
}
void space(byte col, byte row) {
  lcd.setCursor(col, row);
  lcd.print(F(" "));
}
void colon(byte col, byte row) {
  lcd.setCursor(col, row);
  lcd.print(F(":"));
}
