/**
 * Arduino - Radio Frequency Identification MFRC522
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */
#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_MFRC522_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_MFRC522_H__ 1

#include <RadioFrequencyIdentification.h>
#include <RegisterBasedDevice.h>
#include <Arduino.h>

#define MFRC522_DEFAULT_TIMEOUT 100

#define MFR522_INT_TO_EN_REG(i)     (i > COM_ALL_IRQ) ? DIV_I_EN : COM_I_EN
#define MFR522_INT_TO_EN_MASK(i)    (i > COM_ALL_IRQ) ? (i >> 8) & DIV_I_EN_INTERRUPT_EN : i & COM_I_EN_INTERRUPT_EN

#define MFR522_INT_TO_IRQ_REG(i)    (i > COM_ALL_IRQ) ? DIV_IRQ : COM_IRQ
#define MFR522_INT_TO_IRQ_MASK(i)   (i > COM_ALL_IRQ) ? (i >> 8) & DIV_IRQ_ALL_IRQ : i & COM_IRQ_ALL_IRQ

// Version 0.0 (0x90)
// Philips Semiconductors; Preliminary Specification Revision 2.0 - 01 August 2005; 16.1 self-test
const unsigned char MFRC522_FIRMWARE_REFERENCE_V0_0[] PROGMEM = { 0x00, 0x87, 0x98, 0x0f,
        0x49, 0xff, 0x07, 0x19, 0xbf, 0x22, 0x30, 0x49, 0x59, 0x63, 0xad, 0xca, 0x7f, 0xe3,
        0x4e, 0x03, 0x5c, 0x4e, 0x49, 0x50, 0x47, 0x9a, 0x37, 0x61, 0xe7, 0xe2, 0xc6, 0x2e,
        0x75, 0x5a, 0xed, 0x04, 0x3d, 0x02, 0x4b, 0x78, 0x32, 0xff, 0x58, 0x3b, 0x7c, 0xe9,
        0x00, 0x94, 0xb4, 0x4a, 0x59, 0x5b, 0xfd, 0xc9, 0x29, 0xdf, 0x35, 0x96, 0x98, 0x9e,
        0x4f, 0x30, 0x32, 0x8d };

// Version 1.0 (0x91)
// NXP Semiconductors; Rev. 3.8 - 17 September 2014; 16.1.1 self-test
const byte MFRC522_FIRMWARE_REFERENCE_V1_0[] PROGMEM = { 0x00, 0xc6, 0x37, 0xd5, 0x32, 0xb7,
        0x57, 0x5c, 0xc2, 0xd8, 0x7c, 0x4d, 0xd9, 0x70, 0xc7, 0x73, 0x10, 0xe6, 0xd2, 0xaa,
        0x5e, 0xa1, 0x3e, 0x5a, 0x14, 0xaf, 0x30, 0x61, 0xc9, 0x70, 0xdb, 0x2e, 0x64, 0x22,
        0x72, 0xb5, 0xbd, 0x65, 0xf4, 0xec, 0x22, 0xbc, 0xd3, 0x72, 0x35, 0xcd, 0xaa, 0x41,
        0x1f, 0xa7, 0xf3, 0x53, 0x14, 0xde, 0x7e, 0x02, 0xd9, 0x0f, 0xb5, 0x5e, 0x25, 0x1d,
        0x29, 0x79 };

// Version 2.0 (0x92)
// NXP Semiconductors; Rev. 3.8 - 17 September 2014; 16.1.1 self-test
const byte MFRC522_FIRMWARE_REFERENCE_V2_0[] PROGMEM = { 0x00, 0xeb, 0x66, 0xba, 0x57, 0xbf,
        0x23, 0x95, 0xd0, 0xe3, 0x0d, 0x3d, 0x27, 0x89, 0x5c, 0xde, 0x9d, 0x3b, 0xa7, 0x00,
        0x21, 0x5b, 0x89, 0x82, 0x51, 0x3a, 0xeb, 0x02, 0x0c, 0xa5, 0x00, 0x49, 0x7c, 0x84,
        0x4d, 0xb3, 0xcc, 0xd2, 0x1b, 0x81, 0x5d, 0x48, 0x76, 0xd5, 0x71, 0x61, 0x21, 0xa9,
        0x86, 0x96, 0x83, 0x38, 0xcf, 0x9d, 0x5b, 0x6d, 0xdc, 0x15, 0xba, 0x3e, 0x7d, 0x95,
        0x3b, 0x2f };

// Clone
// Fudan Semiconductor FM17522 (0x88)
const byte FM17522_FIRMWARE_REFERENCE[] PROGMEM = { 0x00, 0xd6, 0x78, 0x8c, 0xe2, 0xaa,
        0x0c, 0x18, 0x2a, 0xb8, 0x7a, 0x7f, 0xd3, 0x6a, 0xcf, 0x0b, 0xb1, 0x37, 0x63, 0x4b,
        0x69, 0xae, 0x91, 0xc7, 0xc3, 0x97, 0xae, 0x77, 0xf4, 0x37, 0xd7, 0x9b, 0x7c, 0xf5,
        0x3c, 0x11, 0x8f, 0x15, 0xc3, 0xd7, 0xc1, 0x5b, 0x00, 0x2a, 0xd0, 0x75, 0xde, 0x9e,
        0x51, 0x64, 0xab, 0x3e, 0xe9, 0x15, 0xb5, 0xab, 0x56, 0x9a, 0x98, 0x82, 0x26, 0xea,
        0x2a, 0x62 };

