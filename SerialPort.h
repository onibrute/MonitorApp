#pragma once
#include "ISerialProtocol.h"
#include <windows.h>

class SerialPort : public ISerialProtocol
{
public:
    SerialPort();
    virtual ~SerialPort();

    bool Open(const std::string& portName, int baudRate) override;
    void Close() override;
    bool Write(const std::vector<uint8_t>& data) override;
    bool Read(std::vector<uint8_t>& buffer, size_t size, unsigned int timeoutMs) override;
    bool IsOpen() const override { return m_hComm != INVALID_HANDLE_VALUE; }

private:
    HANDLE m_hComm;
};
