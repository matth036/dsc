#ifndef CHARACTER_RECIEVER_H_
#define CHARACTER_RECIEVER_H_

/*! @brief
 * Interface indicating that the class will recieve and respond to input characters.
 */

/**
 * Classes that implement this interface respond to manually input characters. 
 * This would be a Listener to a Stream in non-embedded environments.
 **/
/** \ingroup os */
class Character_Reciever {
 public:
  virtual void put_char(char c) = 0;
};

#endif  // CHARACTER_RECIEVER_H_
