# ESP32_WeatherStation
基于ESP32单片机的小型气象站

基本功能：从网络获取城市天气信息并将其由TFT屏幕显示。

使用硬件：ESP32 DevKit v1开发板、Hiletgo 2.8寸SPI液晶屏。

该项目基于ESP官方的IDF框架开发，天气信息从openWeatherMap网站的API采用HTTP连接的方式获取。

当前效果展示：![image](https://github.com/taqinlaodedie/ESP32_WeatherStation/blob/master/img/res.png)

未来可能加入的功能：
  1. 通过UART串口配置城市；
  2. 时间显示。
