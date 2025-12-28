#include <Arduino.h>
#include <ESP32Servo.h>
#include <WebServer.h>
#include <WiFi.h>

#define SERVO_PIN 2

const char* ssid = "your Wifi Name";
const char* password = "wife password here";

const char index_html[] = R"(   
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>ESP32 light switch</title>
  <style>
    :root {
      --bg: #f5f6fa;
      --primary: #0078d7;
      --text: #333;
      --card-bg: #fff;
      --border: #ddd;
    }

    body {
      font-family: 'Segoe UI', Roboto, sans-serif;
      background-color: var(--bg);
      margin: 0;
      padding: 0;
      color: var(--text);
      display: flex;
      flex-direction: column;
      align-items: center;
      min-height: 100vh;
    }

    header {
      width: 100%;
      background: var(--primary);
      color: white;
      text-align: center;
      padding: 0;
      box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
    }

    .button-holder {
      flex: 1;
      display: flex;
      flex-direction: column;
      justify-items: center;
      width: 90%;
      max-width: 400px;
      padding: 2rem;
    }

    .switch {
      background: var(--primary);
      color: white;
      border: none;
      border-radius: 5px;
      padding: 1rem;
      margin: 0.5rem;
      font-size: 2rem;
      cursor: pointer;
      transition: background 0.2s;
    }

    button:hover {
      background: #005ea3;
    }

    footer {
      text-align: center;
      padding: 1rem;
      font-size: 0.9rem;
      color: #666;
    }
  </style>
</head>

<body>
  <header>
    <h1>Light Switch Controls</h1>
  </header>

  <div class="button-holder">
    <button class="switch" id="toggleBtn">Toggle</button>
  </div>

  <footer>
    &copy; Trademarked by Tunnel Hermits cuz yea we made this and not GPT
  </footer>

  <!-- JavaScript -->
  <script>
    document.getElementById("toggleBtn").addEventListener("click", toggleLed);

    async function toggleLed() {
      try {
        await fetch("/toggle");
      } catch (err) {
        console.error("Error toggling LED:", err);
      }
    }

  </script>
</body>

</html>
)";

Servo myservo;
WebServer server(80);  // HTTP server on port 80

void handleRoot() {
    server.send(200, "text/html", index_html);
}

void handleServo() {
    if (server.hasArg("pos")) {
        int pos = server.arg("pos").toInt();
        if (pos >= 0 && pos <= 180) {
            myservo.write(pos);
            String message = "Moved servo to position: " + String(pos);
            server.send(200, "text/plain", message);
        } else {
            server.send(400, "text/plain", "Invalid angle. Use 0-180.");
        }
    } else {
        server.send(400, "text/plain", "Missing 'pos' parameter.");
    }
}

void handleToggle() {
    myservo.write(60);
    delay(400);
    myservo.write(140);
    delay(400);
    myservo.write(60);

    server.send(200, "text/plain", "yes");
}

void setup() {
    Serial.begin(115200);

    // Attach servo
    myservo.attach(SERVO_PIN);
    myservo.write(90);  // Center position

    // Connect to WiFi
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.print("ESP32 MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Register endpoints
    server.on("/", handleRoot);
    server.on("/toggle", handleToggle);
    server.on("/servo", handleServo);
    myservo.write(60);

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();  // Handle incoming HTTP requests
}