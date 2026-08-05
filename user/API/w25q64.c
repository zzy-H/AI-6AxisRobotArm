#include "w25q64.h"

/**
 * @brief 读取W25Q64的ID
 * 设备ID: 0x4017 (W25Q64) 或 0x4018 (W25Q128)
 */
void W25Q64_ReadID_0x90(void)
{
    uint8_t Buff[2]={0};

    //1.把片选拉低
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);

    //2.发送0x90命令
    SPI_Send_Rec_Byte(0x90);//0x90命令，读取ID

    //3.依次发送24位地址的高8位中8位低8位，每个字节之间有1us的时钟周期
    SPI_Send_Rec_Byte(0x00);//高8位
    SPI_Send_Rec_Byte(0x00);//中8位
    SPI_Send_Rec_Byte(0x00);//低8位

    //4.接收ID字节
    Buff[0]=SPI_Send_Rec_Byte(0xFF);
	Buff[1]=SPI_Send_Rec_Byte(0xFF);

    //5.把片选拉高
    GPIO_SetBits(GPIOB, GPIO_Pin_12);

    //6.打印ID
    printf("0x90的ID返回值=%x\r\n",((Buff[0]<<8)+Buff[1]));
}

/**
 * @brief 读取W25Q64的JEDEC ID
 * 
 *  BYTE1: 制造商 ID (Winbond = 0xEF)
    BYTE2: 设备 ID 高位 (W25Q64 = 0x40)
    BYTE3: 设备 ID 低位 (W25Q64 = 0x17)
 */
void W25Q64_ReadID_0x9F(void)
{
    uint8_t Buff[3]={0};

    // 1. 片选拉低
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);

    // 2. 发送 0x9F 命令并读取 3 字节 ID
    SPI_Send_Rec_Byte(0x9F);	
    Buff[0] = SPI_Send_Rec_Byte(0xFF); 
    Buff[1] = SPI_Send_Rec_Byte(0xFF);
    Buff[2] = SPI_Send_Rec_Byte(0xFF);

    // 3. 片选拉高
    GPIO_SetBits(GPIOB, GPIO_Pin_12);

    // 4. 打印ID
    printf("0x9F的ID返回值=%x\r\n",((Buff[0]<<16)+(Buff[1]<<8)+Buff[2]));
}

/**
 * @brief 写入使能
 * 使能写入操作，允许写入数据到Flash内存
 */
void sFLASH_WriteEnable(void)
{
    // 1. 片选拉低
    sFLASH_CS_LOW();

    // 2. 发送写入使能命令
    sFLASH_SendByte(sFLASH_CMD_WREN);

    // 3. 片选拉高
    sFLASH_CS_HIGH();
}

/**
 * @brief 等待写入完成
 * 等待Flash内存写入操作完成，确保数据写入到正确位置
 */
void sFLASH_WaitForWriteEnd(void)
{
    uint8_t flashstatus = 0;//Flash状态寄存器值

    //1.拉低片选线
    sFLASH_CS_LOW();

    //2.发送读取状态寄存器命令 0x05
    sFLASH_SendByte(sFLASH_CMD_RDSR);

    do
    {
        //3.接收状态寄存器值
        flashstatus = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
    }while((flashstatus & sFLASH_WIP_FLAG) == SET);

    //4.把片选拉高
    sFLASH_CS_HIGH();
}

/**
 * @brief 写入页面
 * 写入指定页面的Flash内存，每个页面大小为256字节
 * @param pBuffer 要写入的数据缓冲区
 * @param WriteAddr 写入地址（24位）
 * @param NumByteToWrite 要写入的字节数（256字节以内）
 */
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    //1.使能写入
    sFLASH_WriteEnable();
    
    //2.拉低片选线
    sFLASH_CS_LOW();

    //3.发送写入页面命令 0x02
    sFLASH_SendByte(sFLASH_CMD_WRITE);

    //4.发送写入地址的高8位
    sFLASH_SendByte((WriteAddr & 0xFF0000) >> 16);//24位地址高八位

    //5.发送写入地址的中8位
    sFLASH_SendByte((WriteAddr & 0xFF00) >> 8);//24位地址中八位

    //6.发送写入地址的低8位
    sFLASH_SendByte(WriteAddr & 0xFF);//24位地址低八位

    while (NumByteToWrite--)
    {
        /*
         * 历史 bug 记录（已修复）：
         * 原代码是 "sFLASH_SendByte(*pBuffer++); pBuffer++;"，
         * *pBuffer++ 已经让指针 +1，后面又写了一次 pBuffer++，
         * 导致每写 1 字节就跳过 1 字节 → Flash 数据全部错乱
         * （表现：动作保存后读回 5682/10310 等乱值）。
         * 修复：只保留一次指针后移。
         */
        sFLASH_SendByte(*pBuffer);
        pBuffer++;
    }

    //9.片选线拉高
    sFLASH_CS_HIGH();

    //10.等待写入完成
    sFLASH_WaitForWriteEnd();
}

