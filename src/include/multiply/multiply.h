
#ifndef MULTIPLY_H
#define MULTIPLY_H 

#include "Std_Type.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define NULL 0
#define CMD_OK                            (0x00u)
#define CMD_ERROR                         (0xFFu)
#define IO_E_OK         0x00u             
#define IO_E_NOT_OK     0x01u
#define IDT_ASCII                         (0x00u)
#define IDT_VECTOR_MAPNAMES               (0xDBu)
#define ERR_OUT_OF_RANGE       0x22U
#define ERR_GENERIC            0x31U
#define ERR_ACCESS_DENIED            0x32U
#define NO_SECTORS (15u)
#define STRING_LENGTH    0x07u
#define b_FALSE ((boolean)0)
#define b_TRUE ((boolean)1)
#define NULL_PTR ((void *)0)
#define NUMBER_OF_SECTOR_BLOCKS            2u
#define STATE_DISCONNECTED                              1U
#define CONNECTION_ID                           0xFFU
#define VALUE32 0xFFFFFFFFuL
#define MAX_BUFFER_LENGTH 0x400u
#define USER                                            0x02u
#define TRIGGER 0x3u
#define Ok                0x00u
#define N_Ok            0x01u
#define DataIdentifier               2u
#define AssemblyNumber                    ((uint16) 0xF102u)
#define DeliveryAssemblyNumber   (uint16)0xF1
#define SerialNumber   (uint16)0xA4
#define Identification  (uint16)0xA34
#define DiagnosticSession  (uint16)0xC34
#define DiagnosticApp  (uint16)0xC54
#define SpecificationVersion (uint16)0xB54
#define DownloadStatus (uint16)0xD53
#define FileIdentifier (uint16)0xDD6
#define PartNumber    (uint16)0xDD22
#define IdentificationNumber   (uint16)0xFF
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///shift 1 byte size
#define SHIFT_1_BYTE 8u
///shift 2 bytes size
#define SHIFT_2_BYTES 16u
///shift 3 bytes size
#define SHIFT_3_BYTES 24u
///shift 4 bytes size
#define SHIFT_4_BYTES 32u




///Round to integer macro
#define ROUND_TO_INT_HW(x)       (((x) >= 0.f) ? (sint32)((x) + 0.5f) : (sint32)((x) - 0.5f)) 

///Macro to swap 2 bytes
#define BYTESWAP16(n) ((((n)&0xFF00u)>>8u)|(((n)&0x00FFu)<<8u))

///Macro to swap 4 bytes
#define BYTESWAP32(n) ((BYTESWAP16(((n)&0xFFFF0000u)>>16u))|((BYTESWAP16((n)&0x0000FFFFu))<<16u))


#define ROUND_TO_UINT16(x)           (uint16)((float32)(x)+0.5f)


/**
 * Returns the high byte of the given 16 bit word.
 */
#define GetHiByte(data)                  ((uint8)(((uint16)(data))>>8))

 /**
  * Returns the low byte of the given 16 bit word.
  */
#define GetLoByte(data)                  ((uint8)(data))

  /// Extract the MSB of a DWORD value
# define GetHiHiByte(data32)                                  ((uint8)(((uint32)(data32))>>24))
/// Extract the byte next to the MSB of a DWORD value
# define GetHiLoByte(data32)                                  ((uint8)(((uint32)(data32))>>16))
/// Extract the byte next to the LSB of a DWORD value
# define GetLoHiByte(data32)                                  ((uint8)(((uint32)(data32))>>8))
/// Extract the LSB of a DWORD value
# define GetLoLoByte(data32)                                  ((uint8)(((uint32)(data32)) & 0xFFu))

#define INVALID_MILEAGE_QF 0U




#define SECOND_BYTE_MASK                  0x00FFu

#define FIRST_BYTE_MASK                   0xFF00u

# define TCPIP_AF_UNDEFINED       0x0000U
# define TCPIP_AF_INET            0x0002U  /* IPv4 */
# define TCPIP_AF_INET6           0x001CU  /* IPv6 */

# define SOAD_AF_INET                             (TCPIP_AF_INET) /*!< Domain type for IPv4. */
# define SOAD_AF_INET6                            (TCPIP_AF_INET6) /*!< Domain type for IPv6. */
# define SOAD_AF_INVALID                          (TCPIP_AF_UNDEFINED) /*!< Invalid domain type. */


#  if (CPU_BYTE_ORDER == HIGH_BYTE_FIRST)  /* BIGENDIAN (i.e. Network byte order) */                                    /* COV_TCPIP_BYTE_ORDER */
#   define IPV4_MULTICAST_PREFIX                  0xE0000000U
#   define IPV4_MULTICAST_PREFIX_MASK             0xF0000000U /* 4Bit mask */
#   define IPV4_LOCALNET_PREFIX                   0x7F000000U
#   define IPV4_LOCALNET_PREFIX_MASK              0xFF000000U /* 8Bit mask */
#  else /* LITTLEENDIAN */
#   define IPV4_MULTICAST_PREFIX                  0x000000E0U
#   define IPV4_MULTICAST_PREFIX_MASK             0x000000F0U /* 4Bit mask */
#   define IPV4_LOCALNET_PREFIX                   0x0000007FU
#   define IPV4_LOCALNET_PREFIX_MASK              0x000000FFU /* 8Bit mask */
#  endif

