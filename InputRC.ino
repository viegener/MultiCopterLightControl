#if defined (INPUT_RC)

void setupInputRC(){
  pinMode(RC_PIN_1, INPUT);
}

void loopInputRC(){
  int rc1 = pulseIn(RC_PIN_1, HIGH, 25000);
  boolean lostConnection = rc1<650;
  if (rc1<1000) rc1 = 1000;
  else if (rc1>2000) rc1 = 2000;

  if (lostConnection) mode = MODE_RC_NO_CONNECTION;
  else if (rc1<1350) mode = MODE_RC_LOW;
  else if (rc1>1700) mode = MODE_RC_HIGH;
  else mode = MODE_RC_MID;
}

#endif

