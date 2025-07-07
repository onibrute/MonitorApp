#include "pch.h"
#include "ModbusRTU.h"
#include <winsock2.h>
#include <windows.h>
#include <cstdint>
#include <vector>

ModbusRTU::ModbusRTU(ISerialProtocol* serial)
    : m_serial(serial)
{
}

ModbusRTU::~ModbusRTU()
{
}


uint16_t ModbusRTU::CalcCRC(const uint8_t* data, size_t length)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b) {
            if (crc & 1) crc = (crc >> 1) ^ 0xA001;
            else         crc = (crc >> 1);
        }
    }
    return crc;
}

bool ModbusRTU::SendFrame(const std::vector<uint8_t>& frame,
    std::vector<uint8_t>& reply,
    unsigned int replySize,
    unsigned int timeoutMs)
{
    if (!m_serial->Write(frame)) return false;
    return m_serial->Read(reply, replySize, timeoutMs);
}


bool ModbusRTU::ReadHoldingRegisters(uint8_t slaveId,
    uint16_t startAddr,
    uint16_t count,
    std::vector<uint16_t>& out,
    CString& errorMsg)
{
    std::vector<uint8_t> req(8);
    req[0] = slaveId;
    req[1] = 0x03;
    req[2] = startAddr >> 8;
    req[3] = startAddr & 0xFF;
    req[4] = count >> 8;
    req[5] = count & 0xFF;
    uint16_t crc = CalcCRC(req.data(), 6);
    req[6] = crc & 0xFF;
    req[7] = crc >> 8;

    if (!m_serial->Write(req)) {
        errorMsg = _T("Eroare: nu s-a putut trimite comanda către modulul slave.");
        return false;
    }

    unsigned int expected = 5 + 2 * count;
    std::vector<uint8_t> resp;

    if (!m_serial->Read(resp, expected, 200)) {
        errorMsg = _T("Eroare: modulul slave nu a răspuns în timp util.");
        return false;
    }

    if (resp.size() < expected) {
        errorMsg.Format(_T("Eroare: răspuns incomplet. Așteptat %u, primit %zu."), expected, resp.size());
        return false;
    }

    uint16_t crcCalc = CalcCRC(resp.data(), expected - 2);
    uint16_t crcRecv = uint16_t(resp[expected - 2]) | (uint16_t(resp[expected - 1]) << 8);

    if (crcCalc != crcRecv) {
        errorMsg = _T("Eroare CRC: datele primite sunt corupte.");
        return false;
    }

    out.resize(count);
    for (int i = 0; i < (int)count; ++i) {
        out[i] = (uint16_t(resp[3 + 2 * i]) << 8) | resp[4 + 2 * i];
    }

    return true;
}



bool ModbusRTU::WriteSingleRegister(uint8_t slaveId,
    uint16_t address,
    uint16_t value)
{
    std::vector<uint8_t> req(8);
    req[0] = slaveId;
    req[1] = 0x06;
    req[2] = address >> 8;
    req[3] = address & 0xFF;
    req[4] = value >> 8;
    req[5] = value & 0xFF;
    uint16_t crc = CalcCRC(req.data(), 6);
    req[6] = crc & 0xFF;
    req[7] = crc >> 8;

    std::vector<uint8_t> resp;
    if (!SendFrame(req, resp, 8)) return false;

    
    return std::memcmp(req.data(), resp.data(), 8) == 0;
}