/**
 * @brief 写入缓冲区
 * 写入指定缓冲区的数据到Flash内存，支持跨页写入
 * @param pBuffer 要写入的数据缓冲区
 * @param WriteAddr 写入地址（24位）
 * @param NumByteToWrite 要写入的字节数（256字节以内）
 * */
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint8_t NumOfPage = 0;    // 需要写入的**整页数量**
    uint8_t NumOfSingle = 0;  // 最后不足一页的**剩余字节数**（零头）
    uint8_t Addr = 0;         // 写入地址在**当前页内的偏移量**（0~255）
    uint8_t count = 0;        // 从当前偏移写到页尾，还能写多少字节
    uint8_t temp = 0;         // 临时变量，用于计算跨页拆分后的字节数

    /* ====================== 核心计算 ====================== */
    // 1. 计算写入地址在当前页内的偏移（页大小 = 256 字节）
    Addr = WriteAddr % sFLASH_SPI_PAGESIZE;

    // 2. 计算：从当前偏移 写到 页尾，还能写多少字节
    count = sFLASH_SPI_PAGESIZE - Addr;

    // 3. 计算需要写入的整页数量
    NumOfPage = NumByteToWrite / sFLASH_SPI_PAGESIZE;

    // 4. 总字节数 写完整页后，剩余的零头字节
    NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

    /* ====================== 分支1：写入地址 刚好是页对齐 ====================== */
    if (Addr == 0)
    {
        //情况A：要写入的字节数 < 一页(256)，直接单页写入
        if (NumOfPage == 0)
        {
            sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
        }
        // 情况B：要写入的字节数 > 一页，先写所有整页，再写剩余零头
        else
        {
            // 循环写入每一个整页
            while (NumOfPage--)
            {
                sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
                WriteAddr += sFLASH_SPI_PAGESIZE; // 更新写入地址,地址向后挪一页
                pBuffer += sFLASH_SPI_PAGESIZE;    // 更新缓冲区指针,数据指针向后挪一页
            }

            // 写入剩余零头
            sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
        }
    }
    // ====================== 分支2：写入地址 不是页对齐（最常用） ======================
    else
    {
        // 情况A：要写入的字节数 < 一页(256)，且当前页剩余空间足够
        if (NumOfPage == 0)
        {
            // 当前页剩余空间足够写入所有数据
            if (NumByteToWrite <= count)
            {
                sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
            }
            // 当前页剩余空间不足，先写满当前页，再写剩余零头
            else
            {
                temp = NumByteToWrite - count; // 计算剩余零头字节数
                sFLASH_WritePage(pBuffer, WriteAddr, count); // 写满当前页
                WriteAddr += count; // 更新写入地址
                pBuffer += count;   // 更新缓冲区指针
                sFLASH_WritePage(pBuffer, WriteAddr, temp); // 写入剩余零头
            }
        }
        // 情况B：要写入的字节数 > 一页，先写满当前页，再循环写整页，最后写零头
        else
        {   // 先把当前页剩余空间填满->转换成页对齐的写入操作
            NumByteToWrite -= count;
            // 重新计算剩余数据的整页数、零头数
            NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE;
            NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

            // 第一步：写满当前页剩余空间
            sFLASH_WritePage(pBuffer, WriteAddr, count);
            WriteAddr +=  count;
            pBuffer += count;

            // 第二步：循环写入所有整页
            while (NumOfPage--)
            {
                sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
                WriteAddr +=  sFLASH_SPI_PAGESIZE;
                pBuffer += sFLASH_SPI_PAGESIZE;
            }

            // 第三步：如果还有剩余零头，写入最后一页
            if (NumOfSingle != 0)
            {
                sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

/**
 * @brief 从 Flash 读取数据（可跨页，无页边界限制）
 *
 * 读取不需要先擦除，也不受 256 字节页限制，一次可以读任意长度。
 *
 * @param pBuffer      接收数据的缓冲区指针
 * @param ReadAddr     读取起始地址（24 位，0~0xFFFFFF）
 * @param NumByteToRead 要读取的字节数
 */
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    /* 1. 片选拉低，选中 Flash */
    sFLASH_CS_LOW();

    /* 2. 发送读取命令 0x03 */
    sFLASH_SendByte(sFLASH_CMD_READ);

    /* 3. 发送 24 位读取地址（高 8 位 → 中 8 位 → 低 8 位） */
    sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
    sFLASH_SendByte((ReadAddr & 0xFF00) >> 8);
    sFLASH_SendByte(ReadAddr & 0xFF);

    /* 4. 连续读取数据（发哑元 0xA5 把数据顶回来） */
    while (NumByteToRead--)
    {
        *pBuffer = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
        pBuffer++;
    }

    /* 5. 片选拉高，结束通信 */
    sFLASH_CS_HIGH();
}

/**
 * @brief 擦除指定扇区（4KB）
 *
 * 重要：Flash 写入前必须先擦除！擦除后该扇区所有字节变为 0xFF。
 * 擦除是"1 变 0"的过程，如果已有数据，必须先擦除才能重新写入。
 *
 * @param SectorAddr 要擦除的扇区起始地址（4KB 对齐，如 0x000000、0x001000）
 */
void sFLASH_EraseSector(uint32_t SectorAddr)
{
    /* 1. 写使能（擦除/写入前必须先发 0x06 命令） */
    sFLASH_WriteEnable();

    /* 2. 片选拉低 */
    sFLASH_CS_LOW();

    /* 3. 发送扇区擦除命令 0x20 */
    sFLASH_SendByte(sFLASH_CMD_SE);

    /* 4. 发送 24 位扇区地址 */
    sFLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
    sFLASH_SendByte((SectorAddr & 0xFF00) >> 8);
    sFLASH_SendByte(SectorAddr & 0xFF);

    /* 5. 片选拉高 */
    sFLASH_CS_HIGH();

    /* 6. 等待擦除完成（擦除 4KB 大约需要 50~400ms） */
    sFLASH_WaitForWriteEnd();
}


