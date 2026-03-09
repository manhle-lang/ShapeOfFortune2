
#ifndef RG_RNG_GLOBAL
#define RG_RNG_GLOBAL
#include <string>
#include <vector>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

static char *checkENV(const char *env_var)
{
    char *value = std::getenv(env_var);
    if (value)
    {
        // std::cout << env_var << " = " << value << std::endl;
        return value;
    }
    else
    {
        // std::cout << "[Warning]: " << env_var << " is not set." << std::endl;
        return nullptr;
    }
}

static bool IS_RG_RNG_ENABLE_BY_ENV()
{
    if (checkENV("IS_RG_RNG_ENABLE"))
    {
        return true;
    }
    return false;
}

namespace RgRNG
{
    struct ServerState
    {
        string serverSeed;
        string clientSeedHash;
        int round;
        int cursor;
    };

    struct ClientState
    {
        string serverSeedHash;
        string clientSeed;
        int nonce;
    };

    class RNG
    {
    public:
        RNG(
            const RgRNG::ServerState &serverState,
            const RgRNG::ClientState &clientState)
        {
            server = serverState;
            client = clientState;

            mac = EVP_MAC_fetch(nullptr, "HMAC", nullptr);
            if (!mac)
                throw std::runtime_error("EVP_MAC_fetch failed");

            ctx = EVP_MAC_CTX_new(mac);
            if (!ctx)
                throw std::runtime_error("EVP_MAC_CTX_new failed");
        }

        ~RNG()
        {
            EVP_MAC_CTX_free(ctx);
            EVP_MAC_free(mac);
        }

        double Next()
        {
            if (this->server.cursor + 8 <= 32)
            {
                std::string msg = client.clientSeed + ":" +
                                  std::to_string(client.nonce) + ":" +
                                  std::to_string(server.round) + ":" +
                                  std::to_string(server.cursor);

                // cout << "Next msg: " << msg << endl;

                vector<unsigned char> buffer = hmac_sha256_raw(this->server.serverSeed, msg, ctx);
                uint64_t result = 0;
                for (int i = 0; i < 8; ++i)
                {
                    result += static_cast<uint64_t>(buffer[this->server.cursor + i]) << (8 * (7 - i));
                }

                this->server.cursor += 8;
                return static_cast<double>(result) / TWO_POW_64;
            }

            this->server.cursor = 0;
            this->server.round += 1;
            return Next();
        }

        void Enable() { this->isEnable = true; }
        void Disable()
        {
            if (!IS_RG_RNG_ENABLE_BY_ENV())
            {
                this->isEnable = false;
            }
        }
        bool IsEnable() const { return this->isEnable; }

        int RandomInt(int min, int max)
        {
            // cout << "Random using RgRNG" << endl;
            if (max <= min)
            {
                return min;
            }
            return static_cast<int>(Next() * (max - min)) + min;
        }

        double RandomDouble()
        {
            return Next();
        }

        int GetWeightedIndex(const vector<int> &weightedVec)
        {
            int totalWeight = 0;

            // Sum up the total weight of the vector
            for (size_t i = 0; i < weightedVec.size(); i++)
                totalWeight += weightedVec[i];

            int randomWeightage = RandomInt(0, totalWeight);
            int loopWeight = 0;

            for (size_t i = 0; i < weightedVec.size(); i++)
            {
                loopWeight += weightedVec[i];
                if (loopWeight > randomWeightage)
                    return i;
            }

            return -1; // Should not reach here if weights are positive
        }

        int GetNonce() const
        {
            return this->client.nonce;
        }

        string GetServerSeed() const
        {
            return this->server.serverSeed;
        }

        void SetState(const RgRNG::ServerState &serverState, const RgRNG::ClientState &clientState)
        {
            this->server = serverState;
            this->client = clientState;
        }

