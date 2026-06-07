// libapsfixup.so — permanent native fix for the OnePlus(aston) APS turbo soft/green/crash bug.
//
// Root cause: the port's gralloc reports a wrong plane layout for the 12.5MP P010 capture
// output, so the byte-identical ArcSoft/Algo blobs compute a garbage chroma plane pointer
// (align_up(luma,0) = 4GB), a zero chroma stride, and run the P010 LSB->MSB conversion with an
// uninitialized source-stride. This lib re-applies, at runtime, the exact corrections proven
// with Frida (op_chroma_repair.js):
//   (1) ARC_Turbo_RAW_Process: output struct plane[1] (UV) ptr = luma + Ysize (=2/3 of buffer);
//       plane[1] pitch = plane[0] (Y) pitch.
//   (2) p010LSB2MSBNeon: set w5 so the loop length (w4*w5*1.5) == buffer size  (full Y+UV, no
//       overrun).
//
// IMPORTANT: this version uses ONLY GOT redirection (writing data pointers) -- NO inline code
// patching -- so it needs NEITHER execmem NOR execmod (execmod is neverallow'd for app domains).
//   * p010LSB2MSBNeon is called via a PLT JUMP_SLOT in libAlgoProcess  -> overwrite that GOT slot.
//   * ARC_Turbo_RAW_Process is resolved by libAlgoInterface via dlsym() -> overwrite the dlsym
//     JUMP_SLOT in libAlgoInterface so our wrapper is what gets stored in the engine struct.
//
// Loaded into com.oplus.camera as a DT_NEEDED of /odm/lib64/libAlgoProcess.so. Offsets are
// pinned to the aston blobs (TODO: update offsets for actual aston blob builds):
//   libAlgoProcess.so    BuildId ???  p010LSB2MSBNeon @ ???, its GOT slot @ ???
//   libAlgoInterface.so  BuildId ???  dlsym GOT slot @ ???
//
#include <android/log.h>
#include <dlfcn.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

#define TAG "apsfixup"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)

static const uintptr_t P010_FUNC_OFF = 0x4bd934;   // TODO: update for aston blob
static const uintptr_t P010_GOT_OFF  = 0x62db58;   // TODO: update for aston blob
static const uintptr_t DLSYM_GOT_OFF = 0x23c8c58;  // TODO: update for aston blob

static inline bool is_buf(uint64_t v)     { uint32_t hi=(uint32_t)(v>>32); return hi>=0x70 && hi<=0x7f && (uint32_t)v >= 0x100000u; }
static inline bool is_garbage(uint64_t v) { uint32_t hi=(uint32_t)(v>>32); return hi>=0x70 && hi<=0x7f && (uint32_t)v <  0x100000u; }

static bool range_of(uint64_t addr, uint64_t* out_base, uint64_t* out_size) {
    int fd = open("/proc/self/maps", O_RDONLY | O_CLOEXEC);
    if (fd < 0) return false;

    char buf[8192];
    ssize_t bytes;
    uint64_t lo = 0, hi = 0;
    int state = 0;

    while ((bytes = read(fd, buf, sizeof(buf))) > 0) {
        for (ssize_t i = 0; i < bytes; i++) {
            char c = buf[i];
            if (state == 0) {
                if (c == '-') state = 1;
                else lo = (lo << 4) | (c <= '9' ? c - '0' : (c & 0xDF) - 'A' + 10);
            } else if (state == 1) {
                if (c == ' ') {
                    if (addr >= lo && addr < hi) {
                        *out_base = lo;
                        *out_size = hi - lo;
                        close(fd);
                        return true;
                    }
                    state = 2;
                } else {
                    hi = (hi << 4) | (c <= '9' ? c - '0' : (c & 0xDF) - 'A' + 10);
                }
            } else if (state == 2) {
                if (c == '\n') {
                    state = 0;
                    lo = 0; hi = 0;
                }
            }
        }
    }
    close(fd);
    return false;
}
static bool module_base(const char* name, uint64_t* out_base) {
    FILE* f = fopen("/proc/self/maps", "re");
    if (!f) return false;
    char line[512]; uint64_t best = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, name)) { uint64_t lo;
            if (sscanf(line, "%" SCNx64, &lo) == 1) if (best == 0 || lo < best) best = lo; }
    }
    fclose(f);
    if (best) { *out_base = best; return true; }
    return false;
}

static bool got_redirect(uint64_t slot, void* newval, void** old) {
    void** got = (void**)slot;
    uintptr_t page = slot & ~(uintptr_t)0xfff;
    if (mprotect((void*)page, 0x1000, PROT_READ | PROT_WRITE) != 0) { LOGW("mprotect GOT %p failed", (void*)slot); return false; }
    if (old) *old = *got;
    *got = newval;
    mprotect((void*)page, 0x1000, PROT_READ);
    return true;
}

