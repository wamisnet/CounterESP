#include<ESP8266WebServer.h>
#include "server.h"
#include "rtc.h"
ESP8266WebServer WebServer(80);
String network_html, network_list;

String createHtml(String title, String head, String body)
{
  String content = F(
                     "<!DOCTYPE HTML><html><head><meta charset=\"UTF-8\">"
                     "<link rel=\"stylesheet\"type=\"text/css\"href=\"/nefry_css\">"
                     "<script type=\"text/javascript\"src=\"jsform\"></script>"
                     "<title>");
  content += title;
  content += F("</title>");
  content += head;
  content += F("</head><body><div>");
  content += body;
  content += F("</div><body></html>");
  return content;
}

void scanWiFi() {
  int founds = WiFi.scanNetworks();
  Serial.println();
  Serial.println(F("scan done"));
  if (founds <= 0) {
    Serial.println(F("no networks found"));
  }
  else {
    Serial.print(founds);
    Serial.println(F(" networks found"));
    network_html = F("<ol>");
    network_list = F("<datalist id=\"network_list\">");
    for (int i = 0; i < founds; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.println(")");
      network_html += F("<li>");
      network_html += WiFi.SSID(i);
      network_html += " (";
      network_html += WiFi.RSSI(i);
      network_html += ")";
      //network_html += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
      network_html += F("</li>");
      network_list += F("<option value=\"");
      network_list += WiFi.SSID(i);
      network_list += F("\"></option>");
    }
    network_html += F("</ol>");
    network_list += F("</datalist>");
  }
}

String getAddressStr(IPAddress ip)
{
  if (ip.toString().equals("0.0.0.0"))return "192.168.4.1";
  else return ip.toString();
}

