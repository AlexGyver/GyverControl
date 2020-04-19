// мини-класс для работы с энкодером, версия 1.0

class encMinim
{
  public:
    encMinim(uint8_t clk, uint8_t dt, int8_t sw, boolean dir, boolean type);
    void tick();
    boolean isClick();
    boolean isHolded();
    boolean isTurn();
    boolean isRight();
    boolean isLeft();
    boolean isRightH();
    boolean isLeftH();
    boolean isFast();

  private:
    int8_t _clk, _dt, _sw;
    boolean _type = false;
    boolean _state, _lastState, _turnFlag, _swState, _swFlag, _turnState, _holdFlag;
    byte _encState;
    uint32_t _debTimer;
    bool _fast = false;
    // 0 - ничего, 1 - лево, 2 - право, 3 - правоНажат, 4 - левоНажат, 5 - клик, 6 - удержан
};

encMinim::encMinim(uint8_t clk, uint8_t dt, int8_t sw, boolean dir, boolean type) {
  if (dir) {
    _clk = clk;
    _dt = dt;
  } else {
    _clk = dt;
    _dt = clk;
  }

  _sw = sw;
  _type = type;
  pinMode(_clk, INPUT);
  pinMode(_dt, INPUT);
  if (_sw >= 0) pinMode(_sw, INPUT_PULLUP); // для режима без кнопки
  _lastState = digitalRead(_clk);
}

void encMinim::tick() {
  uint32_t thisMls = millis();
  _encState = 0;
  _state = digitalRead(_clk);
  if (_sw >= 0) _swState = digitalRead(_sw);
  else _swState = true;  // для режима без кнопки

  if (_state != _lastState) {
    _turnState = true;
    _turnFlag = !_turnFlag;
    if (_turnFlag || !_type) {
      if (digitalRead(_dt) != _lastState) {
        _encState = _swState ? 1 : 3;
      } else {
        _encState = _swState ? 2 : 4;
      }
      if (_encState != 0 && thisMls - _debTimer < 30) _fast = true;
      else _fast = false;
    }
    _lastState = _state;
    _debTimer = thisMls;
  }

  if (!_swState && !_swFlag && thisMls - _debTimer > 80) {
    _debTimer = thisMls;
    _swFlag = true;
    _turnState = false;
    _holdFlag = false;
  }

  if (!_swState && _swFlag && !_holdFlag) {
    if (_encState != 0 && thisMls - _debTimer < 2000) {
      _holdFlag = true;
    }
    if (_encState == 0 && thisMls - _debTimer > 2000) {
      _encState = 6;
      _holdFlag = true;
    }
  }

  if (_swState && _swFlag && _holdFlag) {
    _debTimer = thisMls;
    _swFlag = false;
  }

  if (_swState && _swFlag && !_holdFlag && thisMls - _debTimer > 80) {
    _debTimer = thisMls;
    _swFlag = false;
    if (!_turnState) _encState = 5;
  }
}

boolean encMinim::isFast() {
  if (_fast) return true;
  else return false;
}
boolean encMinim::isTurn() {
  if (_encState > 0 && _encState < 5) {
    return true;
  } else return false;
}
boolean encMinim::isRight() {
  if (_encState == 1) {
    _encState = 0;
    return true;
  } else return false;
}
boolean encMinim::isLeft() {
  if (_encState == 2) {
    _encState = 0;
    return true;
  } else return false;
}
boolean encMinim::isRightH() {
  if (_encState == 3) {
    _encState = 0;
    return true;
  } else return false;
}
boolean encMinim::isLeftH() {
  if (_encState == 4) {
    _encState = 0;
    return true;
  } else return false;
}
boolean encMinim::isClick() {
  if (_encState == 5) {
    _encState = 0;
    return true;
  } else return false;
}
boolean encMinim::isHolded() {
  if (_encState == 6) {
    _encState = 0;
    return true;
  } else return false;
}
