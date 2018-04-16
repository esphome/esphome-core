//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_COMPONENT_H
#define ESPHOMELIB_COMPONENT_H

#include <functional>
#include <map>
#include <vector>

#define assert_is_pin(num) assert((0 <= (num) && (num) <= 39) && "Is not a valid pin number")
#define assert_setup(t) assert((t)->get_component_state() == esphomelib::Component::SETUP || (t)->get_component_state() == esphomelib::Component::LOOP)
#define assert_construction_state(t) assert((t)->get_component_state() == esphomelib::Component::CONSTRUCTION)

namespace esphomelib {

/// default setup priorities for components of different types.
namespace setup_priority {

const float HARDWARE = 100.0f; ///< for hardware initialization, but only where it's really necessary (like outputs)
const float WIFI = 10.0f; ///< for WiFi initialization
const float MQTT_CLIENT = 7.5f; ///< for the MQTT client initialization
const float HARDWARE_LATE = 0.0f;
const float MQTT_COMPONENT = -5.0f; ///< for MQTT component initialization
const float LATE = -10.0f;

} // namespace setup_priority

/** The base class for all esphomelib components.
 *
 * esphomelib uses components to separate code for self-contained units such as
 * peripheral devices in order to keep the library clean and simple. Each component
 * should be registered in the Application instance via add_component. The application
 * will then call the component's setup() and loop() methods at the appropriate time.
 * Each component should save all the information required for setup in the constructor,
 * and only do the actual hardware initialization, such as pinMode, in setup().
 *
 * Additionally, the get_setup_priority() and get_loop_priority() can be overwritten
 * in order to force the setup() or loop() methods to be executed earlier than other
 * components. For example, setting up a GPIO pin to be output-only should be done before
 * WiFi and MQTT are initialized - so the component can assign itself a high priority via
 * get_setup_priority().
 *
 * @see Application::add_component()
 */
class Component {
 public:
  enum ComponentState {
    CONSTRUCTION = 0,
    SETUP = 1,
    LOOP = 2,
    FAILED = 3,
  };

  /** Where the component's initialization should happen.
   *
   * Analogous to Arduino's setup(). This method is guaranteed to only be called once.
   * Defaults to doing noting.
   */
  virtual void setup();

  /** This method will be called repeatedly.
   *
   * Analogous to Arduino's loop(). setup() is guaranteed to be called before this.
   * Defaults to doing nothing.
   */
  virtual void loop();

  /** priority of setup(). higher -> executed earlier
   *
   * Defaults to 0.
   *
   * @return The setup priority of this component
   */
  virtual float get_setup_priority() const;

  /** priority of loop(). higher -> executed earlier
   *
   * Defaults to 0.
   *
   * @return The loop priority of this component
   */
  virtual float get_loop_priority() const;

  /** Public loop() functions. These will be called by the Application instance.
   *
   * Note: This should normally not be overriden, unless you know what you're doing.
   * They're basically to make creating custom components easier. For example the
   * SensorComponent can override these methods to not have the user call some super
   * methods within their custom sensors. These methods should ALWAYS call the loop_internal()
   * and setup_internal() methods.
   *
   * Basically, it handles stuff like interval/timeout functions and eventually calls loop().
   */
  virtual void loop_();
  virtual void setup_();

  ComponentState get_component_state() const;

  /** Mark this component as failed. Any future timeouts/intervals/setup/loop will no longer be called.
   *
   * This might be useful if a component wants to indicate that a connection to its peripheral failed.
   * For example, i2c based components can check if the remote device is responding and otherwise
   * mark the component as failed. Eventually this will also enable smart status LEDs.
   */
  virtual void mark_failed();

 protected:
  void loop_internal();
  void setup_internal();

  /** Simple typedef for interval/timeout functions
   *
   * @see set_interval()
   * @see set_timeout()
   */
  using time_func_t = std::function<void()>;

  void set_interval(uint32_t interval, time_func_t &&f);

