
#include <PIRDS.h>
#include <Ethernet.h>
#include <WiFi.h>
//#include <WiFiUdp.h>
#include <EthernetUdp.h>
#include <Dns.h>
#include "display.h"

// Set true to use wifi, false to use ethernet.
//bool using_wifi = false;

// NETWORKING

#define PARAM_HOST "ventmon.coslabs.com"
#define PARAM_PORT 6111 // Can these 2 variables be combined?
#define LOCAL_PORT 6111

byte mac[6];  // MAC address
char macs[18]; // Formatted MAC address

char *log_host = strdup(PARAM_HOST);
uint16_t log_port = PARAM_PORT;
IPAddress log_host_ip_addr;

// Ethernet
EthernetUDP udp_client;
bool ethernet_connected = false;

// WIFI
const char* ssid = "networkSSID";
const char* password = "networkPASS";
//WiFiUDP wifi_udp;

void get_mac_addr() {
  WiFi.macAddress(mac); // Get MAC address of wifi chip

  // Format MAC address
  snprintf(macs, sizeof macs, "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  Serial.print(F("MAC: "));
  Serial.print(macs);
  Serial.println();
}

void udp_init(bool using_wifi){
  Serial.println("Starting UDP...");
  
//  if (using_wifi){
//    wifi_udp.begin(LOCAL_PORT);

    // Currently not transmitting to the Data Lake via Wifi.
//  }
//  else {
    udp_client.begin(LOCAL_PORT);

    DNSClient dns_client;
    dns_client.begin(Ethernet.dnsServerIP());
    
    Serial.print("Serial: ");
    Serial.println(Ethernet.dnsServerIP());
    
    if (dns_client.getHostByName(log_host, log_host_ip_addr) == 1) {
      Serial.print("DNS host is ");
      Serial.println(log_host_ip_addr);
    }
    else {
      Serial.print("DNS host failed to resolve.");
    }
  //}
}

void ethernet_init() {
  Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  get_mac_addr();
  display_ethernet_starting();
  ethernet_connected = false;

  delay(2000);

  Serial.print(F("Connecting to to Ethernet, hardware status: "));
  Serial.println(Ethernet.hardwareStatus());
  
  // Check the Ethernet hardware is connected.
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println(F("WARNING! Ethernet shield was not found!")); // Why is this triggering?
  }

  // Check the Ethernet cable is connected
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println(F("WARNING! Ethernet cable is not connected!"));
  }
  else
  {
    // Start the Ethernet connection
    if (Ethernet.begin(mac) == 0) {
      // Ethernet failed to connect.
      Serial.print(F("WARNING! Ethernet failed to connect!"));
    } else {
      // Ethernet connected successfully.
      ethernet_connected = true;
      Serial.print(F("Ethernet connected!\nLocal IP: "));
      Serial.println(Ethernet.localIP());
    }
  }
  // OLED Display
  display_ethernet_connected(ethernet_connected);

  // Give time for user to view display and also let ethernet
  // initialize (if any is connected)
  delay(2000);
  
  if (!ethernet_connected)
    return;

  udp_init(false);
}

void networking_init(){
  //wifi_init()
  ethernet_init();  
}

// WIFI
// https://techtutorialsx.com/2017/06/29/esp32-arduino-getting-started-with-wifi/
/*
char* translateEncryptionType(wifi_auth_mode_t encryptionType) {

  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
  }
}

void scanNetworks() {

  int number_of_networks = WiFi.scanNetworks();

  Serial.print("Number of networks found: ");
  Serial.println(number_of_networks);

  for (int i = 0; i < number_of_networks; i++) {

    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));

    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));

    Serial.print("MAC address: ");
    Serial.println(WiFi.BSSIDstr(i));

    Serial.print("Encryption type: ");
    String encryption_type_description = translateEncryptionType(WiFi.encryptionType(i));
    Serial.println(encryption_type_description);
    Serial.println("-----------------------");

  }
}

void wifi_init() {
  WiFi.begin(ssid, password);

  int retries = 10;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    retries--;
    if (retries < 0) {
      Serial.println(F("ERROR! Failed to connect to Wifi!"));
      return;
    }
    Serial.println(F("Establishing connection to WiFi.."));
  }

  delay(1000);
  Serial.println(F("Connected to Wifi!"));

  get_mac_addr();
  Serial.println(WiFi.localIP());

  udp_init();
}
*/
bool send_data_message(Message ma) {

  if (!ethernet_connected){
    return false;
  }
  
  unsigned char m[264];

  fill_byte_buffer_message(&ma, m, 264);
  // I don't know how to compute this byte.
  m[263] = '\n';

    if (udp_client.beginPacket(log_host_ip_addr, log_port) != 1) {
      Serial.println("Ethernet send_data_message begin failed!");
      return false;
    }
    udp_client.write(m, 264);
    if (udp_client.endPacket() != 1) {
      Serial.println("Ethernet send_data_message end failed!");
      return false;
    }
  return true;
}

bool send_data_measurement(Measurement ma) {
  if (!ethernet_connected){
    return false;
  }
  
  unsigned char m[14];

  fill_byte_buffer_measurement(&ma, m, 13);

  m[13] = '\n';
    if (udp_client.beginPacket(log_host_ip_addr, log_port) != 1) {
      Serial.println("Ethernet send_data_measurement begin failed!");
      return false;
    }
    udp_client.write(m, 14);
    if (udp_client.endPacket() != 1) {
      Serial.println("Ethernet send_data_measurement end failed!");
      return false;
    }
  return true;
}