static void repair_struct(void* p) {
    if (!p) return;
    uint64_t mb, ms; if (!range_of((uint64_t)p, &mb, &ms)) return;
    uint8_t* b = (uint8_t*)p;
    for (int off = 0; off + 16 <= 0x80; off += 8) {
        uint64_t luma = *(uint64_t*)(b + off), chroma = *(uint64_t*)(b + off + 8);
        if (is_buf(luma) && is_garbage(chroma)) {
            uint64_t lb, ls; if (!range_of(luma, &lb, &ls)) continue;
            uint64_t avail = (lb + ls) - luma;
            uint64_t ysize = (avail * 2 / 3) & ~0xfffULL;
            *(uint64_t*)(b + off + 8) = luma + ysize;
            if (off == 0x40) {
                uint32_t yp = *(uint32_t*)(b + 0x60);
                if (yp > 0 && *(uint32_t*)(b + 0x64) == 0) *(uint32_t*)(b + 0x64) = yp;
            }
            LOGI("chroma fix: luma=%p -> %p (ysize=0x%llx)", (void*)luma, (void*)(luma + ysize), (unsigned long long)ysize);
        }
    }
}
extern "C" __attribute__((visibility("hidden"))) void* aps_real_arc = nullptr;
extern "C" __attribute__((visibility("hidden"))) void aps_repair_structs(void* a1, void* a2, void* a3) {
    repair_struct(a1); repair_struct(a2); repair_struct(a3);
}
extern "C" void wrap_arc();
__asm__(
"    .text\n"
"    .balign 4\n"
"    .global wrap_arc\n"
"    .type wrap_arc, %function\n"
"wrap_arc:\n"
"    stp x29, x30, [sp, #-0x60]!\n"
"    mov x29, sp\n"
"    stp x0, x1, [sp, #0x10]\n"
"    stp x2, x3, [sp, #0x20]\n"
"    stp x4, x5, [sp, #0x30]\n"
"    stp x6, x7, [sp, #0x40]\n"
"    str x8, [sp, #0x50]\n"
"    ldr x0, [sp, #0x18]\n"
"    ldr x1, [sp, #0x20]\n"
"    ldr x2, [sp, #0x28]\n"
"    bl  aps_repair_structs\n"
"    ldp x0, x1, [sp, #0x10]\n"
"    ldp x2, x3, [sp, #0x20]\n"
"    ldp x4, x5, [sp, #0x30]\n"
"    ldp x6, x7, [sp, #0x40]\n"
"    ldr x8, [sp, #0x50]\n"
"    ldp x29, x30, [sp], #0x60\n"
"    adrp x16, aps_real_arc\n"
"    add  x16, x16, #:lo12:aps_real_arc\n"
"    ldr  x16, [x16]\n"
"    br   x16\n"
);

typedef void* (*dlsym_t)(void*, const char*);
static dlsym_t g_real_dlsym = nullptr;
static void* wrap_dlsym(void* handle, const char* symbol) {
    void* res = g_real_dlsym(handle, symbol);
    if (symbol && res && strcmp(symbol, "ARC_Turbo_RAW_Process") == 0) {
        aps_real_arc = res;
        LOGI("interposing ARC_Turbo_RAW_Process (real=%p)", res);
        return (void*)wrap_arc;
    }
    return res;
}

typedef void (*p010_t)(uint16_t*, uint16_t*, uint32_t, uint32_t, uint32_t, uint32_t);
static p010_t g_real_p010 = nullptr;
static void wrap_p010(uint16_t* dst, uint16_t* src, uint32_t w2, uint32_t w3, uint32_t w4, uint32_t w5) {
    if (w4 > 0) {
        uint64_t sb, ss;
        if (range_of((uint64_t)src, &sb, &ss)) {
            uint64_t avail  = (sb + ss) - (uint64_t)src;
            uint32_t new_w5 = (uint32_t)((avail * 2 / 3) / w4);
            if (new_w5 > 0 && new_w5 != w5) {
                LOGI("p010 fix: avail=0x%llx w4=%u w5 %u->%u", (unsigned long long)avail, w4, w5, new_w5);
                w5 = new_w5;
            }
        }
    }
    g_real_p010(dst, src, w2, w3, w4, w5);
}

static bool g_p010_done = false, g_dlsym_done = false;
static void try_install() {
    uint64_t base;
    if (!g_p010_done && module_base("libAlgoProcess.so", &base)) {
        void* old = nullptr;
        if (got_redirect(base + P010_GOT_OFF, (void*)wrap_p010, &old)) {
            g_real_p010 = (p010_t)old;
            void* expect = (void*)(base + P010_FUNC_OFF);
            if (old != expect) LOGW("GOT[p010]=%p expected %p (blob drift?)", old, expect);
            LOGI("GOT-hooked p010 (real=%p)", old);
            g_p010_done = true;
        }
    }
    if (!g_dlsym_done && module_base("libAlgoInterface.so", &base)) {
        void* old = nullptr;
        if (got_redirect(base + DLSYM_GOT_OFF, (void*)wrap_dlsym, &old)) {
            g_real_dlsym = (dlsym_t)old;
            LOGI("GOT-hooked dlsym in libAlgoInterface (real=%p)", old);
            g_dlsym_done = true;
        }
    }
}
static void* poller(void*) {
    for (int i = 0; i < 24000 && !(g_p010_done && g_dlsym_done); i++) { try_install(); usleep(25 * 1000); }
    if (!(g_p010_done && g_dlsym_done)) LOGW("install incomplete: p010=%d dlsym=%d", g_p010_done, g_dlsym_done);
    return nullptr;
}

__attribute__((constructor))
static void apsfixup_init() {
    LOGI("libapsfixup loaded (pid %d)", getpid());
    try_install();
    if (!(g_p010_done && g_dlsym_done)) {
        pthread_t t; pthread_create(&t, nullptr, poller, nullptr); pthread_detach(t);
    }
}
