#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

BLEScan* pBLEScan;

int scanTime = 5;      // 单次扫描时间 (秒)
int repeatTimes = 12;  // 总扫描次数，5秒 * 12 = 60秒

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 初始化 BLE
  String deviceName = "ESP32_BLE_Scan";
  BLEDevice::init(deviceName.c_str());

  Serial.println("===== ESP32 BLE 扫描 =====");
  Serial.printf("设备名称: %s\n", deviceName.c_str());
  Serial.printf("MAC 地址 : %s\n", BLEDevice::getAddress().toString().c_str());

  // 初始化扫描器
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);  // 主动扫描
  pBLEScan->setInterval(150);     // 扫描间隔 ms
  pBLEScan->setWindow(150);       // 扫描窗口 ms
}

void loop() {
  Serial.println("\n===== 开始连续扫描 BLE 设备 =====");

  for (int r = 0; r < repeatTimes; r++) {
    Serial.printf("扫描轮次 %d/%d\n", r + 1, repeatTimes);

    BLEScanResults* pResults = pBLEScan->start(scanTime, false);

    // 空指针检查
    if (pResults == nullptr) {
      Serial.println("扫描失败，返回空指针");
      delay(1000);
      continue;
    }

    int count = pResults->getCount();
    for (int i = 0; i < count; i++) {
      BLEAdvertisedDevice device = pResults->getDevice(i);

      // 打印所有设备，可根据名称过滤 QCC3024
      String name = device.haveName() ? device.getName().c_str() : "(未知)";
      Serial.printf("[%d] 名称: %s\n", i + 1, name.c_str());
      Serial.printf("    MAC: %s\n", device.getAddress().toString().c_str());
      Serial.printf("    RSSI: %d dBm\n\n", device.getRSSI());
    }

    // 释放内存
    pBLEScan->clearResults();

    delay(500);  // 每轮扫描间隔 0.5 秒
  }

  Serial.println("===== 本轮连续扫描完成 =====");
  delay(5000); // 等待 5 秒再开始下一轮扫描
}
