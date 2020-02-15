/*
 * 
 * spicy-bits.org True Random Number Generator (TRNG)
 * Copyright (C) 2020 Manny Peterson <me@mannypeterson.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 */
#include <SparkFunSi4703.h>
#include <Adafruit_ADS1015.h>
#include <Wire.h>

Adafruit_ADS1115 adc;
Si4703_Breakout radio(2, A4, A5);
int adc0 = 0;

void setup()
{
  adc.setGain(GAIN_SIXTEEN);
  radio.powerOn();
  radio.setVolume(7);
  radio.setChannel(937);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
}

void loop()
{
  delay(10);
  adc0 = adc.readADC_SingleEnded(0);
  digitalWrite(8,LOW);
  digitalWrite(9,bitRead(adc0, 0));
  delay(10);
  adc0 = adc.readADC_SingleEnded(0);
  digitalWrite(8,HIGH);
  digitalWrite(9,bitRead(adc0, 1));
}
