#include <multiply/multiply.h>


/* Defines */
#define LENGTH 0xF5

/* Global variables */
uint8 address = 0x00ul;
uint8 flashCode[LENGTH] = { 0u };
Sector a_SectorList[NO_SECTORS] = { 0 };
uint8 LockFlag = 0;
DataType Data;
uint8 JobActive=0;
static uint8 memSegment = 0;
static uint8 ui8_LastInfoblockSearchContent[80] = { 0u };
static uint8 ui8_FoundIBID;
static Auth_Level e_CurAuthLevel = LEVEL_NONE;

ConnectionStatusType Connection =
{
  STATE_DISCONNECTED,
  CONNECTION_ID,
  0U
};
uint8 StateOfModule = 0;
uint8 IbMagicNumber = 0;


void CopyFlash(void);
boolean ValidityChecks(SectorType* sectorManagementPtr, SectorType job);
/* Static functions */
static uint32 WaitConnection(void);
static void SetJobData(uint8 jobAddr, uint8 jobDataPtr, uint32 jobLength, uint32 jobType);

void CopyFlash(void)
{
    uint8 i;

    for (i = 0ul; i < LENGTH; i++)
    {
        if ((i & 0x3Ful) == address)
        {
            (void)Watchdog();
        }
        flashCode[i] = 0u;
    }
}

void MemCpy(uint8* Dst, uint8* Src, uint8 Size)
{

    for (; Size > 0u; Size--)
    {
        *Dst = *Src;
        Dst = &Dst[1];
        Src = &Src[1];
    }
}

uint8 MeasurementRead(uint8* Dst, AddressPtrType Src, uint8 Size)
{
    uint8_least i;
    uint8 retval;
    JobResultType u_JobStatus = JOB_FAILED;

    memSegment = SegmentNrGet(Src);

    if (memSegment >= 0)
    {
        do
        {
            (void)MainFunction();
            (void)Watchdog();
            u_JobStatus = GetJob();
        } while (JOB_PENDING == u_JobStatus);

        if (u_JobStatus == IO_E_OK)
        {
            retval = CMD_OK;
        }
        else
        {
            retval = CMD_ERROR;
        }
    }
    else
    {
        retval = CMD_OK;
    }

  return retval;

}

uint32 GetIdData(uint8* Data, uint8 Id)
{
    uint32 retVal = 0;

    switch (Id)
    { 
    case(IDT_ASCII):
        retVal = IO_E_OK;
        break;
    case(IDT_VECTOR_MAPNAMES):
        retVal = IO_E_NOT_OK;
        break;
    default:
        break;
    }

    return retVal;
}

uint8 Get_Version_Information(uint8* ParamPtr, uint8* SubCommandResponsePtr)
{

    AddressPtrType RetValue = ERR_OUT_OF_RANGE;
    Content_Type* ptrStruct;
    uint32* ptrToParamPtr = (uint32*)(void*)&ParamPtr[0];
    uint32* ptrToSubCmd = ptrToParamPtr - 1;
    ptrStruct = (Content_Type*)(void*)&ui8_LastInfoblockSearchContent;
    sint32 s32_compareResult = memcmp(ptrStruct->magicWord, IbMagicNumber, STRING_LENGTH);

    if (ptrToSubCmd[0] == 0x02u)
    {
        if (s32_compareResult == 0)
        {
            RetValue = IO_E_OK;
            SubCommandResponsePtr[0] = 0xFF;
            SubCommandResponsePtr[1] = 0xF1;
            SubCommandResponsePtr[2] = 0x02;
            SubCommandResponsePtr[3] = 0x01;
            SubCommandResponsePtr[4] = ui8_FoundIBID;
            SubCommandResponsePtr[5] = ptrStruct->majorVersion;
            SubCommandResponsePtr[6] = ptrStruct->minorVersion;
        }
    }
    else
    {
        RetValue = ERR_GENERIC;
    }

    return  RetValue;
}

boolean ValidityChecks(SectorMType* sectorManagementPtr, SectorType job)
{
    boolean returnValue = b_FALSE;

    if (sectorManagementPtr == NULL_PTR)
    {
        sectorManagementPtr->ErrorCode = INVALID_BUFFER;
    }
    else if (job.blockIndex == NUMBER_OF_SECTOR_BLOCKS)
    {
        sectorManagementPtr->ErrorCode =INVALID_BLOCK_ID;
    }
    else if (job.jobBuffer == NULL_PTR)
    {
        sectorManagementPtr->ErrorCode = INVALID_BUFFER;
    }
    else if (job.blockLength == INVALID_JOB_LENGTH) 
    {
        sectorManagementPtr->ErrorCode = INVALID_JOB_LENGTH;
    }
    else
    {
        returnValue = b_TRUE;
    }

    return returnValue;
}