        void SetStateByCheatSeed(string cheatSeed)
        {
            if (!cheatSeed.empty())
            {
                // cout << "SetStateByCheatSeed - cheatSeed: " << cheatSeed << endl;
                // cheatSeed example: "serverSeed:clientSeed:nonce:round:cursor"
                string serverSeed = cheatSeed.substr(0, cheatSeed.find(':'));
                cheatSeed = cheatSeed.substr(cheatSeed.find(':') + 1);
                string clientSeed = cheatSeed.substr(0, cheatSeed.find(':'));
                cheatSeed = cheatSeed.substr(cheatSeed.find(':') + 1);

                string nonceStr = cheatSeed.substr(0, cheatSeed.find(':'));
                if (nonceStr.empty()) {
                    SetServerSeed(serverSeed);
                    SetClientSeed(clientSeed);
                    return;
                }

                int nonce = stoi(nonceStr);
                cheatSeed = cheatSeed.substr(cheatSeed.find(':') + 1);
                int round = stoi(cheatSeed.substr(0, cheatSeed.find(':')));
                cheatSeed = cheatSeed.substr(cheatSeed.find(':') + 1);
                int cursor = stoi(cheatSeed);
                const RgRNG::ServerState serverState = {
                    serverSeed,
                    RNG::hmac_sha256_key_only(clientSeed),
                    round,
                    cursor};
                const RgRNG::ClientState clientState = {
                    RNG::hmac_sha256_key_only(serverSeed),
                    clientSeed,
                    nonce,
                };
                SetState(serverState, clientState);
            }
        }

        string GetCheatSeedFromState() {
            return this->server.serverSeed + ":" +
                   this->client.clientSeed + ":" +
                   to_string(this->client.nonce) + ":" +
                   to_string(this->server.round) + ":" +
                   to_string(this->server.cursor);
        }

        void SetServerSeed(string serverSeed)
        {
            this->server.serverSeed = serverSeed;
        }

        void SetClientSeed(string clientSeed)
        {
            this->client.clientSeed = clientSeed;
        }

        void SetNonce(int nonce)
        {
            this->client.nonce = nonce;
        }

        void SetRound(int round)
        {
            this->server.round = round;
        }

        void SetCursor(int cursor)
        {
            this->server.cursor = cursor;
        }

        RgRNG::ServerState GetServerState() const
        {
            return this->server;
        }

        RgRNG::ClientState GetClientState() const
        {
            return this->client;
        }

        EVP_MAC *mac;
        EVP_MAC_CTX *ctx;
        const double TWO_POW_64 = 18446744073709551616.0;
        static string random_bytes_hex(size_t byte_len)
        {
            vector<unsigned char> buffer(byte_len);

            if (RAND_bytes(buffer.data(), byte_len) != 1)
            {
                throw runtime_error("RAND_bytes failed");
            }

            ostringstream oss;
            for (auto b : buffer)
            {
                oss << hex << setw(2) << setfill('0') << static_cast<int>(b);
            }
            return oss.str();
        }

        static vector<uint8_t> hmac_sha256_raw(const string &key, const string &message, EVP_MAC_CTX *ctx)
        {
            std::vector<uint8_t> result(32); // SHA256 = 32 bytes
            size_t out_len = 0;

            OSSL_PARAM params[] = {
                OSSL_PARAM_construct_utf8_string("digest", (char *)"SHA256", 0),
                OSSL_PARAM_construct_end()};

            if (EVP_MAC_init(ctx, reinterpret_cast<const unsigned char *>(key.data()), key.size(), params) <= 0)
                throw std::runtime_error("EVP_MAC_init failed");

            if (EVP_MAC_update(ctx, reinterpret_cast<const unsigned char *>(message.data()), message.size()) <= 0)
                throw std::runtime_error("EVP_MAC_update failed");

            if (EVP_MAC_final(ctx, result.data(), &out_len, result.size()) <= 0)
                throw std::runtime_error("EVP_MAC_final failed");

            result.resize(out_len);

            return result;
        }

        static string to_hex(const unsigned char *digest, unsigned int length)
        {
            ostringstream oss;
            for (unsigned int i = 0; i < length; ++i)
            {
                oss << hex << setw(2) << setfill('0') << static_cast<int>(digest[i]);
            }
            return oss.str();
        }

