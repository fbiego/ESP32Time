/* 

Power ON the ESP32, select ESP32-RTC-Logger network on your phone and enter the password.

Visit 192.168.4.1 using your browser and press syncTime.

This will run 'rtc.setTime(epochTime)' and send the ESP32 to sleep for the duration of 'TIME_TO_SLEEP'. 

*/

/*
   MIT License

  Copyright (c) 2024 Francisco Bexiga

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <ESP32Time.h>
#include <WiFi.h>

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  30        /* Time ESP32 will go to sleep (in seconds) */

ESP32Time rtc;

// Set your access point name and password
const char *ssid = "ESP32-RTC-Logger";
const char *password = "12345678";

// Create an instance of the server
WiFiServer server(80);

// Variable to store the state
RTC_DATA_ATTR bool timeWasSet = false;

// LED pin
const int ledPin = 2;

void wakeup_reason() {
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();
    switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0 :
        Serial.println("Wakeup caused by external signal using RTC_IO");
        break;
    case ESP_SLEEP_WAKEUP_EXT1 :
        Serial.println("Wakeup caused by external signal using RTC_CNTL");
        break;
    case ESP_SLEEP_WAKEUP_TIMER :
        Serial.println("Wakeup caused by timer");
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD :
        Serial.println("Wakeup caused by touchpad");
        break;
    case ESP_SLEEP_WAKEUP_ULP :
        Serial.println("Wakeup caused by ULP program");
        break;
    default :
        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
        break;
    }
}

void blinkLED(int numBlinks, int blinkInterval) {
    for (int i = 0; i < numBlinks; i++) {
        digitalWrite(ledPin, HIGH); // Turn on the LED
        delay(blinkInterval);
        digitalWrite(ledPin, LOW); // Turn off the LED
        delay(blinkInterval);
    }
}

void setup() {
    // Start the Serial communication to see some output
    Serial.begin(115200);
    delay(1000);

    // Initialize LED pin as an output
    pinMode(ledPin, OUTPUT);

    // Display wake up reason
    wakeup_reason();

    Serial.print("timeWasSet: ");
    Serial.println(timeWasSet);

    // Check if time has been set
    if (timeWasSet) {
        // Blink the LED in a loop
        while (true) {
            blinkLED(3, 100); // Blink the LED 3 times with a 100ms interval

            Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));   // (String) returns time with specified format
			
			// Do the logging
			/* YOUR CODE HERE */
			
			// Go to sleep again
            esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

            Serial.println("Going to sleep now");
            Serial.flush();
            esp_deep_sleep_start();
        }
    }

    // Set up the access point
    Serial.print("Setting up access point...");
    WiFi.softAP(ssid, password);

    // Print the IP address
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Start the server
    server.begin();
}

void loop() {
    // Check if a client has connected
    WiFiClient client = server.available();

    if (client) {
        Serial.println("New client connected");
        String currentLine = "";                // make a String to hold incoming data from the client

        while (client.connected()) {            // loop while the client's connected
            if (client.available()) {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                if (c == '\n') {                    // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        // the content of the HTTP response follows the header:
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><title>ESP32 Web Server</title></head>");

                        client.println("<body>");

                        client.println("<div>");

                        client.println("<h1 id=\"currentTime\"></h1>");
                        client.println("<h2 id=\"showEpochTime\"></h2>");
                        client.println("<script>");
                        client.println("function updateTime() {");
                        client.println("  var now = new Date();");
                        client.println("  now.setHours(now.getHours() + 1);"); // Apply the GMT+1 offset");
                        client.println("  var timeString = now.toLocaleTimeString();");
                        client.println("  var showEpochTime = Math.floor(now.getTime() / 1000);"); // Calculate Epoch time");
                        client.println("  document.getElementById('currentTime').innerHTML = 'Current Time: ' + timeString;");
                        client.println("  document.getElementById('showEpochTime').innerHTML = 'Epoch Time: ' + showEpochTime;");
                        client.println("  document.getElementById('hiddenEpochTime').value = showEpochTime;"); // Set hidden field value
                        client.println("}");
                        client.println("setInterval(updateTime, 1000);");
                        client.println("window.onload = updateTime;");
                        client.println("</script>");

                        client.println("<form action=\"/syncTime\" method=\"POST\">");
                        client.println("  <input type=\"hidden\" name=\"epochTime\" id=\"hiddenEpochTime\">");
                        client.println("  <input type=\"submit\" value=\"syncTime\">");
                        client.println("</form>");

                        client.println("</div>");

                        client.println("</body>");
                        client.println("</html>");

                        // The HTTP response ends with another blank line:
                        client.println();
                        break;
                    } else { // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                } else if (c != '\r') {  // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }

                // Check if the line ends with "POST /syncTime"
                if (currentLine.endsWith("POST /syncTime")) {
                    Serial.println();
                    Serial.println("syncTime requested");

                    // Read the POST request body
                    String requestBody = "";
                    while (client.available()) {
                        char c = client.read();
                        requestBody += c;
                    }

                    // Extract the epochTime value from the request body
                    int epochTimeIndex = requestBody.indexOf("epochTime=");
                    if (epochTimeIndex != -1) {
                        String epochTimeStr = requestBody.substring(epochTimeIndex + 10); // Extract the value
                        long epochTime = epochTimeStr.toInt(); // Convert to integer
                        Serial.print("Epoch Time from request: ");
                        Serial.println(epochTime);

                        // Set the RTC time (if desired, depending on how you want to use this value)
                        rtc.setTime(epochTime);

                        timeWasSet = true; // Update timeWasSet flag
                    }

                    delay(1000); // Give some time to the client to receive the response

                    // Set up deep sleep
                    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

                    Serial.println("Going to sleep now");
                    Serial.flush();
                    esp_deep_sleep_start();
                }
            }
        }
        // close the connection:
        client.stop();
        Serial.println("Client disconnected");
        Serial.println("");
    }
}
