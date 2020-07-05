double average_adc(int value)
{
  double adc = 0;
  double average;

  for (int i = 0; i <= 19; i++)
  {
    int16_t adsread = ads.readADC_SingleEnded(adcNumber);
    adc = adc + adsread;
    delay(10);
  }

  average = adc / 20;

  return average;
}