#include "wifi_editor.h"
#include "display.h"
#include "lunch_logic.h"
#include <WiFi.h>
#include <WebServer.h>

#define BUTTON_PIN 21
#define AP_SSID "LunchRemixer"

static WebServer server(80);

// HTML page served to the user's phone
static const char HTML_PAGE[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Lunch Remixer</title>
<style>
  * { box-sizing: border-box; font-family: -apple-system, sans-serif; }
  body { margin: 0; padding: 16px; background: #f5f5f5; }
  h1 { font-size: 22px; margin: 0 0 8px; }
  .cat { background: #fff; border-radius: 12px; padding: 16px; margin-bottom: 12px; box-shadow: 0 1px 3px rgba(0,0,0,0.1); }
  .cat-header { display: flex; align-items: center; gap: 8px; margin-bottom: 8px; }
  .cat-header input { font-size: 18px; font-weight: 600; border: none; border-bottom: 2px solid #e0e0e0; padding: 4px 0; flex: 1; }
  .cat-header input:focus { border-color: #333; outline: none; }
  .items textarea { width: 100%; min-height: 80px; border: 1px solid #e0e0e0; border-radius: 8px; padding: 8px; font-size: 14px; resize: vertical; }
  .hint { font-size: 12px; color: #888; margin-top: 2px; }
  button { padding: 10px 20px; border-radius: 8px; border: none; font-size: 16px; cursor: pointer; }
  .add { background: #e8e8e8; color: #333; margin-bottom: 12px; }
  .save { background: #222; color: #fff; width: 100%; font-size: 18px; padding: 14px; }
  .del { background: none; border: none; color: #c00; font-size: 20px; padding: 4px 8px; cursor: pointer; }
  .status { text-align: center; padding: 12px; margin-top: 8px; border-radius: 8px; display: none; }
  .ok { background: #d4edda; color: #155724; display: block; }
  .err { background: #f8d7da; color: #721c24; display: block; }
</style>
</head>
<body>
<h1>Lunch Remixer</h1>
<p style="color:#666;font-size:14px;">Edit categories and items below. One item per line.</p>
<div id="cats"></div>
<button class="add" onclick="addCat()">+ Add Category</button>
<br>
<button class="save" onclick="save()">Save to Device</button>
<div id="status" class="status"></div>

<script>
let data = {};

async function load() {
  const r = await fetch('/api/food');
  data = await r.json();
  render();
}

function render() {
  const el = document.getElementById('cats');
  el.innerHTML = '';
  (data.categories || []).forEach((cat, i) => {
    el.innerHTML += `
      <div class="cat">
        <div class="cat-header">
          <input value="${cat.name}" onchange="data.categories[${i}].name=this.value" placeholder="Category name">
          <span class="del" onclick="data.categories.splice(${i},1);render()">✕</span>
        </div>
        <textarea onchange="data.categories[${i}].items=this.value.split('\\n').map(s=>s.trim()).filter(s=>s)">${(cat.items||[]).join('\n')}</textarea>
        <div class="hint">One item per line</div>
      </div>`;
  });
}

function addCat() {
  if (!data.categories) data.categories = [];
  data.categories.push({name: '', items: []});
  render();
}

async function save() {
  const st = document.getElementById('status');
  st.className = 'status';
  st.style.display = 'none';
  try {
    const r = await fetch('/api/food', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify(data)
    });
    if (r.ok) { st.textContent = 'Saved!'; st.className = 'status ok'; }
    else { st.textContent = 'Save failed'; st.className = 'status err'; }
  } catch(e) { st.textContent = 'Connection error'; st.className = 'status err'; }
}

load();
</script>
</body>
</html>
)rawhtml";

static void handleRoot() {
    server.send(200, "text/html", HTML_PAGE);
}

static void handleGetFood() {
    String json = getFoodListRaw();
    server.send(200, "application/json", json);
}

static void handlePostFood() {
    String body = server.arg("plain");
    if (saveFoodList(body)) {
        server.send(200, "application/json", "{\"ok\":true}");
    } else {
        server.send(400, "application/json", "{\"ok\":false,\"error\":\"Invalid JSON\"}");
    }
}

void startWifiEditor() {
    // Start WiFi access point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID);
    delay(500);

    IPAddress ip = WiFi.softAPIP();
    char ipStr[16];
    snprintf(ipStr, sizeof(ipStr), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    // Show connection instructions on the e-ink display
    displayWifiMode(AP_SSID, ipStr);

    // Set up web server routes
    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/food", HTTP_GET, handleGetFood);
    server.on("/api/food", HTTP_POST, handlePostFood);
    server.begin();

    Serial.printf("WiFi AP started: %s at %s\n", AP_SSID, ipStr);

    // Serve until button press
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    while (digitalRead(BUTTON_PIN) == HIGH) {
        server.handleClient();
        delay(10);
    }

    // Clean up
    server.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);

    Serial.println("WiFi editor closed");
}
