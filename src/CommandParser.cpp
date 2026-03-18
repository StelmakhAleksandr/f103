#include "CommandParser.h"

void CommandParser::poll()
{
    while (true) {
        const auto byteOpt = m_usart.receiveByte();
        if (!byteOpt.has_value()) {
            break;
        }
        processByte(byteOpt.value());
    }
}

void CommandParser::processByte(char byte)
{
    if (byte == '\n' || byte == '\r') {
        if (m_commandLength > 0) {
            m_commandBuffer[m_commandLength] = '\0';
            m_callback(m_commandBuffer.data());
            m_commandLength = 0;
        }
    } else {
        if (m_commandLength < m_commandBuffer.size() - 1) {
            m_commandBuffer[m_commandLength++] = byte;
        }
    }
}