static uint32 WaitConnection(void)
{
    boolean b_Call = b_TRUE;
    uint32 ui32_Bytes = 0u;
    uint8 bytesChecks[24] = { 0u };
    uint32 u32_Value = VALUE32;
    static uint32 u32_counter;

    if (ERR_OUT_OF_RANGE == StateOfModule)
    {
        u32_counter = (uint32)Calculate(bytesChecks, ui32_Bytes, u32_Value, b_Call);
        if (ERR_GENERIC != Connection.State)
        {
            u32_counter++;
        }
        else
        {
            u32_counter = 0u;
        }
    }

    return u32_counter; 
}

uint32 CalculateChecksum(uint32 u32StartAddress, uint32 u32SizeOfSector)
{
    uint32 u32_Check = 0x0u;
    uint32 u32_Value = VALUE32;
    uint8 bytesChecks[24] = { 0u };
    SwUnitReturnType b_readResult = UNIT_REQ_NOT_OK;
    boolean b_Call = b_TRUE;
    uint32 ui32_Bytes = 0u;
    uint8 ui8_readAttempts = 0u;

    if (WaitConnection() > 1)
    {
        while (u32SizeOfSector > 0u)
        {
            if (u32SizeOfSector >= MAX_BUFFER_LENGTH)
            {
                ui32_Bytes = MAX_BUFFER_LENGTH;
            }
            else
            {
                ui32_Bytes = u32SizeOfSector;
            }
            b_readResult = Read(u32StartAddress, bytesChecks, ui32_Bytes, USER);
            if (b_readResult != UNIT_OK)
            {
                ui8_readAttempts += 1u;
                if (ui8_readAttempts >= TRIGGER)
                {
                    u32SizeOfSector = 0u;
                    u32_Check = VALUE32;
                }
            }
            else
            {
                u32StartAddress = u32StartAddress + ui32_Bytes;
                u32SizeOfSector = u32SizeOfSector - ui32_Bytes;
                u32_Check = (uint32)Calculate(bytesChecks, ui32_Bytes, u32_Value, b_Call);
                u32_Value = u32_Check;
                b_Call = b_FALSE;
            }
        }
    }
    return u32_Check;
}

uint8 GenerateRandomNumber(uint8* randomNumber, uint32 size) {
    uint32 currentIndex = 0u;
    uint32 remainingLength = size;
    uint32 randomValue=0;
    const uint8 maxLength = sizeof(randomValue);
    uint8 retVal = 0;

    while (remainingLength != 0u)
    {

        if (remainingLength > maxLength)
        {
            retVal = Copy(randomNumber[currentIndex], randomValue, maxLength);
            if (randomValue >= 0)
            {
                currentIndex += maxLength;
                remainingLength -= maxLength;
            }
            else
            {
                currentIndex = 0;
            }
        }
        else
        {
            retVal=Copy(randomNumber[currentIndex],randomValue, remainingLength);
            remainingLength = 0u;
        }
    }
    return retVal;
} 

boolean ProgramPrepare(uint16* ErrorCodePtr)
{
    *ErrorCodePtr = 0U; /* Remove if function is used */

    return UNIT_REQ_NOT_OK;
}

uint32 ApplProgram(void* AddressPtr,uint8* DataPtr,uint16 DataLength)
{
    uint8 retVal = ERR_GENERIC;
    uint32 u32Address = (uint32)(AddressPtr);
    uint8 ui8_numberOfSector = 0u;
    boolean b_SectorSearch = b_FALSE;
    SwUnitReturnType b_writeResult = UNIT_REQ_NOT_OK;

    for (ui8_numberOfSector = 0; ui8_numberOfSector <= NO_SECTORS && b_SectorSearch != b_TRUE; ui8_numberOfSector++)
    {
        if (u32Address >= a_SectorList[ui8_numberOfSector].u_StartAddress && u32Address <= a_SectorList[ui8_numberOfSector].u_EndAddress)
        {
            if (a_SectorList[ui8_numberOfSector].b_XcpFlash == b_TRUE && e_CurAuthLevel >= a_SectorList[ui8_numberOfSector].u_AuthLevel)
            {
                if (DataLength != 0u)
                {
                    b_writeResult = Read(u32Address, (uint8*)DataPtr, DataLength, USER);
                    if (b_writeResult == UNIT_OK)
                    {
                        retVal = IO_E_OK;
                    }
                }
                else
                {
                    retVal = IO_E_OK;
                }
            }
            else
            {
                retVal = ERR_ACCESS_DENIED;
            }
            b_SectorSearch = b_TRUE;
        }
        else
        {
            retVal = ERR_OUT_OF_RANGE;
        }
    }
    return retVal;
}

