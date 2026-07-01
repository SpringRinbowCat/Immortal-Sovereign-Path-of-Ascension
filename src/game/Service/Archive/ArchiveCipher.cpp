// ArchiveCipher.cpp

// 存档轻量加解密与校验和实现,CRC32 完整性校验加密钥流式 XOR,篡改可检测
#include "Service/Archive/ArchiveCipher.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

namespace {

// CRC32 标准反射多项式与初值,跨进程稳定,不用 std::hash
constexpr std::uint32_t kCrc32Init = 0xFFFFFFFFu;
constexpr std::uint32_t kCrc32Polynomial = 0xEDB88420u;

// 字节与位运算基准
constexpr int kBitsPerByte = 8;
constexpr std::uint32_t kByteMask = 0xFFu;
constexpr std::uint32_t kLsbMask = 1u;
constexpr std::uint32_t kHighByteShift = 24u;

// 明文前缀的校验和字节数,小端序序列化
constexpr std::size_t kChecksumSize = 4;
static_assert(kChecksumSize == sizeof(std::uint32_t), "校验和字节数须与 uint32 一致");

// 加密定位权衡,客户端本地加密仅为轻量防随手改档,密钥随包分发不具真正安全性,真正防作弊依赖后续服务器权威
// 密钥不以单个明文具名常量硬编码,下列多段分散种子在运行期派生出真正的密钥字节以作混淆
constexpr std::uint32_t kKeySeedA = 0x9E3779B9u;
constexpr std::uint32_t kKeySeedB = 0x85EBCA77u;
constexpr std::uint32_t kKeySeedC = 0xC2B2AE3Du;
constexpr std::uint32_t kKeySeedD = 0x27D4EB2Fu;
constexpr std::size_t kKeyLength = 32;

// xorshift32 位移量,用于把多段种子扩散成密钥字节
constexpr int kXorShiftLeftA = 13;
constexpr int kXorShiftRightB = 17;
constexpr int kXorShiftLeftC = 5;

std::uint32_t crc32(const std::string& data)
{
    std::uint32_t crc = kCrc32Init;
    for (const char ch : data)
    {
        crc ^= static_cast<unsigned char>(ch);
        for (int bit = 0; bit < kBitsPerByte; ++bit)
        {
            const bool lsbSet = (crc & kLsbMask) != 0u;
            crc >>= 1;
            if (lsbSet)
            {
                crc ^= kCrc32Polynomial;
            }
        }
    }
    return crc ^ kCrc32Init;
}

// 逐字节扩散多段种子,单个种子不等于任何密钥字节
std::uint32_t mixState(std::uint32_t state, std::uint32_t salt)
{
    state += salt + kKeySeedC;
    state ^= state << kXorShiftLeftA;
    state ^= state >> kXorShiftRightB;
    state ^= state << kXorShiftLeftC;
    return state * kKeySeedD;
}

std::string deriveKey()
{
    std::uint32_t state = kKeySeedA ^ kKeySeedB ^ kKeySeedC ^ kKeySeedD;

    std::string key;
    key.reserve(kKeyLength);
    for (std::size_t i = 0; i < kKeyLength; ++i)
    {
        state = mixState(state, static_cast<std::uint32_t>(i));
        key.push_back(static_cast<char>((state >> kHighByteShift) & kByteMask));
    }
    return key;
}

// 对称变换,加解密共用,keystream 只依赖密钥与位置,故可逆
void applyKeystream(std::string& buffer, const std::string& key)
{
    if (key.empty())
    {
        return;
    }

    const std::size_t keyLen = key.size();
    for (std::size_t i = 0; i < buffer.size(); ++i)
    {
        const unsigned char keyByte = static_cast<unsigned char>(key[i % keyLen]);
        const unsigned char posByte = static_cast<unsigned char>(i & kByteMask);
        const unsigned char cur = static_cast<unsigned char>(buffer[i]);
        buffer[i] = static_cast<char>(cur ^ keyByte ^ posByte);
    }
}

// 校验和以小端序追加,跨平台字节序确定
void appendChecksum(std::string& out, std::uint32_t checksum)
{
    for (std::size_t i = 0; i < kChecksumSize; ++i)
    {
        const std::uint32_t shift = static_cast<std::uint32_t>(i) * kBitsPerByte;
        out.push_back(static_cast<char>((checksum >> shift) & kByteMask));
    }
}

std::uint32_t readChecksum(const std::string& frame)
{
    std::uint32_t checksum = 0;
    for (std::size_t i = 0; i < kChecksumSize; ++i)
    {
        const std::uint32_t byte = static_cast<unsigned char>(frame[i]);
        const std::uint32_t shift = static_cast<std::uint32_t>(i) * kBitsPerByte;
        checksum |= byte << shift;
    }
    return checksum;
}

}  // namespace

namespace ArchiveCipher {

std::string encrypt(const std::string& plaintext)
{
    const std::uint32_t checksum = crc32(plaintext);

    std::string frame;
    frame.reserve(kChecksumSize + plaintext.size());
    appendChecksum(frame, checksum);
    frame.append(plaintext);

    const std::string key = deriveKey();
    applyKeystream(frame, key);  // 就地变换,frame 变为密文
    return frame;
}

bool decrypt(const std::string& ciphertext, std::string& outPlaintext)
{
    if (ciphertext.size() < kChecksumSize)
    {
        return false;  // 连校验和都容不下,判畸形
    }

    try
    {
        std::string frame = ciphertext;
        const std::string key = deriveKey();
        applyKeystream(frame, key);  // 对称逆变换,还原校验和与明文

        const std::uint32_t stored = readChecksum(frame);
        std::string plaintext = frame.substr(kChecksumSize);
        if (crc32(plaintext) != stored)
        {
            return false;  // 校验和不符,篡改或损坏
        }

        outPlaintext = std::move(plaintext);
        return true;
    }
    catch (...)
    {
        return false;  // 任何异常兜底,不逃逸到调用方
    }
}

}  // namespace ArchiveCipher