  /** Set an interval function with a unique name. Empty name means no cancelling possible.
   *
   * This will call f every interval ms. Can be cancelled via CancelInterval().
   * Similar to javascript's setInterval().
   *
   * IMPORTANT: Do not rely on this having correct timing. This is only called from
   * loop() and therefore can be significantly delay. If you need exact timing please
   * use hardware timers.
   *
   * @param name The identifier for this interval function.
   * @param interval The interval in ms.
   * @param f The function (or lambda) that should be called
   *
   * @see cancel_interval()
   */
  void set_interval(const std::string &name, uint32_t interval, time_func_t &&f);

  /** Cancel an interval function.
   *
   * @param name The identifier for this interval function.
   * @return Whether an interval functions was deleted.
   */
  bool cancel_interval(const std::string &name);

  void set_timeout(uint32_t timeout, time_func_t &&f);

  /** Set a timeout function with a unique name.
   *
   * Similar to javascript's setTimeout(). Empty name means no cancelling possible.
   *
   * IMPORTANT: Do not rely on this having correct timing. This is only called from
   * loop() and therefore can be significantly delay. If you need exact timing please
   * use hardware timers.
   *
   * @param name The identifier for this timeout function.
   * @param timeout The timeout in ms.
   * @param f The function (or lambda) that should be called
   *
   * @see cancel_timeout()
   */
  void set_timeout(const std::string &name, uint32_t timeout, time_func_t &&f);

  /** Cancel a timeout function.
   *
   * @param name The identifier for this timeout function.
   * @return Whether a timeout functions was deleted.
   */
  bool cancel_timeout(const std::string &name);

  void defer(const std::string &name, time_func_t &&f);

  void defer(time_func_t &&f);

  bool cancel_defer(const std::string &name);

  /// Internal struct for storing timeout/interval functions.
  struct TimeFunction {
    std::string name; ///< The name/id of this TimeFunction.
    enum Type { TIMEOUT, INTERVAL, DEFER } type; ///< The type of this TimeFunction. Either TIMEOUT, INTERVAL or DEFER.
    uint32_t interval; ///< The interval/timeout of this function.
    /// The last execution for interval functions and the time, SetInterval was called, for timeout functions.
    uint32_t last_execution;
    time_func_t f; ///< The function (or callback) itself.
    bool remove;

    bool should_run() const;
  };

  /// Cancel an only time function. If name is empty, won't do anything.
  bool cancel_time_function(const std::string &name, TimeFunction::Type type);

  /** Storage for interval/timeout functions.
   *
   * Intentionally a vector despite its map-like nature, because of the
   * memory overhead.
   */
  std::vector<TimeFunction> time_functions_;

  ComponentState component_state_{CONSTRUCTION}; ///< State of this component.
};

/** This class simplifies creating components that periodically check a state.
 *
 * You basically just need to implement the update() function, it will be called every update_interval ms
 * after startup. Note that this class cannot guarantee a correct timing, as it's not using timers, just
 * a software polling feature with set_interval() from Component.
 */
class PollingComponent : public Component {
 public:
  /** Initialize this polling component with the given update interval in ms.
   *
   * @param update_interval The update interval in ms.
   */
  explicit PollingComponent(uint32_t update_interval);

  /** Manually set the update interval in ms for this polling object.
   *
   * Override this if you want to do some validation for the update interval.
   *
   * @param update_interval The update interval in ms.
   */
  virtual void set_update_interval(uint32_t update_interval);

  // ========== OVERRIDE METHODS ==========
  // (You'll only need this when creating your own custom sensor)
  virtual void update() = 0;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup_() override;

  /// Get the update interval in ms of this sensor
  virtual uint32_t get_update_interval() const;
 protected:
  uint32_t update_interval_;
};

/// Helper class that enables naming of objects so that it doesn't have to be re-implement every single time.
class Nameable {
 public:
  explicit Nameable(const std::string &name);
  const std::string &get_name() const;
  void set_name(const std::string &name);
  const std::string &get_name_id();
 protected:
  std::string name_;
  std::string name_id_;
};

} // namespace esphomelib

#endif //ESPHOMELIB_COMPONENT_H
