
#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 4
#define PWMPIN 16
#define LIGHTSENSEPIN 34
#define RELAYPIN 17

#define DHTTYPE DHT11 // DHT 11

struct State
{
	float temp;
	float pres;
	int humid;
	int light_lvl; // mapped 0 - 255
	int relay;	   // 0 or 1
	int fan;	   // 0 - 255
};

typedef struct State State_t;

int report_state_string(State_t *state, char *s, size_t cnt);
int handle_command(char *s);

State_t myState;
char s[100];
WebSocketsClient webSocket;
DHT_Unified dht(DHTPIN, DHTTYPE);

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

	switch (type)
	{
	case WStype_DISCONNECTED:
		Serial.printf("[WSc] Disconnected!\n");
		break;
	case WStype_CONNECTED:
		Serial.printf("[WSc] Connected to url: %s\n", payload);

		// send message to server when Connected
		webSocket.sendTXT("e:data:0:test:0");
		break;
	case WStype_TEXT:
		Serial.printf("[WSc] get text: %s\n", payload);
		handle_command((char *)payload);

		break;
	case WStype_BIN:
		Serial.printf("[WSc] get binary length: %u\n", length);
		// hexdump(payload, length);
		break;

	case WStype_ERROR:
	case WStype_FRAGMENT_TEXT_START:
	case WStype_FRAGMENT_BIN_START:
	case WStype_FRAGMENT:
	case WStype_FRAGMENT_FIN:
		break;
	}
}

void setup()
{
	Serial.begin(9600);
	Serial.setDebugOutput(true);
	Serial.println();

	for (uint8_t t = 2; t > 0; t--)
	{
		Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
		Serial.flush();
		delay(1000);
	}

	pinMode(PWMPIN, OUTPUT);
	pinMode(RELAYPIN, OUTPUT);
	digitalWrite(RELAYPIN, HIGH); // active low!
	dht.begin();

	WiFi.mode(WIFI_STA);
	WiFi.begin("David's Pixel 7a", "aoleu123");

	while (WiFi.status() != WL_CONNECTED)
		delay(1000);

	// server address, port and URL
	webSocket.begin("192.168.196.173", 8080, "/");
	Serial.println(WiFi.localIP());

	// event handler
	webSocket.onEvent(webSocketEvent);

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
}

void loop()
{
	webSocket.loop();

	if (millis() % 500 == 0)
	{
		sensors_event_t event;
		dht.temperature().getEvent(&event);
		myState.temp = event.temperature;

		dht.humidity().getEvent(&event);
		myState.humid = event.relative_humidity;

		myState.light_lvl = analogRead(LIGHTSENSEPIN);

		report_state_string(&myState, s, sizeof(s));
		webSocket.sendTXT(s);
	}
}

int report_state_string(State_t *state, char *s, size_t cnt)
{
	if (snprintf(s, cnt, "e:data:0:%.2f:%.2f:%d:%d:%d:%d", state->temp, state->pres, state->humid, state->light_lvl, state->relay, state->fan) != EOF)
		return 0;
	return -1;
}

int handle_command(char *s)
{
	char *tok = strtok(s, ":");
	if (tok && strcmp(tok, "w"))
		return -1;

	tok = strtok(NULL, ":");
	if (tok && strcmp(tok, "set"))
		return -1;

	tok = strtok(NULL, ":");
	// optionally check id
	// if(tok && strcmp(tok, "set"))
	// return -1;

	tok = strtok(NULL, ":"); // ce e
	switch (tok[0])
	{
	case 'r':
	{
		tok = strtok(NULL, ":");
		if (tok[0] == '0')
		{
			printf("Relay off \n");
			myState.relay = 0;
			digitalWrite(RELAYPIN, HIGH); // active low!
		}
		if (tok[0] == '1')
		{
			printf("Relay on \n");
			myState.relay = 1;
			digitalWrite(RELAYPIN, LOW); // active low!
		}
		break;
	}

	case 'f':
	{
		tok = strtok(NULL, ":");
		int fan_speed = atoi(tok);
		printf("Fan speed %d\n", fan_speed);
		myState.fan = fan_speed;
		analogWrite(PWMPIN, fan_speed);
		break;
	}

	default:
		printf("Unknown command %s\n", tok);
		return -1;
	}

	return 0;
}
