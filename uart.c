#include <stm32F10x.h>

/*----------------------------------------------------------------------------
  Initialize UART pins, Baudrate
 *----------------------------------------------------------------------------*/
void USART1_Init (void) {
  int i;

  RCC->APB2ENR |=  (   1UL <<  0);        /* enable clock Alternate Function  */
  AFIO->MAPR   &= ~(   1UL <<  2);        /* clear USART1 remap               */

  RCC->APB2ENR |=  (   1UL <<  2);        /* enable GPIOA clock               */
  GPIOA->CRH   &= ~(0xFFUL <<  4);        /* clear PA9, PA10                  */
  GPIOA->CRH   |=  (0x0BUL <<  4);        /* USART1 Tx (PA9) output push-pull */
  GPIOA->CRH   |=  (0x04UL <<  8);        /* USART1 Rx (PA10) input floating  */

  RCC->APB2ENR |=  (   1UL << 14);        /* enable USART#1 clock             */

  USART1->BRR   = 0x0271;                 /* 115200 baud @ PCLK2 72MHz        */
  USART1->CR1   = ((   1UL <<  2) |       /* enable RX                        */
                   (   1UL <<  3) |       /* enable TX                        */
                   (   0UL << 12) );      /* 1 start bit, 8 data bits         */
  USART1->CR2   = 0x0000;                 /* 1 stop bit                       */
  USART1->CR3   = 0x0000;                 /* no flow control                  */
  for (i = 0; i < 0x1000; i++) __NOP();   /* avoid unwanted output            */

  USART1->CR1  |= ((   1UL << 13) );      /* enable USART                     */
}


/*----------------------------------------------------------------------------
  SendChar
  Write character to Serial Port.
 *----------------------------------------------------------------------------*/
int SendChar (int ch)  {

  while (!(USART1->SR & USART_SR_TXE));
  USART1->DR = (ch & 0x1FF);

  return (ch);
}


/*----------------------------------------------------------------------------
  GetKey
  Read character to Serial Port.
 *----------------------------------------------------------------------------*/
int GetKey (void)  {

  while (!(USART1->SR & USART_SR_RXNE));

  return ((int)(USART1->DR & 0x1FF));
}
