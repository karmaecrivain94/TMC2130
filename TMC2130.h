// ----------------------------------------------------------------------------
// TMC2130.h
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------

#ifndef TMC2130_H
#define TMC2130_H
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "SPI.h"

#include "Streaming.h"


class TMC2130
{
public:
  void setup(const size_t cs_pin);
  void setup(const size_t cs_pin,
             const size_t enable_pin);

  void initialize();

  void enable();
  void disable();

  uint8_t getVersion();
  bool checkVersion();

  // valid values = 1,2,4,8,...128,256, other values get rounded down
  void setMicrostepsPerStep(const size_t microsteps_per_step);
  size_t getMicrostepsPerStep();

  void setRunCurrent(const uint8_t percent);
  void setHoldCurrent(const uint8_t percent);
  void setHoldDelay(const uint8_t percent);
  void setAllCurrentValues(const uint8_t run_current_percent,
                           const uint8_t hold_current_percent,
                           const uint8_t hold_delay_percent);

  struct Status
  {
    uint32_t load : 10;
    uint32_t space0 : 5;
    uint32_t full_step_active : 1;
    uint32_t current_scaling : 5;
    uint32_t space1 : 3;
    uint32_t stall : 1;
    uint32_t over_temperature_shutdown : 1;
    uint32_t over_temperature_warning : 1;
    uint32_t short_to_ground_a : 1;
    uint32_t short_to_ground_b : 1;
    uint32_t open_load_a : 1;
    uint32_t open_load_b : 1;
    uint32_t standstill : 1;
  };
  Status getStatus();

private:
  // SPISettings
  const static uint32_t SPI_CLOCK = 1000000;
  const static uint8_t SPI_BIT_ORDER = MSBFIRST;
  const static uint8_t SPI_MODE = SPI_MODE3;

  // Datagrams
  const static uint8_t DATAGRAM_SIZE = 5;

  // MOSI Datagram
  union MosiDatagram
  {
    struct Fields
    {
      uint64_t data : 32;
      uint64_t address : 7;
      uint64_t rw : 1;
      uint64_t space : 24;
    } fields;
    uint64_t uint64;
  };

  const static uint8_t RW_READ = 0;
  const static uint8_t RW_WRITE = 1;

  struct SpiStatus
  {
    uint8_t reset_flag : 1;
    uint8_t driver_error : 1;
    uint8_t stallguard : 1;
    uint8_t standstill : 1;
    uint8_t space : 4;
  };
  SpiStatus spi_status_;

  // MISO Datagram
  union MisoDatagram
  {
    struct Fields
    {
      uint64_t data : 32;
      SpiStatus spi_status;
      uint64_t space : 24;
    } fields;
    uint64_t uint64;
  };

  // General Configuration Registers
  const static uint8_t ADDRESS_GCONF = 0x00;
  union GlobalConfig
  {
    struct Fields
    {
      uint32_t i_scale_analog : 1;
      uint32_t internal_rsense : 1;
      uint32_t en_pwm_mode : 1;
      uint32_t enc_commutation : 1;
      uint32_t shaft : 1;
      uint32_t diag0_error : 1;
      uint32_t diag0_otpw : 1;
      uint32_t diag0_stall : 1;
      uint32_t diag1_index : 1;
      uint32_t diag1_onstate : 1;
      uint32_t diag1_steps_skipped : 1;
      uint32_t diag1_int_pushpull : 1;
      uint32_t diag1_pushpull : 1;
      uint32_t small_hysteresis : 1;
      uint32_t stop_enable : 1;
      uint32_t direct_mode : 1;
      uint32_t test_mode : 1;
      uint32_t space : 15;
    } fields;
    uint32_t uint32;
  };
  GlobalConfig global_config_;

  const static uint8_t ADDRESS_GSTAT = 0x01;
  union GlobalStatus
  {
    struct Fields
    {
      uint32_t reset : 1;
      uint32_t drv_err : 1;
      uint32_t uv_cp : 1;
      uint32_t space : 29;
    } fields;
    uint32_t uint32;
  };

