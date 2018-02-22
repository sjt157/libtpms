/********************************************************************************/
/*										*/
/*		Test case for marshalling and unmarshalling NvChip 		*/
/*			     Written by Stefan Berger				*/
/*		       IBM Thomas J. Watson Research Center			*/
/*										*/
/*  Licenses and Notices							*/
/*										*/
/*  1. Copyright Licenses:							*/
/*										*/
/*  - Trusted Computing Group (TCG) grants to the user of the source code in	*/
/*    this specification (the "Source Code") a worldwide, irrevocable, 		*/
/*    nonexclusive, royalty free, copyright license to reproduce, create 	*/
/*    derivative works, distribute, display and perform the Source Code and	*/
/*    derivative works thereof, and to grant others the rights granted herein.	*/
/*										*/
/*  - The TCG grants to the user of the other parts of the specification 	*/
/*    (other than the Source Code) the rights to reproduce, distribute, 	*/
/*    display, and perform the specification solely for the purpose of 		*/
/*    developing products based on such documents.				*/
/*										*/
/*  2. Source Code Distribution Conditions:					*/
/*										*/
/*  - Redistributions of Source Code must retain the above copyright licenses, 	*/
/*    this list of conditions and the following disclaimers.			*/
/*										*/
/*  - Redistributions in binary form must reproduce the above copyright 	*/
/*    licenses, this list of conditions	and the following disclaimers in the 	*/
/*    documentation and/or other materials provided with the distribution.	*/
/*										*/
/*  3. Disclaimers:								*/
/*										*/
/*  - THE COPYRIGHT LICENSES SET FORTH ABOVE DO NOT REPRESENT ANY FORM OF	*/
/*  LICENSE OR WAIVER, EXPRESS OR IMPLIED, BY ESTOPPEL OR OTHERWISE, WITH	*/
/*  RESPECT TO PATENT RIGHTS HELD BY TCG MEMBERS (OR OTHER THIRD PARTIES)	*/
/*  THAT MAY BE NECESSARY TO IMPLEMENT THIS SPECIFICATION OR OTHERWISE.		*/
/*  Contact TCG Administration (admin@trustedcomputinggroup.org) for 		*/
/*  information on specification licensing rights available through TCG 	*/
/*  membership agreements.							*/
/*										*/
/*  - THIS SPECIFICATION IS PROVIDED "AS IS" WITH NO EXPRESS OR IMPLIED 	*/
/*    WARRANTIES WHATSOEVER, INCLUDING ANY WARRANTY OF MERCHANTABILITY OR 	*/
/*    FITNESS FOR A PARTICULAR PURPOSE, ACCURACY, COMPLETENESS, OR 		*/
/*    NONINFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS, OR ANY WARRANTY 		*/
/*    OTHERWISE ARISING OUT OF ANY PROPOSAL, SPECIFICATION OR SAMPLE.		*/
/*										*/
/*  - Without limitation, TCG and its members and licensors disclaim all 	*/
/*    liability, including liability for infringement of any proprietary 	*/
/*    rights, relating to use of information in this specification and to the	*/
/*    implementation of this specification, and TCG disclaims all liability for	*/
/*    cost of procurement of substitute goods or services, lost profits, loss 	*/
/*    of use, loss of data or any incidental, consequential, direct, indirect, 	*/
/*    or special damages, whether under contract, tort, warranty or otherwise, 	*/
/*    arising in any way out of use or reliance upon this specification or any 	*/
/*    information herein.							*/
/*										*/
/*  (c) Copyright IBM Corp. and others, 2018					*/
/*										*/
/********************************************************************************/

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

#include "Tpm.h"

int memchk(const uint8_t *s1, const uint8_t *s2, size_t len)
{
    unsigned i;
    int rc = 0;

    for (i = 0; i < len; i++) {
        uint8_t c1 = s1[i], c2 = s2[i];
        if (c1 != c2) {
            printf("diff at %d: x%02x vs x%02x\n", i, c1, c2);
            rc = 1;
        }
    }
    return rc;
}

