#include "uart_dma.h"
#include "string.h"
/*
这里是串口1配置为空闲中断方式接收的bsp文件
此为使用示例
    初始化
        USARTx_CFG(115200);
        DMA_INIT();
    数据调用
        if (ring_buffer.RemainCount > 0)
        {
            printf("recv %d >>>\n", ring_buffer.RemainCount);
            while (ring_buffer.RemainCount > 0)
            {
                printf("%c", ring_buffer_pop());
            }
            printf("\n<<<\n");
        }
*/




void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

// ring buffer size




#define USART_RX_CH         DMA1_Channel5

// 在文件顶部或全局变量区域添加
uint8_t g_rx_data_buffer[RX_BUFFER_LEN];  // 全局接收数据数组
volatile uint8_t g_rx_data_ready = 0;     // 数据就绪标志位
volatile uint16_t g_rx_data_len = 0;      // 接收到的数据长度

struct
{
    volatile uint8_t DMA_USE_BUFFER;
    uint8_t          Rx_Buffer[2][RX_BUFFER_LEN];

} USART_DMA_CTRL = {
    .DMA_USE_BUFFER = 0,
    .Rx_Buffer      = {0},
};

// 定义结构体类型


// 定义全局变量
RingBuffer ring_buffer = {{0}, 0, 0, 0};

/*********************************************************************
 * @fn      USARTx_CFG
 *
 * @brief   Initializes the USART1 peripheral.
 *
 * @return  none
 */
void USARTx_CFG(uint32_t baudrate)
{
    GPIO_InitTypeDef  GPIO_InitStructure  = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure  = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB| RCC_APB2Periph_USART1, ENABLE);

    /* USART1 TX-->A.9   RX-->A.10 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate            = baudrate;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

/*********************************************************************
 * @fn      DMA_INIT
 *
 * @brief   Configures the DMA for USART1.
 *
 * @return  none
 */
void DMA_INIT(void)
{
    DMA_InitTypeDef  DMA_InitStructure  = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(USART_RX_CH);
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;

    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)USART_DMA_CTRL.Rx_Buffer[0];
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = RX_BUFFER_LEN;
    DMA_Init(USART_RX_CH, &DMA_InitStructure);

    DMA_ITConfig(USART_RX_CH, DMA_IT_TC, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_Cmd(USART_RX_CH, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
}


/*********************************************************************
 * @fn      ring_buffer_push_huge
 *
 * @brief   Put a large amount of data into the ring buffer.
 *
 * @return  none
 */
void ring_buffer_push_huge(uint8_t *buffer, uint16_t len)
{
    const uint16_t bufferRemainCount = RING_BUFFER_LEN - ring_buffer.RemainCount;
    if (bufferRemainCount < len)
    {
        len = bufferRemainCount;
    }

    const uint16_t bufferSize = RING_BUFFER_LEN - ring_buffer.RecvPos;
    if (bufferSize >= len)
    {
        memcpy(&(ring_buffer.buffer[ring_buffer.RecvPos]), buffer, len);
        ring_buffer.RecvPos += len;
    }
    else
    {
        uint16_t otherSize = len - bufferSize;
        memcpy(&(ring_buffer.buffer[ring_buffer.RecvPos]), buffer, bufferSize);
        memcpy(ring_buffer.buffer, &(buffer[bufferSize]), otherSize);
        ring_buffer.RecvPos = otherSize;
    }
    ring_buffer.RemainCount += len;
}

/*********************************************************************
 * @fn      ring_buffer_pop
 *
 * @brief   Get a data from the ring buffer.
 *
 * @return  the Data
 */
uint8_t ring_buffer_pop()
{
    uint8_t data = ring_buffer.buffer[ring_buffer.SendPos];

    ring_buffer.SendPos++;
    if (ring_buffer.SendPos >= RING_BUFFER_LEN)
    {
        ring_buffer.SendPos = 0;
    }
    ring_buffer.RemainCount--;
    return data;
}

/*********************************************************************
 * @fn      USART1_IRQHandler
 *
 * @brief   This function handles USART1 global interrupt request.
 *
 * @return  none
 */
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        // IDLE
        uint16_t rxlen     = (RX_BUFFER_LEN - USART_RX_CH->CNTR);
        uint8_t  oldbuffer = USART_DMA_CTRL.DMA_USE_BUFFER;

        USART_DMA_CTRL.DMA_USE_BUFFER = !oldbuffer;

        DMA_Cmd(USART_RX_CH, DISABLE);
        DMA_SetCurrDataCounter(USART_RX_CH, RX_BUFFER_LEN);
        // Switch buffer
        USART_RX_CH->MADDR = (uint32_t)(USART_DMA_CTRL.Rx_Buffer[USART_DMA_CTRL.DMA_USE_BUFFER]);
        DMA_Cmd(USART_RX_CH, ENABLE);

        USART_ReceiveData(USART1); // clear IDLE flag
        ring_buffer_push_huge(USART_DMA_CTRL.Rx_Buffer[oldbuffer], rxlen);
    }
}

