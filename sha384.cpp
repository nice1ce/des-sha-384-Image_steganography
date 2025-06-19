#include "sha384.h"

// Начальные значения хеша для SHA-384
const quint64 SHA384::H0[8] = {
    0xcbbb9d5dc1059ed8ULL, 0x629a292a367cd507ULL, 0x9159015a3070dd17ULL, 0x152fecd8f70e5939ULL,
    0x67332667ffc00b31ULL, 0x8eb44a8768581511ULL, 0xdb0c2e0d64f98fa7ULL, 0x47b5481dbefa4fa4ULL
};

// Константы раунда для SHA-384/SHA-512
const quint64 SHA384::K[80] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
    0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
    0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
    0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
    0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
    0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
    0xd192e819d6ef5218ULL, 0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
    0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
    0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
    0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
    0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
    0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

SHA384::SHA384(QObject *parent) : QObject(parent), bitCount(0)
{
    initialize();
}

QByteArray SHA384::hash(const QByteArray &data)
{
    // Инициализация состояния
    initialize();
    
    // Копируем данные в буфер
    buffer = data;
    
    // Дополняем данные
    buffer = padData(buffer);
    
    // Обрабатываем блоки
    for (int i = 0; i < buffer.size(); i += BLOCK_SIZE) {
        processBlock(reinterpret_cast<const quint8 *>(buffer.constData() + i));
    }
    
    // Финализируем хеш
    finalize();
    
    // Преобразуем состояние в массив байтов
    QByteArray result;
    result.resize(HASH_SIZE);
    wordsToBytes(state, reinterpret_cast<quint8 *>(result.data()), 6); // Только первые 6 слов (384 бита)
    
    return result;
}

void SHA384::hash(const uint8_t* data, size_t length, uint8_t* output)
{
    // Инициализация состояния
    initialize();
    
    // Обновляем контекст хеширования данными
    update(data, length);
    
    // Финализируем хеш и записываем результат
    finalize(output);
}

void SHA384::update(const uint8_t* data, size_t length)
{
    // Добавляем данные в буфер
    buffer.append(reinterpret_cast<const char*>(data), length);
    
    // Обрабатываем полные блоки
    while (buffer.size() >= BLOCK_SIZE) {
        processBlock(reinterpret_cast<const quint8*>(buffer.constData()));
        buffer.remove(0, BLOCK_SIZE);
    }
    
    // Обновляем счетчик бит
    bitCount += length * 8;
}

void SHA384::finalize(uint8_t* output)
{
    // Дополняем оставшиеся данные
    QByteArray paddedData = padData(buffer);
    
    // Обрабатываем оставшиеся блоки
    for (int i = 0; i < paddedData.size(); i += BLOCK_SIZE) {
        processBlock(reinterpret_cast<const quint8*>(paddedData.constData() + i));
    }
    
    // Преобразуем состояние в массив байтов
    wordsToBytes(state, output, 6); // Только первые 6 слов (384 бита)
    
    // Сбрасываем состояние
    initialize();
}

QString SHA384::hashToHex(const QByteArray &data)
{
    return hash(data).toHex();
}

QString SHA384::lastError() const
{
    return m_lastError;
}

void SHA384::initialize()
{
    // Копируем начальные значения хеша
    for (int i = 0; i < 8; i++) {
        state[i] = H0[i];
    }
    
    // Сбрасываем счетчик бит
    bitCount = 0;
    
    // Очищаем буфер
    buffer.clear();
}

