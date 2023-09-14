

// from https://www.elektormagazine.de/news/eineinfacher2wlanschalterderfunktioniert
// see https://docs.arduino.cc/library-examples/wifi-library/WiFiWebServer

#include <ESP8266WiFi.h>
#include <arduino-timer.h>            // requires arduino-timer library

// const char* ssid = "******";     // fill in here your router or wifi SSID
// const char* password = "******"; // fill in here your router or wifi password

const char* ssid = "4GCPESFC";      // fill in here your router or wifi SSID
const char* password = "A1b2c3d4";  // fill in here your router or wifi password

IPAddress  ip(192, 168, 100, 124);     // fill in here the desired IP Address
IPAddress  gateway(192, 168, 100, 1);  // set gateway to match your network
IPAddress  subnet(255, 255, 255, 0);   // set subnet mask to match your network
WiFiServer server(8083);               // WEB port 8083

String msg = "{\"deviceId\":\"_watchdog03\", \"data\": {\"dps\":{";       // set deviceId

int tdelay = 60000;     // default value test time ms
int cutoff =  3000;     // default value OFF time ms

//-------------------------
Timer<2> timer;         // create 2 timers with default settings
int relay = 0;          // ESP01-relay uses GPIO 0;
String state = " Off - device ON";
boolean connected = true;
String outStile = "HTML";
String answ = "";
String operation = "SET";
String property;

String readString;      // for debug

//  -------  functions
// timer callbacks

bool end_cutoff(void *argument) {
  digitalWrite(relay, HIGH);           // turns relay off
  digitalWrite(LED_BUILTIN, 0);
  state = " Off - device ON";
  Serial.println("** relay" + state);
  return false;
}

bool end_delay(void *argument) {
  if (connected) {
    Serial.println("** connected: delay ended");
    return false;
  }
  digitalWrite(relay, LOW);            //  turns relay on
  digitalWrite(LED_BUILTIN, 1);
  state = " On - device OFF";
  timer.in(cutoff, end_cutoff);
  Serial.println("** disconnected: delay restart, relay" + state);
  return true;                        // return true: repeat delay
}

String quote( String item) {
  return ("\"" + item + "\"");
}

// iterable, if required, value must be quoted
void setAnswJSON(String dp, String value) {
  if (answ == "")
    answ = msg +  quote(dp) + ": " + value;
  else
    answ += ", " + quote(dp) + ": " + value ;
}

// ------------------------
void setup() {
  Serial.begin(115200);
  pinMode(relay, OUTPUT);      // set the LED pin mode
  digitalWrite(relay, HIGH);   // relay OFF
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}
/*
  SET msg.payload:{
  //    "toDev":"60a823fffe****",     // tuyaDAEMON extrension: deviceID|gatewayID
      "payload": {
  //        "cid": "60a453fffe*****",  // cid, only for virtual devices (string)
          "dps":"103",                                // DP or pseudoDP (string)
          "set":220                                 // BOOLEAN, INT/ENUM, STRING
          } }
*/
/*
  GET  msg.payload: {
  //     "toDev":"60a823fff******",   // tuyaDAEMON extrension: deviceID/gatewayID
      "payload": {
  //         "cid": "60a453fffe******",         //cid, only if toDev = gatewayID
          "operation":"GET",
          "dps":"103"
          } }

*/

/*
  Answer  msg.payload:{
  //        "remote_from": undefined|remoteName,      // only from remote instance, see global.remotemap
        "deviceId":    gatewayID|deviceid,          // from virtual devices => "deviceId": gatewayId
  //        "deviceName":  name,                     // from tuya-smart-device node, optional - not used
        "data": {
  //            "t": Math.floor( Date.now() / 1000 );       // timestamp (sec), from tuya node, optional
  //            "cid": deviceid;                                 // only from virtual devices, mandatory
            "dps":{
                "[dp]": value                               // encoded, more than one dp in some cases
                ....
            }}}}
*/

