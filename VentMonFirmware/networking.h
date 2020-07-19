#ifndef NETWORKING_H
#define NETWORKING_H

void networking_init();
void get_mac_addr();
void udp_init(bool using_wifi);
void ethernet_init();
//char* translateEncryptionType(wifi_auth_mode_t encryptionType);
//void scanNetworks();
void wifi_init();
bool send_data_message(Message ma);
bool send_data_measurement(Measurement ma);

#endif