void SHA384::processBlock(const quint8 *block)
{
    // Преобразуем блок в 16 64-битных слов
    quint64 w[80];
    bytesToWords(block, w, 16);
    
    // Расширяем 16 слов до 80 слов
    for (int i = 16; i < 80; i++) {
        w[i] = gamma1(w[i - 2]) + w[i - 7] + gamma0(w[i - 15]) + w[i - 16];
    }
    
    // Инициализируем рабочие переменные
    quint64 a = state[0];
    quint64 b = state[1];
    quint64 c = state[2];
    quint64 d = state[3];
    quint64 e = state[4];
    quint64 f = state[5];
    quint64 g = state[6];
    quint64 h = state[7];
    
    // Основной цикл
    for (int i = 0; i < 80; i++) {
        quint64 t1 = h + sigma1(e) + ch(e, f, g) + K[i] + w[i];
        quint64 t2 = sigma0(a) + maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    
    // Обновляем состояние
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
    
    // Обновляем счетчик бит
    bitCount += BLOCK_SIZE * 8;
}

void SHA384::finalize()
{
    // Ничего не делаем, так как дополнение данных и обработка блоков уже выполнены
}

quint64 SHA384::ch(quint64 x, quint64 y, quint64 z)
{
    return (x & y) ^ (~x & z);
}

quint64 SHA384::maj(quint64 x, quint64 y, quint64 z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

quint64 SHA384::sigma0(quint64 x)
{
    return ((x >> 28) | (x << 36)) ^ ((x >> 34) | (x << 30)) ^ ((x >> 39) | (x << 25));
}

quint64 SHA384::sigma1(quint64 x)
{
    return ((x >> 14) | (x << 50)) ^ ((x >> 18) | (x << 46)) ^ ((x >> 41) | (x << 23));
}

quint64 SHA384::gamma0(quint64 x)
{
    return ((x >> 1) | (x << 63)) ^ ((x >> 8) | (x << 56)) ^ (x >> 7);
}

quint64 SHA384::gamma1(quint64 x)
{
    return ((x >> 19) | (x << 45)) ^ ((x >> 61) | (x << 3)) ^ (x >> 6);
}

void SHA384::bytesToWords(const quint8 *bytes, quint64 *words, int size)
{
    for (int i = 0; i < size; i++) {
        words[i] = ((quint64)bytes[i * 8] << 56) |
                   ((quint64)bytes[i * 8 + 1] << 48) |
                   ((quint64)bytes[i * 8 + 2] << 40) |
                   ((quint64)bytes[i * 8 + 3] << 32) |
                   ((quint64)bytes[i * 8 + 4] << 24) |
                   ((quint64)bytes[i * 8 + 5] << 16) |
                   ((quint64)bytes[i * 8 + 6] << 8) |
                   ((quint64)bytes[i * 8 + 7]);
    }
}

void SHA384::wordsToBytes(const quint64 *words, quint8 *bytes, int size)
{
    for (int i = 0; i < size; i++) {
        bytes[i * 8] = (words[i] >> 56) & 0xFF;
        bytes[i * 8 + 1] = (words[i] >> 48) & 0xFF;
        bytes[i * 8 + 2] = (words[i] >> 40) & 0xFF;
        bytes[i * 8 + 3] = (words[i] >> 32) & 0xFF;
        bytes[i * 8 + 4] = (words[i] >> 24) & 0xFF;
        bytes[i * 8 + 5] = (words[i] >> 16) & 0xFF;
        bytes[i * 8 + 6] = (words[i] >> 8) & 0xFF;
        bytes[i * 8 + 7] = words[i] & 0xFF;
    }
}

QByteArray SHA384::padData(const QByteArray &data)
{
    // Вычисляем размер дополнения
    quint64 originalBitLength = data.size() * 8;
    int paddingLength = BLOCK_SIZE - ((data.size() + 16) % BLOCK_SIZE);
    if (paddingLength < 1) {
        paddingLength += BLOCK_SIZE;
    }
    
    // Создаем дополненные данные
    QByteArray paddedData = data;
    
    // Добавляем бит '1'
    paddedData.append(static_cast<char>(0x80));
    
    // Добавляем нули
    for (int i = 0; i < paddingLength - 1; i++) {
        paddedData.append(static_cast<char>(0x00));
    }
    
    // Добавляем длину исходных данных в битах (128 бит = 16 байт)
    quint64 highBits = 0; // Для данных меньше 2^64 бит верхние 64 бита равны 0
    quint64 lowBits = originalBitLength;
    
    // Добавляем верхние 64 бита (старшие байты идут первыми)
    for (int i = 7; i >= 0; i--) {
        paddedData.append(static_cast<char>((highBits >> (i * 8)) & 0xFF));
    }
    
    // Добавляем нижние 64 бита (старшие байты идут первыми)
    for (int i = 7; i >= 0; i--) {
        paddedData.append(static_cast<char>((lowBits >> (i * 8)) & 0xFF));
    }
    
    return paddedData;
}

