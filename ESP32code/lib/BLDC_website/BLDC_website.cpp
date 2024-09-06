#include "BLDC_website.h"

// Replace with your network credentials


AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>BLDC engine driver</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    .container { text-align: center; }
    canvas { width: 80%; height: auto; }
    input[type=text], input[type=submit] { margin: 5px; }
  </style>
</head>
<body>
  <div class="container">
    <h2>BLDC engine driver</h2>
    <p>Voltage: <span id="voltage">0</span> mV</p>
    <p>Set RPM: <span id="setRPM">0</span></p>
    <p>Direction: <span id="direction">Forward</span></p>
    <p>Read RPM: <span id="readRPM">0</span></p>
    
    <h3>Enter Voltage and RPM:</h3>
    <form id="voltageForm">
      Voltage (0-4400 mV): <input type="text" name="voltage" id="voltageInput">
      <input type="submit" value="set Voltage">
    </form>
    <form id="rpmForm">
      RPM (0-60): <input type="text" name="rpm" id="rpmInput">
      <input type="submit" value="set RPM">
    </form>

    <button id="toggleDirectionBtn">Toggle Direction</button>

    <h3>Voltage Over Time</h3>
    <canvas id="voltageChart"></canvas>

    <h3>RPM Over Time</h3>
    <canvas id="rpmChart"></canvas>
  </div>

  <script>
    const voltageChartCtx = document.getElementById('voltageChart').getContext('2d');
    const rpmChartCtx = document.getElementById('rpmChart').getContext('2d');

    const voltageData = [];
    const rpmData = [];
    const labels = [];
    for (let i = 0; i < 900; i++) {
      labels.push('');
      voltageData.push(null);
      rpmData.push(null);
    }

    // Voltage chart setup
    const voltageChart = new Chart(voltageChartCtx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [{
          label: 'Voltage (mV)',
          borderColor: 'rgba(255, 99, 132, 1)',
          data: voltageData
        }]
      },
      options: {
        scales: {
          x: { display: false },
          y: { min: 0, max: 4400 }  // Y-axis constraint for voltage
        }
      }
    });

    // RPM chart setup
    const rpmChart = new Chart(rpmChartCtx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [{
          label: 'RPM',
          borderColor: 'rgba(54, 162, 235, 1)',
          data: rpmData
        }]
      },
      options: {
        scales: {
          x: { display: false },
          y: { min: 0, max: 65 }  // Y-axis constraint for RPM
        }
      }
    });

    // Function to update the webpage data
    function updatePage() {
      fetch('/getData')
        .then(response => response.json())
        .then(data => {
          document.getElementById('voltage').textContent = data.voltage;
          document.getElementById('setRPM').textContent = data.setRPM;
          document.getElementById('direction').textContent = data.direction;
          document.getElementById('readRPM').textContent = data.readRPM;

          // Shift and update chart data for voltage and rpm
          voltageData.push(data.voltage);
          voltageData.shift();
          rpmData.push(data.readRPM);
          rpmData.shift();

          voltageChart.update();
          rpmChart.update();
        });
    }

    // Set update interval
    setInterval(updatePage, 1000);

    // Handle Voltage form submission
    document.getElementById('voltageForm').addEventListener('submit', function (event) {
      event.preventDefault();
      const formData = new FormData(this);
      fetch('/submitVoltage', {
        method: 'POST',
        body: formData
      }).then(response => response.text()).then(text => {
        alert(text);
      });
    });

    // Handle RPM form submission
    document.getElementById('rpmForm').addEventListener('submit', function (event) {
      event.preventDefault();
      const formData = new FormData(this);
      fetch('/submitRPM', {
        method: 'POST',
        body: formData
      }).then(response => response.text()).then(text => {
        alert(text);
      });
    });

    // Handle Direction toggle button
    document.getElementById('toggleDirectionBtn').addEventListener('click', function () {
      fetch('/toggleDirection')
        .then(response => response.text())
        .then(text => {
          alert(text);
        });
    });
  </script>
</body>
</html>
)rawliteral";

bool isWifiConnected(){
    return WiFi.status() != WL_CONNECTED;
}

void initWIFI(String ssid, String password) {
    if (ssid.length() > 0 && password.length() > 0) {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    }
    else{
        Serial.print("No WiFi password detected \n");
        return;
    }
  // Connect to Wi-Fi
   WiFi.begin(ssid, password);
   int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    if (i++ > 5){
        Serial.println("Failed connecting to WiFi");
        return;
    }
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/getData", HTTP_GET, [](AsyncWebServerRequest *request){
     String json = "{\"voltage\":" + String(voltageDACS) + 
                  ",\"setRPM\":" + (turnEngineControlPID ? formatRPM(engineSetRPM) : "\"voltage control\"") + 
                  ",\"direction\":\"" + (engineDirection ? "↻" : "↺") + 
                  "\",\"readRPM\":" + formatRPM(engineReadRPM) + 
                  "}";
    request->send(200, "application/json", json);
  });
  // Endpoint to handle form submissions
 server.on("/submitRPM", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("rpm", true)) {
      int enteredRPM = request->getParam("rpm", true)->value().toInt();
      if (enteredRPM > ENGINE_MAX_RPM/ENGINE_TORQUE || enteredRPM < 0 ){
            request->send(200, "text/plain", "Enter correct RPM");
      }
      else {
          turnOnRegulationPID(enteredRPM * ENGINE_TORQUE);  // Only update the RPM
      }

    }

  });

  server.on("/submitVoltage", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("voltage", true)) {
      float enteredVoltage = request->getParam("voltage", true)->value().toFloat();
      if (enteredVoltage > DAC_MAX_VOLTAGE || enteredVoltage < 0 ){
         request->send(200, "text/plain", "Enter correct voltage");
      }
      else {
        voltageDACS = enteredVoltage;
        setCombinedDACOutput(enteredVoltage);  // Only update the Voltage
        turnOffRegulationPID();
      }

    }
  });

  server.on("/toggleDirection", HTTP_GET, [](AsyncWebServerRequest *request){
    changeDirection();
  });

  server.begin();
}

/* saving wifi credentials*/

void saveWiFiCredentials(const String &ssid, const String &password) {
  EEPROM.begin(EEPROM_SIZE);  // Initialize EEPROM

  // Save SSID to EEPROM
  for (int i = 0; i < ssid.length(); i++) {
    EEPROM.write(SSID_ADDR + i, ssid[i]);
  }
  EEPROM.write(SSID_ADDR + ssid.length(), '\0');  // Null-terminate SSID

  // Save Password to EEPROM
  for (int i = 0; i < password.length(); i++) {
    EEPROM.write(PASS_ADDR + i, password[i]);
  }
  EEPROM.write(PASS_ADDR + password.length(), '\0');  // Null-terminate Password

  EEPROM.commit();  // Commit changes to EEPROM
}

void readWiFiCredentials(String &ssid, String &password) {
  EEPROM.begin(EEPROM_SIZE);  // Initialize EEPROM

  // Read SSID from EEPROM
  char ch;
  ssid = "";
  for (int i = SSID_ADDR; i < SSID_ADDR + 256; i++) {
    ch = EEPROM.read(i);
    if (ch == '\0') break;  // Stop reading at null terminator
    ssid += ch;
  }

  // Read Password from EEPROM
  password = "";
  for (int i = PASS_ADDR; i < PASS_ADDR + 256; i++) {
    ch = EEPROM.read(i);
    if (ch == '\0') break;  // Stop reading at null terminator
    password += ch;
  }
}