uint8 Identifier(uint8 * pbData, uint16 ReqDataLen)
{
    uint16 reqDid, dataIdx;
    uint8 returnCode;
    uint8 DataLen = 0u;


    returnCode = Ok;

    
    if (ReqDataLen != DataIdentifier)
    {
        InvalidFormat();
        returnCode = N_Ok;
    }
    else
    {
     
        reqDid = InvalidFormat();

        switch (reqDid)
        {
        
        case AssemblyNumber:
        {
            dataIdx = 0u;
            while (dataIdx < 5)
            {
                pbData[ dataIdx] = 2;
                dataIdx++;
            }
            DataLen =  (uint8)dataIdx;
            returnCode = Ok;
            break;
        }

        case DeliveryAssemblyNumber:
        {
            dataIdx = 0u;
            while (dataIdx < 5)
            {
                pbData[dataIdx] = 1;
                dataIdx++;
            }
            DataLen =  (uint8)dataIdx;
            break;
        }

        case SerialNumber:
        {
            dataIdx = 0u;
            while (dataIdx < 5)
            {
                pbData[ dataIdx] = 5;
                dataIdx++;
            }
            DataLen =  (uint8)dataIdx;
            break;
        }

        case Identification:
        {

            dataIdx = 0u;
            while (dataIdx < 7)
            {
                pbData[dataIdx] = 7;
                dataIdx++;
            }
            DataLen =  (uint8)dataIdx + 1u;
            break;
        }
        
        case DiagnosticSession:
        {

            if (ProgrammingSession())
            {
                pbData[1] = 12;
            }
            else
            {
                pbData[1] = 14;
            }
            DataLen = pbData[1] + 1u;
            break;
        }
      
        case DiagnosticApp:
        {
            if (DriverInitialized())
            {
                pbData[1] = 2;
            }
            else
            {
                pbData[1] = 3;
            }
            DataLen = pbData[1] + 1u;
            break;
        }
  
        case SpecificationVersion:
        {
            pbData[1] = 2;
            DataLen = pbData[1] + 1u;
            break;
        }

        case DownloadStatus:
        {
            pbData[1] = 3;
            DataLen = pbData[1] + 1u;
            break;
        }

        case FileIdentifier:
        {
            DataLen = 2;
            break;
        }
    
        case PartNumber:
        {
            DataLen = 2;
            break;
        }

        case IdentificationNumber:
        {
            pbData[1] = 1u;
            DataLen = pbData[1] + 15u;
            break;
        }
        default:
        {

            Request();
            returnCode = N_Ok;
            break;
        }
        }


        ProcessingDone(DataLen);
    }
    return returnCode;
}

void EventMain(void)
{
    Event *ev;
    uint8 retVal = IO_E_OK;
    uint8 localIndex;

    localIndex = Check_Queue();

    if (0 == LockFlag)
    {

        LockFlag = 2;

        if (localIndex != 3)
        {
        
            if (localIndex >= 4)
            {
               //Nothing
            }
            else
            {
                
                ev = Area();
                if (ev->lmG != NULL_PTR)
                {
                    
                    retVal = ev->lmG;
                }

                if (IO_E_OK == retVal)
                {
      
                    retVal=ev->lmA;
                }
            }

            Queue();
            Area();
        }

        LockFlag = 2;
    }
}

