#ifndef ESPHOME_COVER_TEMPLATE_COVER_H
#define ESPHOME_COVER_TEMPLATE_COVER_H

#include "esphome/defines.h"

#ifdef USE_TEMPLATE_COVER

#include "esphome/cover/cover.h"
#include "esphome/automation.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

enum class TemplateCoverRestoreMode {
  NO_RESTORE,
  RESTORE,
  RESTORE_AND_CALL,
};

class TemplateCover : public Cover, public Component {
 public:
  explicit TemplateCover(const std::string &name);

  void set_state_lambda(std::function<optional<float>()> &&f);
  Trigger<> *get_open_trigger() const;
  Trigger<> *get_close_trigger() const;
  Trigger<> *get_stop_trigger() const;
  Trigger<float> *get_position_trigger() const;
  Trigger<float> *get_tilt_trigger() const;
  void set_optimistic(bool optimistic);
  void set_assumed_state(bool assumed_state);
  void set_tilt_lambda(std::function<optional<float>()> &&tilt_f);
  void set_has_position(bool has_position);
  void set_has_tilt(bool has_tilt);

  void setup() override;
  void loop() override;
  void dump_config() override;

  float get_setup_priority() const override;

 protected:
  void control(const CoverCall &call) override;
  CoverTraits get_traits() override;
  void stop_prev_trigger_();

  TemplateCoverRestoreMode restore_mode_{TemplateCoverRestoreMode::RESTORE};
  optional<std::function<optional<float>()>> state_f_;
  optional<std::function<optional<float>()>> tilt_f_;
  bool assumed_state_{false};
  bool optimistic_{false};
  Trigger<> *open_trigger_;
  Trigger<> *close_trigger_;
  Trigger<> *stop_trigger_;
  Trigger<> *prev_command_trigger_{nullptr};
  Trigger<float> *position_trigger_;
  bool has_position_{false};
  Trigger<float> *tilt_trigger_;
  bool has_tilt_{false};
};

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_TEMPLATE_COVER

#endif  // ESPHOME_COVER_TEMPLATE_COVER_H