class RadioFrequencyIdentificationMFRC522: public RadioFrequencyIdentification,
        public RegisterBasedDevice {

    RegisterBasedDevice *device;

    unsigned char resetPin;

    unsigned char lastError;

public:

    enum Register {

        // Starts and stops command execution
        COMMAND = 0x01,

        // Enable and disable interrupt request control bits
        COM_I_EN = 0x02,

        // Enable and disable interrupt request control bits
        DIV_I_EN = 0x03,

        // Interrupt request bits
        COM_IRQ = 0x04,

        // Interrupt request bits Table 31 on page 40
        DIV_IRQ = 0X05,

        // Error bits showing the error status of the last command
        ERROR = 0X06,

        // Communication status bits
        STATUS1 = 0x07,

        // Receiver and transmitter status bits
        STATUS2 = 0x08,

        // Input and output of 64 byte FIFO buffer
        FIFO_DATA = 0X09,

        // Number of bytes stored in the FIFO buffer
        FIFO_LEVEL = 0x0a,

        // Level for FIFO underflow and overflow warning
        WATER_LEVEL = 0x0b,

        // Miscellaneous control registers
        CONTROL = 0x0c,

        // Adjustments for bit-oriented frames
        BIT_FRAMING = 0x0d,

        // Bit position of the first bit-collision detected on the RF
        COLL = 0x0e,

        // Defines general modes for transmitting and receiving
        MODE = 0x11,

        // Defines transmission data rate and framing
        TX_MODE = 0x12,

        // Defines reception data rate and framing
        RX_MODE = 0x13,

        // Controls the logical behavior of the antenna driver pins TX1 and TX2
        TX_CONTROL = 0x14,

        // Controls the setting of the transmission modulation
        TX_ASK = 0x15,

        // Selects the internal sources for the antenna driver
        TX_SEL = 0x16,

        // Selects internal receiver settings
        RX_SEL = 0x17,

        // Selects thresholds for the bit decoder
        RX_THRESHOLD = 0x18,

        // Defines demodulator settings
        DEMOD = 0x19,

        // Some MIFARE communication transmit parameters
        MF_TX = 0x1c,

        // Controls some MIFARE communication receive parameters
        MF_RX = 0x1d,

        // Selects the speed of the serial UART interface
        SERIAL_SPEED = 0x1f,

        // Shows the MSB and LSB values of the CRC calculation (HIGH)
        CRC_RESULT_HIDH = 0x21,

        // Shows the MSB and LSB values of the CRC calculation (LOW)
        CRC_RESULT_LOW = 0x22,

        // Controls the ModWidth setting
        MOD_WIDTH = 0x24,

        // Configures the receiver gain
        RFC_FG = 0x26,

        // Selects the conductance of the antenna driver pins TX1 and TX2 for modulation
        GS_N = 0x27,

        // The conductance of the p-driver output during periods of no modulation
        CW_GS_P = 0x28,

        // Defines the conductance of the p-driver output during periods of modulation
        MOD_GS_P = 0x29,

        // Defines settings for the internal timer
        T_MODE = 0x2a,

        // Defines settings for the internal timer
        T_PRESCALER_LOW = 0x2b,

        // Defines the 16-bit timer reload value (HIGH)
        T_RELOAD_HIGH = 0x2c,

        // Defines the 16-bit timer reload value (LOW)
        T_RELOAD_LOW = 0x2d,

        // Shows the 16-bit timer value (HIGH)
        T_COUNTER_VAL_HIGH = 0x2e,

        // Shows the 16-bit timer value (LOW)
        T_COUNTER_VAL_LOW = 0x2f,

        // Test signal configuration
        TEST_SEL1 = 0x31,

        // Test signal configuration and PRBS control
        TEST_SEL2 = 0x32,

        // Enables pin output driver on pins D1 to D7
        TEST_PIN_EN = 0x33,

        // Defines the values for D1 to D7 when it is used as an I/O bus
        TEST_PIN_VALUE = 0x34,

        // Shows the status of the internal test bus
        TEST_BUS = 0x35,

        // Controls the digital self test
        AUTO_TEST = 0x36,

        // Shows the software version
        VERSION = 0x37,

        // Controls the pins AUX1 and AUX2
        ANALOG_TEST = 0x38,

        // Defines the test value for TestDAC1
        TEST_DAC1 = 0x39,

        // Defines the test value for TestDAC2
        TEST_DAC2 = 0x3a,

        // Shows the value of ADC I and Q channels
        TEST_ADC = 0x3b
    };

    // All MIFARE Classic commands use the MIFARE Crypto1 and require an authentication.
    enum MIFARECommand {

        // REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
        MIFARE_REQUEST = 0x26,

        // Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
        MIFARE_WAKE_UP = 0x52,

        // Anti collision/Select, Cascade Level 1
        MIFARE_ANTI_COLLISION_CL1 = 0x93,

        // HaLT command, Type A. Instructs an ACTIVE PICC to go to state HALT.
        MIFARE_HLT_A = 0x50,

        // Perform authentication with Key A.
        MIFARE_AUTH_KEY_A = 0x60,

        // Perform authentication with Key B.
        MIFARE_AUTH_KEY_B = 0x61,

        // Reads one 16 byte block from the authenticated sector of the PICC. Also used for MIFARE Ultralight.
        MIFARE_READ = 0x30,

        // Writes one 16 byte block to the authenticated sector of the PICC. Called "COMPATIBILITY WRITE" for MIFARE Ultralight.
        MIFARE_WRITE = 0xa0,

        // Decrements the contents of a block and stores the result in the internal data register.
        MIFARE_DECREMENT = 0xc0,

        // Increments the contents of a block and stores the result in the internal data register.
        MIFARE_INCREMENT = 0xc1,

        // Reads the contents of a block into the internal data register.
        MIFARE_RESTORE = 0xc2,

        // Writes the contents of the internal data register to a block.
        MIFARE_TRANSFER = 0xb0,
    };

    enum Command {

        // no action, cancels current command execution
        IDLE = 0x00,

        // stores 25 bytes into the internal buffer
        MEM = 0x01,

        // generates a 10-byte random ID number
        GENERATE_RANDOM_ID = 0x02,

        // activates the CRC calculation or performs a self test
        CALC_CRC = 0x03,

        // Transmit data
        TRANSMIT = 0x04,

        // no command change, can be used to modify the CommandReg register bits without affecting the command, for example, the PowerDown bit
        NO_CMD_CHANGE = 0x07,

        // activates the receiver circuits (receive data)
        RECEIVE = 0x08,

        // transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission (transmit and receive data)
        TRANSCEIVE = 0x0c,

        // performs the MIFARE standard authentication as a reader (authentication)
        MF_AUTHENT = 0x0e,

        // resets the MFRC522
        SOFT_RESET = 0x0F
    };

    enum Error {
        NO_ERROR = 0x00,
        GENERAL_ERROR = 0x01,
        TIMEOUT_ERROR = 0x02,
        COMMUNICATION_ERROR = 0x03,
        CRC_ERROR = 0x04,
        MIFARE_NACK = 0x05
    };

    enum Mask {
        TX_CONTROL_TX1_RF_EN = 0x01,
        TX_CONTROL_TX2_RF_EN = 0x02,
        TX_CONTROL_TX_RF_EN = TX_CONTROL_TX1_RF_EN | TX_CONTROL_TX2_RF_EN,
        CONTROL_T_STOP_NOW = 0x80,
        CONTROL_T_START_NOW = 0x40,
        COM_I_EN_INTERRUPT_EN = 0x7f,
        COM_IRQ_TIMER_IRQ = 0x01,
        COM_IRQ_ERR_IRQ = 0x02,
        COM_IRQ_LO_ALERT_IRQ = 0x04,
        COM_IRQ_HI_ALERT_IRQ = 0x08,
        COM_IRQ_IDLE_IRQ = 0x10,
        COM_IRQ_RX_IRQ = 0x20,
        COM_IRQ_TX_IRQ = 0x40,
        COM_IRQ_ALL_IRQ = 0x7f,
        COM_IRQ_SET1 = 0x80,
        DIV_I_EN_CRC_I_EN = 0x04,
        DIV_I_EN_MFIN_ACT_I_EN = 0x10,
        DIV_I_EN_INTERRUPT_EN = DIV_I_EN_CRC_I_EN | DIV_I_EN_MFIN_ACT_I_EN,
        DIV_IRQ_CRC_IRQ = 0x04,
        DIV_IRQ_MFIN_ACT_IRQ = 0x10,
        DIV_IRQ_ALL_IRQ = DIV_IRQ_CRC_IRQ | DIV_IRQ_MFIN_ACT_IRQ,
        DIV_IRQ_SET2 = 0x80,
        FIFO_LEVEL_FLUSH_BUFFER = 0x80,
        FIFO_LEVEL_FIFO_LEVEL = 0x7f,
        WATER_LEVEL_WATER_LEVEL = 0x3f,
        BIT_FRAMING_START_SEND = 0x80,
        AUTO_TEST_ENABLE = 0x09
    };

    enum Interrupt
        : unsigned int {
            NONE_IRQ = 0x0000,
        COM_TIMER_IRQ = 0x0001,
        COM_ERR_IRQ = 0x0002,
        COM_LO_ALERT_IRQ = 0x0004,
        COM_HI_ALERT_IRQ = 0x0008,
        COM_IDLE_IRQ = 0x0010,
        COM_RX_IRQ = 0x0020,
        COM_TX_IRQ = 0x0040,
        COM_ALL_IRQ = 0x007f,
        DIV_CRC_IRQ = 0x0400,
        DIV_MFIN_ACT_IRQ = 0x1000,
        DIV_ALL_IRQ = DIV_CRC_IRQ | DIV_MFIN_ACT_IRQ
    };

    /**
     * COMMAND register (address 01h)
     * Reset value: 20h bit allocation
     */
    union COMMANDbits {

        struct {

            // Activates a command based on the Command value;
            // reading this register shows which command is executed
            unsigned char COMMAND :4;

            // 1: Soft power-down mode entered
            // 0: when the MFRC522 is ready
            // Remark: The PowerDown bit cannot be set when the SoftReset command is activated
            unsigned char POWER_DOWN :1;

            // Analog part of the receiver is switched off
            unsigned char RCV_OFF :1;

            // Reserved
            unsigned char :2;
        };
        unsigned char value;
    };

    /**
     * COM_I_EN register
     *
     * Control bits to enable and disable the passing of interrupt requests.
     */
    union COM_I_ENbits {

        struct {

            // Allows the timer interrupt request (TimerIRq bit) to be propagated to pin IRQ
            unsigned char TIMER_I_EN :1;

            // Allows the error interrupt request (ErrIRq bit) to be propagated to pin IRQ
            unsigned char ERR_I_EN :1;

            // Allows the low alert interrupt request (LoAlertIRq bit) to be propagated to pin IRQ
            unsigned char LO_ALERT_I_EN :1;

            // Allows the high alert interrupt request (HiAlertIRq bit) to be propagated to pin IRQ
            unsigned char HI_ALERT_I_EN :1;

            // Allows the idle interrupt request (IdleIRq bit) to be propagated to pin IRQ
            unsigned char IDLE_I_EN :1;

            // Allows the receiver interrupt request (RxIRq bit) to be propagated to pin IRQ
            unsigned char RX_I_EN :1;

            // Allows the transmitter interrupt request (TxIRq bit) to be propagated to pin IRQ
            unsigned char TX_I_EN :1;

            // 1: Signal on pin IRQ is inverted with respect to the Status1Reg register's IRq bit
            // 0: signal on pin IRQ is equal to the IRq bit; in combination with the DivIEnReg register's
            // IRqPushPull bit, the default value of logic 1 ensures that the output level on pin IRQ is 3-state
            unsigned char I_RQ_INV :1;
        };
        unsigned char value;
    };

    /**
     * DIV_I_EN register
     *
     * Control bits to enable and disable the passing of interrupt requests.
     */
    union DIV_I_ENbits {

        struct {

            // Reserved
            unsigned char :2;

            // Allows the CRC interrupt request, indicated by the DivIrqReg register's CRCIRq bit, to be propagated to pin IRQ
            unsigned char CRC_I_EN :1;

            // Reserved
            unsigned char :1;

            // Allows the MFIN active interrupt request to be propagated to pin IRQ
            unsigned char MFIN_ACT_I_EN :1;

            // Reserved
            unsigned char :2;

            // 1: pin IRQ is a standard CMOS output pin
            // 0: pin IRQ is an open-drain output pin
            unsigned char IRQ_PUSH_PULL :1;
        };
        unsigned char value;
    };

    /**
     * COM_IRQ register
     *
     * Interrupt request bits.
     */
    union COM_IRQbits {

        struct {

            // The timer decrements the timer value in register TCounterValReg to zero
            unsigned char TIMER_IRQ :1;

            // Any error bit in the ErrorReg register is set
            unsigned char ERR_IRQ :1;

            // Status1Reg register's LoAlert bit is set in opposition to the LoAlert bit,
            // the LoAlertIRq bit stores this event and can only be reset as indicated by
            // the Set1 bit in this register
            unsigned char LO_ALERT_IRQ :1;

            // Status1Reg register's HiAlert bit is set in opposition to the HiAlert bit,
            // the HiAlertIRq bit stores this event and can only be reset as indicated by
            // the Set1 bit in this register
            unsigned char HI_ALERT_IRQ :1;

            // If a command terminates, for example, when the CommandReg changes
            // its value from any command to the Idle command if an unknown command is started,
            // the CommandReg register Command[3:0] value changes to the idle state and the IdleIRq bit is set
            // The microcontroller starting the Idle command does not set the IdleIRq bit
            unsigned char IDLE_IRQ :1;

            // Receiver has detected the end of a valid data stream
            // if the RxModeReg register's RxNoErr bit is set to logic 1, the RxIRq bit is
            // only set to logic 1 when data bytes are available in the FIFO
            unsigned char RX_IRQ :1;

            // Set immediately after the last bit of the transmitted data was sent out
            unsigned char TX_IRQ :1;

            // 1: indicates that the marked bits in the ComIrqReg register are set
            // 0: indicates that the marked bits in the ComIrqReg register are cleared
            unsigned char SET1 :1;
        };
        unsigned char value;
    };

    /**
     * DIV_IRQ register
     *
     * Interrupt request bits.
     */
    union DIV_IRQbits {

        struct {

            // Reserved
            unsigned char :2;

            // The CalcCRC command is active and all data is processed
            unsigned char CRC_IRQ :1;

            // Reserved
            unsigned char :1;

            // MFIN is active this interrupt is set when either a rising or falling signal edge is detected.
            unsigned char MFIN_ACT_IRQ :1;

            // Reserved
            unsigned char :2;

            // 1: indicates that the marked bits in the DivIrqReg register are set
            // 0: indicates that the marked bits in the DivIrqReg register are cleared
            unsigned char SET2 :1;
        };
        unsigned char value;
    };

    /**
     * ERROR register
     *
     * Error bit register showing the error status of the last command executed.
     */
    union ERRORbits {

        struct {

            // Set to logic 1 if the SOF is incorrect automatically cleared during receiver start-up phase
            // bit is only valid for 106 kBd during the MFAuthent command, the ProtocolErr bit is set to
            // logic 1 if the number of bytes received in one data stream is incorrect
            unsigned char PROTOCOL_ERR :1;

            // Parity check failed. Automatically cleared during receiver start-up phase
            // only valid for ISO/IEC 14443 A/MIFARE communication at 106 kBd
            unsigned char PARITY_ERR :1;

            // The RxModeReg register's RxCRCEn bit is set and the CRC calculation fails
            // automatically cleared to logic 0 during receiver start-up phase
            unsigned char CRC_ERR :1;

            // A bit-collision is detected cleared automatically at receiver start-up phase
            // only valid during the bitwise anticollision at 106 kBd always set to logic 0 during communication
            // protocols at 212 kBd, 424 kBd and 848 kBd
            unsigned char COLL_ERR :1;

            // The host or a MFRC522's internal state machine (e.g. receiver) tries to
            // write data to the FIFO buffer even though it is already full
            unsigned char BUFFER_OVFL :1;

            // Reserved
            unsigned char :1;

            // Internal temperature sensor detects overheating, in which case the antenna drivers are automatically switched off
            unsigned char TEMP_ERR :1;

            // Data is written into the FIFO buffer by the host during the MFAuthent command or if data is written
            // into the FIFO buffer by the host during the time between sending the last bit on the RF interface and
            // receiving the last bit on the RF interface
            unsigned char WR_ERR :1;
        };
        unsigned char value;
    };

    /**
     * STATUS1 register
     *
     * Contains status bits of the CRC, interrupt and FIFO buffer.
     */
    union STATUS1bits {

        struct {

            // The number of bytes stored in the FIFO buffer corresponds to equation:
            // HiAlert = FIFOLength <= WaterLevel
            // example:
            // FIFO length = 4, WaterLevel = 4 > LoAlert = 1
            // FIFO length = 5, WaterLevel = 4 > LoAlert = 0
            unsigned char LO_ALERT :1;

            // The number of bytes stored in the FIFO buffer corresponds to equation:
            // HiAlert = (64 - FIFOLength) <= WaterLevel
            // example:
            // FIFO length = 60, WaterLevel = 4 > HiAlert = 1
            // FIFO length = 59, WaterLevel = 4 > HiAlert = 0
            unsigned char HI_ALERT :1;

            // MFRC522's timer unit is running, i.e. the timer will decrement the TCounterValReg register with the next timer clock
            // Remark: in gated mode, the TRunning bit is set to logic 1 when the timer is enabled by TModeReg register's TGated[1:0] bits;
            // this bit is not influenced by the gated signal
            unsigned char T_RUNNING :1;

            // Indicates if any interrupt source requests attention with respect to the setting of the interrupt enable bits:
            // see the ComIEnReg and DivIEnReg registers
            unsigned char IRQ :1;

            // The CRC calculation has finished only valid for the CRC coprocessor calculation using the CalcCRC command
            unsigned char CRC_READY :1;

            // The CRC result is zero
            // for data transmission and reception, the CRCOk bit is undefined: use the
            // ErrorReg register's CRCErr bit indicates the status of the CRC coprocessor, during calculation the value
            // changes to logic 0, when the calculation is done correctly the value changes to logic 1
            unsigned char CRC_OK :1;

            // Reserved
            unsigned char :1;
        };
        unsigned char value;
    };

    /**
     * STATUS2 register
     *
     * Contains status bits of the receiver, transmitter and data mode detector.
     */
    union STATUS2bits {

        struct {

            // Shows the state of the transmitter and receiver state machines:
            //  000: idle
            //  001: wait for the BitFramingReg register's StartSend bit
            //  010: TxWait: wait until RF field is present if the TModeReg register's
            // TxWaitRF bit is set to logic 1 the minimum time for TxWait is defined by the TxWaitReg register
            //  011: transmitting
            //  100: RxWait: wait until RF field is present if the TModeReg register's TxWaitRF bit is set to logic 1
            // the minimum time for RxWait is defined by the RxWaitReg register
            //  101: wait for data
            //  110: receiving
            unsigned char MODEM_STATE :3;

            // Indicates that the MIFARE Crypto1 unit is switched on and therefore all data communication with the card is encrypted
            // can only be set to logic 1 by a successful execution of the MFAuthent command only valid in Read/Write mode for
            // MIFARE standard cards this bit is cleared by software
            unsigned char MF_CRYPTO1_ON :1;

            // Reserved
            unsigned char :2;

            // I2C-bus input filter settings:
            // 1: the I2C-bus input filter is set to the High-speed mode independent of the I2C-bus protocol
            // 0: the I2C-bus input filter is set to the I2C-bus protocol used
            unsigned char I2C_FORCE_HS :1;

            // Clears the temperature error if the temperature is below the alarm limit of 125C
            unsigned char TEMP_SENS_CLEAR :1;
        };
        unsigned char value;
    };

    /**
     * FIFO_LEVEL register
     *
     * Indicates the number of bytes stored in the FIFO.
     */
    union FIFO_LEVELbits {

        struct {

            // Indicates the number of bytes stored in the FIFO buffer writing to the FIFODataReg
            // register increments and reading decrements the FIFOLevel value
            unsigned char FIFO_LEVEL :7;

            // Immediately clears the internal FIFO buffer's read and write pointer and ErrorReg
            // register's BufferOvfl bit reading this bit always returns 0
            unsigned char FLUSH_BUFFER :1;
        };
        unsigned char value;
    };

    /**
     * WATER_LEVEL register
     *
     * Defines the level for FIFO under- and overflow warning.
     */
    union WATER_LEVELbits {

        struct {

            // Defines a warning level to indicate a FIFO buffer overflow or underflow:
            // Status1Reg register's HiAlert bit is set to logic 1 if the remaining
            // number of bytes in the FIFO buffer space is equal to, or less than the defined number of WaterLevel bytes
            // Status1Reg register's LoAlert bit is set to logic 1 if equal to, or less than the WaterLevel bytes in the FIFO buffer
            // Remark: to calculate values for HiAlert and LoAlert see Section 9.3.1.8 on page 42.
            unsigned char WATER_LEVEL :7;

            // Reserved
            unsigned char :1;
        };
        unsigned char value;
    };

    /**
     * CONTROL register
     *
     * Miscellaneous control bits.
     */
    union CONTROLbits {

        struct {

            // Indicates the number of valid bits in the last received byte if this value is 000b, the whole byte is valid
            unsigned char RX_LAST_BITS :3;

            // Reserved
            unsigned char :2;

            // Timer starts immediately
            // reading this bit always returns it to logic 0
            unsigned char T_START_NOW :1;

            // Timer stops immediately
            // reading this bit always returns it to logic0
            unsigned char T_STOP_NOW :1;
        };
        unsigned char value;
    };

    /**
     * BIT_FRAMING register
     *
     * Miscellaneous control bits.
     */
    union BIT_FRAMINGbits {

        struct {

            // Used for transmission of bit oriented frames: defines the number of bits of the last byte that will be transmitted
            // 000b indicates that all bits of the last byte will be transmitted
            unsigned char TX_LAST_BITS :3;

            // Reserved
            unsigned char :1;

            // used for reception of bit-oriented frames: defines the bit position for the first bit received to be stored in the FIFO buffer
            // example:
            // 0: LSB of the received bit is stored at bit position 0, the second received bit is stored at bit position 1
            // 1: LSB of the received bit is stored at bit position 1, the second received bit is stored at bit position 2
            // 7: LSB of the received bit is stored at bit position 7, the second received bit is stored in the next byte that follows at bit position 0
            // These bits are only to be used for bitwise anticollision at 106 kBd, for all other modes they are set to 0
            unsigned char RX_ALIGN :3;

            // Starts the transmission of data only valid in combination with the Transceive command
            unsigned char START_SEND :1;
        };
        unsigned char value;
    };

    /**
     * COLL register
     *
     * Miscellaneous control bits.
     */
    union COLLbits {

        struct {

            // Shows the bit position of the first detected collision in a received frame only data bits are interpreted
            // example:
            //  00h: indicates a bit-collision in the 32nd bit
            //  01h: indicates a bit-collision in the 1st bit
            //  08h: indicates a bit-collision in the 8th bit
            // These bits will only be interpreted if the CollPosNotValid bit is set to logic 0
            unsigned char COLL_POS :5;

            // No collision detected or the position of the collision is out of the range of CollPos[4:0]
            unsigned char COLL_POS_NOT_VALID :1;

            // Reserved
            unsigned char :1;

            // All received bits will be cleared after a collision only used during bitwise anticollision at 106 kBd, otherwise it is set to logic 1
            unsigned char VALUES_AFTER_COLL :1;
        };
        unsigned char value;
    };

    /**
     * MODE register
     *
     * Defines general mode settings for transmitting and receiving.
     */
    union MODEbits {

        struct {

            // defines the preset value for the CRC coprocessor for the CalcCRC command
            // Remark: during any communication, the preset values are selected automatically according to
            // the definition of bits in the RxModeReg and TxModeReg registers
            //  00: 0000h
            //  01: 6363h
            //  10: A671h
            //  11: FFFFh
            unsigned char CRC_PRESET :2;

            // Reserved
            unsigned char :1;

            // Defines the polarity of pin MFIN
            // Remark: the internal envelope signal is encoded active LOW, changing this bit generates a MFinActIRq event
            // 1: polarity of pin MFIN is active HIGH
            // 0: polarity of pin MFIN is active LOW
            unsigned char POL_M_FIN :1;

            // Reserved
            unsigned char :1;

            // Transmitter can only be started if an RF field is generated
            unsigned char TX_WAIT_RF :1;

            // Reserved
            unsigned char :1;

            // CRC coprocessor calculates the CRC with MSB first in the CRCResultReg register the values for the
            // CRCResultMSB[7:0] bits and the CRCResultLSB[7:0] bits are bit reversed
            // Remark: during RF communication this bit is ignored
            unsigned char MSB_FIRST :1;
        };
        unsigned char value;
    };

    /**
     * TX_MODE register
     *
     * Defines the data rate during transmission.
     */
    union TX_MODEbits {

        struct {

            // Reserved
            unsigned char :3;

            // Modulation of transmitted data is inverted
            unsigned char INV_MOD :1;

            // Defines the bit rate during data transmission the MFRC522 handles transfer speeds up to 848 kBd
            //  000: 106 kBd
            //  001: 212 kBd
            //  010: 424 kBd
            //  011: 848 kBd
            unsigned char TX_SPEED :3;

            // Enables CRC generation during data transmission
            // Remark: can only be set to logic 0 at 106 kBd
            unsigned char TX_CRC_EN :1;
        };
        unsigned char value;
    };

    /**
     * RX_MODE register
     *
     * Defines the data rate during reception.
     */
    union RX_MODEbits {

        struct {

            // Reserved
            unsigned char :2;

            // 0: receiver is deactivated after receiving a data frame
            // 1: able to receive more than one data frame only valid for data rates above 106 kBd in order to handle
            // the polling command after setting this bit the Receive and Transceive commands will not terminate automatically.
            // Multiple reception can only be deactivated by writing any command (except the Receive command) to the CommandReg
            // register, or by the host clearing the bit if set to logic 1, an error byte is added to the FIFO buffer at the
            // end of a received data stream which is a copy of the ErrorReg register value. For the MFRC522 version 2.0 the CRC status is
            // reflected in the signal CRCOk, which indicates the actual status of the CRC coprocessor. For the MFRC522 version 1.0 the CRC
            // status is reflected in the signal CRCErr.
            unsigned char RX_MULTIPLE :1;

            // An invalid received data stream (less than 4 bits received) will be ignored and the receiver remains active
            unsigned char RX_NO_ERR :1;

            // Defines the bit rate while receiving data the MFRC522 handles transfer speeds up to 848 kBd
            //  000: 106 kBd
            //  001: 212 kBd
            //  010: 424 kBd
            //  011: 848 kBd
            unsigned char RX_SPEED :3;

            // Enables the CRC calculation during reception
            // Remark: can only be set to logic 0 at 106 kBd
            unsigned char RX_CRC_EN :1;
        };
        unsigned char value;
    };

    /**
     * TX_CONTROL register
     *
     * Controls the logical behavior of the antenna driver pins TX1 and TX2.
     */
    union TX_CONTROLbits {

        struct {

            // Output signal on pin TX1 delivers the 13.56 MHz energy carrier modulated by the transmission data
            unsigned char TX1_RF_EN :1;

            // Output signal on pin TX2 delivers the 13.56 MHz energy carrier modulated by the transmission data
            unsigned char TX2_RF_EN :1;

            // Reserved
            unsigned char :1;

            // 1: output signal on pin TX2 continuously delivers the unmodulated 13.56 MHz energy carrier
            // 0: Tx2CW bit is enabled to modulate the 13.56 MHz energy carrier
            unsigned char TX2_CW :1;

            // Output signal on pin TX1 inverted when driver TX1 is disabled
            unsigned char INV_TX1_RF_OFF :1;

            // Output signal on pin TX2 inverted when driver TX2 is disabled
            unsigned char INV_TX2_RF_OFF :1;

            // Output signal on pin TX1 inverted when driver TX1 is enabled
            unsigned char INV_TX1_RF_ON :1;

            // Output signal on pin TX2 inverted when driver TX2 is enabled
            unsigned char INV_TX2_RF_ON :1;
        };

        struct {

            // Output signal on pin TX1 delivers the 13.56 MHz energy carrier modulated by the transmission data
            // Output signal on pin TX2 delivers the 13.56 MHz energy carrier modulated by the transmission data
            unsigned char TX_RF_EN :2;

            // Reserved
            unsigned char :2;

            // Output signal on pin TX1 inverted when driver TX1 is disabled
            // Output signal on pin TX2 inverted when driver TX2 is disabled
            unsigned char INV_TX_RF_OFF :2;

            // Output signal on pin TX1 inverted when driver TX1 is enabled
            // Output signal on pin TX2 inverted when driver TX2 is enabled
            unsigned char INV_TX_RF_ON :2;
        };
        unsigned char value;
    };

    /**
     * TX_ASK register
     *
     * Controls transmit modulation settings.
     */
    union TX_ASKbits {

        struct {

            // Reserved
            unsigned char :6;

            // Forces a 100% ASK modulation independent of the ModGsPReg register setting
            unsigned char FORCE_100_ASK :1;

            // Reserved
            unsigned char :1;
        };
        unsigned char value;
    };

    /**
     * TX_SEL register
     *
     * Selects the internal sources for the analog module.
     */
    union TX_SELbits {

        struct {

            // Selects the input for pin MFOUT
            //  0000: 3-state
            //  0001: LOW
            //  0010: HIGH
            //  0011: test bus signal as defined by the TestSel1Reg register's TstBusBitSel[2:0] value
            //  0100: modulation signal (envelope) from the internal encoder, Miller pulse encoded
            //  0101: serial data stream to be transmitted, data stream before Miller encoder
            //  0110: reserved
            //  0111: serial data stream received, data stream after Manchester decoder
            //  1000: to 1111 reserved
            unsigned char MF_OUT_SEL :4;

            // Selects the input of drivers TX1 and TX2
            //  00: 3-state; in soft power-down the drivers are only in 3-state mode if the DriverSel[1:0] value is set to 3-state mode
            //  01: modulation signal (envelope) from the internal encoder, Miller pulse encoded
            //  10: modulation signal (envelope) from pin MFIN
            //  11: HIGH; the HIGH level depends on the setting of bits InvTx1RFOn/InvTx1RFOff and InvTx2RFOn/InvTx2RFOff
            unsigned char :2;

            // Reserved
            unsigned char :2;
        };
        unsigned char value;
    };

    /**
     * RX_SEL register
     *
     * Selects internal receiver settings.
     */
    union RX_SELbits {

        struct {

            // After data transmission the activation of the receiver is delayed for RxWait bit-clocks, during this ‘frame guard time
            // any signal on pin RX is ignored this parameter is ignored by the Receive command all other commands, such as Transceive,
            // MFAuthent use this parameter the counter starts immediately after the external RF field is switched on
            unsigned char RX_WAIT :6;

            // Selects the input of the contactless UART
            //  00: constant LOW
            //  01: Manchester with subcarrier from pin MFIN
            //  10: modulated signal from the internal analog module, default
            //  11: NRZ coding without subcarrier from pin MFIN which is only valid for transfer speeds above 106 kBd
            unsigned char UART_SEL :2;
        };
        unsigned char value;
    };

    /**
     * RX_THRESHOLD register
     *
     * Selects thresholds for the bit decoder.
     */
    union RX_THRESHOLDbits {

        struct {

            // defines the minimum signal strength at the decoder input that must be reached by the weaker half-bit of the
            // Manchester encoded signal to generate a bit-collision relative to the amplitude of the stronger half-bit
            unsigned char COLL_LEVEL :3;

            // Reserved
            unsigned char :1;

            // Defines the minimum signal strength at the decoder input that will be accepted if the signal strength is below this level it is not evaluated
            unsigned char MIN_LEVEL :4;
        };
        unsigned char value;
    };

    /**
     * DEMOD register
     *
     * Defines demodulator settings.
     */
    union DEMODbits {

        struct {

            // Changes the time-constant of the internal PLL during burst
            unsigned char TAU_SYNC :2;

            // Changes the time-constant of the internal PLL during data reception
            // Remark: if set to 00b the PLL is frozen during data reception
            unsigned char TAU_RCV :2;

            // If set to logic 0 the following formula is used to calculate the timer frequency of the prescaler:
            // F_timer = 13.56 MHz / (2*TPreScaler+1).
            // If set to logic 1 the following formula is used to calculate the timer frequency of the prescaler:
            // F_timer = 13.56 MHz / (2*TPreScaler+2)
            unsigned char T_PRESCAL_EVEN :1;

            // If AddIQ[1:0] are set to X0b, the reception is fixed to I channel
            // If AddIQ[1:0] are set to X1b, the reception is fixed to Q channel
            unsigned char FIX_IQ :1;

            // Defines the use of I and Q channel during reception
            // Remark: the FixIQ bit must be set to logic 0 to enable the following settings:
            //  00: selects the stronger channel
            //  01: selects the stronger channel and freezes the selected channel during communication
            unsigned char ADD_IQ :2;
        };
        unsigned char value;
    };

    /**
     * MF_TX register
     *
     * Controls some MIFARE communication transmit parameters.
     */
    union MF_TXbits {

        struct {

            // Defines the additional response time 7 bits are added to the value of the register bit by default
            unsigned char TX_WAIT :2;

            // Reserved
            unsigned char :6;
        };
        unsigned char value;
    };

    /**
     * MF_RX register
     *
     * Controls some MIFARE communication receive parameters.
     */
    union MF_RXbits {

        struct {

            // Reserved
            unsigned char :4;

            // Generation of the parity bit for transmission and the parity check for receiving is switched off
            // the received parity bit is handled like a data bit
            unsigned char PARITY_DISABLE :1;

            // Reserved
            unsigned char :3;
        };
        unsigned char value;
    };

    /**
     * SERIAL_SPEED register
     *
     * Selects the speed of the serial UART interface.
     */
    union SERIAL_SPEEDbits {

        struct {

            // Factor BR_T1 adjusts the transfer speed
            unsigned char BR_T1 :5;

            // Factor BR_T0 adjusts the transfer speed
            unsigned char BR_T0 :3;
        };
        unsigned char value;
    };

    /**
     * RF_CFG register
     *
     * Configures the receiver gain.
     */
    union RF_CFGbits {

        struct {

            // Reserved
            unsigned char :4;

            // Defines the receiver's signal voltage gain factor:
            //  000: 18 dB
            //  001: 23 dB
            //  010: 18 dB
            //  011: 23 dB
            //  100: 33 dB
            //  101: 38 dB
            //  110: 43 dB
            //  111: 48 dB
            unsigned char RX_GAIN :3;

            // Reserved
            unsigned char :1;
        };
        unsigned char value;
    };

    /**
     * GS_N register
     *
     * Defines the conductance of the antenna driver pins TX1 and TX2 for the n-driver when the driver is switched on.
     */
    union GS_Nbits {

        struct {

            // Defines the conductance of the output n-driver during periods without modulation which can be used to regulate the modulation index
            // Remark: the conductance value is binary weighted during soft Power-down mode the highest bit is forced to logic 1
            // value is only used if driver TX1 or TX2 is switched on
            unsigned char MOD_GS_N :4;

            // defines the conductance of the output n-driver during periods without modulation which can be used to regulate the output power and
            // subsequently current consumption and operating distance
            // Remark: the conductance value is binary-weighted during soft Power-down mode the highest bit is forced to logic 1
            // value is only used if driver TX1 or TX2 is switched on
            unsigned char CW_GS_N :4;
        };
        unsigned char value;
    };

    /**
     * CW_GS_P register
     *
     * Defines the conductance of the p-driver output during periods of no modulation.
     */
    union CW_GS_Pbits {

        struct {

            // defines the conductance of the p-driver output which can be used to regulate the output power and subsequently current consumption and operating distance
            // Remark: the conductance value is binary weighted during soft Power-down mode the highest bit is forced to logic 1
            unsigned char CW_GS_P :6;

            // Reserved
            unsigned char :2;
        };
        unsigned char value;
    };

    /**
     * MOD_GS_P register
     *
     * Defines the conductance of the p-driver output during modulation.
     */
    union MOD_GS_Pbits {

        struct {

            // Defines the conductance of the p-driver output during modulation which can be used to regulate the modulation index
            // Remark: the conductance value is binary weighted during soft Power-down mode the highest bit is forced to logic 1
            // if the TxASKReg register's Force100ASK bit is set to logic 1 the value of ModGsP has no effect
            unsigned char MOD_GS_P :6;

            // Reserved
            unsigned char :2;
        };
        unsigned char value;
    };

    /**
     * T_MODE register
     *
     * These registers define the timer settings.
     *
     * Remark: The TPrescaler setting higher 4 bits are in the TModeReg register and the lower 8 bits are in the TPrescalerReg register.
     */
    union T_MODEbits {

        struct {

            // Defines the higher 4 bits of the TPrescaler value
            // The following formula is used to calculate the timer frequency if the DemodReg register's TPrescalEven bit in Demot Regis set to logic 0:
            // F_timer = 13.56 MHz / (2*TPreScaler+1)
            // Where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo] (TPrescaler value on 12 bits) (Default TPrescalEven bit is logic 0)
            // The following formula is used to calculate the timer frequency if the DemodReg register's TPrescalEven bit is set to logic 1:
            // F_timer = 13.56 MHz / (2*TPreScaler+2).
            unsigned char T_PRESCALER_HI :4;

            // 1: timer automatically restarts its count-down from the 16-bit timer reload value instead of counting down to zero
            // 0: timer decrements to 0 and the ComIrqReg register's TimerIRq bit is set to logic 1
            unsigned char T_AUTO_RESTART :1;

            // Internal timer is running in gated mode
            // Remark: in gated mode, the Status1Reg register's TRunning bit is logic 1 when the timer is enabled by the
            // TModeReg register's TGated[1:0] bits this bit does not influence the gating signal
            //  00: non-gated mode
            //  01: gated by pin MFIN
            //  10: gated by pin AUX1
            unsigned char T_GATED :2;

            // 1: timer starts automatically at the end of the transmission in all communication modes at all speeds
            // if the RxModeReg register's RxMultiple bit is not set, the timer stops immediately after receiving the 5th bit (1 start bit, 4 data bits)
            // if the RxMultiple bit is set to logic 1 the timer never stops, in which case the timer can be stopped by setting the ControlReg register's
            // TStopNow bit to logic 1
            // 0: indicates that the timer is not influenced by the protocol
            unsigned char T_AUTO :1;
        };
        unsigned char value;
    };

    /**
     * VERSION register
     *
     * Shows the MFRC522 software version.
     */
    union VERSIONbits {

        struct {

            // '1' stands for MFRC522 version 1.0 and '2' stands for MFRC522 version 2.0.
            unsigned char VERSION :4;

            // '9' stands for MFRC522
            unsigned char CHIPTYPE :4;
        };
        unsigned char value;
    };

    struct Uid {

        // Number of bytes in the UID. 4, 7 or 10.
        unsigned char size;

        unsigned char uid[10];

        // The SAK (Select acknowledge) byte returned from the PICC after successful selection.
        unsigned char sak;
    };

    enum Version {
        CLONE = 0x88,
        V0_0 = 0x90,
        V1_0 = 0x91,
        V2_0 = 0x92
    };

    RadioFrequencyIdentificationMFRC522(RegisterBasedDevice *device,
            unsigned char resetPin);

    void initialize();

    void sendCommand(Command command) {
        writeRegister(COMMAND, (unsigned char) command);
    }

    unsigned char getLastError() {
        return lastError;
    }

    void clearLastError() {
        lastError = NO_ERROR;
    }

    void softReset();

    void setAntennaOn();

    void setAntennaOff();

    /**
     * The timer unit can be used to measure the time interval between two events or to indicate
     * that a specific event occurred after a specific time. The timer can be triggered by events
     * explained in the paragraphs below. The timer does not influence any internal events, for
     * example, a time-out during data reception does not automatically influence the reception
     * process. Furthermore, several timer-related bits can be used to generate an interrupt.
     * The timer has an input clock of 13.56 MHz derived from the 27.12 MHz quartz crystal
     * oscillator. The timer consists of two stages: prescaler and counter.
     * The prescaler (TPrescaler) is a 12-bit counter. The reload values (TReloadVal_Hi[7:0] and
     * TReloadVal_Lo[7:0]) for TPrescaler can be set between 0 and 4095 in the TModeReg
     * register’s TPrescaler_Hi[3:0] bits and TPrescalerReg register’s TPrescaler_Lo[7:0] bits.
     * The reload value for the counter is defined by 16 bits between 0 and 65535 in the
     * TReloadReg register.
     * The current value of the timer is indicated in the TCounterValReg register.
     * When the counter reaches 0, an interrupt is automatically generated, indicated by the
     * ComIrqReg register’s TimerIRq bit setting. If enabled, this event can be indicated on
     * pin IRQ. The TimerIRq bit can be set and reset by the host. Depending on the
     * configuration, the timer will stop at 0 or restart with the value set in the TReloadReg
     * register.
     * The timer status is indicated by the Status1Reg register’s TRunning bit.
     * The timer can be started manually using the ControlReg register’s TStartNow bit and
     * stopped using the ControlReg register’s TStopNow bit.
     * The timer can also be activated automatically to meet any dedicated protocol
     * requirements by setting the TModeReg register’s TAuto bit to logic 1.
     *
     * @param   prescaler       12 bit prescaler value.
     * @param   reload          16 bit reload value.
     * @param   autoStart       1: timer starts automatically at the end of the transmission in
     *                          all communication modes at all speeds
     *                          if the RxModeReg register’s RxMultiple bit is not set, the
     *                          timer stops immediately after receiving the 5th bit (1 start bit, 4 data bits)
     *                          if the RxMultiple bit is set to logic 1 the timer never stops, in
     *                          which case the timer can be stopped by setting the
     *                          ControlReg register’s TStopNow bit to logic 1
     *                          0: indicates that the timer is not influenced by the protocol
     * @param   autoRestart     1: timer automatically restarts its count-down from the 16-bit
     *                          timer reload value instead of counting down to zero
     *                          0 timer decrements to 0 and the ComIrqReg register’s
     *                          TimerIRq bit is set to logic 1
     */
    void configureTimer(unsigned int prescaler, unsigned int reload, bool autoStart,
    bool autoRestart);

    void startTimer();

    void stopTimer();

    /**
     *
     */
    void enableInterrupt(Interrupt interrupt);

    void disableInterrupt(Interrupt interrupt);

    void clearInterrupt(Interrupt interrupt);

    void flushQueue();

    void setWaterLevel(unsigned char level);

    int generateRandomId(unsigned char buf[10]);

    int communicate(Command command, unsigned char *output, unsigned char *input,
            unsigned char outputLen, bool checkCRC);

    inline int communicate(Command command, unsigned char *output, unsigned char *input,
            unsigned char outputLen) {
        return communicate(command, output, input, outputLen, false);
    }

    /**
     * @param   output          Pointer to the data to transfer to the FIFO.
     * @param   input           NULL or pointer to buffer if data should be read back after executing the command. (max 64 bytes).
     * @param   outputLen       Size of the data to transfer to the FIFO.
     */
    int tranceiveData(unsigned char *output, unsigned char *input, unsigned char outputLen,
            bool checkCRC);

    inline int tranceiveData(unsigned char *output, unsigned char *input, unsigned char outputLen) {
        return tranceiveData(output, input, outputLen, false);
    }

    unsigned int calculateCRC(unsigned char *buff, unsigned char len);

    bool waitForRegisterBits(unsigned char reg, unsigned char mask) {
        return waitForRegisterBits(reg, mask, MFRC522_DEFAULT_TIMEOUT);
    }

    bool waitForRegisterBits(unsigned char reg, unsigned char mask, unsigned long timeout);

    Version getVersion();

    /**
     * 1. Perform a soft reset.
     * 2. Clear the internal buffer by writing 25 bytes of 00h and implement the Config command.
     * 3. Enable the self test by writing 09h to the AutoTestReg register.
     * 4. Write 00h to the FIFO buffer.
     * 5. Start the self test with the CalcCRC command.
     * 6. The self test is initiated.
     * 7. When the self test has completed, the FIFO buffer contains the following 64 bytes:
     *
     * FIFO buffer byte values for MFRC522 version 1.0:
     * 00h, C6h, 37h, D5h, 32h, B7h, 57h, 5Ch,
     * C2h, D8h, 7Ch, 4Dh, D9h, 70h, C7h, 73h,
     * 10h, E6h, D2h, AAh, 5Eh, A1h, 3Eh, 5Ah,
     * 14h, AFh, 30h, 61h, C9h, 70h, DBh, 2Eh,
     * 64h, 22h, 72h, B5h, BDh, 65h, F4h, ECh,
     * 22h, BCh, D3h, 72h, 35h, CDh, AAh, 41h,
     * 1Fh, A7h, F3h, 53h, 14h, DEh, 7Eh, 02h,
     * D9h, 0Fh, B5h, 5Eh, 25h, 1Dh, 29h, 79h
     *
     * FIFO buffer byte values for MFRC522 version 2.0:
     * 00h, EBh, 66h, BAh, 57h, BFh, 23h, 95h,
     * D0h, E3h, 0Dh, 3Dh, 27h, 89h, 5Ch, DEh,
     * 9Dh, 3Bh, A7h, 00h, 21h, 5Bh, 89h, 82h,
     * 51h, 3Ah, EBh, 02h, 0Ch, A5h, 00h, 49h,
     * 7Ch, 84h, 4Dh, B3h, CCh, D2h, 1Bh, 81h,
     * 5Dh, 48h, 76h, D5h, 71h, 061h, 21h, A9h,
     * 86h, 96h, 83h, 38h, CFh, 9Dh, 5Bh, 6Dh,
     * DCh, 15h, BAh, 3Eh, 7Dh, 95h, 03Bh, 2Fh
     */
    bool performSelfTest();

    void setBitFraming(unsigned char rxAlign, unsigned char txLastBits);

    /**
     * Checks if there is a card near.
     */
    bool sendRequestTypeA();

    bool sendWakeUp();

    bool sendHalt();

    /**
     * Reads values from the device, starting by the reg register.
     *
     * @param reg           The register number.
     * @param buf           The buffer where to place read bytes.
     *                      MSB become LSB inside buffer.
     * @param len           How many bytes to read.
     * @return              If >= 0: How many bytes were read.
     *                      If < 0: Error code:
     *                      <ul>
     *                          <li>-1: data too long to fit in transmit buffer</li>
     *                          <li>-2: received NACK on transmit of address</li>
     *                          <li>-3: received NACK on transmit of data</li>
     *                          <li>-4: other error</li>
     *                          <li>-5: timeout</li>
     *                      </ul>
     */
    int readRegisterBlock(unsigned char reg, unsigned char *buf, unsigned char len);

    int readRegisterBlock(unsigned char reg, unsigned char *buf, unsigned char len, unsigned char rxAlign);

    /**
     * Writes a sequence of values to a sequence of registers, starting by the reg address.
     *
     * @param reg           The register number.
     * @param buf           The buffer.
     * @param len           Buffer length.
     * @return              The result of Wire.endTransmission().
     */
    unsigned char writeRegisterBlock(unsigned char reg, unsigned char *buf,
            unsigned char len);


private:

    bool hasValisCRC(unsigned char *buf, unsigned char len);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_MFRC522_H__