        static string hmac_sha256(const string &key, const string &message)
        {
            unsigned int len = EVP_MAX_MD_SIZE;
            unsigned char digest[EVP_MAX_MD_SIZE];

            HMAC(EVP_sha256(),
                 key.data(), key.size(),
                 reinterpret_cast<const unsigned char *>(message.data()), message.size(),
                 digest, &len);

            return to_hex(digest, len); // Convert raw bytes to hex string
        }

        static string hmac_sha256_key_only(const string &key)
        {
            unsigned char digest[EVP_MAX_MD_SIZE];
            unsigned int digest_len = 0;

            // Empty message, real key
            HMAC(EVP_sha256(),
                 key.data(), key.size(),
                 nullptr, 0, // Empty message
                 digest, &digest_len);

            return to_hex(digest, digest_len);
        }

    private:
        bool isEnable = false;
        RgRNG::ServerState server;
        RgRNG::ClientState client;
    };

    static RNG *globalRgRNG = nullptr;

    static RNG *GetRNGInstance(string cheatSeed)
    {
        if (!cheatSeed.empty())
        {
            cout << "RgRNG will be initialized with cheatSeed: " << cheatSeed << endl;
            // cheatSeed example: "serverSeed:clientSeed:nonce:round:cursor"
            string serverSeed = cheatSeed.substr(0, cheatSeed.find(':'));
            cheatSeed = cheatSeed.substr(cheatSeed.find(':') + 1);
            string clientSeed = cheatSeed.substr(0, cheatSeed.find(':'));
            cheatSeed = cheatSeed.substr(cheatSeed.find(':') + 1);
            int nonce = stoi(cheatSeed.substr(0, cheatSeed.find(':')));
            cheatSeed = cheatSeed.substr(cheatSeed.find(':') + 1);
            int round = stoi(cheatSeed.substr(0, cheatSeed.find(':')));
            cheatSeed = cheatSeed.substr(cheatSeed.find(':') + 1);
            int cursor = stoi(cheatSeed);
            const RgRNG::ServerState serverState = {
                serverSeed,
                RNG::hmac_sha256_key_only(clientSeed),
                round,
                cursor};
            const RgRNG::ClientState clientState = {
                RNG::hmac_sha256_key_only(serverSeed),
                clientSeed,
                nonce,
            };
            RgRNG::globalRgRNG = new RNG(serverState, clientState);
            return RgRNG::globalRgRNG;
        }

        if (RgRNG::globalRgRNG != nullptr)
        {
            // cout << "RgRNG already initialized." << endl;
            return RgRNG::globalRgRNG;
        }

        const string clientSeed = RNG::random_bytes_hex(32);
        const string serverSeed = RNG::random_bytes_hex(32);
        const string clientSeedHash = RNG::hmac_sha256_key_only(clientSeed);
        const string serverSeedHash = RNG::hmac_sha256_key_only(serverSeed);

        const RgRNG::ServerState serverState = {
            serverSeed,
            clientSeedHash,
            0,
            0};
        const RgRNG::ClientState clientState = {
            serverSeedHash,
            clientSeed,
            0,
        };

        RgRNG::globalRgRNG = new RNG(serverState, clientState);
        cout << "RgRNG initialized with" << endl;
        cout << "   serverSeed: " << RgRNG::globalRgRNG->GetServerSeed() << endl;
        cout << "   clientSeed: " << RgRNG::globalRgRNG->GetClientState().clientSeed << endl;
        if (IS_RG_RNG_ENABLE_BY_ENV())
        {
            RgRNG::globalRgRNG->Enable();
            cout << "   isEnable: true - due to env IS_RG_RNG_ENABLE = " << RgRNG::globalRgRNG->IsEnable() << endl;
        }
        else
        {
            cout << "   isEnable: false (default)" << endl;
        }
        return RgRNG::globalRgRNG;
    }
}

#endif // RG_RNG_GLOBAL
