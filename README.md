# esp32_smartfarm_mqtt
this project is about iot smartfarm 
the concept is receive input data from dht11 and then create a node-red dashboard display and also create control dashboard on rasberry pi for monitoring relay on esp32

concept ของโปรเจคนี้ : 
  - รับค่าอุณหภูมิและความชื้นจาก dht11 จากนั้นส่งค่าที่ได้แสดงผลบน node-read dash board โดยใช้ adafruit/DHT sensor library@^1.4.6 library และ publish ไปที่ mqtt topic ที่ตั้งไว้
  <img width="424" alt="Screenshot 2566-12-14 at 11 28 09" src="https://github.com/aelde/esp32_smartfarm_mqtt/assets/79216582/c939c1e4-b407-4d54-b247-c2634e930619">
  <img width="361" alt="Screenshot 2566-12-14 at 11 29 18" src="https://github.com/aelde/esp32_smartfarm_mqtt/assets/79216582/b69173ba-aab0-4ce2-b67d-6a5c22455b05">
  - จากนั้นจะมีการ subscribe topic ที่ส่งมาจาก dashboard เพื่อควบคุมการเปิดปิด relay (กด เปิดปิด switch ใน node-red dashboard)
  <img width="781" alt="Screenshot 2566-12-14 at 12 34 24" src="https://github.com/aelde/esp32_smartfarm_mqtt/assets/79216582/9d2665ee-fa71-4ab3-871b-c5d334aeab5d">
  