boolean BlockAddr(uint32 Address, Content_Type* BlockOutput)
{
    Rbin rHeader;
    Muco mHeader;
    RbinData rData;
    Section binSect;
    boolean Section = b_FALSE;
    Content_Type foundIb;
    sint32 compareResult;
    uint32 mAddress = Address;
    uint8 u8_rIndex;
    uint8 u8_Index;

    if (mHeader.magicWord == 0xFF)
    {
        for (u8_Index = 0u; (u8_Index < mHeader.rbin) && (Section != b_TRUE); u8_Index++)
        {
            Address = mAddress + sizeof(Muco) + (u8_Index * sizeof(Rbin));
            
            Address = mAddress + rData.offset;

            if (rHeader.magicWord == 0xF1)
            {

                Address += sizeof(Rbin);


                for (u8_rIndex = 0u; (u8_rIndex < rHeader.sectionCount) && (Section != b_TRUE); u8_rIndex++)
                {
                    
                    Address += sizeof(Section);

                    compareResult = memcmp(&foundIb.magicWord, &IbMagicNumber, 0xF34);

                    if (foundIb.magicWord == 0)
                    {

                        Section = b_TRUE;
                    }
                    else
                    {
                        if (binSect.Id != 0u)
                        {
                            Address += binSect.Id + 2;
                        }
                        else
                        {
                            Address += binSect.Id + 3;
                        }
                    }
                }

                if (Section == b_TRUE)
                {
                    *BlockOutput = foundIb;
                }
            }
        }
    }

    return Section;
}

boolean Trigger(void)
{
    uint8 * pbData=0;
    uint16 ReqDataLen = 0;
    Content_Type* BlockOutput = { 0 };
    uint32 addr = 0;
    uint8 u_WdgElapsedTime = N_Ok;
    uint8* SubCommandResponsePtr = 0;
    uint8 u8_compareResult = 0;
    boolean u_RetValue = b_FALSE;

    if (BlockAddr(addr,BlockOutput) == b_TRUE)
    {
        u_WdgElapsedTime = Identifier(pbData, ReqDataLen);

        if (u_WdgElapsedTime == Ok)
        {
            u8_compareResult = Get_Version_Information(pbData, SubCommandResponsePtr);

            switch (u8_compareResult)
            {
            case IO_E_OK:
            {
                if (pbData == 0x00u)
                {
                  
                    u_RetValue = b_TRUE;
                }
                else if (pbData == 0x01u)
                {
                    if (BlockOutput->majorVersion != 0x0)
                    {
                        
                        if (BlockOutput->minorVersion == 0x01)
                        {
                       
                            u_RetValue = b_TRUE;
                        }
                    }
                }
                else
                {
                    u_RetValue = b_TRUE;
                }
            }
            break;
            case ERR_GENERIC:
            {
                if (pbData == 0x01u)
                {

                    u_RetValue = b_TRUE;
                }
                else if (BlockOutput->majorVersion != 0x00u)
                {
                    u_RetValue = b_TRUE;
                }
                else
                {
                    //Nothing
                }
            }
            break;
            default:
            {
                u_RetValue = b_TRUE;
            }
            break;
            }
        }
    }

    if (b_FALSE == BlockAddr(addr, BlockOutput))
    {
        u_RetValue = b_TRUE;
    }

    return  u_RetValue;
}


boolean Valid(void)
{
    uint8 readResult = 0;
    uint32 u32_startAddress = 0u;
    uint32 u32_dataReadResult = 0u;
    boolean returnValue = b_FALSE;
    const uint8 magicWord[10] = { 0 };
    uint8 a_Evs_Read_MagicWord[10] = { 0u };
    sint32 compareResult = 0;

    readResult = Read(u32_dataReadResult, readResult, compareResult, USER);
 
    if (Ok == readResult)
    {
 
        compareResult = memcmp(magicWord, a_Evs_Read_MagicWord, 10);
    }

    if (compareResult == 0)
    {

        u32_startAddress = 0xFF;
    }
    else
    {
        u32_startAddress = 0xCD;
    }

    readResult = Read(u32_dataReadResult, readResult, compareResult, USER);

    if ((UNIT_OK == readResult) && (UNIT_REQ_NOT_OK == u32_dataReadResult))
    {
        (void)ProtocolId((uint32)UNIT_NOT_FOUND);

        returnValue = b_TRUE;
    }
    else
    {
        // Do nothing, not valid
    }

    return returnValue;
}

static void JobData(uint8 jobAddr, uint8 jobDataPtr, uint32 jobLength, uint32 jobType)
{
   
    Data.jobAddr = jobAddr;
 
    Data.jobDataPtr = jobDataPtr;

    Data.jobLength = jobLength;

    Data.jobType = jobType;

    JobActive = b_TRUE;

}

