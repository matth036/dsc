#ifndef SEXAGESIMAL_H_
#define SEXAGESIMAL_H_

#include <string>
  
/** \defgroup custom_data_types Custom Data Types */

/** \addtogroup custom_data_types */
namespace sexagesimal {

  /**
   * @brief A concrete class holding base 60 numerical data.  
   *
   * Used for angles in degrees,arc minutes, and arc seconds.  
   * Also used for time in HH:MM:SS.sss format.
   **/
  class Sexagesimal {
  public:
    /**
     * @param hhh Hours. The sign of this value determines the sign of the created Sexagesimal.
     * @param mm  Minutes = Hours/60.
     * @param ss  Seconds = Minutes/60 = Hours/3600.
     * @param xxx Milliseconds = Seconds/1000.
     *
     * This class can be used to represent either time or angle data.
     */
    Sexagesimal(int32_t hhh, uint8_t mm, uint8_t ss, uint16_t xxx);
    /** Constructs a Sexagesimal with the specified value. */
    explicit Sexagesimal(double value);
    /** Default constructor creates a Sexagesimal with value zero. */
    Sexagesimal() {
      data.ui = 0x00000000;
    }
    /** Comparison operator.  */
    bool operator<(Sexagesimal other) {
      return data.i < other.data.i;
    }
    bool operator==(Sexagesimal other) {
      return data.i == other.data.i;
    }
    
    const uint32_t millis();   /*!< Returns the milliseconds internal data. */
    const uint32_t seconds();  /*!< Returns the seconds internal data. */
    const uint32_t minutes();  /*!< Returns the minutes internal data. */
    const uint32_t msb();      /*!< Returns hours of time or degrees of angle. */
    const double to_double();  /*!< Returns the value as a double. */
    const float to_float();    /*!< Returns the value as a float. */

    /** Conversion to a std::string in hh:mm:ss.xxx format. */
    const std::string to_string();
    /** Conversion to a std::string in hh:mm.xxxx format. Hours and minutes and decimal minutes. */
    const std::string to_hmxxxx_string();
    
    const std::string to_dddmmxxxx_string();
    /** Conversion to a std::string as hours minutes and seconds. */
    const std::string to_hms_string();
    /** Conversion to a std::string as hours minutes and seconds, with upper case H,M,S. */
    const std::string to_HMS_string();
    /** Conversion to a std::string representing degrees, minutes, and seconds. */
    const std::string to_dms_string();
    /** Conversion to a std::string suitable for expressing geographic latitude. */
    const std::string to_latitude_string();
    /** Conversion to a std::string suitable for expressing geographic longitude. */
    const std::string to_longitude_string();
    void setData(int32_t deg, uint32_t min, uint32_t sec, uint32_t xxx);
    void setData(double value);
    const uint32_t get_data();
    /** Returns the internal binary data.  */
    const uint32_t get_binary_data() {
      return data.ui;
    }
    /** Sets the internal binary data.  */
    void set_binary_data(uint32_t bin_value) {
      data.ui = bin_value;
    }

  private:
    union {
      int32_t i;
      uint32_t ui;
    } data;
  };

  /*! Non member function (but in the namespace.) */
  Sexagesimal double2sexagesimal(double d);
  template < typename N > Sexagesimal toSexagesimal(N realnum);

  /** 
   * Converts to sexagesimal::Sexagesimal.get_data() to double. 
   * Non member function (but in the namespace.) 
   **/
  double data_to_double(uint32_t data);

  /** This implements the to_string() function that seems to be missing
   * in my toolkit.  The only arguement types supported are uint32_t and int32_t.  */
  std::string to_string_hack(uint32_t);
  std::string to_string_hack(int32_t);

  typedef struct {
    Sexagesimal right_ascension;
    Sexagesimal declination;  // Naming this DEC caused conflict with macro DEC in Print.h
  } Equatorial_pair;

  typedef struct {
    Sexagesimal longitude;
    Sexagesimal latitude;
  } Long_Lat_pair;

  const Sexagesimal VOID_SEXAGESIMAL {
  -512, 59, 59, 999};

}  // namespace sexagesimal;
  
#endif  // SEXAGESIMAL_H_
  
