// ArchiveCipher.h

// 存档轻量加解密与完整性校验,附确定性校验和后基于密钥流式 XOR,纯标准库不依赖 SFML
#pragma once

#include <string>

// 存档明文的轻量对称加解密与校验和封装,仅防随手改档,非真正安全
namespace ArchiveCipher {

// 附明文的确定性校验和后做基于密钥的流式 XOR,返回作字节容器的密文串,可含 \0
std::string encrypt(const std::string& plaintext);

// 逆变换并校验校验和,一致写出明文返回 true,篡改损坏或过短畸形输入返回 false
bool decrypt(const std::string& ciphertext, std::string& outPlaintext);

}  // namespace ArchiveCipher
