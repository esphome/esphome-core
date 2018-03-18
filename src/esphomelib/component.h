//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_COMPONENT_H
#define ESPHOMELIB_COMPONENT_H

#include <functional>
#include <map>
#include <vector>

#define assert_is_pin(num) assert((0 <= (num) && (num) <= 39) && "Is not a valid pin number")
#define assert_positive(num) assert((num) >= 0)
#define assert_0_to_1(num) assert(0 <= (num) && (num) <= 1)
#define assert_nullptr(p) assert((p) == nullptr)
#define assert_not_nullptr(p) assert((p) != nullptr)
#define assert_setup(t) assert((t)->get_component_state() == esphomelib::Component::SETUP || (t)->get_component_state() == esphomelib::Component::LOOP)
#define assert_construction_state(t) assert((t)->get_component_state() == esphomelib::Component::CONSTRUCTION)
#define assert_setup_state(t) assert((t)->get_component_state() == esphomelib::Component::SETUP)
#define assert_loop_state(t) assert((t)->get_component_state() == esphomelib::Component::LOOP)

namespace esphomelib {

/// default setup priorities for components of different types.
namespace setup_priority {

const float HARDWARE = 100.0f; ///< for hardware initialization, such as GPIO
const float WIFI = 10.0f; ///< for WiFi initialization
const float MQTT_CLIENT = 7.5f; ///< for the MQTT client initialization
const float MQTT_COMPONENT = 5.0f; ///< for MQTT component initialization

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
  Component();

  enum ComponentState { CONSTRUCTION, SETUP, LOOP };

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

  /** Internal loop() function.
   *
   * Basically, it handles stuff like interval/timeout functions and eventually calls loop().
   */
  void loop_();
  void setup_();

  ComponentState get_component_state() const;

 protected:
  /** Simple typedef for interval/timeout functions
   *
   * @see set_interval()
   * @see set_timeout()
   */
  using time_func_t = std::function<void()>;

  /** Set an interval function with a unique name.
   *
   * This will call f every interval ms. Can be cancelled via CancelInterval().
   * Similar to javascript's setInterval().
   *
   * @param name The identifier for this interval function.
   * @param interval The interval in ms.
   * @param f The function (or lambda) that should be called
   *
   * @see cancel_interval()
   */
  void set_interval(const std::string &name, uint32_t interval, time_func_t f);

  /** Cancel an interval function.
   *
   * @param name The identifier for this interval function.
   * @return Whether an interval functions was deleted.
   */
  bool cancel_interval(const std::string &name);

  /** Set a timeout function with a unique name.
   *
   * Similar to javascript's setTimeout().
   *
   * @param name The identifier for this timeout function.
   * @param timeout The timeout in ms.
   * @param f The function (or lambda) that should be called
   *
   * @see cancel_timeout()
   */
  void set_timeout(const std::string &name, uint32_t timeout, time_func_t f);

  /** Cancel a timeout function.
   *
   * @param name The identifier for this timeout function.
   * @return Whether a timeout functions was deleted.
   */
  bool cancel_timeout(const std::string &name);

 protected:
  /// Internal struct for storing timeout/interval functions.
  struct TimeFunction {
    std::string name; ///< The name/id of this TimeFunction.
    enum Type { TIMEOUT, INTERVAL } type; ///< The type of this TimeFunction. Either TIMEOUT or INTERVAL.
    uint32_t interval; ///< The interval/timeout of this function.
    /// The last execution for interval functions and the time, SetInterval was called, for timeout functions.
    uint32_t last_execution;
    time_func_t f; ///< The function (or callback) itself.
  };

  bool cancel_time_function(const std::string &name, TimeFunction::Type type);

  /** Storage for interval/timeout functions.
   *
   * Intentionally a vector despite its map-like nature, because of the
   * memory overhead.
   */
  std::vector<TimeFunction> time_functions_;

  ComponentState component_state_;
};

} // namespace esphomelib

#endif //ESPHOMELIB_COMPONENT_H