void loop() {
  auto ticks = timer.tick();
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");          // print a message out the serial port
    outStile = "HTML";                      // init defaults
    operation = "SET";
    property = "";
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        readString += c;
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine == "") {
            break ;                         // last line
          } else
            currentLine = "";               // one more line
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // ================== Check to see if the client request was REST: "GET /H" or "GET /L":
        // tests currentLine only one time
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(relay, LOW);               // GET /H turns relay on
          digitalWrite(LED_BUILTIN, 1);
          state = " On - device OFF";
          outStile = "HTML";
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(relay, HIGH);              // GET /L turns relay off
          digitalWrite(LED_BUILTIN, 0);
          state = " Off - device ON";
          outStile = "HTML";
        }

        // URLencoded: "GET ?key=value&more..." :
        if (currentLine.endsWith("HTTP/1")  && (currentLine.indexOf("=") > 0)) {     // Checks URLencoded
          outStile = "JSON";                       // default JSON answer
          answ = "";                               // init JSON answer
          String data = currentLine.substring(currentLine.indexOf("?") + 1, currentLine.lastIndexOf(" "));  // trims parameters
          while (data.length() > 3) {
            // parse data, extracts one URLencoded pair: key=value&more...
            int dend = (data.indexOf("&") > 0) ? data.indexOf("&") : data.length();
            String key = data.substring(0, data.indexOf("="));
            String value = data.substring(data.indexOf("=") + 1, dend );
            data = data.substring(dend + 1);
            //  Serial.println(" parsed => " + key + " = " + value + " + " + data );

            // ================ Check to see if the client request was basic URLencoded (without uderscore) :
            // do not use as keys: 'dps','operation', 'set' and 'outmode' is reserved

            if (key == "outmode") {               // to force an outmode, 'HTML' or 'JSON' (default)
              outStile = value;
              setAnswJSON("outmode", quote(outStile));
            }

            if (key == "delay") {
              tdelay = value.toInt();             // SET value
              setAnswJSON("delay",  String(tdelay));
            }
            if (key == "cutoff") {
              cutoff = value.toInt();             // SET value
              setAnswJSON("cutoff", String(cutoff));
            }
            if (key == "relay") {
              if (value) {                        // Execute
                digitalWrite(relay, LOW);
                digitalWrite(LED_BUILTIN, 1);
                state = " On - device OFF";
                setAnswJSON("relay", quote("ON"));

              } else {
                digitalWrite(relay, HIGH);
                digitalWrite(LED_BUILTIN, 0);
                state = " Off - device ON";
                setAnswJSON("relay", quote("OFF"));
              }
            }
            // ================ Check to see if the client request was TuyaDAEMON JSON SET / GET (with underscore):
            if (key == "dps") {
              property = value;                   //  GET answers
              if (property == "_cutoff")
                setAnswJSON(property, String(cutoff));
              if (property == "_delay")
                setAnswJSON(property, String(tdelay));
              if (property == "_outDC")
                setAnswJSON("_outDC", quote(state == " On - device OFF" ? "OFF" : "ON"));
            }


            if (key == "operation") {
              operation = value;
              if (operation == "GET") {          // GET answers
                if (property == "_cutoff")
                  setAnswJSON(property, String(cutoff));
                if (property == "_delay")
                  setAnswJSON(property, String(tdelay));
                if (property == "_outDC")
                  setAnswJSON(property, quote(state == " On - device OFF" ? "OFF" : "ON"));
              }
            }

            if ((key == "set") && (property) && (operation == "SET")) {     // SET Answers
              if (property == "_cutoff") {
                if (value != "")
                  cutoff = value.toInt();
                setAnswJSON(property, String(cutoff));
              }

              if (property == "_delay") {
                if (value != "")
                  tdelay = value.toInt();
                setAnswJSON(property, String(tdelay));
              }

              if (property == "_dv_conn") {
                if (value != "") {
                  connected = ( value != "false");
                  if ((!connected) && (timer.empty())) {
                    Serial.println("** disconnected: delay start...");
                    timer.every(tdelay, end_delay);
                  }
                }
                setAnswJSON(property, connected ? "true" : "false");
              }

              if (property == "_outDC") {
                if (value != "") {
                  if ((value == "OFF") || (!value)) {
                    digitalWrite(relay, LOW);
                    digitalWrite(LED_BUILTIN, 1);
                    state = " On - device OFF";
                  } else {
                    digitalWrite(relay, HIGH);
                    digitalWrite(LED_BUILTIN, 0);
                    state = " Off - device ON";
                  }
                }
                setAnswJSON(property, quote(state == " On - device OFF" ? "OFF" : "ON"));
              }

            } // end SET

          }
        }// end URLencoded

      }
    } // end connected

    // ----------------- input process done: now builds the response

    if ( outStile == "HTML") {

      // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
      // and a content-type so the client knows what's coming, then a blank line:
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println();
      // the content of the HTTP response follows the header:
      client.print("<HTML><HEAD>");
      client.print("<title>Watchdog03</title></HEAD><BODY><br>");

      client.println("Welcome to Watchdog03 WiFi<p>");

      //  standard HTML form
      client.println("<form action='.' method='GET'>");
      client.print("Time Configuration <p>");
      client.print("delay &nbsp;&nbsp;&nbsp; <input type='text' id='delay' name='delay' size='10'  value='");
      client.print(tdelay);
      client.print("'> <p>");
      client.print("cutoff &nbsp;&nbsp; <input type='text' id='cutoff' name='cutoff' size='8'  value='");
      client.print(cutoff);
      client.print("'> <p>");
      client.print("<input type='hidden' id='outmode' name='outmode' value='HTML'>");
      client.print("<input type=submit value='save'> ");
      client.print("</form><p>");

      //  simple REST interface
      client.print("<input type=submit value=ON style=width:100px;height:45px onClick=location.href='/H'>");
      client.print("<input type=submit value=OFF style=width:100px;height:45px onClick=location.href='/L'>");
      // info status
      client.print("<p>");
      client.print("The relay is: ");
      client.print(state);
      client.print("<br><br>");
      client.println("</BODY></HTML>");
      client.println();
    }

    if ( outStile == "JSON") {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:application/json");
      client.println();
      client.print(answ + "}}}");
      client.println();
      Serial.println("JSON: " + answ + "}}}");
    }

    // close the connection:
    client.stop();

    // debug
    Serial.println("==>" + readString + "<==");
    readString = "";
    Serial.println("Client Disconnected.");
  } // end if client
}
