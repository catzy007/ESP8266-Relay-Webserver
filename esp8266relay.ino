#include <ESP8266WiFi.h>
 
const char* ssid = "YOUR_SSID";         //fill in here your router or wifi SSID
const char* password = "YOUR_PASSWORD"; //fill in here your router or wifi password
const int LedGpioPin = 2;               //GPIO pin for onboard LED
const int RelayGpioPin = 0;             //relay connected to GPIO0
const int WebPort = 80;                 //web server port. default=80
IPAddress ip(192, 168, 1, 252);         //set static ip
IPAddress gateway(192, 168, 1, 254);    //set gateway
IPAddress subnet(255, 255, 255, 0);     //set subnet
WiFiServer server(WebPort);             //set server port
 
void setup() {
    //Set baud rate for serial monitor
    Serial.begin(115200);

    //Configure and set initial value GPIO PIN
    pinMode(LedGpioPin, OUTPUT);
    pinMode(RelayGpioPin, OUTPUT);
    digitalWrite(RelayGpioPin, LOW);

    //Connect to WiFi network
    Serial.println();
    Serial.print("CON : Connecting to "); Serial.println(ssid);
    WiFi.config(ip, gateway, subnet); WiFi.begin(ssid, password);
    Serial.print("CON : Connecting");
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println(""); Serial.println("CON : WiFi connected");

    //Start the server
    server.begin();
    Serial.println("SYS : Server started");

    //Print the IP address
    Serial.print("CON : Connected using IP ");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.println(WebPort);
}

String WebHeader(){
    //set server response as http text/html
    String HtmlHeader = 
        String("HTTP/1.1 200 OK\r\n") +
        "Content-Type: text/html\r\n" +
        "Connection: close\r\n" +
        "\r\n";
    return HtmlHeader;
}
 
void loop() {
    //Show led to monitor activity
    digitalWrite(LedGpioPin, HIGH);
    delay(2000);
    digitalWrite(LedGpioPin, LOW);

    //Check if a client has connected
    WiFiClient client = server.available();
    if(!client){
        return;
    }

    //Wait until the client sends some data
    Serial.println("SYS : Incoming Connection");
    while(!client.available()){
        delay(1);
    }

    //Read the first line of the request
    String request = client.readStringUntil('\r');
    Serial.print("SYS : ");
    Serial.println(request);
    client.flush();

    //Process the request
    int value = LOW;
    if (request.indexOf("/RELAY=ON") != -1){
        Serial.println("SYS : RELAY ON");
        digitalWrite(RelayGpioPin, LOW);
        value = LOW;
    }
    if (request.indexOf("/RELAY=OFF") != -1){
        Serial.println("SYS : RELAY OFF");
        digitalWrite(RelayGpioPin, HIGH);
        value = HIGH;
    }

    //Return the response
    client.println(WebHeader()); //this is a must
    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("<head>");
    client.println("    <meta charset=\"UTF-8\">");
    client.println("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
    client.println("    <title>Hello</title>");
    client.println("    <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\">");
    client.println("    <style>");
    client.println("        body{");
    client.println("            background-image: url(\"https://catzy007.github.io/assets/background.jpg\");");
    client.println("            min-height: 500px;");
    client.println("            background-attachment: fixed;");
    client.println("            background-position: center;");
    client.println("            background-repeat: no-repeat;");
    client.println("            background-size: cover;");
    client.println("        }");
    client.println("        .mycontainer{");
    client.println("            top: 50px;");
    client.println("            left: 0;");
    client.println("            right: 0;");
    client.println("            padding: 15%;");
    client.println("            margin-left: auto;");
    client.println("            margin-right: auto;");
    client.println("            background-color: rgba(253, 253, 253, 0.9);");
    client.println("            min-height: 100vh;");
    client.println("		    overflow: auto;");
    client.println("            border: 2px solid #ccc;");
    client.println("            border-radius: 10px;");
    client.println("        }");
    client.println("    </style>");
    client.println("</head>");
    client.println("<body>");
    client.println("    <div class=\"container\">");
    client.println("        <div class=\"row justify-content-center\">");
    client.println("            <div class=\"col-sm-12\">");
    client.println("                <div id=\"main\" class=\"mycontainer\">");
    client.println("                    <h1>Xeon-Server Remote Management</h1>");
    client.println("                    <h3>Relay Status "); if(value == HIGH){client.print("OFF");}else{client.print("ON");} client.print("</h3>");
    client.println("                    <h4>Turn <a href=\"/RELAY=OFF\">OFF</a> RELAY<h4>");
    client.println("                    <h4>Turn <a href=\"/RELAY=ON\">ON</a> RELAY<h4>");
    client.println("                </div>");
    client.println("            </div>");
    client.println("        </div>");
    client.println("    </div>");
    client.println("    <script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\" crossorigin=\"anonymous\"></script>");
    client.println("    <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\" integrity=\"sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1\" crossorigin=\"anonymous\"></script>");
    client.println("    <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\" integrity=\"sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM\" crossorigin=\"anonymous\"></script>");
    client.println("</body>");
    client.println("</html>");

    delay(1);
    Serial.println("SYS : Connection Terminated");
    Serial.println("");
}