#  define IPV4_ADDR_ANY               0x00000000u
#  define IPV4_PORT_ANY               0x00000000u
# define TCPIP_INADDR_BROADCAST            0xFFFFFFFFu

#  define IPV4_ADDR_IN_SUBNET(ADDR, SUBNET_PREFIX, SUBNET_PREFIX_MASK) \
          ((SUBNET_PREFIX) == ((ADDR) & (SUBNET_PREFIX_MASK)))

/* ... multicast */
#  define IPV4_ADDR_IS_MULTICAST(ADDR) \
          (IPV4_ADDR_IN_SUBNET((ADDR), IPV4_MULTICAST_PREFIX, IPV4_MULTICAST_PREFIX_MASK))

/* ... limited or directed broadcast */
#  define IPV4_ADDR_IS_LIMITED_BROADCAST(ADDR) \
          ((ADDR) == TCPIP_INADDR_BROADCAST)
#  define IPV4_ADDR_IS_DIRECTED_BROADCAST(IpV4CtrlIdx, ADDR) \
          ((ADDR) == (   TcpIp_GetActiveNetAddrOfIpV4CtrlDyn(IpV4CtrlIdx) \
                      | ~(TcpIp_GetActiveNetMaskOfIpV4CtrlDyn(IpV4CtrlIdx))))
#  define IPV4_ADDR_IS_BROADCAST(IpV4CtrlIdx, ADDR) \
          (   IPV4_ADDR_IS_LIMITED_BROADCAST(ADDR) \
           || IPV4_ADDR_IS_DIRECTED_BROADCAST(IpV4CtrlIdx, ADDR))

		  
#define MAX_VALUE_OUTPUT_STRING		0xFFFFu	 

//masc for 4 bits
#define BITS_4_POSITIONS		4u 


#define MASK_HALF_BYTE			0XFu


typedef uint32 mileage_dt;
typedef uint8 EventDataQualifier;

typedef struct
{
   mileage_dt mileage;
   EventDataQualifier mileage_Qf;

} Mileage_t;


/* 2000-03-01 (mod 400 year, immediately after feb29 */
#define LEAPOCH (946684800LL + (86400*(31+29)))

#define DAYS_PER_400Y ((365*400) + 97)
#define DAYS_PER_100Y ((365*100) + 24)
#define DAYS_PER_4Y   ((365*4)   + 1)
#define INT_MIN -2147483648LL
#define INT_MAX 2147483647LL
#define FOUR_BYTE_SIZE             4u

///physical address length
#define PHYS_ADDR_LEN            6U



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct Content_Type
{
  uint8 magicWord[7];
  uint8 projectName[11];
  uint8 univID[9];
  uint8 majorVersion;
  uint8 minorVersion;
} Content_Type;

const uint8 AsciiString[11] = "HelloWorld";
const uint8 MapName[9] = "Test.map";

typedef enum
{
    JOB_OK = 0,
    JOB_FAILED,
    JOB_PENDING,
    JOB_CANCELED,
    BLOCK_INCONSISTENT,
    BLOCK_INVALID
} JobResultType;

typedef struct {

    uint8       blockHeaderData[10];
    uint8       ErrorCode;
    uint8       currentJob;
    uint8       DataAddress;
    uint8       HeaderAddress;
    uint8       WorkingSector;
    uint8       oldestSectorIndex;
    uint8       newestSectorIndex;
    uint8       oldestActiveSectorIndex;
    uint8       newestActiveSectorIndex;
    boolean     switchOngoing;
    uint8       sectorBuffer[20];
    uint8       sectorStatus[20];
}SectorMType;

typedef struct {

    uint32        blockIndex;
    uint8*        jobBuffer;
    uint8         blockLength;
}SectorType;

typedef enum {
    NO_ERROR = 0x00u, ///< No error detected.
    SW_UNIT_NOT_FOUND = 0x01u, ///< software unit not found, or user does not have access to the software unit.
    SW_UNIT_EXCEEDED = 0x02u, ///< StartAddress + length does not fit into the software unit.
    FLS_JOB_REQ_FAILED = 0x03u, ///< Fls job request was not accepted.
    FLS_JOB_PROC_FAILED = 0x04u, ///< Fls job processing finished with error.
    INVALID_BUFFER = 0x05u, ///< An invalid input/output buffer has been received
    INVALID_BLOCK_ID = 0x06u, ///< A blockId which is not supported has been requested
    INVALID_JOB_LENGTH = 0x07u, ///< Request block length is either zero, or not matching to the configured block size
    BLOCK_MISSING = 0x08u, ///< Request block length is either zero, or not matching to the configured block size
    ID_BLOCK_INVALID = 0x09u, ///< Request block length is either zero, or not matching to the configured block size
    UNEXPECTED_ERROR = 0x0Au, ///< Request block length is either zero, or not matching to the configured block size
    EC_UNKNOWN = 0xFFu  ///< Unsupported error code, should not be reachable
}ErrorCodeType;

