#ifndef CHARACTER_RECIEVER_H
#define CHARACTER_RECIEVER_H

/* Classes that implement this interface respond to manually input characters. 
 * This would be a Listener to a Stream in non-embedded environments.
 **/
class Character_Reciever {
 public:
  virtual void put_char(char c) = 0;
};

#endif