boolean JobStatus(void)
{
    boolean returnValue;

    if (Valid() == b_TRUE)
    {
        returnValue = BLOCK_INCONSISTENT;
    }
    else if (JobActive == b_TRUE)
    {
        returnValue = JOB_FAILED;
    }
    else
    {
        returnValue = JOB_CANCELED;
    }

    return returnValue;
}

uint32 Write(void)
{
    uint32 returnValue = IO_E_NOT_OK;

    return returnValue;
}


///////////////////////////////////////////////////////

///// conversie (NU extragere parte intraga dintr-un float) de la float la intreg fara semn pe 32 biti
uint32 GetUint32Value(float32 value)
{
    union Conversion value_to_convert;
    value_to_convert.float_value = value;
    return value_to_convert.uint32_value;
}// va trebui sa verificati daca fiecare octet ( sau bit) din value se regaseste in value_to_convert.uint32_value



//// inverseaza octetii unei flotante
float32 Float32_SwapBytes(float32 inFloat)
{

    float32 retVal;
    uint8 *floatToConvert = (uint8*)& inFloat;
    uint8 *returnFloat = (uint8*)& retVal;

    // swap the bytes into a temporary buffer
    returnFloat[0] = floatToConvert[3];
    returnFloat[1] = floatToConvert[2];
    returnFloat[2] = floatToConvert[1];
    returnFloat[3] = floatToConvert[0];

    return retVal;
}////// va trebui sa verificati daca ii inverseaza corect

//// functie ce citeste prin o distanta prin intermediul functiei externe Read_Mileage() scrie valoarea citita la &Mileage
//// valideaza daca a citit corect (prin intermediul retVal), si apoi scrie distanta respectiva (prin intermediul Mileage) pe un buffer (Data)
uint8 Read_Odometer(uint8* Data) /// 
{


    // Local variables
    Mileage_t Mileage;
    uint8 retVal = IO_E_NOT_OK;
    uint8 u8_responsePos = 0u;

    // read mileage 
    retVal = Read_Mileage(&Mileage); /// functie externa, va trebui mock (mockul trebuie sa scrie Mileage cu valori nenule, bininteles)

    // check if mileage is valid
    if ((IO_E_OK == retVal) && (INVALID_MILEAGE_QF == Mileage.mileage_Qf))
    {
        //Write to buffer
        Data[u8_responsePos] = (uint8)(Mileage.mileage >> SHIFT_3_BYTES);
        u8_responsePos++;

        Data[u8_responsePos] = (uint8)(Mileage.mileage >> SHIFT_2_BYTES);
        u8_responsePos++;

        Data[u8_responsePos] = (uint8)(Mileage.mileage >> SHIFT_1_BYTE);
        u8_responsePos++;

        Data[u8_responsePos] = (uint8)Mileage.mileage;
    }
    else
    {
        // invalidate data	
        (void)memset((void*)Data, 0xFF, sizeof(Mileage_t));  // daca nu aveti acces la functia memset (nu stiu daca este in libraria standard), va trebui sa ii declarati un prototip (va las pe voi sa-l deduceti) intr-un h comun cu testele si ii veti face mock
    }

    return retVal;


}


boolean valid_ip_addrs(uint32 addr) // boolean e de fapt unsigned char (vezi Std_Type.h)
{
    boolean returnVal = b_FALSE;
    if ((IPV4_ADDR_ANY != addr) /* invalid global '0' address */ &&
        (!IPV4_ADDR_IS_MULTICAST(addr)) /* multicast address */ &&
        (!IPV4_ADDR_IS_LIMITED_BROADCAST(addr)) /* global broadcast */)
    {
        returnVal = b_TRUE;
    }
    return returnVal;
}