void beginWeb() {
  WebServer.on("/", [&]() {
    String content = F(
                       "<h1>Setup</h1>"
                       "<form  name=\"myForm\" method='get' action='set_wifi'><div class=\"row\"> <label for=\"ssid\">SSID : </label> <div> <input name=\"ssid\" id=\"ssid\" maxlength=\"32\"list=\"network_list\" value=\"\" required> </div></div>"
                       "<div class=\"row\"> <label for=\"pwd\">PASS : </label> <div> <input type=\"password\" name=\"pwd\" id=\"pwd\"maxlength=\"64\"> </div></div>"
                       "<div class=\"row\"> <label for=\"time\">Send Interval(h):</label> <div> <input type=\"number\" name=\"time\" value=\"");
    content += memory_getTimer();
    content += F("\" min=\"1\" max=\"24\"></div></div>"
    "<div class=\"row\"> <label for=\"wifimode\">非公開SSID対応:</label> <div> <input type=\"checkbox\" name=\"wifimode\" value=\"1\"");
    if(memory_getMode() == 1){
      content += F("checked=\"checked\"");
    }
    content += F("><div class=\"footer\"><input type=\"submit\"value=\"Save\"></div></form></div>"
                 "<div><form  name=\"myForm\" method='post' action='clear_data'>"
                 "<p>Count Timer Data Clear</p><div class=\"footer\"><input type=\"submit\"value=\"Clear\"></div></form></div>"
                 "<div><form  name=\"myForm\" method='post' action='send_data'>"
                 "<p>Count Timer Data Server Send</p><div class=\"footer\"><input type=\"submit\"value=\"Send\"></div></form></div>"
                 "<div><table><tr><th>WiFi Spot</th><td>");
    if (WiFi.status() != WL_CONNECTED) {
      content += "None";
    } else {
      content += WiFi.SSID();
    }
    content += F("</td></tr><tr><th>IP Address</th><td>");
    content += getAddressStr(WiFi.localIP());
    content += F("</td></tr><tr><th>Gateway IP Address</th><td>");
    content += getAddressStr(WiFi.gatewayIP());
    content += F("</td></tr><tr><th>SubnetMask</th><td>");
    content += getAddressStr(WiFi.subnetMask());
    content += F("</td></tr><tr><th>MAC Address</th><td>");
    content += WiFi.macAddress();
    content += F("</td></tr><tr><th>Count</th><td>");
    content += memory_getCount();
    content += F("</td></tr><tr><th>ModuleName</th><td>");
    content += memory_getModule();
    content += F("</td></tr></table></div><div>");
    content += network_html;
    content += network_list;
    content += F("</div>");
    WebServer.send(200, "text/html", createHtml(F("Setup"), "", content));
  });

  WebServer.on("/clear_data", [&]() {
    uint32_t nowTime = rtc_getTime();
    memory_setLastTime(nowTime);
    memory_setCount(0);
    memory_save();

    WebServer.send(200, "text/html", createHtml(F("Nefry Data Clear"), "", (String)F("<h1>Data Clear</h1><a href=\"/\">Back to top</a>")));
    Serial.print("dateClearSend");

    for (int i = 0; i < 500; i++) {
      WebServer.handleClient();
      delay(10);
    }
  });

  WebServer.on("/send_data", [&]() {
    String content = "<p>";
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("wifi connect!");
      Serial.println("server send Start!");
      if (sendData() == true) {
        memory_setLastTime(rtc_getTime());
        memory_setCount(0);
        memory_save();
        Serial.println("server send Finish!");
        content += "Server Send Finish";
      } else {
        Serial.println("server send Error!");
        content += "Server Send Error";
      }
    } else {
      Serial.println("wifi no connect");
      content += "No network Connection";
    }
    content += "</p>";
    WebServer.send(200, "text/html", createHtml(F("Data Send"), "", (String)F("<h1>Data Send</h1>") + content + (String)F("<a href=\"/\">Back to top</a>")));
    Serial.print("dateClearSend");

    for (int i = 0; i < 500; i++) {
      WebServer.handleClient();
      delay(10);
    }
  });

  WebServer.on("/set_wifi", [&]() {
    String newssid = WebServer.arg("ssid");
    String newpwd = WebServer.arg("pwd");
    String timer_str = WebServer.arg("time");
    String wifimode_str = WebServer.arg("wifimode");
    int timer_int = timer_str.toInt();
    int wifimode_int = wifimode_str.toInt();
    Serial.print("new send interval : ");
    Serial.println(timer_int);
    if (timer_int <= 24 && timer_int >= 1) {
      memory_setTimer(timer_int);
    }
    Serial.print("newssid : ");
    Serial.println(newssid);
    Serial.print("wifimode : ");
    Serial.println(wifimode_int);
    memory_setMode(wifimode_int);
    String content;
    if (newssid.length() > 0) {
      memory_setSSID(newssid.c_str());
      memory_setPass(newpwd.c_str());
      content = F("Save SSID:");
      content += newssid;
      content += F(" Restart to boot into new WiFi");
    }
    else {
      content = F("Empty SSID is not acceptable.");
    }
    memory_save();

    WebServer.send(200, "text/html", createHtml(F("Nefry Wifi Set"), "", (String)F("<h1>Nefry Wifi Set</h1><p>") + content + (String)F("</p><a href=\"/\">Back to top</a>")));
    Serial.print("dateSend");
    if (newssid.length() > 0) {
      for (int i = 0; i < 500; i++) {
        WebServer.handleClient();
        delay(10);
      }
    }
  });

  WebServer.on("/nefry_css", [&]() {
    WebServer.send(200, "text/css", F(
                     "@charset \"UTF-8\"; *, :after, : before{ box - sizing:border - box }body{ font:16px / 1.65 \"Noto Sans\",\"HelveticaNeue\",\"Helvetica Neue\",Helvetica,Arial,sans-serif;margin:0;padding:0 20px;color:#555}"
                     "ol,ul{padding-left:20px;margin:0}a{color:#54AFBA}a:hover{text-decoration:none}body>div{background:#fff;margin:20px auto;padding:20px 24px;box-shadow:0 0 1px 1px rgba(0,0,0,.1);border-radius:4px;max-width:540px}"
                     "body>div input,body>div li{word-wrap:break-word}body>div>h1{font-size:1.4em;line-height:1.3;padding-bottom:4px;border-bottom:1px solid #efefef;margin-top:0;margin-bottom:20px}input,select,textarea{font:inherit inherit inherit}"
                     "input{background:rgba(0,0,0,0);padding:.4em .6em;border:1px solid rgba(0,0,0,.12);border-radius:3px;-webkit-appearance:none;-moz-appearance:none;appearance:none}input:focus{border:1px solid #6E5F57;outline:0}"
                     "input[type=submit],input[type=button],button[type=button]{margin:6px;cursor:pointer;line-height:2.6;display:inline-block;padding:0 1.2rem;text-align:center;vertical-align:middle;color:#FFF;border:0;border-radius:3px;background:#6E5F57;-webkit-appearance:none;-moz-appearance:none;appearance:none}"
                     ".row,.row>div,.row>label{display:block}input[type=submit]:hover{color:#FFF;background:#372F2A}input[type=submit]:focus{outline:0}input[type=file]{width:100%}.row{margin-bottom:14px}"
                     ".row>label{float:left;width:110px;font-size:14px;position:relative;top:8px}.row>div{margin-left:120px;margin-bottom:12px}.row>div>input{width:86%;display:inline-block}.footer{text-align:right;margin-top:16px;margin-right:35px}"
                     ".psrow{text-align: center;}.psrow>div{display:inline-block;margin:10px;}.writemode{color:#ff0000}input[type=\"checkbox\"] {-webkit-appearance: checkbox;appearance: checkbox;}.row>div>.ip{width:17%}"
                   ));
  });
}

void webServer_begin() {
  Serial.println("scan Start");
  scanWiFi();
  WebServer.begin();
  beginWeb();
}

void webServer_loop()
{
  WebServer.handleClient();
}
