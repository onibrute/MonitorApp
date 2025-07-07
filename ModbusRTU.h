#pragma once
#include "ISerialProtocol.h"
#include <vector>
#include <cstdint>

class ModbusRTU
{
public:
    ModbusRTU(ISerialProtocol* serial);
    ~ModbusRTU();

 
        bool ReadHoldingRegisters(uint8_t slaveId,
        uint16_t startAddr,
        uint16_t count,
        std::vector<uint16_t>& out,
        CString& errorMsg);

        bool WriteSingleRegister(uint8_t slaveId,
        uint16_t address,
        uint16_t value);

private:
    ISerialProtocol* m_serial;

    uint16_t CalcCRC(const uint8_t* data, size_t length);
    bool SendFrame(const std::vector<uint8_t>& frame,
        std::vector<uint8_t>& reply,
        unsigned int replySize,
        unsigned int timeoutMs = 200);
};
