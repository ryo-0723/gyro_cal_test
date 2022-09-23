#define MPU6050_PWR_MGMT_1 0x6B
#define  MPU6050_Address  0x68
#include<Wire.h>
//任意の回数、加速度と角速度を測定し続け、止まっているときの加速度、角速度を正確にゼロにするためのもの
int cal = 500;//測定する回数
int count;
float kazu;
float k;
bool start_flag = true;
struct Axis {
  double  x, y, z;
  //  Axis(int16_t x, int16_t y, int16_t z)
  //    : x(x), y(y), z(z) {}
  Axis operator/(double value)const {
    return { x / value, y / value, z / value};
  }
  Axis& operator+=(const Axis& ac) {
    x += ac.x;
    y += ac.y;
    z += ac.z;
    return *this;
    //これはthisでアドレスを示して、*でそのアドレスの位置まで移動するみたいな感じ
    //total_acc.xのtotal_accを示しているみたいな感じ
  }
};
Axis acc;
Axis gyro;
Axis total_acc;
Axis total_gyro;
Axis ans_acc;
Axis ans_gyro;
void setup() {
  Wire.begin();
  Serial.begin(115200);
  Wire.beginTransmission(MPU6050_Address);
  Wire.write(MPU6050_PWR_MGMT_1); // MPU6050_PWR_MGMT_1レジスタの設定
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(MPU6050_Address);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();

  Wire.beginTransmission(MPU6050_Address);
  Wire.write(0x1B);
  Wire.write(0x08);
  Wire.endTransmission();

  Wire.beginTransmission(MPU6050_Address);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();

  k = cal / 100;
  Serial.println("gyroのオフセット値の測定を時始めるには、何か文字を送信してください");
  while (!Serial.available());
  Serial.println("gyroのキャリブレーションを開始します");
  Serial.println("キャリブレーション中はジャイロセンサを動かさないで下さい");
  delay(1000);
}

void loop() {
  if (start_flag) {
    read(acc, gyro);
    Serial.print(count/k); Serial.println("%");
    if (count == cal) {
      ans_acc = total_acc / count;
      ans_gyro = total_gyro / count;
      start_flag = false;
      Serial.println("キャリブレーションが完了しました。");
      Serial.print("acc_x"); Serial.print("\t");
      Serial.print(ans_acc.x); Serial.print("\t");
      Serial.print("acc_y"); Serial.print("\t");
      Serial.print(ans_acc.y); Serial.print("\t");
      Serial.print("acc_z"); Serial.print("\t");
      Serial.print(ans_acc.z); Serial.print("\t");
      Serial.println();
      Serial.print("gyro_x"); Serial.print("\t");
      Serial.print(ans_gyro.x); Serial.print("\t");
      Serial.print("gyro_y"); Serial.print("\t");
      Serial.print(ans_gyro.y); Serial.print("\t");
      Serial.print("gyro_z"); Serial.print("\t");
      Serial.print(ans_gyro.z); Serial.print("\t");
      Serial.println();
    }
  }

}

void read(Axis &acc, Axis &gyro) {
  /*
    Wire.beginTransmission(address); // ジャイロの通信の開始
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 14, static_cast<int>(true));
  */
  Axis aRaw;
  Axis gRaw;
  /*
    aRaw.x = Wire.read() << 8 | Wire.read();
    aRaw.y = Wire.read() << 8 | Wire.read();
    aRaw.z = Wire.read() << 8 | Wire.read();
    const int16_t Temperature = Wire.read() << 8 | Wire.read();
    gRaw.x = Wire.read() << 8 | Wire.read();
    gRaw.y = Wire.read() << 8 | Wire.read();
    gRaw.z = Wire.read() << 8 | Wire.read();
  */

  aRaw.x = 24576.1;
  aRaw.y = -25403.2;
  aRaw.z = 14386.4;
  const int16_t Temperature = 0;
  gRaw.x = 131.0;
  gRaw.y = -52.0;
  gRaw.z = 23.5;

  acc = aRaw / 16384.0; // FS_SEL_0 16,384 LSB / g
  // 角速度値を分解能で割って角速度(degrees per sec)に変換する
  gyro = gRaw / 131.0;

  total_acc += acc;
  total_gyro += gyro;

  // Serial.println(gRaw.x);
  count++;
  delay(20);
}