/*********************************************************************
 * @fn      DMA1_Channel5_IRQHandler
 *
 * @brief   This function handles DMA1 Channel 5 global interrupt request.
 *
 * @return  none
 */
void DMA1_Channel5_IRQHandler(void)
{
    uint16_t rxlen     = RX_BUFFER_LEN;
    uint8_t  oldbuffer = USART_DMA_CTRL.DMA_USE_BUFFER;
    // FULL

    USART_DMA_CTRL.DMA_USE_BUFFER = !oldbuffer;

    DMA_Cmd(USART_RX_CH, DISABLE);
    DMA_SetCurrDataCounter(USART_RX_CH, RX_BUFFER_LEN);
    // Switch buffer
    USART_RX_CH->MADDR = (uint32_t)(USART_DMA_CTRL.Rx_Buffer[USART_DMA_CTRL.DMA_USE_BUFFER]);
    DMA_Cmd(USART_RX_CH, ENABLE);

    ring_buffer_push_huge(USART_DMA_CTRL.Rx_Buffer[oldbuffer], rxlen);

    DMA_ClearITPendingBit(DMA1_IT_TC5);
}

/*********************************************************************  
 * @fn      DMA1_Channel5_IRQHandler  
 *  
 * @brief   This function handles DMA1 Channel 5 global interrupt request.  
 *  
 * @return  none  
 */ 
// void DMA1_Channel5_IRQHandler(void) 
// {
//     uint16_t rxlen     = RX_BUFFER_LEN;
//     uint8_t  oldbuffer = USART_DMA_CTRL.DMA_USE_BUFFER;
//     // FULL 
    
//     USART_DMA_CTRL.DMA_USE_BUFFER = !oldbuffer;
    
//     DMA_Cmd(USART_RX_CH, DISABLE);
//     DMA_SetCurrDataCounter(USART_RX_CH, RX_BUFFER_LEN);
//     // Switch buffer
//     USART_RX_CH->MADDR = (uint32_t)(USART_DMA_CTRL.Rx_Buffer[USART_DMA_CTRL.DMA_USE_BUFFER]);
//     DMA_Cmd(USART_RX_CH, ENABLE);
    
//     // 新增：将数据拷贝到全局数组并置位标志
//     memcpy(g_rx_data_buffer, USART_DMA_CTRL.Rx_Buffer[oldbuffer], rxlen);
//     g_rx_data_len = rxlen;
//     g_rx_data_ready = 1;  // 置位数据就绪标志
//     printf("len %d\r\n", g_rx_data_len);
//     // 如果仍需要环形缓冲区功能，保留这行；否则可以注释掉
//     //ring_buffer_push_huge(USART_DMA_CTRL.Rx_Buffer[oldbuffer], rxlen);
    
//     DMA_ClearITPendingBit(DMA1_IT_TC5);
// }


/*********************************************************************
 * @fn      UART_SendBytes
 * @brief   多字节发送函数（阻塞模式，带超时）
 * @param   USARTx: UART外设指针 (USART1, USART2等)
 * @param   pData: 要发送的数据指针
 * @param   Size: 要发送的字节数
 * @param   Timeout: 超时时间（毫秒，0表示无限等待）
 * @return  发送状态
 */
UART_Status_t UART_SendBytes(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    //uint32_t tickstart = 0;
    uint16_t i = 0;
    
    // 参数检查
    if (pData == NULL || Size == 0) {
        return UART_ERROR;
    }
    
    // 记录开始时间
    // if (Timeout > 0) {
    //     tickstart = UART_GetTick();
    // }
    
    // 逐字节发送
    for (i = 0; i < Size; i++) {
        
        // 等待发送数据寄存器空（TXE标志）
        while (!(USARTx->STATR & USART_FLAG_TXE)) {
            // 检查超时
            if (Timeout > 0) {

            }
        }
        
        // 发送单个字节
        USART_SendData(USARTx, pData[i]);
    }
    
    // 等待最后一个字节发送完成（TC标志）
    while (!(USARTx->STATR & USART_FLAG_TC)) {

        }
    
    
    return UART_OK;
}