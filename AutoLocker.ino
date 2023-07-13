#include <Servo.h>

#define R_LED 11
#define B_LED 10
#define G_LED 9
#define SWITCH1 8
#define SWITCH2 7
#define SWITCH_NUM 2
const int SWITCH[SWITCH_NUM] = {SWITCH1, SWITCH2};
#define SERVO1 6
#define SERVO2 5
#define MIC 0
#define SPEAKER 4

//システムの状態
int systemState = 1; //シリアル受信で書き換わるがデフォルトを1とする

//音量測定用設定
const int GENKAN = 14000; //玄関前からの呼び出し時は大体10,000peaksくらいだった
const int TATEMONO = 11500; //建物前からの呼び出し時は大体15,000peaksくらいだった
const int TATEMONO_TIMES = 3; //建物前からの呼び出しのパターンの測定回数
const long INTERVAL = 3500;   //計測時間(建物前からの呼び出し音のパターン周期が大体3.5sだった)

//音声認識終了時間設定
const long END_JULIUS_TIME = 10000; //10秒間何も音声が検知されなかったら終了する。
const int CHALLENGE_TIMES = 3; //可能な音声認識試行回数

//チャタリング防止設定
const int SWITCH_COUNT_MIN = 500; //スイッチが押されてから判定に入るまでの最低カウンタ
const int SWITCH_COUNT_LIMIT = SWITCH_COUNT_MIN + 10; //カウンタの値の上限(オーバフロー対策)

//サーボクラス
Servo servo1;
Servo servo2;

void setup() {
  pinMode(R_LED, OUTPUT);
  pinMode(B_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);

  for(int i = 0; i < SWITCH_NUM; i++){
    pinMode(SWITCH[i], INPUT);
  }

  servo1.attach(SERVO1);
  servo1.write(170);
  servo2.attach(SERVO2);

  pinMode(SPEAKER, OUTPUT);

  
  Serial.begin(9600);
}

