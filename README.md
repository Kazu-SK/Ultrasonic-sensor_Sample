# Atmega328P-PU_ultrasonic-sensor

Atmega328p-puマイコンで超音波センサを使った距離測定をやってみた時のメモ書き。
忘れそうなので...。

![HY-SRF05](https://user-images.githubusercontent.com/61465092/110140616-9f7c4080-7e17-11eb-8c5c-a5ca3290636f.jpg)


# 環境
* 開発環境：Microchip Studio 7.0.2542
* マイコン：Atmega328p-pu

#センサ
* HY-SRF05

# プログラム
* 定義
  #defineで音速C[mm/s]とマイコンのクロック周波数F[Hz]を定義しておく。
```
#define C 340290
#define F 8000000.0
```

* 距離測定
  - Trigピンに、パルス幅10[us]の単発パルスを入力し、測定を開始する。この単相パルスを入力することで、センサから超音波を出す。(データシート参照)
```
PORTD |= 1 << PORTD3;
_delay_us(10);
PORTD &= ~(1 << PORTD3);
```

  - Echoピンからのマイコンへの入力がLOWになっていることを確認したときの時間を0[s]とし、そのピンからの信号が次にHIGHになるまでの時間を測定する。Echoピンからの入力がHIGHになるまでの時間は、センサが超音波を出力してから跳ね返ってくるまでの時間を指している。
```
while(!(PIND & (1 << PIND4)));
TCNT1 = 0;
while(PIND & (1 << PIND4));

pulse_time = TCNT1;
```

  - pulse_timeにはタイマカウンタの値が格納される。クロック周波数で除算することで、超音波を出して跳ね返ってくるまでの時間を[s]の単位で算出できる。この時間と音速Cの値を積算し2で除算すると、壁までの距離を測定することができる。
```
measure_distance = (pulse_time / F) * C / 2.0;
```

  - HY-SRF05で測定できる距離は4000[mm]である。センサが超音波を出力して跳ね返ってくる時間から、測定距離が4000[mm]より大きな値になると判断される場合は、距離の計算を行わず、距離測定エラーとしての処理を行う。
```
measure_reset = 2.0 * 4000 * F / C; // max 4000mm
```
```
if(pulse_time > measure_reset){
  SerialWrite('e');
  SerialWrite('\r');
  SerialWrite('\n');
  _delay_ms(500);
  continue;		
}
```