void Test_NvChip_UnMarshal(void)
{
    BYTE buffer[NV_MEMORY_SIZE];
    BYTE *buffer_p = &buffer[0];
    INT32 size = sizeof(buffer);
    TPM_RC rc = TPM_RC_SUCCESS;
    unsigned char bak_NV[NV_MEMORY_SIZE];
    size_t used;
    NV_REF nvend = NvGetEnd();
    NV_RAM_REF nvramend = NvRamGetEnd();

    /* make back of NVChip */
    memcpy(bak_NV, s_NV, sizeof(bak_NV));

    /* marshal */
    printf("Marshalling 'PERSISTENT_ALL'...\n");
    PERSISTENT_ALL_Marshal(&buffer_p, &size);
    used = sizeof(buffer) - size;
    printf("size used by marshalling: %zu\n", used);

    /* clean the NVChip */
    memset(s_NV, 0x0, sizeof(s_NV));

    /* unmarshal */
    buffer_p = &buffer[0];
    size = sizeof(buffer);
    printf("Unmarshalling 'PERSISTENT_ALL'...\n");
    rc = PERSISTENT_ALL_Unmarshal(&buffer_p, &size);

    printf("size used by unmarshalling: %zd\n", sizeof(buffer) - size);
    if (rc != TPM_RC_SUCCESS) {
        sleep(10);
        printf("%s: Unmarshalling failed\n", __func__);
    }
    if (used != sizeof(buffer) - size) {
        printf("marshalling and unmarshalling consumed different number of bytes\n");
        sleep(10);
        goto test_volatile;
    }
    if (nvend != NvGetEnd()) {
        printf("NvGetEnd() returned %d but now %d\n", nvend, NvGetEnd());
        sleep(10);
    }
    if (nvramend != NvRamGetEnd()) {
        printf("NvGetEnd() returned %p but now %p\n", nvramend, NvRamGetEnd());
        sleep(10);
    }

    if (memchk(&bak_NV[NV_PERSISTENT_DATA],
               &s_NV[NV_PERSISTENT_DATA], sizeof(PERSISTENT_DATA))) {
        printf("PERSISTENT DATA IS DIFFERENT\n");
    }
    if (memchk(&bak_NV[NV_ORDERLY_DATA],
               &s_NV[NV_ORDERLY_DATA], sizeof(ORDERLY_DATA))) {
        printf("ORDERLY DATA IS DIFFERENT\n");
    }
    if (memchk(&bak_NV[NV_STATE_RESET_DATA],
               &s_NV[NV_STATE_RESET_DATA], sizeof(STATE_RESET_DATA))) {
        printf("STATE RESET DATA IS DIFFERENT\n");
    }
    if (memchk(&bak_NV[NV_STATE_CLEAR_DATA],
               &s_NV[NV_STATE_CLEAR_DATA], sizeof(STATE_CLEAR_DATA))) {
        printf("STATE CLEAR DATA IS DIFFERENT\n");
    }
    if (memchk(&bak_NV[NV_INDEX_RAM_DATA],
        &s_NV[NV_INDEX_RAM_DATA], NvRamGetEnd() - &s_indexOrderlyRam[0])) {
        printf("NV INDEX ORDERLY RAM DATA IS DIFFERENT (size=%ld)\n",
        NvRamGetEnd() - &s_indexOrderlyRam[0]);
    }
    if (memchk(&bak_NV[NV_USER_DYNAMIC],
               &s_NV[NV_USER_DYNAMIC],
               NvGetEnd() - NV_USER_DYNAMIC)) {
        printf("NV USER RAM DATA IS DIFFERENT (size=%ld)\n",
               NvGetEnd() - NV_USER_DYNAMIC);
    }

    /* restore original NVChip */
    //memcpy(s_NV, bak_NV, sizeof(bak_NV));

test_volatile:
    printf("Marshalling VolatileState...\n");
    buffer_p = &buffer[0];
    size = sizeof(buffer);
    VolatileState_Marshal(&buffer_p, &size);

    used = sizeof(buffer) - size;
    printf("size used by VS marshalling: %zu\n", used);

    printf("Unmarshalling VolatileState...\n");
    buffer_p = &buffer[0];
    size = sizeof(buffer);
    rc = VolatileState_Unmarshal(&buffer_p, &size);

    printf("size used by unmarshalling: %zd\n", sizeof(buffer) - size);
    if (rc != TPM_RC_SUCCESS) {
        printf("%s: Unmarshalling failed\n", __func__);
        sleep(10);
    }
    if (used != sizeof(buffer) - size) {
        printf("marshalling and unmarshalling consumed different number of bytes\n");
        sleep(10);
    }
}
