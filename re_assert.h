#ifndef _RE_ASSERT_H
#define _RE_ASSERT_H

/* 
 * On the microcontroller, during development, the function like macro:
 * assert_param( expr ) 
 * is usually defined by virtue of compile flag -DUSE_FULL_ASSERT
 * with the actual definition in stm32f4xx_conf.h
 * 
 * I want to keep assert_param( ) in place but allow for compilation
 * of certain files on the host machine for testing.
 * 
 * (#include)ing this file achieves that.
 */

inline void re_assert_failed(uint8_t* file, uint32_t line){for(;;){}}

#ifndef assert_param
#define assert_param(expr) ((expr) ? (void)0 : re_assert_failed((uint8_t *)__FILE__, __LINE__))
#endif 


#endif /*  _RE_ASSERT_H  */