  const static uint8_t ADDRESS_IOIN = 0x04;
  union InputPinStatus
  {
    struct Fields
    {
      uint32_t step : 1;
      uint32_t dir : 1;
      uint32_t dcen_cfg4 : 1;
      uint32_t dcin_cfg5 : 1;
      uint32_t drv_enn_cfg6 : 1;
      uint32_t dco : 1;
      uint32_t one : 1;
      uint32_t dont_care : 1;
      uint32_t space : 16;
      uint32_t version : 8;
    } fields;
    uint32_t uint32;
  };
  const static uint8_t VERSION = 0x11;


  // Velocity Dependent Driver Feature Control Register Set
  const static uint8_t ADDRESS_IHOLD_IRUN = 0x10;
  union DriverCurrent
  {
    struct Fields
    {
      uint32_t ihold : 5;
      uint32_t irun : 5;
      uint32_t iholddelay : 4;
      uint32_t space : 18;
    } fields;
    uint32_t uint32;
  };
  const static uint8_t PERCENT_MIN = 0;
  const static uint8_t PERCENT_MAX = 100;
  const static uint8_t CURRENT_SETTING_MIN = 0;
  const static uint8_t CURRENT_SETTING_MAX = 31;
  const static uint8_t HOLD_DELAY_MIN = 0;
  const static uint8_t HOLD_DELAY_MAX = 15;
  DriverCurrent driver_current_;

  const static uint8_t ADDRESS_TPOWERDOWN = 0x11;
  union PowerDownDelay
  {
    struct Fields
    {
      uint32_t value : 8;
      uint32_t space : 24;
    } fields;
    uint32_t uint32;
  };

  const static uint8_t ADDRESS_TSTEP = 0x12;

  const static uint8_t ADDRESS_TPWMTHRS = 0x13;
  const static uint32_t TPWMTHRS_DEFAULT = 500;

  const static uint8_t ADDRESS_TCOOLTHRS = 0x14;

  const static uint8_t ADDRESS_THIGH = 0x15;

  // SPI Mode Register
  const static uint8_t ADDRESS_XDIRECT = 0x2D;

  // dcStep Minimum Velocity Register
  const static uint8_t ADDRESS_VDCMIN = 0x33;

  // Motor Driver Registers

  // Microstepping Control Register Set
  const static uint8_t ADDRESS_MSLUT_0 = 0x60;
  const static uint8_t ADDRESS_MSLUT_1 = 0x61;
  const static uint8_t ADDRESS_MSLUT_2 = 0x62;
  const static uint8_t ADDRESS_MSLUT_3 = 0x63;
  const static uint8_t ADDRESS_MSLUT_4 = 0x64;
  const static uint8_t ADDRESS_MSLUT_5 = 0x65;
  const static uint8_t ADDRESS_MSLUT_6 = 0x66;
  const static uint8_t ADDRESS_MSLUT_7 = 0x67;
  const static uint8_t ADDRESS_MSLUTSEL = 0x68;
  const static uint8_t ADDRESS_MSLUTSTART = 0x69;
  const static uint8_t ADDRESS_MSCNT = 0x6A;
  const static uint8_t ADDRESS_MSCURACT = 0x6B;

