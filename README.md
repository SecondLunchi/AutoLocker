# AutoLocker
音声と遠隔によるオートロックの自動解錠装置
### 概要
本プロジェクトでは、Arduino Uno R3とRaspberry Pi 4を使用し、音声認識と遠隔制御によるオートロックの自動解錠装置を作成しました。本装置は、ユーザーが物理的なキーカード等を使わずにオートロックを開けることを可能とさせます。

### 特徴
音声認識による解錠: 

オープンソースの音声認識である Julius を用いて音声認識を行っています。音声認識では、独自の辞書を作成し、辞書内の特定のフレーズを特定の順番で発声したときに解錠するシステムを取り入れています。

また、本装置の課題である、音声認識を行う前のインターフォン応答については、 Arduino 上で呼び出し音を検知するシステムを導入することで解決しています。処理内容は AutoLocker.ino を確認してください。

遠隔制御による解錠: 

Discord Botによる、Discord上のメッセージ処理によって制御をしています。処理内容は AutoLocker.py を確認してください。



### 使用機材
Arduino Uno R3

Raspberry Pi 4 Model B Rev 1.2

USBマイク

MAX9814

SPT15

SG92R

etc.

### システム構成
システムのイメージ図

![image](https://github.com/SecondLunchi/AutoLocker/assets/21302394/7c30849c-5d91-4265-b1dc-8a2d83ccd52d)

システム構成

![image](https://github.com/SecondLunchi/AutoLocker/assets/21302394/e6eea538-45f0-4fd1-8aa7-12eb38c9f48d)


### 回路図
本装置において、Arduino Uno R3の電源はRaspberry PiからUSBで確保しています。

![image](https://github.com/SecondLunchi/AutoLocker/assets/21302394/1a253f35-a112-4d04-bb43-4afb9ace965d)

