// Drives the real TeslaController against a recording UART and checks the bytes
// it puts on the wire against the format TWCManager documents.
#include <cstdio>
#include <cstring>
#include <vector>
#include "twc_protocol.h"
#include "twc_controller.h"

using namespace esphome::twc_controller;

static int failures = 0;
static void check(bool ok, const char *what) {
    printf("%-6s %s\n", ok ? "ok" : "FAIL", what);
    if (!ok) failures++;
}

// io.h declares this virtual but not pure and never defines it, so it is the
// key function and nothing emits the vtable. Give it a body for the test link.
void esphome::twc_controller::TeslaControllerIO::resetIO(uint16_t) {}

// Minimal concrete IO so we can instantiate the controller.
class NullIO : public TeslaControllerIO {
 public:
    void resetIO(uint16_t) {}
    void writeActualCurrent(uint8_t) {}
    void writeCharger(uint16_t, uint8_t) {}
    void writeChargerCurrent(uint16_t, uint8_t, uint8_t) {}
    void writeChargerSerial(uint16_t, std::string) {}
    void writeChargerTotalKwh(uint16_t, uint32_t) {}
    void writeChargerVoltage(uint16_t, uint16_t, uint8_t) {}
    void writeTotalConnectedChargers(uint8_t) {}
    void writeChargerFirmware(uint16_t, std::string) {}
    void writeChargerActualCurrent(uint16_t, uint8_t) {}
    void writeChargerTotalPhaseCurrent(uint8_t, uint8_t) {}
    void writeChargerConnectedVin(uint16_t, std::string) {}
    void writeChargerState(uint16_t, uint8_t) {}
    void writeTotalConnectedCars(uint8_t) {}
    void writeRaw(uint8_t *, size_t) {}
    void writeRawPacket(uint8_t *, size_t) {}
    void onCurrentMessage(std::function<void(uint8_t)>) {}
};

// Undo SendData's SLIP framing: C0 <escaped payload> C0 FF
static std::vector<uint8_t> unslip(const std::vector<uint8_t> &raw) {
    std::vector<uint8_t> out;
    size_t i = 0;
    if (i < raw.size() && raw[i] == 0xC0) i++;
    for (; i < raw.size() && raw[i] != 0xC0; i++) {
        if (raw[i] == 0xDB && i + 1 < raw.size()) {
            i++;
            out.push_back(raw[i] == 0xDC ? 0xC0 : 0xDB);
        } else {
            out.push_back(raw[i]);
        }
    }
    return out;
}

// TWCManager: checksum = sum(msg[1 .. len-2]) & 0xFF, compared to msg[len-1]
static bool twcmanager_checksum_ok(const std::vector<uint8_t> &m) {
    if (m.size() < 3) return false;
    unsigned sum = 0;
    for (size_t i = 1; i < m.size() - 1; i++) sum += m[i];
    return (sum & 0xFF) == m[m.size() - 1];
}

static void dump(const char *label, const std::vector<uint8_t> &m) {
    printf("       %s (%zu bytes): ", label, m.size());
    for (uint8_t b : m) printf("%02X ", b);
    printf("\n");
}

// Frame a packet the way a real TWC would put it on the wire, so we can feed
// it into Handle().
static void feed(esphome::uart::UARTComponent &u, std::vector<uint8_t> body) {
    unsigned sum = 0;
    for (size_t i = 1; i < body.size(); i++) sum += body[i];
    body.push_back(sum & 0xFF);
    u.rx.push_back(0xC0);
    for (uint8_t b : body) u.rx.push_back(b);
    u.rx.push_back(0xC0);
    u.rx.push_back(0xFF);
}

int main() {
    esphome::uart::UARTComponent uart;
    NullIO io;
    TeslaController twc(&uart, &io, 0xABCD, nullptr, 0);

    twc.SetMinCurrent(0);
    twc.SetMaxCurrent(80);

    printf("== presence ==\n");
    uart.tx.clear();
    twc.SendPresence();
    auto presence = unslip(uart.tx);
    dump("presence", presence);
    check(twcmanager_checksum_ok(presence), "presence checksum matches TWCManager");
    uint16_t adv = (presence[5] << 8) | presence[6];
    check(adv == 8000, "presence advertises configured max (80.00A), not hardcoded 32A");

    printf("\n== protocol 2 heartbeat (default) ==\n");
    twc.SetCurrent(80);
    uart.tx.clear();
    twc.SendHeartbeat(0x1234);
    auto hb2 = unslip(uart.tx);
    dump("heartbeat", hb2);
    check(hb2.size() == 16, "protocol 2 heartbeat is 16 bytes");
    check(hb2[6] == 0x09, "protocol 2 uses opcode 0x09");
    check(((hb2[7] << 8) | hb2[8]) == 8000, "encodes 80.00A as 0x1F40");
    check(twcmanager_checksum_ok(hb2), "heartbeat checksum matches TWCManager");

    printf("\n== a protocol 1 TWC announces itself (14 byte presence) ==\n");
    // SECONDARY_PRESENCE 0xFDE2, 14 bytes total incl. checksum
    feed(uart, {0xFD, 0xE2, 0x77, 0x77, 0x01, 0x1F, 0x40, 0, 0, 0, 0, 0, 0});
    twc.Handle();

    twc.SetCurrent(80);
    uart.tx.clear();
    twc.SendHeartbeat(0x7777);
    auto hb1 = unslip(uart.tx);
    dump("heartbeat", hb1);
    check(hb1.size() == 14, "protocol 1 heartbeat is 14 bytes");
    check(hb1[6] == 0x05, "protocol 1 uses opcode 0x05");
    check(((hb1[7] << 8) | hb1[8]) == 8000, "still encodes 80.00A");
    check(twcmanager_checksum_ok(hb1), "heartbeat checksum matches TWCManager");

    printf("\n== 0A stop command ==\n");
    twc.SetCurrent(0);
    uart.tx.clear();
    twc.SendHeartbeat(0x7777);
    auto stop = unslip(uart.tx);
    dump("heartbeat", stop);
    check(stop[6] == 0x05, "stop still sends a limit opcode, not 0x00");
    check(((stop[7] << 8) | stop[8]) == 0, "stop encodes 0.00A");
    check(twcmanager_checksum_ok(stop), "stop checksum matches TWCManager");

    printf("\n%s\n", failures ? "FAILURES PRESENT" : "all wire-format checks pass");
    return failures;
}