void loop() {

  //----呼び出し感知----
  int v = analogRead(MIC);

  static int peaks = 0;
  static int peakcount = 0;
  static unsigned long previousMillis = 0;
  //マイクの音量が490(ほぼ最大)のときにpeaksを加算
  if(v >= 490) peaks++;

  if(peaks == 1){
    previousMillis = millis(); //前回の時間
    peaks=2;
  }else if(peaks > 1){
    
    //(interval)ms間隔
    unsigned long currentMillis = millis(); //現在の時間
    if(currentMillis - previousMillis >= INTERVAL){
      
      //Serial.println(peaks);

      
      //玄関前呼び出し時
      if(peaks >= GENKAN){
        Serial.write('g');
      
      //建物前呼び出し時
      }else if(peaks >= TATEMONO){
        Serial.write('t');
      }
      

      peaks = 0;

    }

  }
  //-------------------

  //--------音声認識タイムアウト------
  static int challenge = 0; //現在の音声認識試行回数
  static int start = 0; //タイムアウト計測管理(0は開始前, 1は開始, 2は終了)
  static unsigned long previousMillis2 = 0;

  //音声入力待機になった瞬間に過去の時間を取得(初回か、何かしらの音声を認識したら更新)
  if(systemState == 2){
    if(start == 0){
      previousMillis2 = millis();
      start = 1;
    }else if(start == 1){
      unsigned long currentMillis = millis(); //現在の時間
      //待機中にEND_JULIUS_TIME以上の時間が立ったら終了操作
      if(currentMillis - previousMillis2 >= END_JULIUS_TIME){
        Serial.write('c');
        start = 2;
      }
    }

    

  }else{
    start = 0;
  }

  if(challenge == 3){
    Serial.write('c');
    start = 2;
    challenge = 4;
  }

  //----------------------

  //----シリアル受信----
  if(Serial.available() > 0){
    char data = Serial.read();

    //ステータス変更LED
    switch(data){
      case '1':
        digitalWrite(R_LED, LOW);
        digitalWrite(G_LED, HIGH);
        digitalWrite(B_LED, LOW);
        systemState = 1;
        break;
      case '2':
        digitalWrite(R_LED, HIGH);
        digitalWrite(G_LED, LOW);
        digitalWrite(B_LED, LOW);
        systemState = 2;
        break;
      case '0':
        digitalWrite(R_LED, LOW);
        digitalWrite(G_LED, LOW);
        digitalWrite(B_LED, HIGH);
        systemState = 0;
        break;
      default:
        break;
    }

    //操作
    switch(data){
      //建物から呼び出されたとき。
      case 't':
        servo1.write(130);
        tone(SPEAKER, 440, 50);
        delay(70);
        tone(SPEAKER, 554, 50);
        delay(70);
        tone(SPEAKER, 659, 50);
        delay(200);
        tone(SPEAKER, 440, 50);
        delay(70);
        tone(SPEAKER, 554, 50);
        delay(70);
        tone(SPEAKER, 659, 50);
        delay(200);
        tone(SPEAKER, 440, 50);
        delay(70);
        tone(SPEAKER, 554, 50);
        delay(70);
        tone(SPEAKER, 659, 50);
        delay(500);
        challenge = 0;
      case 'f':
        if(challenge < CHALLENGE_TIMES){
          tone(SPEAKER, 440, 500);
          delay(500);
        }
        break;
      case 's':
        if(challenge < CHALLENGE_TIMES){
          tone(SPEAKER, 554, 240);
          delay(250);
          tone(SPEAKER, 554, 250);
          delay(250);
        }
        break;
      case 'l':
        if(challenge < CHALLENGE_TIMES){
          tone(SPEAKER, 659, 156);
          delay(166);
          tone(SPEAKER, 659, 156);
          delay(166);
          tone(SPEAKER, 659, 166);
          delay(166);
        }
        break;
      case 'n':
        if(challenge < CHALLENGE_TIMES){
          tone(SPEAKER, 659, 200);
          delay(200);
          tone(SPEAKER, 523, 200);
          delay(200);
          tone(SPEAKER, 440, 200);
          delay(200);
          challenge += 1;
        }
        break;
      //終わり
      case 'c':
        for(int i = 130; i <= 170; i++){
          servo1.write(i);
          delay(50);
        }
        break;
      //強制解錠
      case 'p':
        servo1.write(130);
        delay(1500);
      //解錠
      case 'o':
        servo2.write(60);
        delay(200);
        servo2.write(90);
        tone(SPEAKER, 880, 500);
        delay(500);
        Serial.write('o');
        break;
      //建物呼出テスト
      case '6':
        Serial.write('t');
        break;
      //玄関呼出テスト
      case '7':
        Serial.write('g');
        break;
      case 'w':
        start = 0;
        break;
      default:
        break;
    }

  }
  //------------------

  //----スイッチ制御----

  //スイッチの状態
  static int old_switch_state[SWITCH_NUM] = {};

  for(int i = 0; i < SWITCH_NUM; i++){
    v = switch_state(i); //現在のスイッチの状態

    //スイッチを押すたびにワンアクション
    if(v == 1 && old_switch_state[i] == 0){
      switch(SWITCH[i]){
        case SWITCH1:
          Serial.write('1');
          break;
        case SWITCH2:
          Serial.write('2');
          tone(SPEAKER, 440,200);
          break;
        default:
          break;
      }
    }

    old_switch_state[i] = v;

  }


  //---------------

  //----ボタン(サービモータ)押し込み----

  //---------------------------------]

  //----スピーカー----
  //-------------------
}



//スイッチのチャタリング防止(複数ボタンに対応)
int switch_state(int a) {
  static unsigned int BUTTONcount[SWITCH_NUM];

  int v = 0;

  //スイッチが押されている間はカウンタを増加、離したらリセット
  if(digitalRead(SWITCH[a])){
    if(BUTTONcount[a] <= SWITCH_COUNT_LIMIT){
      BUTTONcount[a]++;
    }
  }else{
    BUTTONcount[a] = 0;
  }

  //カウンタが最低カウンタを上回ったらスイッチの状態判定
  if(BUTTONcount[a] >= SWITCH_COUNT_MIN){
    v = 1;
  }

  return v;

}