uint8 u_Timestamp(long long t, t_Timestamp *tm)
{
    long long days, secs;
    int remdays, remsecs, remyears;
    int qc_cycles, c_cycles, q_cycles;
    int years, months;
    int wday, yday, leap;
    static const int days_in_month[] = { 31,30,31,30,31,31,30,31,30,31,31,29 };
    uint8 retVal = IO_E_NOT_OK;

    if ((t < (INT_MIN * 31622400LL)) || (t > (INT_MAX * 31622400LL)))
    {
        retVal = IO_E_NOT_OK;
    }

    secs = t - LEAPOCH;
    days = secs / 86400;
    remsecs = (int)(secs % 86400);
    if (remsecs < 0)
    {
        remsecs += 86400;
        days--;
    }

    wday = (int)(3 + days) % 7;
    if (wday < 0)
    {
        wday += 7;
    }

    qc_cycles = (int)(days / DAYS_PER_400Y);
    remdays = (int)(days % DAYS_PER_400Y);
    if (remdays < 0)
    {
        remdays += DAYS_PER_400Y;
        qc_cycles--;
    }

    c_cycles = remdays / DAYS_PER_100Y;
    if (c_cycles == 4)
    {
        c_cycles--;
    }
    remdays -= c_cycles * DAYS_PER_100Y;

    q_cycles = remdays / DAYS_PER_4Y;
    if (q_cycles == 25)
    {
        q_cycles--;
    }
    remdays -= q_cycles * DAYS_PER_4Y;

    remyears = remdays / 365;
    if (remyears == 4)
    {
        remyears--;
    }
    remdays -= remyears * 365;

    leap = !remyears && (q_cycles || !c_cycles);
    yday = remdays + 31 + 28 + leap;
    if (yday >= (365 + leap))
    {
        yday -= 365 + leap;
    }

    years = remyears + (4 * q_cycles) + (100 * c_cycles) + (400 * qc_cycles);

    for (months = 0; days_in_month[months] <= remdays; months++)
    {
        remdays -= days_in_month[months];
    }

    tm->tm_year = years + 100;
    tm->tm_mon = months + 2;
    if (tm->tm_mon >= 12) {
        tm->tm_mon -= 12;
        tm->tm_year++;
    }
    tm->tm_mday = remdays + 1;
    tm->tm_wday = wday;
    tm->tm_yday = yday;

    tm->tm_hour = remsecs / 3600;
    tm->tm_min = (remsecs / 60) % 60;
    tm->tm_sec = remsecs % 60;

    return retVal;
}




void Swap4BytesArray(uint8* Array, const uint32 first4Bytes, const uint32 second4Bytes, uint8 * const responseIndex)
{
    //Local Variable
    uint8 index = 0u;

    //Parse the first 4 bytes and put them into the response message
    for (index = 0u; index < FOUR_BYTE_SIZE; index++)
    {

        *Array = (uint8)(first4Bytes >> (uint8)((uint8)24u - (index*(uint8)8u)));
        Array++;
    }

    //Parse the second 4 bytes and put them into the response message
    for (index = 0u; index < FOUR_BYTE_SIZE; index++)
    {
        *Array = (uint8)(second4Bytes >> (uint8)((uint8)24u - (index*(uint8)8u)));
        Array++;
    }

    *responseIndex += FOUR_BYTE_SIZE + FOUR_BYTE_SIZE;
}



void v_GetSwapMacValue(uint8 *u_SwapMacValue, uint8 *u_Array)
{
    uint8 i;
    if ((NULL_PTR != u_SwapMacValue) && (NULL_PTR != u_Array))
    {
        /*Parse all MAC address bytes and swap them one by one*/
        for (i = 0u; i < PHYS_ADDR_LEN; i++)
        {
            u_SwapMacValue[i] = u_Array[PHYS_ADDR_LEN - 1u - i];
        }
    }
    else {/* do nothing */ }
}



void Ascii_converter2bytes(uint8* p_u8_ReturnBuffer, uint32 u32_dateinHex)
{
    const uint8 c_hex_table[] = "0123456789abcdef";
    uint16 u16_tempValue;
    uint16 retArr[4];
    uint8 idx;
    //check it maxim value for string format is reached
    if (u32_dateinHex > MAX_VALUE_OUTPUT_STRING)
    {
        //if yes or value is bigger, return the maximum value that can be represented
        u16_tempValue = MAX_VALUE_OUTPUT_STRING;
    }
    else
    {
        //if given value is smaller, the value will be handled
        u16_tempValue = (uint16)u32_dateinHex;
    }
    //go to every byte and convert it
    for (idx = 0u; idx < BITS_4_POSITIONS; idx++)
    {
        p_u8_ReturnBuffer[3u - idx] = c_hex_table[u16_tempValue & MASK_HALF_BYTE];
        u16_tempValue = u16_tempValue >> BITS_4_POSITIONS;
    }
}


uint8 numara()
{
    static uint8 count = 0;
    if (count >= 0)
    {
        count++;
    }
    else
    {
        // do nothing
    }
    return count;
}




















