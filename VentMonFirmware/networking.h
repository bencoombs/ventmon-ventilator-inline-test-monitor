#pragma once

void init();
void get_mac_addr();
void udp_init(bool using_wifi);
void ethernet_init();
String translateEncryptionType(wifi_auth_mode_t encryptionType);
void scanNetworks();
void wifi_init();
bool send_data_message(Message ma);
bool send_data_measurement(Measurement ma);