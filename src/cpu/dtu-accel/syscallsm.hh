/*
 * Copyright (c) 2016, Nils Asmussen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the FreeBSD Project.
 */

#ifndef __CPU_DTU_ACCEL_SYSCALLSM_HH__
#define __CPU_DTU_ACCEL_SYSCALLSM_HH__

#include "cpu/dtu-accel/accelerator.hh"
#include "sim/system.hh"

class SyscallSM
{
  public:
    enum Operation
    {
        // sent by the DTU if the PF handler is not reachable
        PAGEFAULT = 0,

        // capability creations
        CREATE_SRV,
        CREATE_SESS,
        CREATE_RGATE,
        CREATE_SGATE,
        CREATE_MGATE,
        CREATE_MAP,
        CREATE_VPEGRP,
        CREATE_VPE,

        // capability operations
        ACTIVATE,
        SRV_CTRL,
        VPE_CTRL,
        VPE_WAIT,
        DERIVE_MEM,
        OPEN_SESS,

        // capability exchange
        DELEGATE,
        OBTAIN,
        EXCHANGE,
        REVOKE,

        // forwarding
        FORWARD_MSG,
        FORWARD_MEM,
        FORWARD_REPLY,

        // misc
        NOOP,
    };

    enum VPEOp
    {
        VCTRL_INIT,
        VCTRL_START,
        VCTRL_YIELD,
        VCTRL_STOP,
    };

    enum State
    {
        SYSC_SEND,
        SYSC_WAIT,
        SYSC_FETCH,
        SYSC_READ_ADDR,
        SYSC_ACK,
    };

    explicit SyscallSM(DtuAccel *_accel)
        : accel(_accel), state(), stateChanged(), waitForReply(), fetched(),
          replyAddr(), syscallSize() {}

    std::string stateName() const;

    bool isWaiting() const { return state == SYSC_FETCH; }

    bool hasStateChanged() const { return stateChanged; }

    void retryFetch() { fetched = false; }

    void start(Addr size, bool wait = true, bool resume = false)
    {
        syscallSize = size;
        state = resume ? SYSC_FETCH : SYSC_SEND;
        fetched = false;
        waitForReply = wait;
    }

    PacketPtr tick();

    bool handleMemResp(PacketPtr pkt);

   private:

    DtuAccel *accel;
    State state;
    bool stateChanged;
    bool waitForReply;
    bool fetched;
    Addr replyAddr;
    Addr syscallSize;
};

#endif /* __CPU_DTU_ACCEL_SYSCALLSM_HH__ */
