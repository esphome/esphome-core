#ifndef ESPHOME_ETHERNET_COMPONENT_H
#define ESPHOME_ETHERNET_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_ETHERNET

#include "esphome/component.h"
#include "esphome/wifi_component.h"
#include "esp_eth.h"

ESPHOME_NAMESPACE_BEGIN

enum EthernetType {
  ETHERNET_TYPE_LAN8720 = 0,
  ETHERNET_TYPE_TLK110,
};

class EthernetComponent : public Component {
 public:
  EthernetComponent();
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;
  bool can_proceed() override;
  bool is_connected();

  void set_phy_addr(uint8_t phy_addr);
  void set_power_pin(const GPIOOutputPin &power_pin);
  void set_mdc_pin(uint8_t mdc_pin);
  void set_mdio_pin(uint8_t mdio_pin);
  void set_type(EthernetType type);
  void set_clk_mode(eth_clock_mode_t clk_mode);
  void set_manual_ip(ManualIP manual_ip);

  IPAddress get_ip_address();
  std::string get_use_address() const;
  void set_use_address(const std::string &use_address);

 protected:
  void on_wifi_event_(system_event_id_t event, system_event_info_t info);
  void start_connect_();
  void dump_connect_params_();

  static void eth_phy_config_gpio_();
  static void eth_phy_power_enable_(bool enable);

  std::string use_address_;
  uint8_t phy_addr_{0};
  GPIOPin *power_pin_{nullptr};
  uint8_t mdc_pin_{23};
  uint8_t mdio_pin_{18};
  EthernetType type_{ETHERNET_TYPE_LAN8720};
  eth_clock_mode_t clk_mode_{ETH_CLOCK_GPIO0_IN};
  optional<ManualIP> manual_ip_{};

  bool initialized_{false};
  bool connected_{false};
  bool last_connected_{false};
  uint32_t connect_begin_;
  eth_config_t eth_config;
  eth_phy_power_enable_func orig_power_enable_fun_;
};

extern EthernetComponent *global_eth_component;

ESPHOME_NAMESPACE_END

#endif  // USE_ETHERNET

#endif  // ESPHOME_ETHERNET_COMPONENT_H
