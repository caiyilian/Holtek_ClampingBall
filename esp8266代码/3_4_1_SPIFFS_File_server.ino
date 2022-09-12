#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <FS.h>  

const char *ssid = "PickBallsTeam";  //让手机或者电脑连这个WiFi即可
const char *password = "12345678";  

ESP8266WiFiMulti wifiMulti;     // 建立ESP8266WiFiMulti对象
String carState = "搜寻小球";
int ballNumber = 0;
int bateryNumber=4;
int read_num;
ESP8266WebServer esp8266_server(80);    // 建立网络服务器对象，该对象用于响应HTTP请求。监听端口（80）
void setup() {
  WiFi.softAP(ssid, password); //esp8266把WiFi展示出来给手机或者电脑连接
  String car_mode = "false"; //小车的模式，默认是false，表示小车处于自动模式
  
  
  Serial.begin(115200);          // 启动串口通讯
  SPIFFS.begin();// 启动闪存文件系统
  Serial.write(0x09);//初始化的时候默认是自动模式
  esp8266_server.onNotFound(handleUserRequet);      // 告知系统如何处理用户请求

  esp8266_server.begin();                           // 启动网站服务
  esp8266_server.on("/CarSate", HTTP_GET, CarSate); //小车的状态
  esp8266_server.on("/ChangeMode", HTTP_POST, ChangeMode); //改变小车的模式
  esp8266_server.on("/ShowBattery", HTTP_GET, ShowBattery); //电池的电量
  esp8266_server.on("/CarDirect", HTTP_POST, CarDirect); //操控小车移动
  
}

void CarSate(void){ 
  esp8266_server.send(200, "application/json", "{\"state\":\""+carState+"\",\"number\":"+String(ballNumber)+"}");
  
}
void ChangeMode(void){
  String car_mode = esp8266_server.arg("mode"); //获取到小车的模式，true为手动模式，false为自动模式
  esp8266_server.send(200, "text/plain", "success");
  Serial.write(car_mode=="true"? 0x08 : 0x09);
  delay(10);
}
void ShowBattery(void){
  esp8266_server.send(200, "text/plain", String(bateryNumber%5));
  bateryNumber--;
  if(bateryNumber<0) bateryNumber=4;
}
void CarDirect(void){
  String car_dir = esp8266_server.arg("direct");
  delay(10);
  if(car_dir=="forward") Serial.write(0x01);
  else if(car_dir=="right") Serial.write(0x02);
  else if(car_dir=="back") Serial.write(0x03);
  else if(car_dir=="left") Serial.write(0x04);
  else if(car_dir=="stop") Serial.write(0x05);
  else if(car_dir=="left_rotation") Serial.write(0x06);
  else if(car_dir=="right_rotation") Serial.write(0x07);
  else if(car_dir=="paw") Serial.write(0x11);
  else if(car_dir=="paw2") Serial.write(0x12);

  delay(10);
  
  esp8266_server.send(200, "text/plain", "success");
}



void loop(void) {
  esp8266_server.handleClient();                    // 处理用户请求
  read_num = Serial.read();
  if(read_num==0x01&&carState=="搜寻小球"){
      carState="抓取小球";
    }
  else if(read_num==0x02&&carState=="抓取小球"){
    ballNumber++;
    carState="搜寻小球";
    }
}

// 处理用户浏览器的HTTP访问
void handleUserRequet() {         
     
  // 获取用户请求网址信息
  String webAddress = esp8266_server.uri();
  
  // 通过handleFileRead函数处处理用户访问
  bool fileReadOK = handleFileRead(webAddress);

  // 如果在SPIFFS无法找到用户访问的资源，则回复404 (Not Found)
  if (!fileReadOK){                                                 
    esp8266_server.send(404, "text/plain", "404 Not Found"); 
  }
}

bool handleFileRead(String path) {            //处理浏览器HTTP访问

  if (path.endsWith("/")) {                   // 如果访问地址以"/"为结尾
    path = "/index.html";                     // 则将访问地址修改为/index.html便于SPIFFS访问
  } 
  
  String contentType = getContentType(path);  // 获取文件类型
  
  if (SPIFFS.exists(path)) {                     // 如果访问的文件可以在SPIFFS中找到
    File file = SPIFFS.open(path, "r");          // 则尝试打开该文件
    esp8266_server.streamFile(file, contentType);// 并且将该文件返回给浏览器
    file.close();                                // 并且关闭文件
    return true;                                 // 返回true
  }
  return false;                                  // 如果文件未找到，则返回false
}

// 获取文件类型
String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
