#include <stdlib.h>

#include "config.h"

#include "tpm_library_intern.h"
#include "tpm_nvfilename.h"
#include "tpm_error.h"
#include "tpm_memory.h"

#include "StateMarshal.h"
#include "Volatile.h"

UINT16
VolatileSave(BYTE **buffer, INT32 *size)
{
    return VolatileState_Save(buffer, size);
}

TPM_RC
VolatileLoad(void)
{
    TPM_RC rc = TPM_RC_SUCCESS;

#ifdef TPM_LIBTPMS_CALLBACKS
    struct libtpms_callbacks *cbs = TPMLIB_GetCallbacks();

    if (cbs->tpm_nvram_loaddata) {
        unsigned char *data = NULL, *p;
        uint32_t length = 0;
        uint32_t tpm_number = 0;
        const char *name = TPM_VOLATILESTATE_NAME;
        TPM_RESULT ret;

        ret = cbs->tpm_nvram_loaddata(&data, &length, tpm_number, name);
        if (ret == TPM_SUCCESS) {
            p = data;
            rc = VolatileState_Load(&data, (INT32 *)&length);
            /*
             * if this failed, VolatileState_Load will have started
             * failure mode.
             */
            TPM_Free(p);
        }
    }
#endif /* TPM_LIBTPMS_CALLBACKS */

    return rc;
}
