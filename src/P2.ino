#include<SoftwareSerial.h>
#include<ArduinoJson.h>
#include<ESP8266WiFi.h>
#include<ESP8266WebServer.h>

#define RX D6 //receive data from arduino(6)
#define TX D5 //transmit data from arduino(5)

char OpMode = '0';
int modeVal = 0;    //initially in normal mode with OpMode = '0'
int lampStatus[4];
int errStatus;
int statReq = 0;
int seenErrStat = 1; 

//for connection to router
String SSID = "Dlink##@";
String PWD = "heisenberg";
int wifiStatus;

//server setting
ESP8266WebServer server(80);

SoftwareSerial S(RX,TX);

void setup() {
  // put your setup code here, to run once:
  S.begin(9600);
  Serial.begin(9600);

  for(int i=0;i<4;i++)
    lampStatus[i] = 0;

  while (!Serial) continue;
  delay(100);
  
  //connect to wifi network
  Serial.println("Connecting to: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PWD);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi Connected. Got IP address: ");
  Serial.println(WiFi.localIP());

  /*server settings*/

  //handle request ot root
  server.on("/", HTTP_GET, handle_OnConnect);
  //handle request for status check
  server.on("/statusCheck", HTTP_POST, handle_StatusCheck);
  //handle request for override
  server.on("/overrideMode", HTTP_POST, handle_OverrideMode);

  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.print("HTTP server started @ ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:

  S.write(OpMode);
  server.handleClient();

  //randNum = random(0, 5);
  
  Serial.print("OpMode: ");
  Serial.println(OpMode);
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(S);
  if(root == JsonObject::invalid()){
    //delay(500);
    return;
  }
  Serial.println("error status json rcvd and parsed!");
  //root.prettyPrintTo(Serial);
  errStatus = root["err"];
  Serial.print("ErrStatus: ");
  Serial.println(errStatus);
  if(errStatus == 1){
    int temp = 99;
    seenErrStat = 0;
    for(int i=0; i<4; i++){
      temp = root["segs"][i];
      lampStatus[i] = temp;
      Serial.print(temp);
    }  
  }
  else{
    if(seenErrStat==1){
      for(int i=0; i<4; i++)
        lampStatus[i] = 0;
    }
  }
  Serial.println();
  //delay(500);
}


/*root directory function */
void handle_OnConnect(){
  Serial.println("root directory client request..");
  String s = getHTMLContent();
  server.send(200, "text/html", s);
}

/*request to unknown page*/
void handle_NotFound(){
  server.send(404, "text/html", "<!DOCTYPE html> <html><head><h3>404: Page not found!!</h3></head></html>");
}


String getHTMLContent(){

  //String s = "<!DOCTYPE html><html><head><title>here</title></head><body><p><a href=\"allon\">Click</a></p></body></html>";
  String header = "<!DOCTYPE html><html><head><title>Smart Street light</title><h2 style=\"text-align: center\">Smart Street LIght System</h2></head><body>";
  String checkStatForm = "<div><form action=\"/statusCheck\" method=\"POST\"><input type=\"submit\" value=\"Check light status\"></form> </div>";
  String preSelectForm = "<div><form action=\"/overrideMode\" method=\"POST\">";
  String select;
  if(modeVal == 0)
    select = "<select name=\"mode\"><optgroup label=\"Primary modes\"><option selected value=\"normal\">Normal</option><option value=\"fullon\">Full on</option><option value=\"fulloff\">Full off</option></optgroup><optgroup label=\"Eastern cities\"><option value=\"kol\">Kolkata</option><option value=\"guw\">Guwahati</option></optgroup><optgroup label=\"Western cities\"><option value=\"ahm\">Ahmedabad</option><option value=\"mum\">Mumbai</option></optgroup></select>";
  else if(modeVal == 1)
    select = "<select name=\"mode\"><optgroup label=\"Primary modes\"><option value=\"normal\">Normal</option><option selected value=\"fullon\">Full on</option><option value=\"fulloff\">Full off</option></optgroup><optgroup label=\"Eastern cities\"><option value=\"kol\">Kolkata</option><option value=\"guw\">Guwahati</option></optgroup><optgroup label=\"Western cities\"><option value=\"ahm\">Ahmedabad</option><option value=\"mum\">Mumbai</option></optgroup></select>";
  else if(modeVal == 9)
    select = "<select name=\"mode\"><optgroup label=\"Primary modes\"><option value=\"normal\">Normal</option><option value=\"fullon\">Full on</option><option selected value=\"fulloff\">Full off</option></optgroup><optgroup label=\"Eastern cities\"><option value=\"kol\">Kolkata</option><option value=\"guw\">Guwahati</option></optgroup><optgroup label=\"Western cities\"><option value=\"ahm\">Ahmedabad</option><option value=\"mum\">Mumbai</option></optgroup></select>";
  else if(modeVal == 21)
    select = "<select name=\"mode\"><optgroup label=\"Primary modes\"><option value=\"normal\">Normal</option><option value=\"fullon\">Full on</option><option value=\"fulloff\">Full off</option></optgroup><optgroup label=\"Eastern cities\"><option selected value=\"kol\">Kolkata</option><option value=\"guw\">Guwahati</option></optgroup><optgroup label=\"Western cities\"><option value=\"ahm\">Ahmedabad</option><option value=\"mum\">Mumbai</option></optgroup></select>";  
  else if(modeVal == 22)
    select = "<select name=\"mode\"><optgroup label=\"Primary modes\"><option value=\"normal\">Normal</option><option value=\"fullon\">Full on</option><option value=\"fulloff\">Full off</option></optgroup><optgroup label=\"Eastern cities\"><option value=\"kol\">Kolkata</option><option selected value=\"guw\">Guwahati</option></optgroup><optgroup label=\"Western cities\"><option value=\"ahm\">Ahmedabad</option><option value=\"mum\">Mumbai</option></optgroup></select>";
  else if(modeVal == 31)
    select = "<select name=\"mode\"><optgroup label=\"Primary modes\"><option value=\"normal\">Normal</option><option value=\"fullon\">Full on</option><option value=\"fulloff\">Full off</option></optgroup><optgroup label=\"Eastern cities\"><option value=\"kol\">Kolkata</option><option value=\"guw\">Guwahati</option></optgroup><optgroup label=\"Western cities\"><option selected value=\"ahm\">Ahmedabad</option><option value=\"mum\">Mumbai</option></optgroup></select>";
  else if(modeVal == 32)
    select = "<select name=\"mode\"><optgroup label=\"Primary modes\"><option value=\"normal\">Normal</option><option value=\"fullon\">Full on</option><option value=\"fulloff\">Full off</option></optgroup><optgroup label=\"Eastern cities\"><option value=\"kol\">Kolkata</option><option value=\"guw\">Guwahati</option></optgroup><optgroup label=\"Western cities\"><option value=\"ahm\">Ahmedabad</option><option selected value=\"mum\">Mumbai</option></optgroup></select>";
  String postSelectForm = "<input type=\"submit\" value=\"Change mode\"></form></div>";
  String footer = "</body></html>";
  String showStat = "";
  if(statReq == 1){
    for(int k=0; k<4; k++){
      if(lampStatus[k] == 1){
        showStat +="<div style=\"border: solid; border-radius: 3px; border-color: #ff0000\"><p>Lamp "+String(k+1)+" is malfunctioning</p></div>"; 
      }
      else{
        showStat +="<div style=\"border: solid; border-radius: 3px; border-color: #00ff00\"><p>Lamp "+String(k+1)+" is ok</p></div>";
      }
    }
    statReq = 0;
    seenErrStat = 1;
  }


  String s = header + preSelectForm + select + postSelectForm + showStat + checkStatForm + footer;
  return s;
}

void handle_StatusCheck(){
  statReq = 1;
  server.sendHeader("Location", "/");
  server.send(303);
}

void handle_OverrideMode(){
  if(server.hasArg("mode")){
    String modeReq = "";
    if(server.arg("mode")!=NULL){
      modeReq = server.arg("mode");
      if(modeReq == "fullon"){
        modeVal = 1;
        OpMode = '1';
      }
      else if(modeReq == "fulloff"){
        OpMode = '9';
        modeVal = 9;
      }
      else if(modeReq == "kol"){
        OpMode = '2';
        modeVal = 21;
      }
      else if(modeReq == "guw"){
        OpMode = '2';
        modeVal = 22;
      }
      else if(modeReq == "ahm"){
        OpMode = '3';
        modeVal = 31;
      }
      else if(modeReq == "mum"){
        OpMode = '3';
        modeVal = 32;
      }
      else{
        OpMode = '0';
        modeVal = 0;
      }
    }
    else{
      OpMode = '0';
      modeVal = 0;
    }

    String mode;
    mode = String(mode+OpMode);
    //String s = "<!DOCTYPE html><html><head><title>here in allon</title></head><body><p>Opmode value: "+mode+"</p></body></html>";
    Serial.print("OpMode = ");
    Serial.println(OpMode);
    server.sendHeader("Location", "/");
    server.send(303);


    //server.send(200, "text/html", s);
  }
  else{
    server.send(400, "text/plain", "400: Invalid request");
  }
}
