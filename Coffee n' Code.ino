// Import required libraries
#include "WiFi.h"               // this library can instantiate Servers, Clients and send/receive UDP packets through WiFi.
#include "ESPAsyncWebServer.h"  // ESPAsyncWebServer library that provides an easy way to build an asynchronous web server.
#include <Adafruit_Sensor.h>   // this library is developed by adafruit which is used to communicate with sensors
#include <DHT.h>               // Arduino library for DHT11, DHT22, etc Temp & Humidity Sensors
#include<Wire.h>               // This library allows you to communicate with I2C devices.
#include<LiquidCrystal_I2C.h> // The library allows to control I2C displays with functions as similar to LiquidCrystal library. 

LiquidCrystal_I2C lcd(0x27,16,4); //I2C address 0x27, 16 column and 4 rows and 0x27 is address of I2C 

const char* ssid = "POCO PHONE"; //ssid name (wifi name)
const char* password = "Prince@61299"; // wifi passwords

const int buttonPin = 23;  // the pin number of the pushbutton
const int redLedPin =  18;      // the pin number of the red LED 
const int greenLedPin = 19; // pin number i=of green LED
int buttonState = 0;  //  initial button state
const int relayPin = 2; // pin number of rellay

#define DHTPIN 27     // Digital pin connected to the DHT sensor
// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11 (define the sensor type)
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE); //to define the object of dht sensor ie pin no & sensor type

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  // if (isnan(t)) {    
  //   Serial.println("Failed to read from DHT sensor!");
  //   return "--";
  // }
  // else {
    Serial.print("TEMPRATURE = "); //to print the message on serial monitor
    Serial.println(t); // to print message on serial monitor in new line
    return String(t);
  // }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // if (isnan(h)) {
  //   Serial.println("Failed to read from DHT sensor!");
  //   return "--";
  // }
  // else {
    Serial.print("HUMIDITY = "); //to print the message on serial monitor
    Serial.println(h); // to print message on serial monitor in new line
    return String(h);
  // }
}

String readAQI()
{
  float a = analogRead(A0); //to calculate the aqi using formula
  a/= 6.9;
  
  Serial.print("AQI = "); //print in serial monitir
  Serial.println(a); //to print value of AQI
  return String(a);

}

String readBDTemp()
{
  
  float val = analogRead(34); //to read the analog pin 34
  float mv = ( val/1024.0)*5000; // formula to calculate temp from voltage change 
  float cel = mv/10; // to convert in celsius  
  float farh = (cel*9)/5 + 32; // to convert in fahrenheit 
  Serial.print("TEMPRATURE = "); // to print message in serial monitor
  Serial.print(farh); // to print temp in fahrenheit
  Serial.print("*F");
  return String(farh);
}

String Beep() 
{
  buttonState = digitalRead(buttonPin); //to read the state of pushbutton

  //code for push button 
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
    delay(100);
    digitalWrite(redLedPin, LOW);
    delay(10);
  } 
  else {
    // turn LED off:
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
  }
  return String(buttonState);
}



const char index_html[] PROGMEM = R"rawliteral(
  
  <!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Fleet Manager DashBoard</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#FF0000;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <i class="fas fa-leaf" style="color:#00FF00;"></i> 
    <span class="dht-labels">AQI</span>
    <span id="AQI">%AQI%</span>
    <sup class="units">PPM</sup>
  </p>
   <p>
    <i class="fas fa-male" style="color:#FFA500;"></i> 
    <span class="dht-labels">Body Temp</span>
    <span id="Body Temp">%Body Temp%</span>
    <sup class="units">&degC</sup>
  </p>
   <p>
   
    <i class="fa fa-bell-slash" style="color:#FF0000;"></i> 
    <span class="dht-labels">SOS</span>
    <span id="Body Temp">%BEEP%</span>
 
  </p>
  
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("AQI").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/AQI", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("Body Temp").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/Body Temp", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("BEEP").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/BEEP", true);
  xhttp.send();
}, 10000 ) ;


</script>
</html>
)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  else if(var== "AQI")
  {
    return readAQI();
  }
  else if(var=="Body Temp")
  {
    return readBDTemp();
  }
  else
  {
    return Beep();
  }
  return String();
}

void setup(){

  pinMode(redLedPin, OUTPUT); // pinMode of red LED
  pinMode(greenLedPin, OUTPUT); //pinMode of green LED
  pinMode(buttonPin, INPUT); //pinMode of pushButton
  pinMode(relayPin, OUTPUT); //pinMode of rellay

  // pinMode(34, INPUT); // input pin for LM35
  // Serial port for debugging purposes
  Wire.begin();         // it is used to start the comuinication with I2C 
  dht.begin();      // to start the dht sensor 
  Serial.begin(115200);  // it is used for start the start the serial monotor 115200 is a baudrate
  Serial.println("\nI2C Scanner"); // to print output in serial monitor
  lcd.init(); // to initalized the lcd

  // Print a message to the LCD.
  lcd.backlight();  //used for baklight of lcd
  lcd.setCursor(2,1); // set cursor of lcd to 2, 1 ie  2nd column and 1st row
  lcd.print("Coffee n' Code");  //to print message to lcd
  lcd.setCursor(4,2);  // to set cursor at 4th column and 2 row 
  lcd.print("Team SIH"); //to print message to lcd
  delay(2000);  // for time delay of 2 sec.
  lcd.clear();  // for reset the lcd screen
  
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }


  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });
   server.on("/AQI", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readAQI().c_str());
  });
  server.on("/readBDTemp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readBDTemp().c_str());
  });

  // Start server
  server.begin(); 
}
 
void loop(){
  //For DHT11 Sensor
  lcd.setCursor(0,0); to //to set cursor of lcd at initial position
  lcd.print("Cabin Temp = "); //print message on lcd screen
  lcd.print(readDHTTemperature()); // to display output of dht sensor on screen
  lcd.print(" C"); //print message on lcd screen 

  //for Humidity 

  lcd.setCursor(0,1);  //to set cursor of lcd at position 0,1 
  lcd.print("Humidity = ");  //to print message on lcd
  lcd.print(readDHTHumidity()); // to print the data of humidity sensor
  lcd.print(" %"); //to print msg

  // For AQI
  lcd.setCursor(0,2); //set cursor to 2 row 
  lcd.print("AQI = "); //to print message on lcd
  lcd.print(readAQI()); // to print the data of AQI sensor
  lcd.print(" PPM"); // to print the message ppm on lcd

  //for Body Temperature 
  lcd.setCursor(0,3); // set cursor to 3rd row
  lcd.print("Body Temp = "); // to print msg on lcd
  lcd.print(readBDTemp()); // to print value of body temp. (LM35)
  lcd.print(" C");
  delay(1000); // for delay of 1 sec
Beep();

// code for emergency buzzer

if (readAQI() > "400"){
  digitalWrite(relayPin, LOW);
}
else {
  digitalWrite(relayPin, HIGH);
}
}
