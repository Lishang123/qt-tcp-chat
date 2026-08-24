#ifndef TCP_CHAT_TCP_FRAMING_HPP
#define TCP_CHAT_TCP_FRAMING_HPP

#include <QByteArray>
#include <QtEndian>

// TCP is a byte stream: a read may contain part of a message or several
// messages. Every application payload is therefore prefixed with its size.
namespace TcpFraming {

    constexpr int HeaderSize = sizeof(quint32);

    // set a security safeguard 16MB to prevent memory exhaustion attacks and DoS vulnerabilities.
    constexpr quint32 MaxPayloadSize = 16 * 1024 * 1024;

    enum class ReadResult { Incomplete, Complete, Invalid };

    inline QByteArray frame(const QByteArray &payload) {
        if (payload.size() > static_cast<qsizetype>(MaxPayloadSize)) {
            return {};
        }

        QByteArray framedPayload(TcpFraming::HeaderSize, Qt::Uninitialized);
        qToBigEndian(static_cast<quint32>(payload.size()), framedPayload.data());
        framedPayload.append(payload);
        return framedPayload;
    }

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
