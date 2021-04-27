#include "il2cpp-config.h"

#include "os/Cryptography.h"
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <kernel.h>
#include <libsfmt607.h>
#include <sce_atomic.h>

#include "utils/Memory.h"

#define CRYPTOCTX SceSfmt607Context
#define CRYPTOINIT sceSfmt607InitGenRand
#define CRYPTOFILL sceSfmt607FillArray32

static CRYPTOCTX* s_cryptoProvider = NULL;

namespace il2cpp
{
namespace os
{
    void* Cryptography::GetCryptographyProvider()
    {
        // Matches posix version, no need for reference counting, just return the provider (see case 896138).
        return (void*)s_cryptoProvider;
    }

    bool Cryptography::OpenCryptographyProvider()
    {
        SceKernelUuid outUuid;
        uint32_t seed = 0;

        int result = sceKernelUuidCreate(&outUuid); // get kernel uuid containing unique machine id and time
        if (result == 0)
        {
            if (s_cryptoProvider == NULL)
                s_cryptoProvider = (CRYPTOCTX*)IL2CPP_MALLOC(sizeof(CRYPTOCTX));

            // merge all the data into one 32bit value
            seed = outUuid.timeLow;
            seed = ((seed << 8) | (seed >> 24)) ^ outUuid.timeMid;
            seed = ((seed << 8) | (seed >> 24)) ^ outUuid.timeHiAndVersion;
            seed = ((seed << 8) | (seed >> 24)) ^ outUuid.clockSeqHiAndReserved;
            seed = ((seed << 8) | (seed >> 24)) ^ outUuid.clockSeqLow;
            seed = ((seed << 8) | (seed >> 24)) ^ outUuid.node[0];
            seed = ((seed << 8) | (seed >> 24)) ^ outUuid.node[1];
            seed = ((seed << 8) | (seed >> 24)) ^ outUuid.node[2];
            seed = ((seed << 8) | (seed >> 24)) ^ outUuid.node[3];
            seed = ((seed << 8) | (seed >> 24)) ^ outUuid.node[4];
            seed = ((seed << 8) | (seed >> 24)) ^ outUuid.node[5];
            CRYPTOINIT(s_cryptoProvider, seed);
            return true;
        }

        return false;
    }

    void Cryptography::ReleaseCryptographyProvider(void* provider)
    {
        // Matches posix version, don't release or destroy the provider (see case 896138).
    }

    bool Cryptography::FillBufferWithRandomBytes(void* provider, uint32_t length, unsigned char* data)
    {
        CRYPTOCTX *cryptoProvider = (CRYPTOCTX*)provider;
        if (cryptoProvider == NULL)
        {
            return false;
        }

        int numblocks = length / (4 * 4 * SCE_SFMT607_ARRAY_SIZE);
        if (numblocks > 0)
        {
            int32_t result = CRYPTOFILL(cryptoProvider, (SceUInt32*)data, numblocks * 4 * SCE_SFMT607_ARRAY_SIZE);
            if (result != 0)
            {
                printf("crypto failed 0x%x\n", result);
                return false;
            }
            length -= (numblocks * 4 * SCE_SFMT607_ARRAY_SIZE * 4);
            data += (numblocks * 4 * SCE_SFMT607_ARRAY_SIZE * 4);
        }


        if (length > 0)
        {
            SceUInt32 minbuf[4 * SCE_SFMT607_ARRAY_SIZE];
            int32_t result = CRYPTOFILL(cryptoProvider, minbuf, 4 * SCE_SFMT607_ARRAY_SIZE);
            if (result == 0)
            {
                memcpy(data, minbuf, length);
                return true;
            }
            else
            {
                printf("crypto2 failed 0x%x\n", result);
                return false;
            }
        }
        return true;
    }
}
}