  // Driver Register Set
  const static uint8_t ADDRESS_CHOPCONF = 0x6C;
  union ChopperConfig
  {
    struct Fields
    {
      uint32_t toff : 4;
      uint32_t hstrt : 3;
      uint32_t hend : 4;
      uint32_t fd3 : 1;
      uint32_t disfdcc : 1;
      uint32_t rndtf : 1;
      uint32_t chm : 1;
      uint32_t tbl : 2;
      uint32_t vsense : 1;
      uint32_t vhighfs : 1;
      uint32_t vhighchm : 1;
      uint32_t sync : 4;
      uint32_t mres : 4;
      uint32_t intpol : 1;
      uint32_t dedge : 1;
      uint32_t diss2g : 1;
      uint32_t space : 1;
    } fields;
    uint32_t uint32;
  };
  const static uint8_t MRES_256 = 0b0000;
  const static uint8_t MRES_128 = 0b0001;
  const static uint8_t MRES_064 = 0b0010;
  const static uint8_t MRES_032 = 0b0011;
  const static uint8_t MRES_016 = 0b0100;
  const static uint8_t MRES_008 = 0b0101;
  const static uint8_t MRES_004 = 0b0110;
  const static uint8_t MRES_002 = 0b0111;
  const static uint8_t MRES_001 = 0b1000;
  const static uint8_t TOFF_DEFAULT = 0b0011; // Nclk = 108
  const static uint8_t HSTRT_DEFAULT = 0b100; // 4
  const static uint8_t HEND_DEFAULT = 0b0001; // 1
  const static uint8_t CHM_DEFAULT = 0b0; // standard
  const static uint8_t TBL_DEFAULT = 0b10; // 36 clocks
  ChopperConfig chopper_config_;

  const static uint8_t ADDRESS_COOLCONF = 0x6D;
  const static uint8_t ADDRESS_DCCTRL = 0x6E;
  const static uint8_t ADDRESS_DRV_STATUS = 0x6F;
  union DriveStatus
  {
    struct Fields
    {
      Status status;
    } fields;
    uint32_t uint32;
  };
  const static uint8_t ADDRESS_PWMCONF = 0x70;
  union PwmConfig
  {
    struct Fields
    {
      uint32_t pwm_ampl : 8;
      uint32_t pwm_grad : 8;
      uint32_t pwm_freq : 2;
      uint32_t pwm_autoscale : 1;
      uint32_t pwm_symmetric : 1;
      uint32_t freewheel : 2;
      uint32_t space : 10;
    } fields;
    uint32_t uint32;
  };
  const static uint8_t PWM_AMPL_DEFAULT = 200;
  const static uint8_t PWM_GRAD_DEFAULT = 1;
  const static uint8_t PWM_FREQ_DEFAULT = 0b00; // 2/1024 fclk
  const static uint8_t PWM_AUTOSCALE_DEFAULT = 1;
  PwmConfig pwm_config_;

  const static uint8_t ADDRESS_PWM_SCALE = 0x71;
  const static uint8_t ADDRESS_ENCM_CTRL = 0x72;
  const static uint8_t ADDRESS_LOST_STEPS = 0x73;

  size_t cs_pin_;
  int enable_pin_;

  const static size_t MICROSTEPS_PER_STEP_MIN = 1;
  const static size_t MICROSTEPS_PER_STEP_MAX = 256;
  const static uint8_t MICROSTEPS_PER_STEP_EXPONENT_MAX = 8;
  uint8_t microsteps_per_step_exponent_;

  void setEnablePin(const size_t enable_pin);

  // void setStepDirInput();
  // void setSpiInput();

  void enableAnalogInputCurrentScaling();
  void disableAnalogInputCurrentScaling();
  void enableStealthChop();
  void disableStealthChop();
  void enableInverseMotorDirection();
  void disableInverseMotorDirection();

  // microsteps = 2^exponent, 0=1,1=2,2=4,...8=256
  void setMicrostepsPerStepPowerOfTwo(const uint8_t exponent);

  uint32_t sendReceivePrevious(MosiDatagram & mosi_datagram);
  uint32_t write(const uint8_t address,
                     const uint32_t data);
  uint32_t read(const uint8_t address);

  uint8_t percentToCurrentSetting(const uint8_t percent);
  uint8_t percentToHoldDelaySetting(const uint8_t percent);

  void setGlobalConfig();
  void setDriverCurrent();
  void setChopperConfig();
  void setPwmThreshold(const uint32_t value);
  void setPwmConfig();

};

#endif