typedef struct
{
    uint32 State;          
    uint8 ConnectionId;             
    uint8 Flags;                    
}ConnectionStatusType;

typedef enum {
    UNIT_OK = 0x00u, ///< Job finished successfully.
    UNIT_REQ_NOT_OK = 0x01u, ///< Validity checks failed.
    UNIT_NOT_FOUND = 0x02u, ///< software unit not found, or user does not have access to the software unit.
    UNIT_EXCEEDED = 0x03u, ///< StartAddress + length do not fit into the software unit.
    UNIT_FLS_JOB_REQ_FAILED = 0x04u, ///< Fls job request failed.
    UNIT_FLS_JOB_PROC_FAILED = 0x05u, ///< Fls job processing finished with error.
    UNIT_UNKNOWN = 0xFFu  ///< Unsupported job status, should not be reachable
}SwUnitReturnType;

typedef enum  {
    LEVEL_NONE,
    LEVEL_CALIBRATION,        ///< Calibration level authorization
    LEVEL_USER,               ///< User level authorization
    LEVEL_SUPERUSER,           ///< Super User level authorization
} Auth_Level;

typedef struct {
    uint32 u_StartAddress;
    uint32 u_EndAddress;
    boolean b_XcpFlash;
    Auth_Level  u_AuthLevel;
    uint8  u_IB_ID;
} Sector;

typedef struct PLM_Event
{
    uint8  lmG;
    uint16 lmA;
} Event;

typedef struct
{
    uint32 magicWord;
    uint32 Point;
    uint8 sectionCount;
    uint8 coreId;
    uint8 image[18u];
}Rbin;

typedef struct
{
    uint32 magicWord;
    uint8  rbin;
    uint8  certProfile;
    uint16 reserved;
    uint32 signatureInterval;
    uint32 signatureLength;
    uint32 ImageSize;
}Muco;

typedef struct
{
    uint32 offset;
    uint32 fileSize;
}RbinData;

typedef struct
{

    uint32 sectionLoadAddress;
    uint8 Id;

}Section;

typedef struct
{
    uint8 jobAddr;
    uint8 jobDataPtr;
    uint32 jobLength;
    uint32 jobType;
}DataType;


typedef struct
{
	int	tm_sec;		/* Seconds: 0-59 (K&R says 0-61?) */
	int	tm_min;		/* Minutes: 0-59 */
	int	tm_hour;	/* Hours since midnight: 0-23 */
	int	tm_mday;	/* Day of the month: 1-31 */
	int	tm_mon;		/* Months *since* january: 0-11 */
	int	tm_year;	/* Years since 1900 */
	int	tm_wday;	/* Days since Sunday (0-6) */
	int	tm_yday;	/* Days since Jan. 1: 0-365 */
	int	tm_isdst;	/* +1 Daylight Savings Time, 0 No DST,
				 * -1 don't know */
}t_Timestamp;

union Conversion
{
    float32 float_value;
    uint32 uint32_value;

};

void MemCpy(uint8* Dst, uint8* Src, uint16 Size);
uint8 MeasurementRead(uint8* Dst, AddressPtrType Src, uint8 Size);
uint32 GetIdData(uint8* Data, uint8 Id);

JobResultType GetJob(void);
void Watchdog(void);
uint8 SegmentNrGet(uint8 Src);
void MainFunction(void);
uint32 Read(uint32 u32_StartAddress, uint8 a_bytesForChecksum, uint32 ui32_NumberOfRemainingBytes, uint8 user);
uint32 CalculateCRC(uint8 a_bytesForChecksum, uint32 ui32_NumberOfRemainingBytes, uint32 u32_CRCStartValue, boolean b_firstCrcCall);
uint8 Copy(uint8 random, uint32 randomValue, uint8 maxLength);
void ProtocolId(uint32 param);
Event* Area(void);
uint32 Calculate(uint32 bit, uint32 ui32_Bytes, uint32 u32_Value, boolean call);
uint8 Check_Queue(void);
uint8 DriverInitialized(void);

////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32 GetUint32Value ( float32 value );
float32 Float32_SwapBytes( float32 inFloat );
uint8 Read_Mileage(Mileage_t * data); // functie externa
uint8 Read_Odometer(uint8 * Data);
uint8 u_Timestamp(long long t, t_Timestamp *tm);
void Swap4BytesArray(uint8* Array, const uint32 first4Bytes, const uint32 second4Bytes, uint8 * const responseIndex);
void v_GetSwapMacValue(uint8 *u_SwapMacValue, uint8 *u_Array);
void Ascii_converter2bytes ( uint8* p_u8_ReturnBuffer, uint32 u32_dateinHex);
uint8 numara();

#ifdef __cplusplus
}
#endif

#endif
