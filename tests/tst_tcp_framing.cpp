#include <QtTest/QtTest>

#include "common/TcpFraming.hpp"

class TcpFramingTest : public QObject {
    Q_OBJECT

private slots:
    void framePrefixesPayloadWithBigEndianSize();
    void emptyPayloadIsAValidFrame();
    void incompleteHeaderWaitsForMoreData();
    void incompletePayloadKeepsBufferedData();
    void completeFrameExtractsPayloadAndRemovesConsumedBytes();
    void multipleFramesCanBeReadFromOneBuffer();
    void oversizedPayloadIsRejectedWhenWriting();
    void oversizedIncomingFrameIsInvalid();
};

void TcpFramingTest::framePrefixesPayloadWithBigEndianSize()
{
    const QByteArray payload("hello");

    const QByteArray framed = TcpFraming::frame(payload);

    QCOMPARE(framed.size(), TcpFraming::HeaderSize + payload.size());
    QCOMPARE(static_cast<uchar>(framed.at(0)), 0x00);
    QCOMPARE(static_cast<uchar>(framed.at(1)), 0x00);
    QCOMPARE(static_cast<uchar>(framed.at(2)), 0x00);
    QCOMPARE(static_cast<uchar>(framed.at(3)), 0x05);
    QCOMPARE(framed.mid(TcpFraming::HeaderSize), payload);
}

void TcpFramingTest::emptyPayloadIsAValidFrame()
{
    QByteArray buffer = TcpFraming::frame({});
    QByteArray payload("unchanged");

    const auto result = TcpFraming::takeNextFrame(buffer, payload);

    QCOMPARE(result, TcpFraming::ReadResult::Complete);
    //  If takeNextFrame() returns Complete, the payload output parameter should
    //  always contain exactly the payload from that frame. For a zero-length frame,
    //  that means it must be empty.
    QVERIFY(payload.isEmpty());
    QVERIFY(buffer.isEmpty());
}

void TcpFramingTest::incompleteHeaderWaitsForMoreData()
{
    QByteArray buffer("\x00\x00", 2);
    QByteArray payload("unchanged");

    const auto result = TcpFraming::takeNextFrame(buffer, payload);

    QCOMPARE(result, TcpFraming::ReadResult::Incomplete);
    QCOMPARE(buffer, QByteArray("\x00\x00", 2));
    QCOMPARE(payload, QByteArray("unchanged"));
}

void TcpFramingTest::incompletePayloadKeepsBufferedData()
{
    QByteArray buffer;
    buffer.append(QByteArray("\x00\x00\x00\x05", TcpFraming::HeaderSize));
    buffer.append("he");
    QByteArray payload("unchanged");

    const auto result = TcpFraming::takeNextFrame(buffer, payload);

    QCOMPARE(result, TcpFraming::ReadResult::Incomplete);
    QCOMPARE(buffer, QByteArray("\x00\x00\x00\x05he", TcpFraming::HeaderSize + 2));
    QCOMPARE(payload, QByteArray("unchanged"));
}

void TcpFramingTest::completeFrameExtractsPayloadAndRemovesConsumedBytes()
{
    QByteArray buffer = TcpFraming::frame("hello");
    buffer.append("tail");
    QByteArray payload;

    const auto result = TcpFraming::takeNextFrame(buffer, payload);

    QCOMPARE(result, TcpFraming::ReadResult::Complete);
    QCOMPARE(payload, QByteArray("hello"));
    QCOMPARE(buffer, QByteArray("tail"));
}

void TcpFramingTest::multipleFramesCanBeReadFromOneBuffer()
{
    QByteArray buffer = TcpFraming::frame("one");
    buffer.append(TcpFraming::frame("two"));
    QByteArray payload;

    QCOMPARE(TcpFraming::takeNextFrame(buffer, payload), TcpFraming::ReadResult::Complete);
    QCOMPARE(payload, QByteArray("one"));

    QCOMPARE(TcpFraming::takeNextFrame(buffer, payload), TcpFraming::ReadResult::Complete);
    QCOMPARE(payload, QByteArray("two"));
    QVERIFY(buffer.isEmpty());
}

void TcpFramingTest::oversizedPayloadIsRejectedWhenWriting()
{
    const QByteArray payload(static_cast<qsizetype>(TcpFraming::MaxPayloadSize) + 1, 'x');

    QVERIFY(TcpFraming::frame(payload).isEmpty());
}

void TcpFramingTest::oversizedIncomingFrameIsInvalid()
{
    const quint32 payloadSize = qToBigEndian(TcpFraming::MaxPayloadSize + 1);
    QByteArray buffer(reinterpret_cast<const char *>(&payloadSize), TcpFraming::HeaderSize);
    QByteArray payload("unchanged");

    const auto result = TcpFraming::takeNextFrame(buffer, payload);

    QCOMPARE(result, TcpFraming::ReadResult::Invalid);
    QCOMPARE(payload, QByteArray("unchanged"));
}

// With Q_OBJECT inside a .cpp, the standard pattern is
QTEST_MAIN(TcpFramingTest)
#include "tst_tcp_framing.moc"
