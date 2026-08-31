#ifndef TCP_CHAT_TCP_FRAMING_HPP
#define TCP_CHAT_TCP_FRAMING_HPP

#include <QByteArray>
#include <QtEndian>

// TCP is a byte stream: a read may contain part of a message or several
// messages. Every application payload is therefore prefixed with its size.
namespace TcpFraming {

    /**
     * @brief Number of bytes used to encode the payload length prefix.
     */
    constexpr int HeaderSize = sizeof(quint32);

    /**
     * @brief Maximum accepted payload size in bytes.
     *
     * This safeguards the receiver from oversized frames that could exhaust
     * memory or be used for denial-of-service attempts.
     */
    constexpr quint32 MaxPayloadSize = 16 * 1024 * 1024;

    /**
     * @brief Result of trying to extract one complete framed payload.
     */
    enum class ReadResult { Incomplete, Complete, Invalid };

    /**
     * @brief Prefixes a payload with its big-endian length header.
     * @param payload Serialized application payload.
     * @return Framed payload, or an empty byte array if the payload is too large.
     */
    inline QByteArray frame(const QByteArray &payload) {
        if (payload.size() > static_cast<qsizetype>(MaxPayloadSize)) {
            return {};
        }

        QByteArray framedPayload(TcpFraming::HeaderSize, Qt::Uninitialized);
        qToBigEndian(static_cast<quint32>(payload.size()), framedPayload.data());
        framedPayload.append(payload);
        return framedPayload;
    }

    /**
     * @brief Extracts the next complete payload from a receive buffer.
     * @param buffer Accumulated TCP bytes; consumed when a frame is completed.
     * @param payload Destination for the extracted application payload.
     * @return Complete when a frame was extracted, Incomplete when more bytes
     *         are required, or Invalid when the frame header is unsafe.
     */
    inline ReadResult takeNextFrame(QByteArray &buffer, QByteArray &payload) {
        if (buffer.size() < HeaderSize) {
            return ReadResult::Incomplete;
        }

        const auto *header = reinterpret_cast<const uchar *>(buffer.constData());
        const quint32 payloadSize = qFromBigEndian<quint32>(header);
        if (payloadSize > MaxPayloadSize) {
            return ReadResult::Invalid;
        }

        const qsizetype frameSize = HeaderSize + static_cast<qsizetype>(payloadSize);
        if (buffer.size() < frameSize) {
            return ReadResult::Incomplete;
        }

        payload = buffer.mid(HeaderSize, payloadSize);
        buffer.remove(0, frameSize);
        return ReadResult::Complete;
    }
} // namespace TcpFraming

#endif // TCP_CHAT_TCP_FRAMING_HPP
